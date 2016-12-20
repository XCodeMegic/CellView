#include <stdio.h>
#include "client.h"
#include "CellConnection.h"
#include "rosteritem.h"
#include "rostermanager.h"
#include "gloox.h"

using namespace gloox;

#ifdef _DEBUG
#pragma comment(lib,"dgloox-1.0.lib")
#else
#pragma comment(lib,"rgloox-1.0.lib")
#endif


CCellConnection::CCellConnection(gloox::Client *client) : m_client(client), m_motion(NULL)
{
	m_client->registerConnectionListener(this);
}


CCellConnection::~CCellConnection()
{
}

void  CCellConnection::onConnect()
{
	//set presence
	m_client->setPresence(gloox::Presence::PresenceType::Available, 1);
	//dispatch connected msg
	if (m_motion) m_motion->handleConnected();
	//get roster
	if (m_motion)
	{
		gloox::Roster *roster = m_client->rosterManager()->roster();
		gloox::Roster::iterator it = roster->begin();
		XMemberList list;
		while (it != roster->end())
		{
			gloox::RosterItem *item = it->second;
			XMember *member = new XMember(item->jidJID().full(),item->online());
			list.push_back(member);
			it++;
		}
		if (list.size() > 0)
			m_motion->handleRoster(list);
	}
}

void CCellConnection::onDisconnect(gloox::ConnectionError e)
{
	//on Disconnect
	if (m_motion) m_motion->handleDisConnected();
}

bool CCellConnection::onTLSConnect(const gloox::CertInfo& info)
{
	return true;
}

