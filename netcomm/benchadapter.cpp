#include <dlfcn.h>
#include <string.h>
#include <iostream>

#include "../utils/log/log.h"
#include "benchadapter.h"

using namespace std;

bench_so_func_t Benchadapter::benchso = {NULL};

int Benchadapter::benchAdaperLoad(const char *sofile)
{
	if(benchso.sohandle != NULL)
	{
		dlclose(benchso.sohandle);
	}

	memset(&benchso, 0, sizeof(bench_so_func_t));
	
	void * handle = dlopen(sofile, RTLD_NOW);
	if(!handle)
	{
		log_debug("[ERROR]dlopen %s failed. errno=%d", sofile, errno);
		cout << "[ERROR]dlopen failed. sofile=" << sofile << endl;
		exit(-1);
	}

	benchso.bench_handle_init = (bench_handle_init_t)dlsym(handle, "bench_handle_init");
	benchso.bench_handle_input = (bench_handle_input_t)dlsym(handle, "bench_handle_input");
	benchso.bench_handle_route = (bench_handle_route_t)dlsym(handle, "bench_handle_route");
	benchso.bench_handle_process = (bench_handle_process_t)dlsym(handle, "bench_handle_process");

	if(benchso.bench_handle_init == NULL)
	{
		log_debug("[ERROR] bench_handle_init not implement.");
		cout << "[ERROR] bench_handle_init not implement." << sofile << endl;
		exit(-1);
	}

	if(benchso.bench_handle_input == NULL)
	{
		log_debug("[ERROR] bench_handle_input not implement.");
		cout << "[ERROR] bench_handle_input not implement." << sofile << endl;
		exit(-1);
	}

	if(benchso.bench_handle_process == NULL)
	{
		log_debug("[ERROR] bench_handle_process not implement.");
		cout << "[ERROR] bench_handle_process not implement." << sofile << endl;
		exit(-1);
	}

	return 0;
}