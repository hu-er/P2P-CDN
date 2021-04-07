/*
 * TCPManager.cpp
 *
 *  Created on: Apr 21, 2016
 *      Author: xiaoyu
 */

#include "TCPManager.h"
#include "MySqlInclude.h"
TCPManager::TCPManager() {
	// TODO Auto-generated constructor stub
	address_len=0;
	fd =0;
	listenfd =0;
}

TCPManager::~TCPManager() {
	// TODO Auto-generated destructor stub
}

void TCPManager::SocketInit(){
	fd=socket(AF_INET,SOCK_STREAM,0);
	address.sin_family=AF_INET;
	address.sin_addr.s_addr=htonl(INADDR_ANY);
	address.sin_port=htons(8086);     //绑定端口和地址
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

long TCPManager::SocketAccept(struct sockaddr_in* client_address ){
	unsigned int len;

	len = sizeof(sockaddr_in);
	client_sockfd=accept(fd,(sockaddr*)client_address,&len);
	//cout<<client_address->sin_port<<endl;
	return client_sockfd;
}

void TCPManager::Close_Socket(int client_sockfd){

	//cout<<client_sockfd<<" close"<<endl;
	close(client_sockfd);
}


ssize_t TCPManager::writen(void *vptr,size_t n){
	size_t nleft,nwritten;
	char *ptr;
	ptr=(char*)vptr;
	nleft=n;
	while (nleft>0) {
		if ((nwritten=write(client_sockfd, ptr, nleft))<=0) {
			if (errno==EINTR) { nwritten=0; }
			else { return -1; }
		}
		nleft-=nwritten; ptr+=nwritten;
	}
	return n;
}

ssize_t TCPManager::readLine(void *vptr,size_t maxlen){
	ssize_t rc;
	size_t n;
	char c,*ptr;
	ptr=(char*)vptr;
	for (n=1; n<maxlen; n++) {
		again:
		if ((rc=read(client_sockfd,&c,1))==1) {
			*ptr++=c;
			if (c=='\n') { break; }  //readover
		}
		else if (rc==0) {
			if (n==1) { return 0; }  //EOF no data read
			else { break; }  //EOF some data read
		}
		else {
			if (errno==EINTR) { goto again; }
			return -1;  //error
		}
	}
	*ptr=0;
	return n;
}
