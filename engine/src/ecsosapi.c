#include "chirp/ecsosapi.h"
#include "chirp/logcategory.h"

#include "flecs/os_api.h"
#include "flecs/private/api_defines.h"

#include <SDL3/SDL_atomic.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_mutex.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_thread.h>
#include <SDL3/SDL_timer.h>

#include <stddef.h>

static void *malloc_(const ecs_size_t size)
{
	return SDL_malloc((size_t) size);
}

static void *realloc_(void *ptr, const ecs_size_t size)
{
	return SDL_realloc(ptr, (size_t) size);
}

static void *calloc_(const ecs_size_t size)
{
	return SDL_calloc(1, (size_t) size);
}

static char *strdup_(const char *str)
{
	if (str == nullptr)
	{
		return nullptr;
	}

	return SDL_strdup(str);
}

static void get_time(ecs_time_t *time)
{
	const Uint64 now = SDL_GetTicksNS();
	const Uint64 sec = now / SDL_NS_PER_SECOND;

	time->sec = (Uint32) sec;
	time->nanosec = (Uint32) (now - (sec * SDL_NS_PER_SECOND));
}

static void log_(const Sint32 level, [[maybe_unused]] const char *file,
	[[maybe_unused]] const Sint32 line, const char *msg)
{
	SDL_LogPriority priority;
	switch (level)
	{
		case 0:
			priority = SDL_LOG_PRIORITY_INFO;
			break;

		case -2:
			priority = SDL_LOG_PRIORITY_WARN;
			break;

		case -3:
			priority = SDL_LOG_PRIORITY_ERROR;
			break;

		case -4:
			priority = SDL_LOG_PRIORITY_CRITICAL;
			break;

		default:
			priority = SDL_LOG_PRIORITY_DEBUG;
			break;
	}

	// flecs logging can be very verbose, only log warnings and errors
	if (priority < SDL_LOG_PRIORITY_WARN)
	{
		return;
	}

	SDL_LogMessage(LOG_CATEGORY_ECS, priority, "%s", msg);
}

static ecs_os_thread_t thread_new(const ecs_os_thread_callback_t callback, void *param)
{
	return (ecs_os_thread_t) SDL_CreateThread((SDL_ThreadFunction) callback, nullptr, param);
}

static void *thread_join(const ecs_os_thread_t thread)
{
	int status;
	SDL_WaitThread((SDL_Thread*) thread, &status);
	return (void*) (uintptr_t) status;
}

static ecs_os_thread_id_t thread_self()
{
	return SDL_GetCurrentThreadID();
}

static Sint32 ainc(Sint32 *count)
{
	return SDL_AtomicIncRef((SDL_AtomicInt*) count) + 1;
}

static Sint32 adec(Sint32 *count)
{
	return SDL_AtomicDecRef((SDL_AtomicInt*) count) - 1;
}

// SDL currently doesn't support Sint64 atomics, so we fall back to full mutexes
// (we could also use C11 atomics, which could cause unexpected platform differences)

static SDL_Mutex *la_mutex = nullptr;

static void la_lock()
{
	if (la_mutex == nullptr)
	{
		la_mutex = SDL_CreateMutex();
		if (la_mutex == nullptr)
		{
			SDL_LogError(LOG_CATEGORY_ECS, "Failed to create mutex: %s", SDL_GetError());
			return;
		}
	}

	SDL_LockMutex(la_mutex);
}

static void la_unlock()
{
	SDL_UnlockMutex(la_mutex);
}

static Sint64 lainc(Sint64 *count)
{
	la_lock();
	const Sint64 value = *count += 1;
	la_unlock();
	return value;
}

static Sint64 ladec(Sint64 *count)
{
	la_lock();
	const Sint64 value = *count -= 1;
	la_unlock();
	return value;
}

static ecs_os_mutex_t mutex_new()
{
	return (ecs_os_mutex_t) SDL_CreateMutex();
}

static void mutex_free(const ecs_os_mutex_t mutex)
{
	SDL_DestroyMutex((SDL_Mutex*) mutex);
}

static void mutex_lock(const ecs_os_mutex_t mutex)
{
	SDL_LockMutex((SDL_Mutex*) mutex);
}

static void mutex_unlock(const ecs_os_mutex_t mutex)
{
	SDL_UnlockMutex((SDL_Mutex*) mutex);
}

static ecs_os_cond_t cond_new()
{
	return (ecs_os_cond_t) SDL_CreateCondition();
}

static void cond_free(const ecs_os_cond_t cond)
{
	SDL_DestroyCondition((SDL_Condition*) cond);
}

static void cond_signal(const ecs_os_cond_t cond)
{
	SDL_SignalCondition((SDL_Condition*) cond);
}

static void cond_broadcast(const ecs_os_cond_t cond)
{
	SDL_BroadcastCondition((SDL_Condition*) cond);
}

static void cond_wait(const ecs_os_cond_t cond, const ecs_os_mutex_t mutex)
{
	SDL_WaitCondition((SDL_Condition*) cond, (SDL_Mutex*) mutex);
}

static void sleep(const Sint32 sec, const Sint32 nano_sec)
{
	SDL_DelayNS((sec * SDL_NS_PER_SECOND) + nano_sec);
}

ecs_os_api_t ecs_os_api_create()
{
	return (ecs_os_api_t){
		// API init
		.init_ = nullptr,
		.fini_ = nullptr,

		// Memory management
		.malloc_ = malloc_,
		.realloc_ = realloc_,
		.calloc_ = calloc_,
		.free_ = SDL_free,

		// Strings
		.strdup_ = strdup_,

		// Threads
		.thread_new_ = thread_new,
		.thread_join_ = thread_join,
		.thread_self_ = thread_self,

		// Tasks
		.task_new_ = thread_new,
		.task_join_ = thread_join,

		// Atomic increment/decrement
		.ainc_ = ainc,
		.adec_ = adec,
		.lainc_ = lainc,
		.ladec_ = ladec,

		// Mutex
		.mutex_new_ = mutex_new,
		.mutex_free_ = mutex_free,
		.mutex_lock_ = mutex_lock,
		.mutex_unlock_ = mutex_unlock,

		// Condition variable
		.cond_new_ = cond_new,
		.cond_free_ = cond_free,
		.cond_signal_ = cond_signal,
		.cond_broadcast_ = cond_broadcast,
		.cond_wait_ = cond_wait,

		// Time
		.sleep_ = sleep,
		.now_ = SDL_GetTicksNS,
		.get_time_ = get_time,

		// Logging
		.log_ = log_,

		// Application termination
		.abort_ = abort,

		// Dynamic library loading
		.dlopen_ = nullptr,
		.dlproc_ = nullptr,
		.dlclose_ = nullptr,

		// Module translation
		.module_to_dl_ = nullptr,
		.module_to_etc_ = nullptr,

		// File I/O
		.fopen_ = nullptr,
		.fclose_ = nullptr,
		.fread_ = nullptr,
		.fwrite_ = nullptr,
		.mkdir_ = nullptr,
		.rmdir_ = nullptr,
		.remove_ = nullptr,

		// Performance tracing
		.perf_trace_push_ = nullptr,
		.perf_trace_pop_ = nullptr,
	};
}
