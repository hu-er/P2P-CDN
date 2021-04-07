/*
 * TCPManager.cpp
 *
 *  Created on: Oct 23, 2015
 *      Author: lagrange
 */

#include "TCPManager.h"
TCPManager::TCPManager(AccessPoint accPoint) {
	// TODO Auto-generated constructor stub
	memset(&address,0,sizeof(address));
	this->accPoint=accPoint;
	fd=0;
	udpPort=0;
	SN=0;
	GN=0;
	BN =0;
}

TCPManager::~TCPManager() {
	// TODO Auto-generated destructor stub
	memset(&address,0,sizeof(address));
	memset(&accPoint,0,sizeof(accPoint));
	fd=0;
	udpPort=0;
	SN=0;
	GN=0;
	BN = 0;
}

bool TCPManager::SocketConnect(){
	fd=socket(AF_INET,SOCK_STREAM,0);
	address.sin_family=AF_INET;
	address.sin_addr.s_addr=inet_addr(accPoint.ipAddress.c_str());
	address.sin_port=htons(8088);
	int res = connect(fd,(struct sockaddr*)&address,sizeof(address));
	if(0==res){
		return true;
	}else{
		return false;
	}
}

void TCPManager::SetAttribute(int SN,int GN,int BN){
	this->SN=SN;
	this->GN=GN;
	this->BN=BN;
}

ssize_t TCPManager::writen(void *vptr,size_t n){
	size_t nleft,nwritten;
	char *ptr;
	ptr=(char*)vptr;
	nleft=n;
	while (nleft>0) {
		if ((nwritten=write(fd, ptr, nleft))<=0) {
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
		if ((rc=read(fd,&c,1))==1) {
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

void TCPManager::SendACK(){
	char msgSend[500]="ACK\n";
	writen(msgSend,strlen(msgSend));
}

int TCPManager::TCPNegotiate(){
	char msgSend[500]="";
	char msgRecv[500]="";
	//发送文件获取消息，包含文件全路径，文件大小，SN,GN
	sprintf(msgSend,"%s %ld %d %d %d\n",accPoint.FilePath.c_str(),accPoint.FileSize,SN,GN,BN);
	writen(msgSend,strlen(msgSend));
	if(SN==0 && GN==0 && (BN!=2049) && (BN!=0)){   //此次请求为重传请求
		SendResendInfo();   //发送重传节点信息
	}
	//从对端得到回复
	readLine(msgRecv,sizeof(msgRecv));
	//发送消息格式错误
	if(strcmp(msgRecv,"WrongMessage！\n")==0){
		SendACK();
		return -1;  //返回-1表明请求格式错误，不代表节点不可用
	}
	if(strcmp(msgRecv,"NOFile！\n")==0){
		SendACK();
		return -2;  //返回-2表明请求的文件不存在，该节点应该被置为不可用，从可用节点中删除
	}
	if(strcmp(msgRecv,"FileExist！\n")==0){   //文件存在，节点可用
		SendACK();
		return 1;    //可用性检查完毕
	}
	if(msgRecv[0] == 'U' && msgRecv[1]=='D' && msgRecv[2]=='P'){   //传输过来的是UDP端口
		for(unsigned int i=8;i<strlen(msgRecv);i++){
			if(msgRecv[i]<='9' && msgRecv[i]>='0'){
				udpPort=udpPort*10+msgRecv[i]-'0';      //得到UDP端口
			}
		}
		//cout << "udpPort "<<udpPort<<endl;
		SendACK();    //协商最后完毕，发送最后确认
	}
	return 1;   //发送1代表协商成功，节点可用
}

unsigned int TCPManager::GetUdpPort(){
	return udpPort;
}

void TCPManager::TCPClose(){
	close(fd);
}

void TCPManager::GetResendInfo(int SN,int GN,int BN){
	ResentInfo temp;
	temp.segNum=SN;
	temp.GN=GN;
	temp.BN=BN;
	resentInfos.push_back(temp);
}

void TCPManager::SendResendInfo(){
	char msgSend[500]="";
	char msgRecv[500]="";
	for(unsigned int i=0;i<resentInfos.size();++i){
		sprintf(msgSend,"%d %d %d\n",resentInfos[i].segNum,resentInfos[i].GN,resentInfos[i].BN);
		writen(msgSend,strlen(msgSend));
	}
	readLine(msgRecv,sizeof(msgRecv));
	if(strcmp(msgRecv,"Get all the point!\n")==0){  //对端发回确认消息
		SendACK();  //发送确认
	}
}
