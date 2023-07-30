#include "CThread.hpp"
#include "Utils.hpp"

#ifdef _WIN32
#include <Windows.h> // for Sleep()
#else
#include <unistd.h>
#endif

void CThread::sleep(uint32_t ms)
{
#ifdef _WIN32
	Sleep(ms);
#else
	usleep(1000 * ms);
#endif
}

CThread::CThread(CThreadFunction func, void* parm)
{
	m_func = func;
	
#ifdef USE_CPP11_THREADS
	std::thread thr(func, parm);
	m_thrd.swap(thr);
#else
	pthread_attr_init(&m_thrd_attr);
	pthread_attr_setdetachstate(&m_thrd_attr, 1);
	pthread_create(&m_thrd, &m_thrd_attr, m_func, parm);
#endif
}

CThread::~CThread()
{
#ifdef USE_CPP11_THREADS
	m_thrd.join();
#else
	pthread_join(m_thrd, 0);
	pthread_attr_destroy(&m_thrd_attr);
#endif
}
