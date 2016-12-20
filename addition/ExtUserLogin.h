#pragma once

#include "tag.h"
#include "stanzaextension.h"
#include "extdefined.h"

using namespace gloox;

class CUserLogin : public StanzaExtension
{
public:
	CUserLogin(int t = 0);
	CUserLogin(const Tag *tag);
	~CUserLogin() {}
public:
	//StanzaExtension
	virtual const std::string & filterString() const;
	virtual Tag* tag() const;
	virtual StanzaExtension* newInstance(const Tag* tag) const
	{
		return new CUserLogin(tag);
	}
	virtual StanzaExtension* clone() const
	{
		CUserLogin *query = new CUserLogin();
		query->m_Type = m_Type;
		return query;
	}
private:
	int		m_Type;
public:
	int		GetType(){ return m_Type; }
};