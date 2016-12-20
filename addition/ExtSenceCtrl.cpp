#include "extdefined.h"	
#include "ExtSenceCtrl.h"

//------------------------------- SenceCtrl -----------------------------------
SenceCtrl::SenceCtrl() : SenceControl(EXT_TYPE_SENCECTRL)
{
	
}

SenceCtrl::SenceCtrl(const Tag *tag) : SenceControl(EXT_TYPE_SENCECTRL)
{
	if (!tag || tag->name() != "query" || tag->xmlns() != XMLNS_SENCE_CTRL)
		return;
	m_info = new SenceInfo(tag);
}

SenceCtrl::SenceCtrl(std::string &curse, std::string &sence, std::string &gate, std::string &life) :
	SenceControl(curse, sence, gate, life, EXT_TYPE_SENCECTRL)
{

}

const std::string & SenceCtrl::filterString() const
{
	const static std::string filter = "/iq/query[@xmlns='"XMLNS_SENCE_CTRL"']";
	return filter;
}

//------------------------------- SenceUpdate -----------------------------------
SenceUpdate::SenceUpdate() : SenceControl(EXT_TYPE_SENCEUPDATE)
{

}

SenceUpdate::SenceUpdate(const Tag *tag) : SenceControl(EXT_TYPE_SENCEUPDATE)
{
	if (!tag || tag->name() != "query" || tag->xmlns() != XMLNS_SENCE_UPDATE)
		return;
	m_info = new SenceInfo(tag);
}

SenceUpdate::SenceUpdate(std::string &curse, std::string &sence, std::string &gate, std::string &life) :
	SenceControl(curse, sence, gate, life, EXT_TYPE_SENCEUPDATE)
{

}

const std::string & SenceUpdate::filterString() const
{
	const static std::string filter = "/iq/query[@xmlns='"XMLNS_SENCE_UPDATE"']";
	return filter;
}

//------------------------------- SenceControl -----------------------------------
SenceControl::SenceControl(std::string &curse, std::string &sence, std::string &gate, std::string &life, int senceType) : StanzaExtension(senceType), m_senceType(senceType)
{
	m_info = new SenceInfo;
	strcpy(m_info->m_curse, curse.c_str());
	strcpy(m_info->m_sence, sence.c_str());
	strcpy(m_info->m_gate, gate.c_str());
	strcpy(m_info->m_life, life.c_str());
}

Tag* SenceControl::tag() const
{ 
	Tag *query = new Tag("query");
	if (m_senceType == EXT_TYPE_SENCECTRL)
		query->addAttribute("xmlns", XMLNS_SENCE_CTRL);
	else
		query->addAttribute("xmlns", XMLNS_SENCE_UPDATE);
	if (m_info)
	{
		Tag *info = m_info->tag();
		query->addChild(info);
	}
	return query;
}