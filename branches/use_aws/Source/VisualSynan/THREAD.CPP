// Thread.cpp : lightweight thread
//
// derive new class, override Run/Wait to do work/wait,
// return FALSE from Run or call Destroy to stop thread
//

#include "stdafx.h"
#include "Thread.h"

#include <process.h>


Thread::Thread()
{	
	m_stop   = 0; 
	m_handle = INVALID_HANDLE_VALUE;  
}

//virtual 
Thread::~Thread()
{	
	Destroy();   
}

BOOL Thread::Create(DWORD dwCreateFlags)
{
	//re-initialize not allowed
	ASSERT(!IsValid());

	//_beginthread returns -1 on an error, in which case errno is 
	//  set to EAGAIN if there are too many threads, or to EINVAL 
	//  if the argument is invalid or the stack size is incorrect

	//_beginthreadex returns 0 on an error, in which case errno 
	//  and doserrno are set

	unsigned threadid;
	m_handle = (HANDLE)_beginthreadex(NULL, 0, &ThreadProc,
							this, dwCreateFlags, &threadid);
	if (!m_handle)
	{
		m_handle = INVALID_HANDLE_VALUE;
		return FALSE;
	}
	return TRUE;
}

BOOL Thread::Destroy()
{
	if (IsValid())
	{
		Resume();
		Stop();
		WaitForExit();
		return TRUE;  //was running, stopped
	}
	return FALSE;	  //was stopped, no change
}

void Thread::WaitForExit()
{
	//no point waiting on invalid handle
	if (IsValid())
	{
		//wait 10 seconds, INFINITE ?
		::WaitForSingleObject(m_handle, 10000);
		::CloseHandle(m_handle);
		m_handle = INVALID_HANDLE_VALUE;
	}
}

UINT Thread::Cycle()
{
	//if you ever break here, try to Create(CREATE_SUSPENDED), 
	// initialize whatever you need and then Resume() thread
	ASSERT(IsValid());


	do
	{
		Wait();
	} 
	while(!m_stop && Run());
		

	//Ruediger R. Asche - Multithreading for Rookies
	// Note that there is one peculiarity with threads in the 
	// handle/object model: You can close the last handle to 
	// a thread while the thread is running, and the thread 
	// will still be in the system! That is because the Windows 
	// NT kernel, whose responsibility it is to schedule threads 
	// for execution, runs independently from the object manager 
	// that assigns and maintains user-visible handles.

	//thread will die if 
	// (1) class is deleted 
	// (2) Destroy is called
	// (3) Run returns false

	//_endthread automatically closes the thread handle
	//_endthreadex needs explicit ::CloseHandle
	
	//we _need_ handle to be closed and set to INVALID_HANDLE,
	// so we do explicit Close on handle here and in Destroy

	//our thread with invalid handle will still live - until 
	// C/C++ runtime will autocall _endthread/_endthreadex on 
	// return from ThreadProc callback, which we assume is OK,
	// as we don't need to control this thread anymore

	::CloseHandle(m_handle);
	m_handle = INVALID_HANDLE_VALUE;
	return 0;
}

UINT __stdcall Thread::ThreadProc(LPVOID pVoid)
{
	ASSERT(pVoid);
	
	Thread *pThread = (Thread *)pVoid;
	return pThread->Cycle();
}

BOOL Thread::SetPriority(int nPriority) const
{	
	if (IsValid())  
		return ::SetThreadPriority(m_handle, nPriority);
	return FALSE;
}

int Thread::GetPriority() const
{	
	if (IsValid())  
		return ::GetThreadPriority(m_handle);  
	return FALSE;
}

DWORD Thread::Suspend()	const
{	
	if (IsValid())
		return ::SuspendThread(m_handle);  
	return FALSE;
}

DWORD Thread::Resume() const
{	
	if (IsValid())
		return ::ResumeThread(m_handle);
	return FALSE;
}

//EOF
