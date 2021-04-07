/*
 * UDPManager.h
 *
 *  Created on: Nov 3, 2015
 *      Author: lagrange
 */

#ifndef UDPMANAGER_H_
#define UDPMANAGER_H_
#include"ClientInclude.h"
//#include "Compress.h"
class UDPManager {
public:
	UDPManager();
	virtual ~UDPManager();
	bool UdpConnect();
	void UdpClose();
	bool UdpRecv();
	void SetAttribute(string ipAddress,unsigned int UdpPort);
	char *GetDataBuf();
	int Getfd();
private:
	struct sockaddr_in address;
	unsigned int UdpPort;
	int fd;
	char data_buffer[519];
	//Compress *com;
};

#endif /* UDPMANAGER_H_ */
