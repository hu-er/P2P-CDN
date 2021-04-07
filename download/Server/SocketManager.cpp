/*
 * SocketManager.cpp
 *
 *  Created on: Oct 20, 2015
 *      Author: lagrange
 */

#include "SocketManager.h"

SocketManager::SocketManager() {
	// TODO Auto-generated constructor stub
	fd =0;
	address_len=0;
	listenfd=0;
}

SocketManager::~SocketManager() {
	// TODO Auto-generated destructor stub
	fd =0;
	address_len=0;
	listenfd=0;
}

void SocketManager::SocketInit(){
	fd=socket(AF_INET,SOCK_STREAM,0);
	address.sin_family=AF_INET;
	address.sin_addr.s_addr=htonl(INADDR_ANY);
	address.sin_port=htons(8088);     //绑定端口和地址
	address_len=sizeof(address);
	int result=-1;
	while(result !=0){
		result =bind(fd,(struct sockaddr*)&address,address_len);//防止bind函数不成功(端口有可能仍然被占用)
		cout << "result is "<<result <<endl;
		if(result !=0){
			sleep(5);        //若bind不成功，等待5s，重新执行bind
		}
	}
	listenfd=listen(fd,144);  //最大连接数是144
}

long SocketManager::SocketAccept(struct sockaddr_in* client_address ){
	unsigned int len;
	int client_sockfd;
	len = sizeof(sockaddr_in);
	client_sockfd=accept(fd,(sockaddr*)client_address,&len);
	//cout<<client_address->sin_port<<endl;
	return client_sockfd;
}

void SocketManager::Close_Socket(int client_sockfd){

	//cout<<client_sockfd<<" close"<<endl;
	close(client_sockfd);
}
