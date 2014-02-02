#include "ctxpp.h"
#include <stdio.h>
#include <cassert>

struct FirstOfTest: ctx_component {
	FirstOfTest(const char* name, ctx_component* parent):
	ctx_component(name,parent)
	{}

	CTX_COMP_SETUP(FirstOfTest)
	
	// Task template
	CS_TASK(task_tpl)
		int dly;
	CS_BEGIN_BODY
		ctx->message(name,"Consuming time",CTX_NONE);
		CS_DELAY(dly);
		ctx->message(name,"Done consuming time",CTX_NONE);
	CS_END_BODY
	
	// Main task
	CS_TASK(main_task)
		ctx_task_base* t;
		std::vector<ctx_task_base*>* l;
	CS_BEGIN_BODY
		l = new std::vector<ctx_task_base*>();
		
		t = task_tpl::call_from(ctx);((task_tpl*)t)->dly=10;l->push_back(t);
		t = task_tpl::call_from(ctx);((task_tpl*)t)->dly=20;l->push_back(t);
		t = task_tpl::call_from(ctx);((task_tpl*)t)->dly=30;l->push_back(t);
		
		ctx->message(name,"Starting the tasks...",CTX_NONE);
		CS_ANY_OF(l);
		ctx->message(name,"Gone on with my life...",CTX_NONE);
	CS_END_BODY
		
	void run_phase() {
		START_TASK(this, main_task);
	}
};

int main() {
	ctx_kernel* ker = ctx_kernel::get();
	FirstOfTest* top = new FirstOfTest("top", NULL);

	ker->run((ctx_component*)top,-1,-1);
	
	assert(ker->time==30);
	

	return 0;
}