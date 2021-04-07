/*
 * TCPManager.h
 *
 *  Created on: Oct 21, 2015
 *      Author: lagrange
 */

#include"ServerInclude.h"
#ifndef TCPMANAGER_H_
#define TCPMANAGER_H_

class TCPManager {
public:
	TCPManager();
	virtual ~TCPManager();
	bool TCPNegotiate();
	int GetSN();
	int GetGN();
	int GetBN();
	void SetUdpPort(unsigned int UdpPort);
	bool IsFileExist();
	void SetAttribute(int client_sockfd,struct sockaddr_in client_address);
	ssize_t readLine(int fd, void *vptr, size_t maxlen);
	ssize_t writen(int fd, void *vptr, size_t n);
	bool waitforACK();
	string GetFileName();
	int  GetFileSize();
	bool SendUdpPort();
	void GetResentBlk();
	vector<ResentInfo> resentInfos;

private:
	int GN;
	int SN;
	int BN;
	struct sockaddr_in client_address;
	int client_sockfd;
	string filename;
	int filesize;
	int UdpPort;
};

#endif /* TCPMANAGER_H_ */
