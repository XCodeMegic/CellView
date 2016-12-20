#pragma once

#include "tag.h"
#include "stanzaextension.h"

using namespace gloox;


extern DocDownAct GetDocStatus(std::string &status);

extern void GetDocStatus(std::string &s, int status);

extern DocDownType GetDocDownType(std::string &type);

extern void GetDocDownType(std::string &s, int type);

class CDocDown :
	public StanzaExtension
{
public:
	CDocDown(DocDownType t = DOWN_NORMAL);
	CDocDown(const Tag *tag);
	~CDocDown();
public:
	//StanzaExtension
	virtual const std::string & filterString() const;
	virtual Tag* tag() const;
	virtual StanzaExtension* newInstance(const Tag* tag) const
	{
		return new CDocDown(tag);
	}
	virtual StanzaExtension* clone() const 
	{
		CDocDown *query = new CDocDown();
		TagList::const_iterator it = m_Docs.begin();
		while (it != m_Docs.end())
		{
			query->m_Docs.push_back((*it)->clone());
		}
		return query;
	}
private:
	TagList m_Docs;
	int		m_Type;
public:
	const TagList &DocList() { return m_Docs; }
	int AddDoc(const std::string &name,const std::string &url, const std::string &status, const std::string &version);
	int		GetType() { return m_Type; }
};

