#include "ExtUserLogin.h"

CUserLogin::CUserLogin(int t) : StanzaExtension(EXT_TYPE_USERLOGIN), m_Type(t)
{

}

CUserLogin::CUserLogin(const Tag *tag) : StanzaExtension(EXT_TYPE_USERLOGIN)
{
	if (!tag || tag->name() != "query" || tag->xmlns() != XMLNS_USER_LOGIN)
		return;
	std::string str = tag->xml();
	if (tag->findAttribute("type") == "success")
		m_Type = 1;
	else
		m_Type = 0;
}

const std::string &CUserLogin::filterString() const
{
	static const std::string filter = "/iq/query[@xmlns='"XMLNS_USER_LOGIN"']";
	return filter;
}

Tag *CUserLogin::tag() const
{
	Tag *tag = new Tag("query");
	tag->addAttribute("xmlns", XMLNS_USER_LOGIN);
	tag->addAttribute("type", m_Type ? "success" : "failure");
	std::string str = tag->xml();
	return tag;
}
