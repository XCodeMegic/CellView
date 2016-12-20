#pragma once

#include "tag.h"
#include "stanzaextension.h"

using namespace gloox;

struct SenceInfo
{
#define SCTRL_INFO	"info"
#define SCTRL_CUR	"course"
#define SCTRL_SENCE	"sence"
#define SCTRL_GATE	"gate"
#define SCTRL_LIFE	"life"
public:
	int		m_extType;
	char	m_curse[128];
	char	m_sence[128];
	char	m_gate[64];
	char	m_life[64];
public:

	SenceInfo() {}

	SenceInfo(const Tag *tag)
	{
		Tag *info = tag->findChild(SCTRL_INFO);
		if (info)
		{
			strcpy(m_curse, info->findAttribute(SCTRL_CUR).c_str());
			strcpy(m_sence, info->findAttribute(SCTRL_SENCE).c_str());
			strcpy(m_gate, info->findAttribute(SCTRL_GATE).c_str());
			strcpy(m_life, info->findAttribute(SCTRL_LIFE).c_str());
		}
	}

	void CloneTo(SenceInfo &info) const
	{
		strcpy(info.m_curse, m_curse);
		strcpy(info.m_sence, m_sence);
		strcpy(info.m_gate, m_gate);
		strcpy(info.m_life, m_life);
	}

	void CloneFrom(const SenceInfo &info)
	{
		strcpy(m_curse, info.m_curse);
		strcpy(m_sence, info.m_sence);
		strcpy(m_gate, info.m_gate);
		strcpy(m_life, info.m_life);
	}

	Tag *tag()
	{
		Tag *info = new Tag(SCTRL_INFO);
		info->addAttribute(SCTRL_CUR, m_curse);
		info->addAttribute(SCTRL_SENCE, m_sence);
		info->addAttribute(SCTRL_GATE, m_gate);
		info->addAttribute(SCTRL_LIFE, m_life);
		return info;
	}
};

class SenceControl : public StanzaExtension
{
public:
	SenceControl(int senceType) : StanzaExtension(senceType), m_senceType(senceType) {}
	SenceControl(std::string &curse, std::string &sence, std::string &gate, std::string &life, int senceType);
	
public:
	//StanzaExtension
	virtual Tag* tag() const;
	
protected:
	SenceInfo	*m_info;
private:
	int			m_senceType;
public:
	const char* GetCurse() { return m_info->m_curse; }
	const char* GetSence() { return m_info->m_sence; }
	const char* GetGate() { return m_info->m_gate; }
	const char* GetLife() { return m_info->m_life; }
};

class SenceCtrl : public SenceControl
{
public:
	SenceCtrl();
	SenceCtrl(std::string &curse, std::string &sence, std::string &gate, std::string &life);
	SenceCtrl(const Tag *tag);
public:
	//StanzaExtension
	virtual const std::string & filterString() const;
	virtual StanzaExtension* newInstance(const Tag* tag) const
	{
		return new SenceCtrl(tag);
	}
	virtual StanzaExtension* clone() const
	{
		SenceCtrl *query = new SenceCtrl();
		query->m_info->CloneFrom(*m_info);
		return query;
	}
};

class SenceUpdate : public SenceControl
{
public:
	SenceUpdate();
	SenceUpdate(std::string &curse, std::string &sence, std::string &gate, std::string &life);
	SenceUpdate(const Tag *tag);
public:
	//StanzaExtension
	virtual const std::string & filterString() const;
	virtual StanzaExtension* newInstance(const Tag* tag) const
	{
		return new SenceUpdate(tag);
	}
	virtual StanzaExtension* clone() const
	{
		SenceUpdate *query = new SenceUpdate();
		query->m_info->CloneFrom(*m_info);
		return query;
	}
};
