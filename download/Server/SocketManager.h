/*
 * SocketManager.h
 *
 *  Created on: Oct 20, 2015
 *      Author: lagrange
 */

#include"ServerInclude.h"
#ifndef SOCKETMANAGER_H_
#define SOCKETMANAGER_H_

class SocketManager {
public:
	SocketManager();
	virtual ~SocketManager();
	void SocketInit();
	long SocketAccept(struct sockaddr_in* client_address );
	void Close_Socket(int client_sockfd);
private:
    sockaddr_in address;
	int fd;
	int address_len;
	int listenfd;

};

#endif /* SOCKETMANAGER_H_ */
