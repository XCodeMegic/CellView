#pragma once

#define XMLNS_USER_LOGIN "cellcom:user:login"
#define XMLNS_DOC_DOWN "cellcom:doc:down"
#define XMLNS_DOC_PLAY "cellcom:doc:play"
#define XMLNS_SCREEN_CUT "cellcom:sc:cut"
#define XMLNS_SENCE_UPDATE	"cellcom:sence:update"
#define XMLNS_SENCE_CTRL	"cellcom:sence:ctrl"

#define ACTION_START "start"
#define ACTION_STOP "stop"

#define DOWN_ACT_BEGIN		"begin"
#define DOWN_ACT_CANCEL		"cancel"
#define DOWN_ACT_SUCCESS	"success"
#define DOWN_ACT_FAILURE	"failure"

#define SDOWN_TYPE_NORMAL	"normal"
#define SDOWN_TYPE_FORCE		"force"

typedef enum
{
	Act_Begin = 0,
	Act_Cancel,
	Act_Success,
	Act_Failure
}DocDownAct;

typedef enum
{
	DOWN_NORMAL = 0,
	DOWN_FORCE
}DocDownType;

/* in "stanzaextension.h",the extensionType is end of 49,so we 
 * start our extension from 50
 */
#define EXT_TYPE_USERLOGIN	51
#define EXT_TYPE_DOCDOWN	52
#define EXT_TYPE_DOCPLAY	53
#define EXT_TYPE_SCCUT		54
#define EXT_TYPE_SENCEUPDATE	55
#define EXT_TYPE_SENCECTRL	56

typedef enum
{
	IQ_Get = 0,
	IQ_Set,
	IQ_Result,
	IQ_Error
}XIQType;
