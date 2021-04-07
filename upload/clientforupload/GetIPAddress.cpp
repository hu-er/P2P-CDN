/*
 * GetIPAddress.cpp
 *
 *  Created on: Oct 15, 2015
 *      Author: lagrange
 */

#include "GetIPAddress.h"
GetIPAddress::GetIPAddress() {
	// TODO Auto-generated constructor stub
	for(int i=0;i<10;i++){
			IPaddr.ipAddr[i]="";
		}
		IPaddr.sum=0;
	IPaddr.sum=0;
}

GetIPAddress::~GetIPAddress() {
	// TODO Auto-generated destructor stub
	for(int i=0;i<10;i++){
		IPaddr.ipAddr[i]="";
	}
	IPaddr.sum=0;
}

bool GetIPAddress::showIP(string &str,int i){
	if(i<IPaddr.sum){
		str=IPaddr.ipAddr[i];
		return true;
	}else{
		return false;
	}
}

void GetIPAddress::getIpAddress(){
    struct ifaddrs *ifAddrStruct = NULL;
    void *tempAddrPtr=NULL;
    //得到本机的IP地址信息
    getifaddrs(&ifAddrStruct);
    while(ifAddrStruct!=NULL){
    	if(ifAddrStruct->ifa_addr->sa_family==AF_INET){   //如果是IPV4
    		tempAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
    		char ipaddresstemp[INET_ADDRSTRLEN + 1];
    		inet_ntop(AF_INET, tempAddrPtr, ipaddresstemp, INET_ADDRSTRLEN);  //把拿到的ip地址转为字符串形式
    		if((strcmp(ipaddresstemp,"127.0.0.1")!=0)&& (strcmp(ipaddresstemp, "10.0.2.15") != 0) ){    //此接口不是本地接口
    			if(IPaddr.sum<10){
    				IPaddr.ipAddr[IPaddr.sum]=ipaddresstemp;         //存入ip结构体中
    				//cout<< IPaddr.ipAddr[IPaddr.sum]<<endl;
    				IPaddr.sum++;
    				//cout << "IPaddr.sum "<< IPaddr.sum <<endl;
    			}
    		}
    	}
    	ifAddrStruct=ifAddrStruct->ifa_next;   //跳到下一个接口
    }
}
