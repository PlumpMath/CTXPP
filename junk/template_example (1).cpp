#include "stdio.h"

struct ctx_task_base {
	virtual void body() { }
};

template<typename SELF_T, typename TASK_T>
struct ctx_task:ctx_task_base {
	static TASK_T* call_from(SELF_T* ctx) {
		TASK_T* result = new TASK_T();
		result->ctx = ctx;
		return result;
	}
};

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
	TASK_END
};


int main() {
	my_comp* comp = new my_comp();
	//comp->get_my_task_inst()->body();
	CALL_TASK(comp,my_task);
	CALL_TASK(comp,other_task);
	delete comp;
}

