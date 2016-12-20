#include "stdafx.h"
#include "DocDownTask.h"
#include "XHttpGet.h"
#include "CellView.h"
#include "CellViewDlg.h"

#pragma comment(lib,"version.lib")

bool GetFileVersion(LPCTSTR lpszFilePath,CString& rest)
{
	CString szFilePath(lpszFilePath);
	CString szResult(_T("0.0.0.0"));

#define RTN(x) {rest = szResult;\
	return x; }

	if (szFilePath.GetLength() > 0 && PathFileExists(szFilePath))
	{
		VS_FIXEDFILEINFO *pVerInfo = NULL;
		DWORD	dwTemp, dwSize;
		LPBYTE	pData = NULL;
		UINT	uLen;

		dwSize = GetFileVersionInfoSize(lpszFilePath, &dwTemp);

		if (dwSize == 0)
			RTN(false);

		pData = new BYTE[dwSize + 1];

		if (!pData)
			RTN(false);
		if (!GetFileVersionInfo(lpszFilePath, 0, dwSize, pData))
		{
			delete[] pData;
			RTN(false);
		}

		if (!VerQueryValue(pData, TEXT("\\"), (void**)&pVerInfo, &uLen))
		{
			delete[] pData;
			RTN(false);
		}

		DWORD verMS = pVerInfo->dwFileVersionMS;
		DWORD verLS = pVerInfo->dwFileVersionLS;
		WORD major = HIWORD(verMS);
		WORD minor = LOWORD(verMS);
		WORD build = HIWORD(verLS);
		WORD	revision = LOWORD(verLS);

		szResult.Format(TEXT("%d.%d.%d.%d"), major, minor, build, revision);
	}
	RTN(true);
}

//------------------------------ CDocDownTask ---------------------------------------
CDocDownTask::CDocDownTask() :CTaskBase(NULL, NULL)
{

}
CDocDownTask::CDocDownTask(CXmppInstance *ins, XDocMessage *msg) : CTaskBase(ins, msg)
{
	m_Qtype = msg->QType;
	XDocInfoList::const_iterator it = msg->Docs.begin();
	while (it != msg->Docs.end())
	{
		AddDoc((*it)->Name, (*it)->Url, (*it)->Status, (*it)->Version);
		it++;
	}
}


CDocDownTask::~CDocDownTask()
{
	if (m_DocList.size() > 0)
	{
		XDocInfoList::iterator it = m_DocList.begin();;
		while (it != m_DocList.end())
		{
			delete (XDocInfo*)(*it);
			m_DocList.erase(it);
			it = m_DocList.begin();
		}
	}
}

bool CDocDownTask::Work()
{
	std::string WorkName;
	bool bRefreshWindow = false;
	if (m_DocList.size() > 0){
		bRefreshWindow = true;
		PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_TASKMSG, m_Msginfo->GetType(), Act_Begin);
	}

	XDocInfoList::iterator it = m_DocList.begin();
	while (it != m_DocList.end())
	{
		WorkName = m_path + (*it)->Name;
		if (m_Qtype == 1 || !CheckFile(WorkName, (*it)->Version))
		{
			if (httpGetFile((*it)->Url, WorkName) > 0)
				(*it)->Status = DOWN_ACT_SUCCESS;
			else
				(*it)->Status = DOWN_ACT_FAILURE;
		}
		else
			(*it)->Status = DOWN_ACT_SUCCESS;
		it++;
	}

	if (bRefreshWindow)
		PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_TASKMSG, m_Msginfo->GetType(), Act_Success);

	return TRUE;
}

void CDocDownTask::WorkEnd(bool bSuccess)
{
	XIQType tp = bSuccess ? IQ_Result : IQ_Error;
	XDocMessage msg(m_Msginfo->Jid, m_Msginfo->Id, tp, EXT_TYPE_DOCDOWN, m_Qtype);
	XDocInfoList::iterator it = m_DocList.begin();
	while (it != m_DocList.end())
	{
		msg.AddDoc((*it)->Name, (*it)->Url, (*it)->Status, (*it)->Version);
		it++;
	}
	m_Instance->Send(&msg);
}

CTaskBase* CDocDownTask::clone() const
{
	CDocDownTask *t = new CDocDownTask;
	t->InitInstance(m_Instance, m_Msginfo);
	t->m_path = m_path;
	t->m_Qtype = m_Qtype;
	XDocInfoList::const_iterator it = m_DocList.begin();
	while (it != m_DocList.end())
	{
		XDocInfo *info = new XDocInfo((*it)->Name, (*it)->Url, (*it)->Status, (*it)->Version);
		t->m_DocList.push_back(info);
		it++;
	}
	return t;
}

void CDocDownTask::AddDoc(std::string &name, std::string &url, std::string& status, std::string& version)
{
	XDocInfo *info = new XDocInfo(name, url, status, version);
	m_DocList.push_back(info);
}

bool CDocDownTask::CheckFile(const std::string &filename, const std::string &version)
{
	CString verLocal,verRemote;
	GetFileVersion(filename.c_str(), verLocal);
	verRemote = version.c_str();
	return verLocal == verRemote;
}
