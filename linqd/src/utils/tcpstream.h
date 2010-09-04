#ifndef _TCP_STREAM_H
#define _TCP_STREAM_H

#include "socket.h"
#include "packet.h"
#include "sync.h"


#define SENDBUF_SIZE	8192
#define RECVBUF_SIZE	2048


class TCPStream : public Socket {
public:
	TCPStream(int fd = -1);

	bool connect(uint32 ip, int port, int timeout);
	void closeSocket();

	virtual bool onSocketRead();
	virtual void onSocketWrite();
	virtual void onSocketTimeout();

	bool sendPacket(OutPacket &out, const char *data, int n);

protected:
	bool reconnect();

	virtual void onPacketReceived(const char *data, int n) {}
	virtual void onSocketConnect();
	virtual void onSocketClose();

private:
	char sendBuf[SENDBUF_SIZE];
	int sendBufSize;
	char recvBuf[RECVBUF_SIZE];
	int recvBufSize;

	bool isConnected;

	// For reconnecting
	uint32 destIP;
	int destPort;
	int connTimeout;

	// Send buffer lock
	Mutex mutex;
};


#endif
