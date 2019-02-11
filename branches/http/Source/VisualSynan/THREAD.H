// Thread.h : lightweight thread
//

#ifndef __THREAD_H_
#define __THREAD_H_

#ifndef	ASSERT
 #define ASSERT  _ASSERT
#endif

#ifndef T
 #ifdef _DEBUG
	#ifdef _MFC_VER
	 #define T(s)       TRACE0(s); TRACE0("\n")
	#else
	 #define T(s)       OutputDebugString(s); OutputDebugString("\r\n")
	 #define TRACE0(s)  OutputDebugString(s)
	#endif
 #else
	#define T(s)
	#define TRACE0(s)
  #endif
#endif


class Thread
{
public:
	Thread();
	virtual ~Thread();

	virtual BOOL Create(DWORD dwCreateFlags = 0);
	virtual BOOL Destroy();

	BOOL SetPriority(int nPriority) const;
	int  GetPriority() const;

	DWORD Suspend() const;
	DWORD Resume() const;

	HANDLE GetHandle() const
	{	return m_handle;  }

	BOOL IsValid() const
	{	return (m_handle != INVALID_HANDLE_VALUE);  }

	void Stop()
	{	InterlockedIncrement(&m_stop); }

protected:
	//overridables
	virtual BOOL Run() = 0;	                     //processing
	virtual void Wait() {};	                     //wait for event

	//implementation
	void WaitForExit();                          //wait until handle closed
	UINT Cycle();                                //'real' thread proc
	static UINT __stdcall ThreadProc(LPVOID pv); //Win32  thread proc

protected:
	HANDLE  m_handle;                            //Win32 thread handle
	LONG    m_stop;                              //0 while running
};

#endif //__THREAD_H_
