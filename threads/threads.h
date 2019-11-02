#pragma once

//Just a frame work for now
//Need to start working on intrupts for C use

typedef struct THREAD {
	interrupt_data_s idata;
	bool is_running;
	double sleep_millis_remaining;
	void (*func)();
} THREAD;

THREAD* thread_create(void (*func)());
void thread_queue(THREAD* thread);
void thread_init();
void thread_enable();
void thread_yield();
void thread_kill_current();
