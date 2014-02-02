#include "stdio.h"

struct ctx_task_base {
	virtual void body() { }
};

template<typename CTX_T, typename TASK_T>
struct ctx_task:ctx_task_base {
	typedef CTX_T ctx_type;
	typedef TASK_T task_type;

	static TASK_T* call_from(CTX_T* ctx) {
		TASK_T* result = new TASK_T();
		result->ctx = ctx;
		return result;
	}

};

// Task macros
#define TASK_BEGIN(NAME)\
struct NAME;\
NAME* get_##NAME##_inst() {\
	return ctx_task<this_type,NAME>::call_from(this);\
}\
struct NAME:ctx_task<this_type,NAME> {\
	this_type* ctx;\
	virtual void body() {

#define TASK_END } };

#define CALL_TASK(COMP,TASK)\
COMP->get_##TASK##_inst()->body()

// Subtask macros : TODO : try to eliminate the parent argument, it should
// work
#define SUBTASK_BEGIN(PARENT,NAME)\
struct NAME:ctx_task_base {\
	this_type* ctx;\
	PARENT* parent;\
	static NAME* call_from(this_type* ctx, PARENT* p) {\
		NAME* result = new NAME();\
		result->ctx=ctx;\
		result->parent = p;\
		return result;\
	}\
	virtual void body() {

#define SUBTASK_END } };

#define CALL_SUBTASK(STASK)\
STASK::call_from(ctx,this)->body()

struct my_comp {
	typedef my_comp this_type;
	// struct my_task;
	// my_task* get_my_task_inst() {
	// 	return ctx_task<my_comp,my_task>::call_from(this);
	// }
	// struct my_task:ctx_task<my_comp,my_task> {
	// 	my_comp* ctx;
	// 	virtual void body() {
	// 		printf("Hello!\n");
	// 	}
	// };
	TASK_BEGIN(my_task)
		printf("Hello!\n");
	TASK_END

	TASK_BEGIN(other_task)
		CALL_TASK(ctx,my_task);// NB : use ctx instead of this
		printf("Hello from other_task!\n");
		
		SUBTASK_BEGIN(other_task,sub_task)
			printf("Hello from sub_task!\n");
		SUBTASK_END

		CALL_SUBTASK(sub_task);

	TASK_END
};

struct new_comp: public my_comp {
	TASK_BEGIN(my_task)
		printf("calling new_comp::my_task\n");
		my_comp::my_task::call_from(ctx)->body();
		printf("Hello from new_comp::my_task!\n");
	TASK_END

};


int main() {
	my_comp* comp = new my_comp();
	//comp->get_my_task_inst()->body();
	CALL_TASK(comp,my_task);
	CALL_TASK(comp,other_task);
	delete comp;
	new_comp* ncomp = new new_comp();
	CALL_TASK(ncomp,my_task);
	CALL_TASK(ncomp,other_task);
}

