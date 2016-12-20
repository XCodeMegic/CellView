#pragma once

#include <iostream>
#include <map>
#include <list>
#include "extdefined.h"

#ifdef LIBVRXMPP_EXPORTS
#define ADD_API __declspec(dllexport)
#else
//#define ADD_API __declspec(dllimport)
#define ADD_API 
#endif

#define X_IQ_TYPE_GET		0
#define X_IQ_TYPE_SET		1
#define X_IQ_TYPE_RESULT	2
#define X_IQ_TYPE_ERROR		3


class XMessage
{
private:
	int	m_type;
public:
	int	GetType() { return m_type; }
public:
	XMessage(std::string &j, std::string &i, int iqtype, int type) : m_type(type), IqType(iqtype) {
		Jid = j;
		Id = i;
	}
	XMessage(int type) : m_type(type) {}
	virtual ~XMessage(){}

public:
	std::string Jid;
	std::string	Id;
	int			IqType;
};

class XULoginMessage : public XMessage
{
public:
	XULoginMessage(int t,
		std::string &j, std::string &i, int iqtype, int type) :XMessage(j, i, iqtype, type), QType(t)
	{
	}
public:
	int		QType;
};

class XSenceMessage : public XMessage
{
public:
	XSenceMessage(int type) :XMessage(type){}
	XSenceMessage(std::string &cur, std::string &sen, std::string &gt, std::string &lf,
		std::string &j, std::string &i, int iqtype, int type) :XMessage(j, i, iqtype, type)
	{
		Course = cur;
		Sence = sen;
		Gate = gt;
		Life = lf;
	}
public:
	std::string	Course;
	std::string	Sence;
	std::string	Gate;
	std::string	Life;
};

class XDocInfo
{
public:
	XDocInfo(std::string &_n, std::string &_u, std::string &status, std::string &ver) :Name(_n), Url(_u), Status(status), Version(ver) {}
public:
	std::string Name;
	std::string Url;
	std::string	Status;
	std::string Version;
};

typedef std::list<XDocInfo *> XDocInfoList;

class XDocMessage : public XMessage
{
public:
	XDocMessage(int type) :XMessage(type){}
	XDocMessage(std::string &j, std::string &i, int iqtype, int type, int qtype = DOWN_NORMAL) :XMessage(j, i, iqtype, type), QType(qtype)
	{
		
	}
	~XDocMessage()
	{
		XDocInfoList::iterator it = Docs.begin();
		while (it != Docs.end())
		{
			delete ((XDocInfo*)*it);
			Docs.erase(it);
			it = Docs.begin();
		}
	}
	void AddDoc(std::string &_n, std::string &_u, std::string &sta, std::string &ver)
	{
		XDocInfo *info = new XDocInfo(_n, _u, sta, ver);
		Docs.push_back(info);
	}
public:
	int			QType;
	XDocInfoList Docs;
};

class XScMessage : public XMessage
{
public:
	XScMessage() :XMessage(EXT_TYPE_SCCUT){}
	XScMessage(std::string &num,
		std::string &j, std::string &i, int iqtype) :XMessage(j, i, iqtype, EXT_TYPE_SCCUT)
	{
		Number = num;
	}
public:
	std::string Number;
};

class XMember
{
public:
	const std::string	jid;
	int					status;
public:
	XMember() {};
	XMember(const std::string &vjid, int vstatus) : jid(vjid), status(vstatus) {};
};

typedef std::list<XMember *> XMemberList;

class ADD_API XMotion
{
public:
	//handle iq
	virtual bool handleLogin(XULoginMessage *msg) = 0;
	virtual bool handleDocDown(XDocMessage *msg) = 0;
	virtual bool handleDocPlay(XDocMessage *msg) = 0;
	virtual bool handleScCap(XScMessage *msg) = 0;
	virtual bool handleSenceCtrl(XSenceMessage *msg) = 0;
	virtual bool handleSenceUpdate(XSenceMessage *msg) = 0;

	//handle presence
	virtual bool handlePresence(const std::string &jid, int status) { return true; }

	//handle connect
	virtual bool handleConnected(void) { return true; }
	virtual bool handleDisConnected(void) { return true; }
	virtual bool handleRoster(const XMemberList &list) { return true; }

};