/*
 * GetIPAddress.h
 *
 *  Created on: Oct 15, 2015
 *      Author: lagrange
 */
#include"ClientInclude.h"

#ifndef GETIPADDRESS_H_
#define GETIPADDRESS_H_
struct ipAddrs{
	string ipAddr[10];
	int sum;
};
class GetIPAddress {
public:
	GetIPAddress();
	virtual ~GetIPAddress();
	bool showIP(string &str,int i);
	void getIpAddress();
private:
	struct ipAddrs IPaddr;
};

#endif /* GETIPADDRESS_H_ */
