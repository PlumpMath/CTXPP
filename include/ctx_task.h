#ifndef CTX_TASK_H
#define CTX_TASK_H

typedef enum {
	CTX_TASK_RUNNING,
	CTX_TASK_PENDING,
	CTX_TASK_FINISHED
} ctx_task_state;

struct ctx_task_base {
	
	static int task_id_counter;
	
	int task_id;
	
	ctx_task_base* parent;
	ctx_component* comp;	
	ctx_task_state state;
	ctx_kernel* ker;
	std::string name;
	bool is_registered;
	ctx_switch* cs;
	
	double time_to_wait;
	bool unique_ttw;
	ctx_event* ev_to_wait;
	
	// Iterators for kernel list management
	std::list<ctx_task_base*>::iterator pending_iterator;
	std::list<ctx_task_base*>::iterator running_iterator;
	std::list<ctx_task_base*>::iterator tct_iterator;
	
	ctx_task_base();
	~ctx_task_base();
	// Jumps to next context switch
	virtual ctx_switch* next();
	// Kill task instance
	void kill();
	// Kill only the children tasks
	void kill_children();
	// Get a list with all the child tasks
	void get_children(std::vector<ctx_task_base*>&);
	// Get a pointer to self
	ctx_task_base* self();
	// Start the task
	void start();
};

template <typename COMP_T, typename TASK_T>
struct ctx_task:ctx_task_base {
	
	typedef COMP_T comp_type;
	typedef TASK_T task_type;
	
	int _line;
	
	COMP_T* ctx;
	
	// Constructor
	ctx_task():_line(-1) {
		
	}
	// Jumps to next context switch
	virtual ctx_switch* next() { return NULL; }
	// Get an instance of this task
	static TASK_T* call_from(COMP_T* ctx) {
		TASK_T* result = new TASK_T();
		result->ctx=ctx;
		result->comp=ctx;
		result->cs=NULL;
		return result;
	}
	~ctx_task() {}
};

// Macros for task declaration
#define CS_TASK(NAME)\
struct NAME;\
NAME* _get_##NAME##_inst() {\
	return ctx_task<this_type,NAME>::call_from(this);\
}\
struct NAME : public ctx_task<this_type,NAME> {\
	NAME() {\
		_line=-1;\
		state=CTX_TASK_RUNNING;\
		std::stringstream ss;\
		task_id=task_id_counter;\
		ss<<task_id;\
		name = #NAME "_"+ss.str();\
		task_id_counter++;\
		ker = ctx_kernel::get();\
		parent = ker->get_current_task();\
		is_registered=false;\
	}
	

#define CS_RETURN(T) T rv;

#define CS_BEGIN_BODY virtual ctx_switch* next() {\
if(_line < 0) { _line=0;}\
switch(_line) { case 0:;

#define CS_END_BODY } _line=0;\
ctx_switch_stop* _cs=new ctx_switch_stop();\
return (ctx_switch*)_cs; }\
};

#define CS_CALL_TASK(COMP,TASK)\
CS_JOIN((COMP)->_get_##TASK##_inst())

#define START_TASK(COMP,TASK)\
(COMP)->_get_##TASK##_inst()->start()

// Macros for subtasks TODO : remove parent argument
#define CS_SUBTASK(PARENT,NAME)\
struct NAME : public ctx_task_base {\
	this_type* ctx;\
	PARENT* parent_ctx;\
	static NAME* call_from(this_type* ctx, PARENT* p) {\
		std::stringstream ss;\
		NAME* result = new NAME();\
		result->ctx=ctx;\
		result->comp=ctx;\
		result->parent_ctx = p;\
		result->task_id=task_id_counter;\
		result->ker=ctx_kernel::get();\
		result->parent = result->ker->get_current_task();\
		ss<<task_id;\
		result->name="#NAME_"+ss.str();\
		task_id_counter++;\
		return result;\
	}

#define CS_CALL_SUBTASK(SUBTASK)\
CS_JOIN(SUBTASK::call_from(ctx,this))

#define START_SUBTASK(SUBTASK)\
SUBTASK::call_from(ctx,this)->start()

#endif
