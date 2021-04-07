/*
 * TCPManager.h
 *
 *  Created on: Apr 21, 2016
 *      Author: xiaoyu
 */
#include "MySqlInclude.h"
#ifndef TCPMANAGER_H_
#define TCPMANAGER_H_
class TCPManager {
public:
	TCPManager();
	virtual ~TCPManager();
	ssize_t writen(void *vptr,size_t n);
	ssize_t readLine(void *vptr,size_t maxlen);
	void SocketInit();
	long SocketAccept(struct sockaddr_in* client_address);
	void Close_Socket(int client_sockfd);

private:
	sockaddr_in address;
	int fd;
	int address_len;
	int listenfd;
	int client_sockfd;
};

#endif /* TCPMANAGER_H_ */
