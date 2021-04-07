/*
 * TCPManager.h
 *
 *  Created on: Oct 23, 2015
 *      Author: lagrange
 */


#ifndef TCPMANAGER_H_
#define TCPMANAGER_H_
#include"ClientInclude.h"

class TCPManager {
public:
	TCPManager(AccessPoint accPoint);
	virtual ~TCPManager();
	bool SocketConnect();
	int TCPNegotiate();
	void TCPClose();
	void SetAttribute(int SN,int GN,int BN);
	unsigned int GetUdpPort();
	void SendACK();
	ssize_t writen(void *vptr,size_t n);
	ssize_t readLine(void *vptr,size_t maxlen);
	void GetResendInfo(int SN,int GN,int BN);
	void SendResendInfo();

private:
	struct AccessPoint accPoint;
	struct sockaddr_in address;
	int fd;
	unsigned int udpPort;
	int SN;
	int GN;
	int BN;
	vector<ResentInfo> resentInfos;
};

#endif /* TCPMANAGER_H_ */
