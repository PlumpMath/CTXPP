
#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc.h"
#include <fstream>
#include <random>
#include <time.h>
#include <papi.h>

using namespace sc_core;


#define MAX_NOF_TASKS 100
#define NOF_CS_VALS 4

struct BasicTest: public sc_module {
	SC_HAS_PROCESS(BasicTest);
	
	std::default_random_engine generator;
	std::uniform_int_distribution<int> dist;
	int nof_cs_values[NOF_CS_VALS];
	
	void task_tpl(int nof_cs) {
		int delay;
		for(int i=0;i<nof_cs;i++) {
			delay = dist(generator);
			wait(delay,SC_NS);
		}
	}
	
	void main_task() {
		std::ofstream f;
		long long t0;
		long long dt;
		sc_core::sc_join join;
		f.open("result_sc");
		for(int k=0;k<NOF_CS_VALS;k++) {
			for(int i=1;i<=MAX_NOF_TASKS;i++) {
				std::cout<<"Running "<<i<<" tasks with "<<nof_cs_values[k]<<" context switches..."<<std::endl;
				// see sc_join.h line 97
				t0 = PAPI_get_real_usec();
				for(int j=0;j<i;j++) {
					sc_process_handle h;
					h=sc_spawn(sc_bind(&BasicTest::task_tpl,this,nof_cs_values[k]));
					join.add_process(h);					
				}
				join.wait();
				//dt = ((double)(clock()-t0))/CLOCKS_PER_SEC;
				dt = PAPI_get_real_usec()-t0;
				f<<nof_cs_values[k]<<" "<<i<<" "<<dt<<std::endl;
			}
			
		}
		f.close();
	}
	
	BasicTest(sc_module_name nm):
	sc_module(nm),
	generator(),
	dist(1,100)
	{
		nof_cs_values[0] = 10;
		nof_cs_values[1] = 20;
		nof_cs_values[2] = 30;
		nof_cs_values[3] = 500;
		SC_THREAD(main_task);
	}
	
};


int sc_main (int argc, char* argv[]) {
	if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
		std::cerr<<"PAPI error"<<std::endl;
		exit(1);
	}
	BasicTest top("top");
	sc_start();
}

