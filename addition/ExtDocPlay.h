#pragma once

#include "tag.h"
#include "stanzaextension.h"

using namespace gloox;

class CDocPlay :
	public StanzaExtension
{
public:
	CDocPlay();
	CDocPlay(const Tag *tag);
	~CDocPlay();
public:
	//StanzaExtension
	virtual const std::string & filterString() const;
	virtual Tag* tag() const;
	virtual StanzaExtension* newInstance(const Tag* tag) const
	{
		return new CDocPlay(tag);
	}
	virtual StanzaExtension* clone() const
	{
		CDocPlay *query = new CDocPlay();
		query->m_Action = m_Action;
		query->m_Name = m_Name;
		return query;
	}
private:
	bool m_Action;
	std::string m_Name;
public:
	std::string &Name() { return m_Name; }
	bool Action() { return m_Action; };
	void SetValue(const std::string &name, bool action)
	{
		m_Name = name;
		action = m_Action;
	}
};

