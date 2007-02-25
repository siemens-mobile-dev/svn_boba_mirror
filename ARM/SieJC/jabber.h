/*
    SieNatJabber Project
    Константы, множества протокола XMPP

*/
#ifndef _JABBER_H_
  #define _JABBER_H_
// Error code
#define JABBER_ERROR_REDIRECT				302
#define JABBER_ERROR_BAD_REQUEST			400
#define JABBER_ERROR_UNAUTHORIZED			401
#define JABBER_ERROR_PAYMENT_REQUIRED		        402
#define JABBER_ERROR_FORBIDDEN				403
#define JABBER_ERROR_NOT_FOUND				404
#define JABBER_ERROR_NOT_ALLOWED			405
#define JABBER_ERROR_NOT_ACCEPTABLE			406
#define JABBER_ERROR_REGISTRATION_REQUIRED	        407
#define JABBER_ERROR_REQUEST_TIMEOUT		        408
#define JABBER_ERROR_CONFLICT				409
#define JABBER_ERROR_INTERNAL_SERVER_ERROR	500
#define JABBER_ERROR_NOT_IMPLEMENTED		501
#define JABBER_ERROR_REMOTE_SERVER_ERROR	502
#define JABBER_ERROR_SERVICE_UNAVAILABLE	503
#define JABBER_ERROR_REMOTE_SERVER_TIMEOUT	504

#define MUCST_BANNED  "301"
#define MUCST_R_BAN  "%s was BANNED! "

#define MUCST_KICKED  "307"
#define MUCST_R_KICK  "%s was kicked! "

#define IQTYPE_GET "get"
#define IQTYPE_SET "set"
#define IQTYPE_RES "result"
#define IQTYPE_ERR "error"

#define IQ_AUTH    "jabber:iq:auth"
#define IQ_VERSION "jabber:iq:version"
#define IQ_ROSTER  "jabber:iq:roster"

#define MSGSTR_CHAT "chat"
#define MSGSTR_GCHAT "groupchat"
#define MSGSTR_NORMAL "normal"
// Not implemented in SieJC yet
//error
//headline

#define XMLNS_MUC "http://jabber.org/protocol/muc"
#define XMLNS_MUC_USER "http://jabber.org/protocol/muc#user"


typedef enum {
	SUB_NONE,
	SUB_TO,
	SUB_FROM,
	SUB_BOTH
} JABBER_SUBSCRIPTION;

typedef enum {
	AFFILIATION_NONE,
	AFFILIATION_OUTCAST,
	AFFILIATION_MEMBER,
	AFFILIATION_ADMIN,
	AFFILIATION_OWNER
} JABBER_GC_AFFILIATION;

typedef enum {
	ROLE_NONE,
	ROLE_VISITOR,
	ROLE_PARTICIPANT,
	ROLE_MODERATOR
} JABBER_GC_ROLE;

////////////////////
#define PRESENCE_ONLINE 0
#define PRESENCE_CHAT 1
#define PRESENCE_AWAY 2
#define PRESENCE_XA 3
#define PRESENCE_DND 4
#define PRESENCE_INVISIBLE 5
#define PRESENCE_OFFLINE 6
#define PRESENCE_ERROR 7
#define PRESENCE_SUBSCRIBE 8
#define PRESENCE_SUBSCRIBED 9
#define PRESENCE_UNSUBSCRIBE 10
#define PRESENCE_UNSUBSCRIBED 11
#endif
