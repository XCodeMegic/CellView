#include "extdefined.h"
#include "ExtDocDown.h"


//-----------------------------------==================
DocDownAct GetDocStatus(std::string &status)
{
	DocDownAct s;
	if (status == DOWN_ACT_BEGIN) s = Act_Begin;
	else if (status == DOWN_ACT_CANCEL) s = Act_Cancel;
	else if (status == DOWN_ACT_SUCCESS) s = Act_Success;
	else s = Act_Failure;
	return s;
}

void GetDocStatus(std::string &s, int status)
{
	switch (status)
	{
	case Act_Begin:
		s = DOWN_ACT_BEGIN;
		break;
	case Act_Cancel:
		s = DOWN_ACT_CANCEL;
		break;
	case Act_Success:
		s = DOWN_ACT_SUCCESS;
		break;
	case Act_Failure:
		s = DOWN_ACT_FAILURE;
		break;
	}
}

DocDownType GetDocDownType(const std::string &type)
{
	DocDownType t;
	if (type == SDOWN_TYPE_FORCE) t = DOWN_FORCE;
	else t = DOWN_NORMAL;
	return t;
}

void GetDocDownType(std::string &s, int type)
{
	switch (type)
	{
	case DOWN_NORMAL:
		s = SDOWN_TYPE_NORMAL;
		break;
	case DOWN_FORCE:
	default:
		s = SDOWN_TYPE_FORCE;
		break;
	}
}

//------------------------- CDocDown ---------------------------------------------------
CDocDown::CDocDown(DocDownType t) :StanzaExtension(EXT_TYPE_DOCDOWN), m_Type(t)
{
	
}

CDocDown::CDocDown(const Tag *tag) : StanzaExtension(EXT_TYPE_DOCDOWN)
{
	if (!tag || tag->name() != "query" || tag->xmlns() != XMLNS_DOC_DOWN)
		return;
	ConstTagList docs;
	if (tag->parent())
		docs = tag->findTagList("/iq/query/docs/doc");
	else
		docs = tag->findTagList("/query/docs/doc");

	m_Type = GetDocDownType(tag->findAttribute("type"));

	ConstTagList::iterator it = docs.begin();
	for (; it != docs.end(); it++)
	{
		Tag *tmp = (*it)->clone();
		m_Docs.push_back(tmp);
	}
}

CDocDown::~CDocDown()
{
	TagList::iterator it = m_Docs.begin();
	while (it != m_Docs.end())
	{
		delete *it;
		m_Docs.erase(it);
		it = m_Docs.begin();
	}
}

const std::string & CDocDown::filterString() const
{
	static std::string filter = "/iq/query[@xmlns='"XMLNS_DOC_DOWN"']";
	return filter;
}

Tag* CDocDown::tag() const
{
	std::string stype;
	GetDocDownType(stype, m_Type);

	Tag *query = new Tag("query");
	query->addAttribute("xmlns", XMLNS_DOC_DOWN);
	query->addAttribute("type", stype);
	Tag *docs = new Tag("docs");
	query->addChild(docs);
	TagList::const_iterator it = m_Docs.begin();
	while (it != m_Docs.end())
	{
		Tag *tmp = (*it)->clone();
		docs->addChild(tmp);
		it++;
	}
	return query;
}

int CDocDown::AddDoc(const std::string &name, const std::string &url, const std::string &status, const std::string &version)
{
	Tag *tag = new Tag("doc");
	tag->addAttribute("name", name);
	tag->addAttribute("status", status);
	tag->addAttribute("url", url);
	tag->addAttribute("version", version);
	m_Docs.push_back(tag);
	return 1;
}
