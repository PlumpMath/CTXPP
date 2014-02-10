#include "ctxpp.h"
#include <stdio.h>
#include <cassert>

struct DelayTest: ctx_component {
	DelayTest(const char* name, ctx_component* parent):
	ctx_component(name,parent)
	{}

	CTX_COMP_SETUP(DelayTest)
	
	// Try some delays
	CS_TASK(try_delay)
		int id;
	CS_BEGIN_BODY
		std::cout<<ker->time<<": "<<"Hello 1 from task "<<id<<std::endl;
		CS_DELAY(10);
		std::cout<<ker->time<<": "<<"Hello 2 from task "<<id<<std::endl;
		CS_DELAY(20);
		std::cout<<ker->time<<": "<<"Hello 3 from task "<<id<<std::endl;
	CS_END_BODY
	
	// Dummy task
	CS_TASK(dummy_task)
	CS_BEGIN_BODY
		std::cout<<ker->time<<": "<<"Dummy says hello!"<<std::endl;
		CS_DELAY(40);
	CS_END_BODY
	
	// The main task
	CS_TASK(main_task)
		std::vector<ctx_task_base*>* tlist;
		ctx_task_base* task1; 
		ctx_task_base* task2;
		ctx_task_base* task3;
		ctx_task_base* dummy_task_inst;
	CS_BEGIN_BODY
		tlist = new std::vector<ctx_task_base*>();
		task1 = try_delay::call_from(ctx);
		task2 = try_delay::call_from(ctx);
		task3 = try_delay::call_from(ctx);
		((try_delay*)task1)->id=1;tlist->push_back(task1);
		((try_delay*)task2)->id=2;tlist->push_back(task2);
		((try_delay*)task3)->id=3;tlist->push_back(task3);
		
		// Run 3 instances of try_delay in parallel 
		CS_ALL_OF(tlist);
		// Wait some time
		std::cout<<ker->time<<": "<<"Waiting drain time ..."<<std::endl;
		CS_DELAY(100);
		std::cout<<ker->time<<": "<<"Done waiting drain time ..."<<std::endl;
		// Run one instance of dummy_task and wait for it to finish
		std::cout<<ker->time<<": "<<"Calling dummy task..."<<std::endl;
		CS_CALL_TASK(ctx, dummy_task);
		std::cout<<ker->time<<": "<<"Done calling dummy task..."<<std::endl;
		
		// Calling another instance of dummy_task in a different way
		std::cout<<ker->time<<": "<<"Starting dummy task..."<<std::endl;
		dummy_task_inst = dummy_task::call_from(ctx);
		dummy_task_inst->start();
		
		std::cout<<ker->time<<": "<<"Waiting 10 seconds..."<<std::endl;
		CS_DELAY(10);
		std::cout<<ker->time<<": "<<"Done waiting 10 seconds..."<<std::endl;
		
		std::cout<<ker->time<<": "<<"Joining dummy_task..."<<std::endl;
		CS_JOIN(dummy_task_inst);
		std::cout<<ker->time<<": "<<"Done joining dummy_task..."<<std::endl;
		
		std::cout<<ker->time<<": "<<"I'm done!!!"<<std::endl;
	CS_END_BODY
	
	void run_phase() {
		std::cout<<ker->time<<": "<<"Doing the run_phase..."<<std::endl;
		START_TASK(this, main_task);
	}
};

int main() {
	std::cout<< "Creating top..."<<std::endl;
	DelayTest* top = new DelayTest("top", NULL);
	std::cout<< "Running top..."<<std::endl;
	ctx_kernel::get()->run((ctx_component*)top,-1,-1);
	std::cout<<"Time="<<ctx_kernel::get()->time<<std::endl;
	assert(ctx_kernel::get()->time == 210);
	return 0;
}