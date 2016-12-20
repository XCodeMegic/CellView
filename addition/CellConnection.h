#pragma once

#include "extdefined.h"
#include "x_motion.h"
#include "connectionlistener.h"

class CCellConnection :
	public gloox::ConnectionListener
{
public:
	CCellConnection(gloox::Client *client);
	virtual ~CCellConnection();
protected:
	//ConnectionListener
	virtual void onConnect();
	virtual void onDisconnect(gloox::ConnectionError e);
	virtual bool onTLSConnect(const gloox::CertInfo& info);

private:
	XMotion			*m_motion;
	gloox::Client	*m_client;
public:
	void SetMotion(XMotion *mt) { m_motion = mt; }
};

