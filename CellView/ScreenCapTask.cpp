#include "stdafx.h"
#include "ScreenCapTask.h"
#include "XCaptureScreen.h"
#include "char_code_change.h"


CScreenCapTask::CScreenCapTask() :CTaskBase(NULL, NULL)
{

}

CScreenCapTask::CScreenCapTask(CXmppInstance *ins, XScMessage *msg) : CTaskBase(ins, msg)
{
	m_RemoteFileName = msg->Number;
}


CScreenCapTask::~CScreenCapTask()
{
}

bool CScreenCapTask::Work()
{
	if (m_RemoteFileName.empty()) return false;
	bool ret = false;
	wchar_t *buf = AnsiToUnicode(m_RemoteFileName.c_str());
	if (buf)
	{
		if (SaveScreenPicture(XPicSaveType::Save_jpg, buf))
		{
			ret = true;
		}
		else
			ret = false;
		delete[] buf;
	}
	else
		ret = false;
	return ret;
}

void CScreenCapTask::WorkEnd(bool bSuccess)
{
	XIQType tp = bSuccess ? IQ_Result : IQ_Error;

	XScMessage msg(m_RemoteFileName, m_Msginfo->Jid, m_Msginfo->Id, tp);
	m_Instance->Send(&msg);
}

CTaskBase* CScreenCapTask::clone() const
{
	CScreenCapTask *t = new CScreenCapTask;
	t->InitInstance(m_Instance, m_Msginfo);
	t->m_RemoteFileName = m_RemoteFileName;
	return t;
}
