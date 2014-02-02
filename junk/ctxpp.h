#ifndef _CTXPP_H
#define _CTXPP_H

#include <vector>
#include <string>

// Standard time units
#define CTX_SIM_FS   1.0e-15
#define CTX_SIM_PS   1.0e-12
#define CTX_SIM_AS   1.0e-10
#define CTX_SIM_NS   1.0e-9
#define CTX_SIM_US   1.0e-6
#define CTX_SIM_MS   1.0e-3
#define CTX_SIM_S    1.0
#define CTX_SIM_MIN  60.0
#define CTX_SIM_H    3600.0

// Context switch types
typedef enum {
	CTX_CS_YIELD,        // Pass control to another task(consume a delta cycle)
	CTX_CS_DELAY,        // Consume simulation time units  
	CTX_CS_DELAY_STD,    // Consume standard time units
	CTX_CS_WAIT_EV,      // Wait for an event to be emitted
	CTX_CS_JOIN,         // Wait for another task to finish
	CTX_CS_ALL_OF,       // Wait for a group of tasks to finish
	
	CTX_CS_FIRST_OF,     // Wait for the first task from a group
			     // to finish then kill the other tasks
			     // from that group
	
	CTX_CS_ANY_OF,       // Wait for the first task from a group
			     // to finish without killing the other
			     // tasks from that group
	
	CTX_CS_STOP,         // Stop the current task
	CTX_CS_FINISH,       // Stop simulation
	CTX_CS_RETURN_VALUE  // Return a value
	
} ctx_type;

typedef enum {
	CTX_NONE,
	CTX_LOW,
	CTX_MEDIUM,
	CTX_HIGH,
	CTX_FULL
} ctx_verb;

struct ctx_event;
struct ctx_task_base;
struct ctx_object;
struct ctx_component;
struct ctx_signal;

struct ctx_switch {
	ctx_type type;
	ctx_task_base* origin;// TODO : remove if not needed
	
	ctx_switch():type(CTX_CS_YIELD) {}
	~ctx_switch() {}

};
#define CS_YIELD \
do {\
	_line=__LINE__;\
	ctx_switch* cs = new ctx_switch();\
	cs->origin = this;\
	return cs;\
	case __LINE__:;\
} while (0)

struct ctx_switch_delay:ctx_switch {
	long int delay_val;
	
	ctx_switch_delay(long int dval) {
		type = CTX_CS_DELAY;
		delay_val=dval;
	}
	~ctx_switch_delay() {}
};
#define CS_DELAY(D)\
do {\
	_line=__LINE__;\
	ctx_switch_delay* cs=new ctx_switch_delay((D));\
	cs->origin = this;\
	return (ctx_switch*)cs;\
	case __LINE__:;\
} while(0)

struct ctx_switch_delay_std:ctx_switch {
	long int delay_std_val;
	double time_unit;

	ctx_switch_delay_std(long int dval, double tunit) {
		type = CTX_CS_DELAY_STD;
		delay_std_val = dval;
		time_unit = tunit;
	}
	~ctx_switch_delay_std() {}
};
#define CS_DELAY_STD(D,TU)\
do {\
	_line=__LINE__;\
	ctx_switch_delay_std* cs=new ctx_switch_delay_std((D),(TU));\
	cs->origin = this;\
	return (ctx_switch*)cs;\
	case __LINE__:;\
} while(0)

struct ctx_switch_wait_ev:ctx_switch {
	ctx_event* event;

	ctx_switch_wait_ev(ctx_event* ev) {
		type = CTX_CS_WAIT_EV;
		event = ev;
	}
	~ctx_switch_wait_ev() {}
};
#define CS_WAIT(E)\
do {\
	_line=__LINE__;\
	ctx_switch_wait_ev* cs=new ctx_switch_wait_ev((E));\
	cs->origin = this;\
	return (ctx_switch*)cs;
	case __LINE__:;\
} while(0)

struct ctx_switch_join_task:ctx_switch {
	ctx_task_base* task_to_join;

	ctx_switch_join_task(ctx_task_base* task) {
		type = CTX_CS_JOIN;
		task_to_join = task;
	}
	~ctx_switch_join_task() {}

};
#define CS_JOIN(T)\
do {\
	_line=__LINE__;\
	ctx_switch_join_task* cs=new ctx_switch_join_task((T));\
	cs->origin = this;\
	return (ctx_switch*)cs;\
	case __LINE__:;\
} while(0)

struct ctx_switch_all_of:ctx_switch {
	vector<ctx_task_base*>* tasks;
	
	ctx_switch_all_of(vector<ctx_task_base*>* tlist) {
		type = CTX_CS_ALL_OF;
		tasks = tlist;
	}
	~ctx_switch_all_of() {}
};
#define CS_ALL_OF(TL)\
do {\
	_line=__LINE__;\
	ctx_switch_all_of* cs=new ctx_switch_all_of((TL));\
	cs->origin = this;\
	return (ctx_switch*)cs;\
	case __LINE__:;\
} while(0)

struct ctx_switch_first_of:ctx_switch {
	vector<ctx_task_base*>* tasks;
	
	ctx_switch_first_of(vector<ctx_task_base*>* tlist) {
		type = CTX_CS_FIRST_OF;
		tasks = tlist;
	}
	~ctx_switch_first_of() {}
};
#define CS_FIRST_OF\
do {\
	_line=__LINE__;\
	ctx_switch_first_of* cs=new ctx_switch_first_of((TL));\
	cs->origin = this;\
	return (ctx_switch*)cs;\
	case __LINE__:;\
} while(0)

struct ctx_switch_any_of:ctx_switch {
	vector<ctx_task_base*>* tasks;

	ctx_switch_any_of(vector<ctx_task_base*>* tlist) {
		type = CTX_CS_ANY_OF;
		tasks = tlist;
	}
	~ctx_switch_any_of() {}
};
#define CS_ANY_OF\
do {\
	_line=__LINE__;\
	ctx_switch_any_of* cs=new ctx_switch_any_of((TL));\
	cs->origin = this;\
	return (ctx_switch*)cs;\
	case __LINE__:;\
} while(0)

struct ctx_switch_stop:ctx_switch {
	ctx_switch_stop() {
		type = CTX_CS_STOP;
	}
	~ctx_switch_stop() {}
};
#define CS_STOP\
do {\
	_line=__LINE__;\
	ctx_switch_stop* cs=new ctx_switch_stop();\
	cs->origin = this;\
	return (ctx_switch*)cs;\
	case __LINE__:;\
} while(0)

struct ctx_switch_finish:ctx_switch {
	ctx_switch_finish() {
		type = CTX_CS_FINISH;
	}
	~ctx_switch_finish() {}
};
#define CS_FINISH\
do {\
	_line=__LINE__;\
	ctx_switch_finish* cs=new ctx_switch_finish();\
	cs->origin = this;\
	return (ctx_switch*)cs;\
	case __LINE__:;\
} while(0)

struct ctx_switch_return_val:ctx_switch {
	ctx_switch_stop() {
		type = CTX_CS_RETURN_VALUE;
	}
	~ctx_switch_stop() {}
}
#define CS_RETURN_VAL(V)\
do {\
	_line=__LINE__;\
	ctx_switch_return_val* cs=new ctx_switch_return_val();\
	cs->origin = this;\
	rv = (RV);\
	return (ctx_switch*)cs;\
	case __LINE__:;\
} while(0)


struct ctx_simulator {

	long double time;
	long int delta;
	double time_unit;
	ctx_task_base* current_task;
	static ctx_simulator* m_inst;

	ctx_simulator();

	static ctx_simulator* get();

	// Simulation logic
	virtual void evaluate();
	virtual void resume_thread();
	virtual void update();
	virtual void advance_time(double time_limit=-1);
	virtual void advance_delta(long int delta_limit=-1);
	virtual void advance_waiting_tasks();
	virtual void simulate(double time_limit=-1,long int delta_limit=-1);
	
	virtual void initialize();
	virtual void register_task(
	ctx_task_base* task,
	ctx_task_base* parent_task=NULL,
	ctx_component* comp=NULL
	);
	virtual void unregister_task(ctx_task_base* task);
	virtual void register_component(ctx_component* c);
	virtual void message(string msg_id, string msg, ctx_verb verb);
	virtual void warning(string wrn_id, string wrn_msg);
	virtual void error(string err_id, string err_msg);

	private:
	map<ctx_component*,ctx_component*> components;
	vector<ctx_task_base*> tasks;
	map<ctx_task_base*,double> times_to_wait;
	map<ctx_task_base*,ctx_event*> events_to_wait;
	vector<ctx_event*> emitted_events;
	vector<ctx_event*> events_to_fire;
	map<ctx_task_base*,ctx_type> context_switches;
	map<ctx_task_base*,ctx_task_base*> tasks_to_join;
	map<ctx_task_base*,vector<ctx_task_base*>*> tasks_to_fork;
	map<ctx_task_base*,ctx_component*> tasks_compoenents;
	map<ctx_task_base*,ctx_task_base*> task_parents;
	vector<ctx_signal*> signals;
	vector<string> disabled_messages;
	vector<string> disabled_errors;
	vector<string> disabled_warnings;
	bool with_real_time;

};

struct ctx_task_base {
};
template <typename CT=ctx_component>
struct ctx_task:ctx_task_base {
	int _line;
	ctx_simulator* sim;
	CT* ctx;
	// Constructor
	ctx_task():_line(-1) {}
	// Jumps to next context switch
	ctx_switch* next();
	// Get an instance of this task
	static ctx_task* call_from(CT*);
	// Kill task instance
	void kill();
	// Kill all children recursively
	void kill_children();
	// Get a list with all the child tasks
	vector<ctx_task*>* get_children();
	// Get a pointer to self
	ctx_task* get_self();

	ctx_task() {
		sim = ctx_simulator::get();
	}
	~ctx_task() {}
};
// Macros for task declaration
#define CS_TASK(NAME)\
typedef struct NAME;\
static ctx_task_base* _get_##NAME##_inst() {\
	return NAME::call_from(this);\
}\
struct NAME : public ctx_task<this_type> {

#define CS_RETURN(T) T rv;

#define CS_BEGIN_BODY\
ctx_switch* next() {\
switch(_line) { case 0:;

#define CS_END_BODY\
} _line=0; \
ctx_switch_stop* _cs=new ctx_switch_stop();\
return (ctx_switch*)_cs; }\
}

#define CS_RUN_TASK(COMP,TASK)\
CS_JOIN((COMP)->this_type::_get_##TASK##_inst())

struct ctx_event {
	ctx_simulator* sim;
	int listeners;
	string name;
	bool state;

	virtual void consume();
	virtual void emit();
	virtual void register_component(ctx_component*);

};

struct ctx_component {
	ctx_simulator* sim;
	ctx_component* parent;
	string name;
	// TODO : Add a list with callbacks for events
	
};
#define CTX_COMP_SETUP

struct ctx_object {

};
#define CTX_OBJ_SETUP

struct ctx_factory {

};


#endif

