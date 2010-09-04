#ifndef _PROTO_H
#define _PROTO_H


/*
 * UDP protocol command
 */
enum {
	CMD_ACK = 1,
	CMD_KEEPALIVE,
	CMD_REGISTER,
	CMD_LOGIN,
	CMD_LOGOUT,

	CMD_CHANGE_STATUS = 0x1000,
	CMD_ADD_CONTACT,
	CMD_DEL_CONTACT,
	CMD_MESSAGE,
	CMD_SEARCH,
	CMD_SEARCH_RANDOM,
	CMD_GET_CONTACT_INFO,
	CMD_GET_USER_INFO,
	CMD_UPDATE_USER_INFO,
	CMD_GET_CONTACT_LIST,
	CMD_CHANGE_PASSWD,

	// Command sent from server to client, or from server to server
	CMD_SRV_USER_ONLINE = 0x2000,
	CMD_SRV_USER_OFFLINE,
	CMD_SRV_USER_STATUS,
	CMD_SRV_PROBE_STATUS,
	CMD_SRV_SEARCH_RESULT,
	CMD_SRV_ADD_CONTACT,
	CMD_SRV_DEL_CONTACT,
	CMD_SRV_CONTACT_AUTH,
	CMD_SRV_MESSAGE,
	CMD_SRV_CONTACT_INFO,
	CMD_SRV_CONTACT_INFO_REPLY,
};

/*
 * c2s command
 */
enum {
	C2S_ADD_SESSION = 1,
	C2S_REMOVE_SESSION,
	C2S_DELIVER,
};

/*
 * Session manager command
 */
enum {
	SM_ADD_SESSION = 1,
	SM_REMOVE_SESSION,
	SM_DELIVER,
	SM_REQUEST,
};

/*
 * Authorization
 */
enum {
	AUTH_ACCEPT,	// Accept your request
	AUTH_REQUEST,	// Authentication request is needed
	AUTH_REJECT,	// Reject your request
};

/*
 * Message types
 */
enum {
	MSG_TEXT,
	MSG_AUTH_ACCEPT,
	MSG_AUTH_REQUEST,
	MSG_AUTH_REJECT,
	MSG_ADDED,
	MSG_ANNOUNCE,
};

/*
 * User status
 */
enum {
	STATUS_OFFLINE,
	STATUS_ONLINE,
	STATUS_AWAY,
	STATUS_INVIS,
};


#endif
