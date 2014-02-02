#ifndef CTX_SWITCH_H
#define CTX_SWITCH_H

typedef enum {
	CTX_CS_YIELD,        // Pass control to another task(consumes a delta cycle)
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



struct ctx_switch {
	ctx_type type;
	ctx_task_base* origin;// TODO : remove if not needed
	
	ctx_switch():type(CTX_CS_YIELD) {}
	~ctx_switch() {}

};
#define CS_YIELD \
do {\
	_line=__LINE__;\
	return (ctx_switch*)(new ctx_switch());\
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
	return (ctx_switch*)(new ctx_switch_delay((D)));\
	case __LINE__:;\
} while(0)

struct ctx_switch_delay_std:ctx_switch {
	long int delay_std_val;
	double time_unit;
	double time_scale;

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
	return (ctx_switch*)(new ctx_switch_delay_std((D),(TU)));\
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
	(E)->add_listener();\
	return (ctx_switch*)(new ctx_switch_wait_ev((E)));\
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
	return (ctx_switch*)(new ctx_switch_join_task((T)));\
	case __LINE__:;\
} while(0)

struct ctx_switch_all_of:ctx_switch {
	std::vector<ctx_task_base*>* tasks;
	
	ctx_switch_all_of(std::vector<ctx_task_base*>* tlist) {
		type = CTX_CS_ALL_OF;
		tasks = tlist;
	}
	~ctx_switch_all_of() {}
};
#define CS_ALL_OF(TL)\
do {\
	_line=__LINE__;\
	return (ctx_switch*)(new ctx_switch_all_of((TL)));\
	case __LINE__:;\
} while(0)

struct ctx_switch_first_of:ctx_switch {
	std::vector<ctx_task_base*>* tasks;
	
	ctx_switch_first_of(std::vector<ctx_task_base*>* tlist) {
		type = CTX_CS_FIRST_OF;
		tasks = tlist;
	}
	~ctx_switch_first_of() {}
};
#define CS_FIRST_OF(TL)\
do {\
	_line=__LINE__;\
	return (ctx_switch*)(new ctx_switch_first_of((TL)));\
	case __LINE__:;\
} while(0)

struct ctx_switch_any_of:ctx_switch {
	std::vector<ctx_task_base*>* tasks;

	ctx_switch_any_of(std::vector<ctx_task_base*>* tlist) {
		type = CTX_CS_ANY_OF;
		tasks = tlist;
	}
	~ctx_switch_any_of() {}
};
#define CS_ANY_OF(TL)\
do {\
	_line=__LINE__;\
	return (ctx_switch*)(new ctx_switch_any_of((TL)));\
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
	return (ctx_switch*)(new ctx_switch_stop());\
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
	return (ctx_switch*)(new ctx_switch_finish());\
	case __LINE__:;\
} while(0)

struct ctx_switch_return_val:ctx_switch {
	ctx_switch_return_val() {
		type = CTX_CS_RETURN_VALUE;
	}
	~ctx_switch_return_val() {}
};
#define CS_RETURN_VAL(RV)\
do {\
	_line=__LINE__;\
	rv = (RV);\
	return (ctx_switch*)(new ctx_switch_return_val());\
	case __LINE__:;\
} while(0)

#endif
