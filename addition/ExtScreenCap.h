#pragma once

#include "tag.h"
#include "stanzaextension.h"

using namespace gloox;

class CScreenCap :
	public StanzaExtension
{
public:
	CScreenCap();
	CScreenCap(const Tag *tag);
	CScreenCap(int number);
	CScreenCap(const std::string number);
	~CScreenCap();
public:
	//StanzaExtension
	virtual const std::string & filterString() const;
	virtual Tag* tag() const;
	virtual StanzaExtension* newInstance(const Tag* tag) const
	{
		return new CScreenCap(tag);
	}
	virtual StanzaExtension* clone() const
	{
		CScreenCap *query = new CScreenCap();
		query->m_Number = m_Number;
		return query;
	}
private:
	std::string m_Number;
public:
	std::string & GetNumber(void) { return m_Number; };
};

