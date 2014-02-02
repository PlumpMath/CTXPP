#include "ctxpp.h"
#include <stdio.h>
#include <cassert>

struct EventTest: ctx_component {
	
	EventTest(const char* name, ctx_component* parent):
	ctx_component(name,parent)
	{}

	CTX_COMP_SETUP(EventTest)
	
	ctx_event* ev;
	ctx_task_base* emitter;
	
	// The build phase
	virtual void build_phase() {
		ev = new ctx_event("ev");
	}
	
	// Task that emits the event
	CS_TASK(emitter_task)
		int emit_counter;
	CS_BEGIN_BODY
		emit_counter=0;
		std::cout<<"Starting emitter_task..."<<std::endl;
		ctx->emitter = ker->get_current_task();
		while(true) {
			std::cout<<"Emitting event no. "<<emit_counter<<std::endl;
			CS_DELAY(10);
			ctx->ev->emit();
			emit_counter++;
		}
	CS_END_BODY
	
	// Task that waits for the event
	CS_TASK(receiver_task)
		int i;
	CS_BEGIN_BODY
		std::cout<<"Starting receiver_task..."<<std::endl;
		for(i=0;i<5;i++) {
			std::cout<<"Waiting event no. "<<i<<std::endl;
			CS_WAIT(ctx->ev);
			std::cout<<"Received event no. "<<i<<std::endl;
		}
		std::cout<<"Trying to kill emitter task."<<std::endl;
		if(ctx->emitter!=NULL) {
			std::cout<<"Killing emitter task."<<std::endl;
			ctx->emitter->kill();
			std::cout<<"Killed emitter task."<<std::endl;
		} else {
			std::cout<<"Pointer to emitter task was NULL. Could not kill task..."<<std::endl;
		}
	CS_END_BODY
	
	// The main task
	CS_TASK(main_task)
		std::vector<ctx_task_base*>* l;
		ctx_task_base* e;
		ctx_task_base* r;
	CS_BEGIN_BODY
		std::cout<<"Starting main_taks..."<<std::endl;
		l = new std::vector<ctx_task_base*>();
		e=emitter_task::call_from(ctx); l->push_back(e);
		r=receiver_task::call_from(ctx); l->push_back(r);
		std::cout<<"Waiting for tasks to end..."<<std::endl;
		CS_ALL_OF(l);		
		std::cout<<"Tasks ended."<<std::endl;
	CS_END_BODY
	
	// The run phase
	void run_phase() {
		std::cout<<"Doing the run_phase..."<<std::endl;
		START_TASK(this, main_task);
	}
};

int main() {
	EventTest* top = new EventTest("top", NULL);
	ctx_kernel *ker = ctx_kernel::get();
	// Run simulation
	ker->run((ctx_component*)top,-1,-1);
	// Check time
	assert(ker->time==50);
	delete ker;

	return 0;
}