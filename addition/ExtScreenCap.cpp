#include "extdefined.h"
#include "ExtScreenCap.h"


CScreenCap::CScreenCap() :StanzaExtension(EXT_TYPE_SCCUT)
{
}

CScreenCap::CScreenCap(const Tag *tag) : StanzaExtension(EXT_TYPE_SCCUT)
{
	if (!tag || tag->name() != "query" || tag->xmlns() != XMLNS_SCREEN_CUT)
		return;
	m_Number = tag->findAttribute("status");
}

CScreenCap::CScreenCap(int number) : StanzaExtension(EXT_TYPE_SCCUT)
{
	m_Number = std::to_string(number);
}

CScreenCap::CScreenCap(const std::string number) : StanzaExtension(EXT_TYPE_SCCUT)
{
	m_Number = number;
}

CScreenCap::~CScreenCap()
{
}

const std::string & CScreenCap::filterString() const
{
	const static std::string filter = "/iq/query[@xmlns='"XMLNS_SCREEN_CUT"']";
	return filter;
}

Tag* CScreenCap::tag() const
{
	Tag *query = new Tag("query");
	query->addAttribute("xmlns", XMLNS_SCREEN_CUT);
	query->addAttribute("status", m_Number);
	return query;
}
