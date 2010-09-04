#include "config.h"
#include "tcpstream.h"
#include "debug.h"


TCPStream::TCPStream(int fd) : Socket(fd)
{
	recvBufSize = sendBufSize = 0;

	isConnected = (fd >= 0);
	destIP = 0;
	destPort = 0;
	connTimeout = 0;

	if (isConnected) {
		setBlockMode();
		addEvent(SOCKET_READ);
	}
}

/*
 * Connect to (ip, port). If timeout is non-zero, enable reconnecting
 */
bool TCPStream::connect(uint32 ip, int port, int timeout)
{
	// Store it for reconnecting later
	destIP = ip;
	destPort = port;
	connTimeout = timeout;

	return reconnect();
}

bool TCPStream::reconnect()
{
	closeSocket();

	if (!createSocket(SOCK_STREAM))
		return false;
	setBlockMode();

	if (Socket::connect(destIP, destPort))
		// Wow, we connect it immediately
		onSocketConnect();
	else if (Socket::getLastError() != EINPROGRESS)
		return false;
	else {
		// Connection is pending, enable write so that we will get notified
		// when the connection is established.
		addEvent(SOCKET_WRITE);
		setTimeout(connTimeout);
	}
	return true;
}

void TCPStream::closeSocket()
{
	Socket::closeSocket();

	isConnected = false;
}

bool TCPStream::sendPacket(OutPacket &hdr, const char *data, int n)
{
	uint16 size = hdr.getLength() + n;

	// Prevent others from intervening us
	mutex.lock();

	// If send buffer is overflow, do not put any bytes in it.
	if (size + sizeof(size) + sendBufSize > SENDBUF_SIZE) {
		ICQ_LOG("send buffer overflow: %d\n", size + sizeof(size) + sendBufSize);
		mutex.unlock();
		return false;
	}

	// Copy into the send buffer
	char *p = sendBuf + sendBufSize;
	*(uint16 *) p = htons(size);
	p += sizeof(size);
	memcpy(p, hdr.data, hdr.getLength());
	p += hdr.getLength();
	if (n > 0)
		memcpy(p, data, n);

	sendBufSize += size + sizeof(size);

	// Enable write since there is something to send.
	if (sockfd >= 0)
		addEvent(SOCKET_WRITE);

	mutex.unlock();
	return true;
}

void TCPStream::onSocketConnect()
{
	ICQ_LOG("Connected to %s:%d\n", inet_ntoa(*(in_addr *) &destIP), destPort);

	isConnected = true;

	addEvent(SOCKET_READ);
}

bool TCPStream::onSocketRead()
{
	int n = recv(sockfd, recvBuf + recvBufSize, RECVBUF_SIZE - recvBufSize, 0);

	if (n <= 0) {
		onSocketClose();
		return false;
	}

	recvBufSize += n;
	char *start = recvBuf;
	char *end = start + recvBufSize;
	uint16 len;

	while (start + sizeof(len) < end) {
		// The first two bytes are the packet length
		len = ntohs(*(uint16 *) start);

		// Not a complete packet
		if (end - start - sizeof(len) < len)
			break;

		start += sizeof(len);
		onPacketReceived(start, len);
		start += len;
	}
	recvBufSize = end - start;
	if (recvBufSize > 0)
		memmove(recvBuf, start, recvBufSize);

	return true;
}

void TCPStream::onSocketWrite()
{
	if (!isConnected)
		onSocketConnect();

	// No data to send
	if (sendBufSize <= 0)
		return;

	int n = send(sockfd, sendBuf, sendBufSize, 0);

	if (n > 0) {
		ICQ_LOG("%d bytes left in send buffer\n", sendBufSize - n);

		mutex.lock();

		if ((sendBufSize -= n) > 0)
			memmove(sendBuf, sendBuf + n, sendBufSize);

		mutex.unlock();
	}

	// If there is still data in send buffer, enable write continously
	if (sendBufSize > 0)
		addEvent(SOCKET_WRITE);
}

void TCPStream::onSocketTimeout()
{
#ifdef DEBUG
	in_addr addr;
	addr.s_addr = destIP;
	ICQ_LOG("Connect to %s:%d timeout\n", inet_ntoa(addr), destPort);
#endif
	reconnect();
}

void TCPStream::onSocketClose()
{
#ifdef DEBUG
	sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	if (getpeername(sockfd, (sockaddr *) &addr, &addrlen) == 0) {
		ICQ_LOG("Lost connection to %s:%d\n",
			inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
	}
#endif

	// Reconnect only in onSocketTimeout()
	if (destIP == 0)
		closeSocket();
}
