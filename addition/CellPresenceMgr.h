#pragma once
#include "extdefined.h"
#include "presencehandler.h"
#include "x_motion.h"

class CCellPresenceMgr :
	public gloox::PresenceHandler
{
public:
	CCellPresenceMgr(gloox::Client *);
	virtual ~CCellPresenceMgr();
public:
	//Presencehandler
	virtual void handlePresence(const gloox::Presence& presence);
private:
	XMotion *m_motion;
	gloox::Client *m_client;
public:
	void SetMotion(XMotion *mt) { m_motion = mt; }
};

