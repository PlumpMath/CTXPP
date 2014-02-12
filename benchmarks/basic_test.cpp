#include "ctxpp.h"
#include <random>
#include <fstream>
#include <time.h>

#define MAX_NOF_TASKS 100
#define NOF_CS_VALS 4

struct BasicTest: public ctx_component { 
	
	std::default_random_engine generator;
	std::uniform_int_distribution<int> dist;
	int nof_cs_values[NOF_CS_VALS];
	
	BasicTest(const char* name, ctx_component *p):
	ctx_component(name,p),
	generator(),
	dist(1,100)
	{
		nof_cs_values[0] = 10;
		nof_cs_values[1] = 20;
		nof_cs_values[2] = 30;
		nof_cs_values[3] = 500;
	}
	
	CTX_COMP_SETUP(BasicTest)
	
	// Template task 
	CS_TASK(task_tpl)
		int nof_cs;
	private:
		int i;
		int delay;		
	CS_BEGIN_BODY
		for(i=0;i<nof_cs;i++) {
			delay = ctx->dist(ctx->generator);
			CS_DELAY(delay);
		}
	CS_END_BODY
	
	// Main task
	CS_TASK(main_task)
		ctx_task_base* t;
		std::vector<ctx_task_base*>* l;
		int i,j,k;
		clock_t t0;
		double dt;
		std::ofstream f;
		std::string fname;
	CS_BEGIN_BODY
		f.open("result_ctxpp");
		for(k=0;k<NOF_CS_VALS;k++) {
			//fname = "result_"+std::to_string(ctx->nof_cs_values[k]);
			//f.open(fname.c_str());
			for(i=1;i<=MAX_NOF_TASKS;i++) {
				l = new std::vector<ctx_task_base*>();
				std::cout<<"Running "<<i<<" tasks with "<<ctx->nof_cs_values[k]<<" context switches..."<<std::endl;
				for(j=0;j<i;j++) {
					t=task_tpl::call_from(ctx);
					((task_tpl*)t)->nof_cs = ctx->nof_cs_values[k];
					l->push_back(t);
				}
				t0=clock();
				CS_ALL_OF(l);
				dt = ((double)(clock() - t0))/CLOCKS_PER_SEC;
				f<<ctx->nof_cs_values[k]<<" "<<i<<" "<<dt<<std::endl;
			}
		}
		f.close();
	CS_END_BODY
	
	void run_phase() {
		START_TASK(this, main_task);
	}
};

int main() {
	ctx_kernel* ker = ctx_kernel::get();
	BasicTest* top = new BasicTest("top",NULL);
	ker->run(top,-1,-1);
	
	return 0;
}
