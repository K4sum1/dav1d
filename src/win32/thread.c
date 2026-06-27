/*
 * Copyright © 2018-2021, VideoLAN and dav1d authors
 * Copyright © 2018, Two Orioles, LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#if defined(_WIN32)

#include <process.h>
#include <stdlib.h>
#include <windows.h>

#include "common/attributes.h"

#include "src/thread.h"

static HRESULT (WINAPI *set_thread_description)(HANDLE, PCWSTR);

COLD void dav1d_init_thread(void) {
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    HANDLE kernel32 = GetModuleHandleW(L"kernel32.dll");
    if (kernel32)
        set_thread_description =
            (void*)GetProcAddress(kernel32, "SetThreadDescription");
#endif
}

#undef dav1d_set_thread_name
COLD void dav1d_set_thread_name(const wchar_t *const name) {
    if (set_thread_description) /* Only available since Windows 10 1607 */
        set_thread_description(GetCurrentThread(), name);
}

static COLD unsigned __stdcall thread_entrypoint(void *const data) {
    pthread_t *const t = data;
    t->arg = t->func(t->arg);
    return 0;
}

COLD int dav1d_pthread_create(pthread_t *const thread,
                              const pthread_attr_t *const attr,
                              void *(*const func)(void*), void *const arg)
{
    const unsigned stack_size = attr ? attr->stack_size : 0;
    thread->func = func;
    thread->arg = arg;
    thread->h = (HANDLE)_beginthreadex(NULL, stack_size, thread_entrypoint, thread,
                                       STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
    return !thread->h;
}

COLD int dav1d_pthread_join(pthread_t *const thread, void **const res) {
    if (WaitForSingleObject(thread->h, INFINITE))
        return 1;

    if (res)
        *res = thread->arg;

    return !CloseHandle(thread->h);
}

COLD int dav1d_pthread_once(pthread_once_t *const once_control,
                            void (*const init_routine)(void))
{
  LONG state;

  /*
    Do "dirty" read to find out if initialization is already done, to
    save an interlocked operation in common case. Memory barriers are ensured by
    Visual C++ volatile implementation.
  */
  if (*once_control == MY_PTHREAD_ONCE_DONE)
    return 0;

  state= InterlockedCompareExchange(once_control, MY_PTHREAD_ONCE_INPROGRESS,
                                        MY_PTHREAD_ONCE_INIT);

  switch(state)
  {
  case MY_PTHREAD_ONCE_INIT:
    /* This is initializer thread */
    (*init_routine)();
    *once_control= MY_PTHREAD_ONCE_DONE;
    break;

  case MY_PTHREAD_ONCE_INPROGRESS:
    /* init_routine in progress. Wait for its completion */
    while(*once_control == MY_PTHREAD_ONCE_INPROGRESS)
    {
      Sleep(1);
    }
    break;
  case MY_PTHREAD_ONCE_DONE:
    /* Nothing to do */
    break;
  }
  return 0;
}

void
InitializeXPConditionVariable(pthread_cond_t *cv)
{
	cv->waiters_count = 0;
	InitializeCriticalSection(&(cv->waiters_count_lock));
	cv->events_[C_SIGNAL] = CreateEvent (NULL, FALSE, FALSE, NULL);
	cv->events_[C_BROADCAST] = CreateEvent (NULL, TRUE, FALSE, NULL);
}

void
DeleteXPConditionVariable(pthread_cond_t *cv)
{
	CloseHandle(cv->events_[C_BROADCAST]);
	CloseHandle(cv->events_[C_SIGNAL]);
	DeleteCriticalSection(&(cv->waiters_count_lock));
}

int
SleepXPConditionVariable(pthread_cond_t *cv, pthread_mutex_t *mtx)
{
	int result, last_waiter;

	EnterCriticalSection(&cv->waiters_count_lock);
	cv->waiters_count++;
	LeaveCriticalSection(&cv->waiters_count_lock);
	LeaveCriticalSection (mtx);
	result = WaitForMultipleObjects(2, cv->events_, FALSE, INFINITE);
	if (result==-1) {
		result = GetLastError();
	}
	EnterCriticalSection(&cv->waiters_count_lock);
	cv->waiters_count--;
	last_waiter = result == (C_SIGNAL + C_BROADCAST && (cv->waiters_count == 0));
	LeaveCriticalSection(&cv->waiters_count_lock);
	if (last_waiter)
		ResetEvent(cv->events_[C_BROADCAST]);
	EnterCriticalSection (mtx);
	return result;
}

void
WakeXPConditionVariable(pthread_cond_t *cv)
{
	int have_waiters;
	EnterCriticalSection(&cv->waiters_count_lock);
	have_waiters = cv->waiters_count > 0;
	LeaveCriticalSection(&cv->waiters_count_lock);
	if (have_waiters)
		SetEvent(cv->events_[C_SIGNAL]);
}

void
WakeAllXPConditionVariable(pthread_cond_t *cv)
{
	int have_waiters;
	EnterCriticalSection(&cv->waiters_count_lock);
	have_waiters = cv->waiters_count > 0;
	LeaveCriticalSection(&cv->waiters_count_lock);
	if (have_waiters)
		SetEvent (cv->events_[C_BROADCAST]);
}

#endif
