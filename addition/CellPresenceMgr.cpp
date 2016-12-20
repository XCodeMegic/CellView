#include "client.h"
#include "CellPresenceMgr.h"


CCellPresenceMgr::CCellPresenceMgr(gloox::Client *client) :m_motion(NULL), m_client(client)
{
	m_client->registerPresenceHandler(this);
}


CCellPresenceMgr::~CCellPresenceMgr()
{
}

void CCellPresenceMgr::handlePresence(const gloox::Presence& presence)
{
	if (m_motion)
	{
		m_motion->handlePresence(presence.from().full(), presence.priority());
	}
}
