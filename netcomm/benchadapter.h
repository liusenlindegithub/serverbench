#ifndef _BENCH_ADAPTER_H_
#define _BENCH_ADAPTER_H_

typedef int (*bench_handle_init_t)(void *, void *);
typedef int (*bench_handle_input_t)(void *, void *);
typedef int (*bench_handle_route_t)(void *, void *);
typedef int (*bench_handle_process_t)(char *, int, int, void *);

typedef struct
{
	void *sohandle;
	bench_handle_init_t bench_handle_init;
	bench_handle_input_t bench_handle_input;
	bench_handle_route_t bench_handle_route;
	bench_handle_process_t bench_handle_process;
}bench_so_func_t;

class Benchadapter
{
public:
	static int benchAdaperLoad(const char *sofile);
	static bench_so_func_t benchso;
};

#endif