#include "stdafx.h"
#include "TaskManager.h"


CTaskManager::CTaskManager() :m_Active(false), m_Thread(NULL)
{
}

CTaskManager::~CTaskManager()
{
}

void CTaskManager::AddTask(CTaskBase *task)
{
	CTaskBase	*t = task->clone();
	m_Mutex.Lock();
	m_Tasks.push_back(t);
	m_Mutex.Unlock();
	m_Event.Signal();
}

void CTaskManager::Run()
{
	m_Active = true;
	m_Thread = (HANDLE)_beginthread(CTaskManager::TaskThread, 0, this);
	/*if (hThread == INVALID_HANDLE_VALUE)
	* error to Create new thread
	*/
	if (m_Thread == INVALID_HANDLE_VALUE)
	{
		m_Active = false;
		m_Thread = NULL;
	}
}

void CTaskManager::Stop(bool waitThreadEnd, UINT waitTime)
{
	m_Active = false;
	if (waitThreadEnd)
		WaitForSingleObject(m_Thread, waitTime);
}

void CTaskManager::_run()
{
	CTaskBase *task = NULL;
	TaskList::iterator it;

	while (m_Active)
	{
		m_Mutex.Lock();
		if (m_Tasks.size() <= 0)
			m_Event.Wait(&m_Mutex);
		else
			m_Event.Reset();
		it = m_Tasks.begin();
		task = (*it);
		m_Tasks.erase(it);
		m_Mutex.Unlock();

		if (task)
		{
			bool rst = task->Work();
			task->WorkEnd(rst);
		}
	}
}

void CTaskManager::TaskThread(LPVOID param)
{
	CTaskManager* _this = (CTaskManager*)param;
	if (_this)
		_this->_run();
}

