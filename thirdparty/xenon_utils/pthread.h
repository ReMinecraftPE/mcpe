#pragma once

#include <sys/time.h>
#include <sys/_pthreadtypes.h>

#define PTHREAD_MUTEX_INITIALIZER (0xFFFFFFFF)

#define PTHREAD_COND_INITIALIZER (0xFFFFFFFF)

// Linking will break without this
extern "C" {

int   pthread_cond_broadcast(pthread_cond_t *);
int   pthread_cond_destroy(pthread_cond_t *);
int   pthread_cond_init(pthread_cond_t *, const pthread_condattr_t *);
int   pthread_cond_signal(pthread_cond_t *);
int   pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *);
int	  pthread_cond_timedwait(pthread_cond_t *, pthread_mutex_t *, const struct timespec *);

int	  pthread_condattr_init(pthread_condattr_t *);
int	  pthread_condattr_destroy(pthread_condattr_t *);

int   pthread_create(pthread_t *, const pthread_attr_t *,
          void *(*)(void *), void *);
void  pthread_exit(void *);
int   pthread_join(pthread_t, void **);

int   pthread_mutex_destroy(pthread_mutex_t *);
int   pthread_mutex_init(pthread_mutex_t *, const pthread_mutexattr_t *);
int   pthread_mutex_lock(pthread_mutex_t *);
int   pthread_mutex_trylock(pthread_mutex_t *);
int   pthread_mutex_unlock(pthread_mutex_t *);

int	  pthread_mutexattr_init(pthread_mutexattr_t *);
int	  pthread_mutexattr_destroy(pthread_mutexattr_t *);

int	  pthread_attr_init(pthread_attr_t *);
int	  pthread_attr_destroy(pthread_attr_t *);
int	  pthread_attr_setstack(pthread_attr_t *, void *, size_t);
int	  pthread_attr_getstack(const pthread_attr_t *, void **, size_t *);
int	  pthread_attr_getstacksize(const pthread_attr_t *, size_t *);
int	  pthread_attr_setstacksize(pthread_attr_t *, size_t);
int	  pthread_attr_getstackaddr(const pthread_attr_t *, void **);
int	  pthread_attr_setstackaddr(pthread_attr_t  *, void *);
int	  pthread_attr_getdetachstate(const pthread_attr_t *, int *);
int   pthread_attr_setdetachstate(pthread_attr_t *, int);
int	  pthread_attr_getguardsize(const pthread_attr_t *, size_t *);
int	  pthread_attr_setguardsize(pthread_attr_t *, size_t);

int	  pthread_attr_setschedparam(pthread_attr_t *, const struct sched_param *);
int	  pthread_attr_getschedpara(const pthread_attr_t *, struct sched_param *);

}