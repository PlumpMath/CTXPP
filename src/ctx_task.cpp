#include "ctxpp.h"


int ctx_task_base::task_id_counter=0;

ctx_task_base::ctx_task_base() {}

ctx_task_base::~ctx_task_base() {}

void ctx_task_base::kill() {
	state = CTX_TASK_FINISHED;
	ker->set_tasks_were_killed();
	ker->unregister_task(this);
	kill_children();
}

void ctx_task_base::kill_children() {
	std::vector<ctx_task_base*>::iterator it;
	std::vector<ctx_task_base*> cl;
	get_children(cl);
	for(it=cl.begin();it!=cl.end();++it)
		(*it)->kill();
}

void ctx_task_base::get_children(std::vector<ctx_task_base*>& result) {
	std::list<ctx_task_base*>::iterator it;
	for(it=ker->running_tasks.begin();it!=ker->running_tasks.end();++it) 
		if((*it)->parent == this && (*it)->state!=CTX_TASK_FINISHED)
			result.push_back(*it);
	for(it=ker->pending_tasks.begin();it!=ker->pending_tasks.end();++it) 
		if((*it)->parent == this && (*it)->state!=CTX_TASK_FINISHED)
			result.push_back(*it);
}

ctx_task_base* ctx_task_base::self() {
	return this;
}

void ctx_task_base::start() {
	ker->register_task(this);
}

ctx_switch* ctx_task_base::next() { return NULL; }