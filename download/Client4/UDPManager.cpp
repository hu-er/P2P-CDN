/*
 * UDPManager.cpp
 *
 *  Created on: Nov 3, 2015
 *      Author: lagrange
 */

#include "UDPManager.h"

UDPManager::UDPManager() {
	// TODO Auto-generated constructor stub
	memset(&address,0,sizeof(address));
	UdpPort=0;
	fd =0;
	memset(data_buffer,0,519*sizeof(char));
	//com = new Compress();
	//com->init();
}

UDPManager::~UDPManager() {
	// TODO Auto-generated destructor stub
	memset(&address,0,sizeof(address));
	UdpPort=0;
	fd =0;
	memset(data_buffer,0,519*sizeof(char));
}

void UDPManager::SetAttribute(string ipAddress,unsigned int UdpPort){
	this->address.sin_addr.s_addr = inet_addr(ipAddress.c_str());
	//cout << ipAddress<<endl;

	this->UdpPort=UdpPort;
}

bool UDPManager::UdpConnect(){
	bool Reuse =true;
	int nRecvBufLen = 256*1024;
	char msg[100]="";
	fd = socket(AF_INET,SOCK_DGRAM,0);
	//重用socket，设置socket的缓冲区为256KB
	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&Reuse,sizeof(bool));
	setsockopt(fd,SOL_SOCKET,SO_RCVBUF, &nRecvBufLen, sizeof(int));
	address.sin_family=AF_INET;
	address.sin_port=htons(UdpPort);
	usleep(1000*1000*2);
	strcpy(msg,"OK\n");
	//发送 OK 信息，代表连接已经建立，可以准备接收数据
	if(sendto(fd,msg,sizeof(msg),0,(struct sockaddr*)&address,sizeof(address))){
			return true;
	}else{
		return false;
	}
}

void UDPManager::UdpClose(){
	close(fd);
}

bool UDPManager::UdpRecv(){

	unsigned int len = sizeof(address);
	int length=0;
	if((length=recvfrom(fd,data_buffer,sizeof(data_buffer),0,(sockaddr*)&address,&len))!=-1){  //接收成功
		//cout<<"receive success "<<UdpPort<<endl;
		//int BN = data_buffer[512]*100+data_buffer[513];//得到数据所在的段号,BN从0算起
		//int size = data_buffer[514]*100+data_buffer[515];//得到数据段的大小
		//cout <<" size "<<size <<" BN "<<BN <<endl;
		//cout << "success in receive,length is" << length<<endl;
		//com->unzip(data_buffer);
		//strcpy(data_buffer,(char*)com->bufOut);
		//cout << "unzip success" <<endl;
		return true;
	}else{
		return false;
	}
}

char* UDPManager::GetDataBuf(){
	return data_buffer;
}

int UDPManager::Getfd(){
	return fd;
}
