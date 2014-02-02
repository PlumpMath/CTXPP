#include "ctxpp.h"
#include <stdio.h>
#include <cassert>

struct YieldText: ctx_component {
	YieldText(const char* name, ctx_component* parent):
	ctx_component(name,parent)
	{}

	CTX_COMP_SETUP(YieldText)

	CS_TASK(try_yield)
		int id;
	CS_BEGIN_BODY
		std::cout<<"Hello 1 from task "<<id<<std::endl;
		CS_YIELD;
		std::cout<<"Hello 2 from task "<<id<<std::endl;
		CS_YIELD;
		std::cout<<"Hello 3 from task "<<id<<std::endl;
	CS_END_BODY

	CS_TASK(main_task)
		std::vector<ctx_task_base*>* tlist;
		ctx_task_base* task1; 
		ctx_task_base* task2;
		ctx_task_base* task3;
	CS_BEGIN_BODY
		tlist = new std::vector<ctx_task_base*>();
		task1 = try_yield::call_from(ctx);
		task2 = try_yield::call_from(ctx);
		task3 = try_yield::call_from(ctx);
		((try_yield*)task1)->id=1;tlist->push_back(task1);
		((try_yield*)task2)->id=2;tlist->push_back(task2);
		((try_yield*)task3)->id=3;tlist->push_back(task3);

		CS_ALL_OF(tlist);
	CS_END_BODY
	
	void run_phase() {
		std::cout<<"Doing the run_phase..."<<std::endl;
		START_TASK(this, main_task);
	}
};

int main() {
	std::cout<< "Creating top..."<<std::endl;
	YieldText* top = new YieldText("top", NULL);
	std::cout<< "Running top..."<<std::endl;
	ctx_kernel::get()->run((ctx_component*)top,-1,-1);
	std::cout<<"Time="<<ctx_kernel::get()->time<<std::endl;
	return 0;
}