#pragma once
#include "gloox.h"
#include "x_motion.h"


class CXmppInstance;

class CViewMotion :
	public XMotion
{
public:
	CViewMotion(CXmppInstance *ins);
	~CViewMotion();
protected:
	//XMotion
	virtual bool handleLogin(XULoginMessage *msg);
	virtual bool handleDocDown(XDocMessage *msg);
	virtual bool handleDocPlay(XDocMessage *msg);
	virtual bool handleScCap(XScMessage *msg);
	virtual bool handleSenceCtrl(XSenceMessage *msg);
	virtual bool handleSenceUpdate(XSenceMessage *msg);

	virtual bool handleConnected(void);
	virtual bool handleDisConnected(void);
private:
	CXmppInstance*	m_Instance;
};

