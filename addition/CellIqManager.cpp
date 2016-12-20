#include "client.h"
#include "clientbase.h"
#include "addition.h"
#include "CellIqManager.h"


static void _changeCode(const std::string& inbuf, std::string& outbuf, bool _2Utf8 = true)
{
	char *charbuf = NULL;
	wchar_t *wcharbuf = NULL;
	DWORD dwWchar, dwChar;

	UINT srccoding = _2Utf8 ? CP_ACP : CP_UTF8;
	UINT dstcoding = _2Utf8 ? CP_UTF8 : CP_ACP;

	dwWchar = MultiByteToWideChar(srccoding, 0, inbuf.c_str(), inbuf.length(), NULL, 0);
	wcharbuf = new wchar_t[dwWchar];
	MultiByteToWideChar(srccoding, 0, inbuf.c_str(), inbuf.length(), wcharbuf, dwWchar);

	dwChar = WideCharToMultiByte(dstcoding, 0, wcharbuf, dwWchar, NULL, 0, NULL, NULL);
	charbuf = new char[dwChar + 1];
	WideCharToMultiByte(dstcoding, 0, wcharbuf, dwWchar, charbuf, dwChar, NULL, NULL);
	*(charbuf + dwChar) = 0;
	outbuf = std::string(charbuf);
	delete wcharbuf;
	delete charbuf;
}

void X_Ansi2Utf8(const std::string& inbuf, std::string& outbuf)
{
	_changeCode(inbuf, outbuf);
}

void X_Utf82Ansi(const std::string& inbuf, std::string& outbuf)
{
	_changeCode(inbuf, outbuf, false);
}

static StanzaExtension *FillExtension(XMessage *msg)
{
	StanzaExtension *ext = NULL;
	switch (msg->GetType())
	{
	case EXT_TYPE_USERLOGIN:
	{
							   XULoginMessage *x = (XULoginMessage *)msg;
							   CUserLogin *u = new CUserLogin(x->QType);
							   ext = u;
	}
		break;
	case EXT_TYPE_DOCDOWN:
	{
							 XDocMessage *x = (XDocMessage *)msg;
							 CDocDown *doc = new CDocDown((DocDownType)x->QType);
							 XDocInfoList::iterator it = x->Docs.begin();
							 while (it != x->Docs.end())
							 {
								 std::string nam, url, sta,ver;
								 X_Ansi2Utf8((*it)->Name, nam);
								 X_Ansi2Utf8((*it)->Url, url);
								 X_Ansi2Utf8((*it)->Status, sta);
								 X_Ansi2Utf8((*it)->Version, ver);
								 doc->AddDoc(nam, url, sta, ver);
								 it++;
							 }
							 ext = doc;
	}
		break;
	case EXT_TYPE_DOCPLAY:
	{
							 CDocPlay *doc = new CDocPlay();
							 XDocMessage *x = (XDocMessage *)msg;
							 XDocInfoList::iterator it = x->Docs.begin();
							 if (it == x->Docs.end())
							 {
								 delete doc;
								 break;
							 }
							 std::string nam, url, sta;
							 X_Ansi2Utf8((*it)->Name, nam);
							 X_Ansi2Utf8((*it)->Status, sta);
							 
							 doc->SetValue(nam, GetDocStatus(sta));
							 ext = doc;
	}
		break;
	case EXT_TYPE_SCCUT:
	{
						   XScMessage *x = (XScMessage *)msg;
						   std::string num;
						   X_Ansi2Utf8(x->Number, num);
						   CScreenCap *sc = new CScreenCap(num);

						   ext = sc;
	}
		break;
	case EXT_TYPE_SENCECTRL:
	{
							   XSenceMessage *x = (XSenceMessage *)msg;
							   std::string curs, sen, gt, lf;
							   X_Ansi2Utf8(x->Course, curs);
							   X_Ansi2Utf8(x->Sence, sen);
							   X_Ansi2Utf8(x->Gate, gt);
							   X_Ansi2Utf8(x->Life, lf);
							   SenceCtrl *ctrl = new SenceCtrl(curs, sen, gt, lf);
							   ext = ctrl;
	}
		break;
	case EXT_TYPE_SENCEUPDATE:
	{
								 XSenceMessage *x = (XSenceMessage *)msg;
								 std::string curs, sen, gt, lf;
								 X_Ansi2Utf8(x->Course, curs);
								 X_Ansi2Utf8(x->Sence, sen);
								 X_Ansi2Utf8(x->Gate, gt);
								 X_Ansi2Utf8(x->Life, lf);
								 SenceUpdate *update = new SenceUpdate(curs, sen, gt, lf);
								 ext = update;
	}
		break;
	}
	return ext;
}

//---------------------------------------------------------------------------------

CCellIqManager::CCellIqManager(gloox::Client *client) : m_client(client), m_motion(NULL)
{
	Init();
}

CCellIqManager::~CCellIqManager()
{

}

bool CCellIqManager::handleIq(const IQ& iq)
{
	bool ret;
	IQ::IqType subtype = iq.subtype();
	const StanzaExtensionList exts = iq.extensions();
	StanzaExtensionList::const_iterator it = exts.begin();
	int extType = (*it)->extensionType();

	XMessage msg(0);
	msg.Jid = iq.from().full();
	msg.Id = iq.id();
	msg.IqType = (int)subtype;

	XMessage *_xmsg = NULL;
	switch (extType)
	{
	case EXT_TYPE_USERLOGIN:
	{
							   CUserLogin *u = (CUserLogin *)(*it);
							   if (m_motion)
							   {
								   _xmsg = new XULoginMessage(u->GetType(), msg.Jid, msg.Id, msg.IqType, EXT_TYPE_USERLOGIN);
								   m_motion->handleLogin((XULoginMessage*)_xmsg);
							   }
	}
		break;
	case EXT_TYPE_DOCDOWN:
	{
							 CDocDown *down = (CDocDown *)(*it);
							 const TagList doclist = down->DocList();
							 TagList::const_iterator it = doclist.begin();
							 if (m_motion && doclist.size() > 0)
							 {
								 _xmsg = new XDocMessage(msg.Jid, msg.Id, msg.IqType, EXT_TYPE_DOCDOWN);
								 while (it != doclist.end())
								 {
									 std::string name, status, url, ver;
									 X_Utf82Ansi((*it)->findAttribute("name"), name);
									 X_Utf82Ansi((*it)->findAttribute("status"), status);
									 X_Utf82Ansi((*it)->findAttribute("url"), url);
									 X_Utf82Ansi((*it)->findAttribute("version"), ver);

									((XDocMessage*)_xmsg)->AddDoc(name, url, status, ver);
									it++;
								 }
								 m_motion->handleDocDown((XDocMessage*)_xmsg);
							 }
	}
		break;
	case EXT_TYPE_DOCPLAY:
	{
							 CDocPlay *play = (CDocPlay *)(*it);
							 if (m_motion)
							 {
								 std::string name;
								 X_Utf82Ansi(play->Name(), name);
								 std::string status = play->Action() ? ACTION_START : ACTION_STOP;
								 _xmsg = new XDocMessage(msg.Jid, msg.Id, msg.IqType, EXT_TYPE_DOCPLAY);
								 ((XDocMessage*)_xmsg)->AddDoc(name, std::string(""), status, std::string(""));
								 m_motion->handleDocPlay((XDocMessage*)_xmsg);
							 }
	}
		break;
	case EXT_TYPE_SCCUT:
	{
						   if (m_motion)
						   {
							   std::string num;
							   X_Utf82Ansi(((CScreenCap *)(*it))->GetNumber(), num);
							   _xmsg = new XScMessage(num, msg.Jid, msg.Id, msg.IqType);
							   m_motion->handleScCap((XScMessage *)_xmsg);
						   }
	}
		break;
	case EXT_TYPE_SENCECTRL:
	case EXT_TYPE_SENCEUPDATE:
	{
								 SenceControl *sen = (SenceControl *)(*it);
								 if (m_motion)
								 {
									 std::string curs, xsen, gt, lf;
									 X_Utf82Ansi(std::string(sen->GetCurse()), curs);
									 X_Utf82Ansi(std::string(sen->GetSence()), xsen);
									 X_Utf82Ansi(std::string(sen->GetGate()), gt);
									 X_Utf82Ansi(std::string(sen->GetLife()), lf);
									 _xmsg = new XSenceMessage(curs,xsen,gt,lf,
										 msg.Jid, msg.Id, msg.IqType, extType);
									 if (extType == EXT_TYPE_SENCEUPDATE)
										 m_motion->handleSenceUpdate((XSenceMessage*)_xmsg);
									 else
										 m_motion->handleSenceCtrl((XSenceMessage*)_xmsg);
								 }
	}
		break;
	}

	if (_xmsg) delete _xmsg;

	return true;
}

void CCellIqManager::handleIqID(const IQ& iq, int context)
{

}

void CCellIqManager::Init()
{
	m_client->registerStanzaExtension(new CUserLogin());
	m_client->registerStanzaExtension(new CDocDown);
	m_client->registerStanzaExtension(new CDocPlay);
	m_client->registerStanzaExtension(new CScreenCap);
	m_client->registerStanzaExtension(new SenceCtrl());
	m_client->registerStanzaExtension(new SenceUpdate());

	m_client->registerIqHandler(this, EXT_TYPE_USERLOGIN);
	m_client->registerIqHandler(this, EXT_TYPE_DOCDOWN);
	m_client->registerIqHandler(this, EXT_TYPE_DOCPLAY);
	m_client->registerIqHandler(this, EXT_TYPE_SCCUT);
	m_client->registerIqHandler(this, EXT_TYPE_SENCECTRL);
	m_client->registerIqHandler(this, EXT_TYPE_SENCEUPDATE);
}

void CCellIqManager::Send(XMessage *msg)
{
	gloox::IQ *iq = new gloox::IQ((gloox::IQ::IqType)msg->IqType, gloox::JID(msg->Jid), msg->Id);
	StanzaExtension *ext = FillExtension(msg);
	if (ext)
		iq->addExtension(ext);
	m_client->send(*iq);
	delete iq;
}

void CCellIqManager::Send(XMemberList &jidList, XMessage *msg)
{
	gloox::IQ *iq = NULL;
	StanzaExtension *ext = FillExtension(msg);
	if (!ext) return;

	XMemberList::iterator it = jidList.begin();
	for (; it != jidList.end(); it++)
	{
		if ((*it)->status == 0) continue;
		iq = new gloox::IQ((gloox::IQ::IqType)msg->IqType, gloox::JID((*it)->jid), msg->Id);
		iq->addExtension(ext);
		m_client->send(*iq);
		ext = ext->clone();
		delete iq;
	}
	delete ext;
}
