#include "ctxpp.h"
#include <limits>

ctx_kernel* ctx_kernel::m_inst = NULL;

ctx_kernel::ctx_kernel():
	components(),
	running_tasks(),
	pending_tasks(),
	finished_tasks(),
	new_tasks(),
	tasks_consuming_time(),
	events_to_wait(),
	emitted_events(),
	events_to_fire(),
	context_switches(),
	tasks_to_join(),
	tasks_to_fork(),
	signals(),
	times_to_wait()
{
	with_real_time = false;
	tasks_were_killed = false;
	time=0;
	delta=0;
	dq = 0;
	time_scale=1;
	time_unit=CTX_SIM_S;
	state=CTX_KERNEL_PAUSED;
	delta_t = std::numeric_limits<double>::max();
}

ctx_kernel::~ctx_kernel() {}

ctx_kernel* ctx_kernel::get() {
	if(m_inst==NULL) m_inst = new ctx_kernel();
	return m_inst;
}

void ctx_kernel::run(ctx_component* r,double time_limit=-1,long int delta_limit=-1) {
	state=CTX_KERNEL_RUNNING;
	set_root(r);
	initialize_components();
	
	while(1) {
		evaluate();
		if(state!=CTX_KERNEL_RUNNING) break;
		
		update();
		
		if(tasks_to_join.size()!=0 || tasks_to_fork.size()!=0)
			advance_waiting_tasks();
		
		if(emitted_events.size()!=0) {
			advance_delta(delta_limit);
			if(state==CTX_KERNEL_RUNNING) continue;
			else return;
		}
		else if( tasks_running() )
			continue;
		else if(tasks_consuming_time.size()!=0) {
			advance_time(time_limit);
			if(state==CTX_KERNEL_RUNNING) continue;
		} else
			break;
	}
}

void ctx_kernel::evaluate() {
	ctx_switch* cs;
	std::vector<ctx_task_base*>::iterator subtask,it;
	std::list<ctx_task_base*>::iterator task;
	std::vector<ctx_event*>::iterator ev;
	std::pair<std::set<double>::iterator,bool> result;
	// Auxiliary task pointer
	ctx_task_base* atp;
	// Run sync callbacks for fired events
	for(ev=events_to_fire.begin();ev!=events_to_fire.end();++ev) {
		(*ev)->call_sync_callbacks();
	}
	// Resume tasks 
	task = running_tasks.begin();
	while(task!=running_tasks.end()) {
		// Advance task execution until the next context switch
		if((*task)->state!=CTX_TASK_FINISHED) {
			current_task = (*task);// TODO Adapt this for native threads
			cs = (*task)->next();
			if((*task)->cs) delete (*task)->cs; // TODO this might hurt performance
			(*task)->cs = cs;
			// context_switches[(*task)] = cs->type;
			// Update kernel state
			switch(cs->type) {
				case CTX_CS_YIELD:
					(*task)->state = CTX_TASK_RUNNING;
					break;
				case CTX_CS_DELAY:
					(*task)->time_to_wait = ((ctx_switch_delay*)cs)->delay_val;
					result=times_to_wait.insert((*task)->time_to_wait+dq);
					(*task)->unique_ttw=result.second;
					tasks_consuming_time.push_back(*task);
					(*task)->tct_iterator= --tasks_consuming_time.end();
					(*task)->state = CTX_TASK_PENDING;
					break;
				case CTX_CS_DELAY_STD:
					(*task)->time_to_wait = 
					((ctx_switch_delay_std*)cs)->delay_std_val *
					(((ctx_switch_delay_std*)cs)->time_unit/(time_unit*time_scale));
					result=times_to_wait.insert((*task)->time_to_wait+dq);
					(*task)->unique_ttw=result.second;
					tasks_consuming_time.push_back(*task);
					(*task)->tct_iterator= --tasks_consuming_time.end();
					(*task)->state = CTX_TASK_PENDING;
					break;
				case CTX_CS_WAIT_EV:
					events_to_wait[(*task)] = ((ctx_switch_wait_ev*)cs)->event;
					(*task)->state = CTX_TASK_PENDING;
					break;
				case CTX_CS_JOIN:
					tasks_to_join[(*task)] = ((ctx_switch_join_task*)cs)->task_to_join;
					atp = ((ctx_switch_join_task*)cs)->task_to_join;
					(*task)->state = CTX_TASK_PENDING;
					if(!atp->is_registered)
						register_task(atp);
					break;
				case CTX_CS_ALL_OF:
					tasks_to_fork[(*task)] =((ctx_switch_all_of*)cs)->tasks;
					(*task)->state = CTX_TASK_PENDING;
					for(subtask=(((ctx_switch_all_of*)cs)->tasks)->begin();subtask!=(((ctx_switch_all_of*)cs)->tasks)->end();++subtask)
						if(!(*subtask)->is_registered) register_task(*subtask);
					break;
				case CTX_CS_FIRST_OF:
					tasks_to_fork[(*task)] = ((ctx_switch_first_of*)cs)->tasks;
					(*task)->state = CTX_TASK_PENDING;
					for(subtask=(((ctx_switch_first_of*)cs)->tasks)->begin();subtask!=(((ctx_switch_first_of*)cs)->tasks)->end();++subtask)
						if(!(*subtask)->is_registered) register_task(*subtask);
					break;
				case CTX_CS_ANY_OF:
					tasks_to_fork[(*task)] = ((ctx_switch_any_of*)cs)->tasks;
					(*task)->state = CTX_TASK_PENDING;
					for(subtask=(((ctx_switch_any_of*)cs)->tasks)->begin();subtask!=(((ctx_switch_any_of*)cs)->tasks)->end();++subtask)
						if(!(*subtask)->is_registered) register_task(*subtask);
					break;
				case CTX_CS_STOP:
					(*task)->state = CTX_TASK_FINISHED;
					break;
				case CTX_CS_FINISH:
					(*task)->state = CTX_TASK_FINISHED;
					state = CTX_KERNEL_FINISHED;
					break;
				case CTX_CS_RETURN_VALUE:
					(*task)->state = CTX_TASK_FINISHED;
					break;
				default:
					break;
			}
		}
		// Update task management lists
		if((*task)->state==CTX_TASK_PENDING) {
			pending_tasks.push_back(*task);
			(*task)->pending_iterator = --pending_tasks.end();
			task = running_tasks.erase(task);			
		} 
		else if((*task)->state==CTX_TASK_FINISHED) {
			// Task was stopped or returned a value
			// or the simulation was finished
			finished_tasks.push_back(*task);
			unregister_task(*task);
			task = running_tasks.erase(task);
			//tasks_were_killed=false;
		} else 
			++task;
	}
	
	// Register newly spawned tasks
	if(new_tasks.size()>0) {
		for(it=new_tasks.begin();it!=new_tasks.end();++it)
			schedule_start_task(*it);
		new_tasks.clear();
	}
	
	// Unregister killed tasks
	if(tasks_were_killed) {
		task = pending_tasks.begin();
		while(task!=pending_tasks.end()) {
			if((*task)->state==CTX_TASK_FINISHED) {
				finished_tasks.push_back(*task);
				unregister_task(*task);
				task = pending_tasks.erase(task);
			} else
				++task;
		}
		tasks_were_killed=false;
	}
}

void ctx_kernel::update() {
}

void ctx_kernel::advance_time(double time_limit=-1) {
	std::list<ctx_task_base*>::iterator t;
	delta = 0;
	
	//std::map<ctx_task_base*,double>::iterator t;
	// Calculate delta_t(minimum of all times to wait)
	//for(t = times_to_wait.begin();t!=times_to_wait.end();++t)
	//	if(t->second < delta_t) delta_t = t->second;
	
	delta_t = *(times_to_wait.begin());
	delta_t -= dq;
	if(time+delta_t>=time_limit && time_limit!=-1)
		delta_t = (time+delta_t)-time_limit;
	dq += delta_t;
	times_to_wait.erase(times_to_wait.begin());
	
	// Resume tasks which wait the smallest time
	t = tasks_consuming_time.begin();
	while(t!=tasks_consuming_time.end())
	{
		if((*t)->time_to_wait==delta_t && (*t)->state!=CTX_TASK_FINISHED ) {
			schedule_resume_task(*t);
			(*t)->cs->type=CTX_CS_YIELD;
			t=tasks_consuming_time.erase(t);
		} else {
			if(time+delta_t<time_limit || time_limit==-1)
				(*t)->time_to_wait -= delta_t;
			else
				(*t)->time_to_wait -= (time+delta_t) - time_limit;
			++t;
		}
	}
	
	// Update time
	if(time_limit==-1) {
		if(with_real_time) ; // TODO : Wait real time here
		time+=delta_t;
		state = CTX_KERNEL_RUNNING;
	} else if(time+delta_t > time_limit) {
		time = time_limit;
		state = CTX_KERNEL_PAUSED;
	}	
}

void ctx_kernel::advance_delta(long int delta_limit=-1) {
	std::map<ctx_task_base*, ctx_event*>::iterator e;
	// Resume tasks waiting for emitted events
	for(e = events_to_wait.begin(); e!=events_to_wait.end(); ++e) {
		if(e->second->state==CTX_EVENT_ON && e->first->state!=CTX_TASK_FINISHED) {
			schedule_resume_task(e->first);
			e->second->consume();
			events_to_wait.erase(e);
		}
	}
	// Schedule execution of sync callbacks
	events_to_fire = emitted_events;
	// Clear list of emitted events for the next delta cycle
	emitted_events.clear();
	// Advance delta counter
	if(delta+1>delta_limit && delta_limit!=-1) {
		delta = delta_limit;
		state = CTX_KERNEL_PAUSED;
	} else {
		delta++;
		state=CTX_KERNEL_RUNNING;
	}	
}

bool all_tasks_finished(std::vector<ctx_task_base*>* tasks) {
	std::vector<ctx_task_base*>::iterator t;
	for(t=tasks->begin();t!=tasks->end();t++)
		if((*t)->state!=CTX_TASK_FINISHED)
			return false;
	return true;
}

bool has_finished_task(std::vector<ctx_task_base*>* tasks) {
	std::vector<ctx_task_base*>::iterator t;
	for(t=tasks->begin();t!=tasks->end();t++)
		if((*t)->state==CTX_TASK_FINISHED)
			return true;
	return false;
}

void get_running_tasks(std::vector<ctx_task_base*>* tasks,std::vector<ctx_task_base*>& running_tasks) {
	std::vector<ctx_task_base*>::iterator it;
	for(it=tasks->begin();it!=tasks->end();it++) {
		if((*it)->state!=CTX_TASK_FINISHED)
			running_tasks.push_back(*it);
	}
}

void ctx_kernel::advance_waiting_tasks() {
	std::map<ctx_task_base*,ctx_task_base*>::iterator jt;
	std::map<ctx_task_base*,std::vector<ctx_task_base*>*>::iterator ft;
	std::vector<ctx_task_base*> aux_task_list;
	std::vector<ctx_task_base*>::iterator aux_it;
	// Advance tasks waiting for a single task to finish
	for(jt=tasks_to_join.begin();jt!=tasks_to_join.end();++jt) {
		if(jt->second->state==CTX_TASK_FINISHED && jt->first->state!=CTX_TASK_FINISHED) {
			schedule_resume_task(jt->first);
			tasks_to_join.erase(jt);
		}
	}
	// Advance tasks waiting for groups of tasks
	for(ft=tasks_to_fork.begin();ft!=tasks_to_fork.end();++ft) {
		switch(ft->first->cs->type) {
			case CTX_CS_FIRST_OF:
				if(has_finished_task(ft->second)) {
					get_running_tasks(ft->second,aux_task_list);
					for(aux_it=aux_task_list.begin();aux_it!=aux_task_list.end();++aux_it)
						(*aux_it)->kill();
					schedule_resume_task(ft->first);
					delete ft->second;
					tasks_to_fork.erase(ft);
				}
				break;
			case CTX_CS_ALL_OF:
				if(all_tasks_finished(ft->second)) {
					schedule_resume_task(ft->first);
					delete ft->second;
					tasks_to_fork.erase(ft);
				}
				break;
			case CTX_CS_ANY_OF:
				if(has_finished_task(ft->second)) {
					schedule_resume_task(ft->first);
					delete ft->second;
					tasks_to_fork.erase(ft);
				}
				break;
			default:
				break;
		}		
	}
	state=CTX_KERNEL_RUNNING;	
}

// Mark task as started without adding it to running_tasks
void ctx_kernel::register_task(ctx_task_base* task) {
	task->is_registered=true;
	new_tasks.push_back(task);
}

// Call this to schedule the task to start in the next iteration
void ctx_kernel::schedule_start_task(ctx_task_base* task) {
	running_tasks.push_back(task);
	task->running_iterator=--running_tasks.end();
}

// Schedule a task to be resumed in the next iteration
void ctx_kernel::schedule_resume_task(ctx_task_base* t) {
	t->state=CTX_TASK_RUNNING;
	pending_tasks.erase(t->pending_iterator);
	running_tasks.push_back(t);
	t->running_iterator=--running_tasks.end();
}

void ctx_kernel::unregister_task(ctx_task_base* task) {
	// Remove from times_to_wait
	//if(times_to_wait.find(task)!=times_to_wait.end())
	//	times_to_wait.erase(task);
	//if(task->unique_ttw)
	//	times_to_wait.erase(task->time_to_wait);
	if(task->cs!=NULL && task->cs->type==CTX_CS_DELAY)
		tasks_consuming_time.erase(task->tct_iterator);
	// Remove from events_to_wait
	//if(events_to_wait.find(task)!=events_to_wait.end())
	events_to_wait.erase(task);
	// Remove from context_switches
	//if(context_switches.find(task)!=context_switches.end())
	context_switches.erase(task);
	// Remove from tasks_to_join
	//if(tasks_to_join.find(task)!=tasks_to_join.end())
	tasks_to_join.erase(task);
	// Remove from tasks_to_fork
	//if(tasks_to_fork.find(task)!=tasks_to_fork.end())
	tasks_to_fork.erase(task);
	
	task->is_registered=false;
}

// Component related methods
void ctx_kernel::register_component(ctx_component* c) {
	components.push_back(c);
}

void ctx_kernel::initialize_components() {
	// Build
	build_components(root);
	// Connect
	connect_components(root);
	// Run
	run_components(root);
}

void ctx_kernel::get_child_components(ctx_component* c, std::vector<ctx_component*>& clist) {
	std::vector<ctx_component*>::iterator it;
	
	for(it=components.begin();it!=components.end();++it)
		if((*it)->parent==c)
			clist.push_back(*it);
}

void ctx_kernel:: build_components(ctx_component* c) {
	std::vector<ctx_component*> child_comps;
	std::vector<ctx_component*>::iterator comp;
	get_child_components(c,child_comps);
	
	c->build_phase();
	
	for(comp=child_comps.begin();comp!=child_comps.end();++comp)
		build_components(*comp);
}

void ctx_kernel::connect_components(ctx_component* c) {
	std::vector<ctx_component*> child_comps;
	std::vector<ctx_component*>::iterator comp;
	get_child_components(c,child_comps);
	
	c->connect_phase();
	
	for(comp=child_comps.begin();comp!=child_comps.end();++comp)
		connect_components(*comp);
}

void ctx_kernel::run_components(ctx_component* c) {
	std::vector<ctx_component*> child_comps;
	std::vector<ctx_component*>::iterator comp;
	get_child_components(c,child_comps);
	
	c->run_phase();
	
	for(comp=child_comps.begin();comp!=child_comps.end();++comp)
		run_components(*comp);
}

ctx_task_base* ctx_kernel::get_current_task() {
	return current_task;
}

bool ctx_kernel::tasks_running() {
	return running_tasks.size()>0;
}

// Getters and setters
void ctx_kernel::set_root(ctx_component* c) {
	root = c;
}

void ctx_kernel::set_tasks_were_killed() {
	tasks_were_killed=true;
}
