#pragma once
#include "TaskBase.h"

class XMutex
{
private:
	CRITICAL_SECTION _section;
public:
	XMutex()
	{
		InitializeCriticalSection(&_section);
	}
	~XMutex()
	{
		DeleteCriticalSection(&_section);
	}

	void Lock()
	{
		EnterCriticalSection(&_section);
	}
	void Unlock()
	{
		LeaveCriticalSection(&_section);
	}
};

class XEvent
{
private:
	HANDLE _hEvent;
public:
	XEvent()
	{
		_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	~XEvent()
	{
		if (_hEvent != NULL)
			CloseHandle(_hEvent);
	}
	void Signal()
	{
		if (_hEvent)
			SetEvent(_hEvent);
	}
	void Reset()
	{
		if (_hEvent)
			ResetEvent(_hEvent);
	}
	void Wait(XMutex *mutex)
	{
		mutex->Unlock();
		WaitForSingleObject(_hEvent, INFINITE);
		mutex->Lock();
	}
};

class CTaskManager
{
public:
	CTaskManager();
	~CTaskManager();
private:
	TaskList	m_Tasks;
	XMutex		m_Mutex;
	XEvent		m_Event;

	bool		m_Active;
	HANDLE		m_Thread;
public:
	void AddTask(CTaskBase *task);

public:
	void Run();
	void _run();

	void Stop(bool waitThreadEnd = true, UINT waitTime = 3000);
private:
	static void TaskThread(LPVOID param);

};

