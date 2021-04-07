/*
 * UDPManager.h
 *
 *  Created on: Nov 3, 2015
 *      Author: lagrange
 */

#ifndef UDPMANAGER_H_
#define UDPMANAGER_H_
//#include "Compress.h"
#include"ServerInclude.h"
//#include "FileDataManager.h"
class UDPManager {
public:
	UDPManager();
	virtual ~UDPManager();
	bool UdpConnect();
	void UdpClose();
	//bool UdpSend(FileDataManager *fileDataManager);
	void SetUDPPort();
	unsigned int GetUDPPort();
	bool UDPTest();
	void UdpSendBlk(char*);

private:
	int fd;
	struct sockaddr_in address;
	unsigned int UdpPort;
	//Compress *com;
};

#endif /* UDPMANAGER_H_ */
