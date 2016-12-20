#include "extdefined.h"
#include "ExtDocPlay.h"


CDocPlay::CDocPlay() :StanzaExtension(EXT_TYPE_DOCPLAY), m_Action(false), m_Name("")
{
}

CDocPlay::CDocPlay(const Tag *tag) : StanzaExtension(EXT_TYPE_DOCPLAY)
{
	if (!tag || tag->name() != "query" || tag->xmlns() != XMLNS_DOC_PLAY)
		return;
	Tag *doc = tag->findChild("doc");
	m_Name = doc->findAttribute("name");
	std::string status = doc->findAttribute("status");
	if (status == ACTION_START)
		m_Action = true;
	else
		m_Action = false;
}

CDocPlay::~CDocPlay()
{
}

const std::string & CDocPlay::filterString() const
{
	const static std::string filter = "/iq/query[@xmlns='"XMLNS_DOC_PLAY"']"; 
	return filter;
}

Tag* CDocPlay::tag() const
{
	Tag *tag = new Tag("query");
	tag->addAttribute("xmlns", XMLNS_DOC_PLAY);
	Tag *doc = new Tag("doc");
	doc->addAttribute("name", m_Name);
	if (m_Action)
		doc->addAttribute("status", ACTION_START);
	else
		doc->addAttribute("status", ACTION_STOP);
	tag->addChild(doc);
	return tag;
}
