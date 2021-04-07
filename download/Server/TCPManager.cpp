/*
 * TCPManager.cpp
 *
 *  Created on: Oct 21, 2015
 *      Author: lagrange
 */

#include "TCPManager.h"

TCPManager::TCPManager() {
	// TODO Auto-generated constructor stub
	GN=0;
	SN=0;
	client_sockfd=0;
	filename="";
	filesize =0;
	UdpPort=0;
	BN=0;
}

TCPManager::~TCPManager() {
	// TODO Auto-generated destructor stub
	GN=0;
	SN=0;
	client_sockfd=0;
	filename="";
	filesize =0;
	BN=0;
	UdpPort=0;
}

int TCPManager::GetGN(){
	return GN;
}

int TCPManager::GetSN(){
	return SN;
}

int TCPManager::GetBN(){
	return BN;
}
string TCPManager::GetFileName(){
	return filename;
}

int TCPManager::GetFileSize(){
	return filesize;
}

void TCPManager::SetAttribute(int client_sockfd,sockaddr_in client_address){
	this->client_sockfd=client_sockfd;
	this->client_address = client_address;
}
//读取一行数据
ssize_t TCPManager::readLine(int fd,void * vptr,size_t maxlen){
	ssize_t rc;
	size_t n;
	char c,*ptr;
	ptr=(char*)vptr;
	for(n=1;n<maxlen;n++){
		again:
		if((rc=read(fd,&c,1))==1){   //一个一个字符的读
			*ptr++=c;
			if(c=='\n'){   //读到换行符退出
				break;
			}
		}else if(rc == 0){   //缓冲区没有数据
			if(n==1){
				return 0;
			}
		}else{
			if(errno==EINTR){   //由于中断而停止
				goto again;
			}
			return -1; //其他错误原因
		}
	}
	*ptr=0;
	return n;
}

ssize_t TCPManager::writen(int fd,void* vptr,size_t n){
	size_t nleft,nwritten;
	char *ptr;
	ptr=(char*)vptr;
	nleft=n;
	while(nleft>0){
		//写入的字符数小于等于0
		if((nwritten = write(fd,ptr,nleft))<=0){
			if(errno == EINTR){
				nwritten = 0;
			}else{
				return -1;
			}
		}
		//第一次写完，进行第二次写入
		nleft-=nwritten;
		ptr+=nwritten;
	}
	return n;
}

bool TCPManager::IsFileExist(){
	struct stat s;
	//文件不存在或文件大小不一样，返回false
	if(lstat(filename.c_str(),&s)<0 || s.st_size!=filesize){
		return false;
	}else{
		return true;
	}
}

bool TCPManager::waitforACK(){
	char msgRec[200]="";
	readLine(client_sockfd,msgRec,sizeof(msgRec));
	if(strcmp(msgRec,"ACK\n")==0){
		return true;
	}else{
		return false;
	}
}

/*void TCPManager::SetUdpPort(){
	srand(getpid());  //随机生成UDP port
	UdpPort=rand()%26530+9000;
	//cout << UdpPort<<endl;
	//cout << "pid is "<<getpid()<<endl;

}*/

void TCPManager::SetUdpPort(unsigned int UdpPort){
	this->UdpPort = UdpPort;
}

bool TCPManager::TCPNegotiate(){
	char msgRec[200]="";
	char msgSend[200] = "";
	string temp;
	unsigned int i;
	readLine(client_sockfd,msgRec,sizeof(msgRec));
	//cout << msgRec <<endl;
	//检索文件名字
	if(strlen(msgRec)<5){   //收到的消息参数不够
		 strcpy(msgSend, "WrongMessage！\n");
		 writen(client_sockfd, (void *)msgSend, strlen(msgSend));
		 if(waitforACK()){
			return false;
		 }
	}else{  //消息格式正确
		for(i=0;i<strlen(msgRec) && msgRec[i]!='\n' && msgRec[i]!=' ';i++){
				filename+=msgRec[i];//读入文件名
		}
		for(i++;i<strlen(msgRec) && msgRec[i]!='\n' && msgRec[i]!=' ';i++){
			temp+=msgRec[i];  //读入文件大小
		}
		filesize=atol(temp.c_str());//得到文件大小
		temp="";
		for(i++;i<strlen(msgRec) && msgRec[i]!='\n' && msgRec[i]!=' ';i++){
			temp+=msgRec[i];
		}
		SN = atol(temp.c_str());    //得到SN
		temp="";
		for(i++;i<strlen(msgRec) && msgRec[i]!='\n' && msgRec[i]!=' ';i++){
			temp+=msgRec[i];
		}
		GN = atol(temp.c_str());//得到GN
		temp="";
		for(i++;i<strlen(msgRec) && msgRec[i]!='\n' && msgRec[i]!=' ';i++){
			temp+=msgRec[i];
		}
		BN = atol(temp.c_str());//得到BN
		cout << "SN is "<<SN <<"GN is "<<GN<<"BN is "<<BN<<endl;
		if(SN==0 && GN==0 && BN==0){    //说明这次连接是为了检查可用性
			if(IsFileExist()){      //如果文件存在
				strcpy(msgSend,"FileExist！\n");
				writen(client_sockfd, (void *)msgSend, strlen(msgSend));
				if(waitforACK()){
					return true;
				}
			}else{  //文件不存在
				strcpy(msgSend,"NOFile！\n");
				writen(client_sockfd, (void *)msgSend, strlen(msgSend));
				if(waitforACK()){
					return false;
				}
			}
		}else if(SN==0 && GN==0 && (BN!=2049) && (BN!=0)){   //重传文件块,此时BN代表重传的块的数量
			GetResentBlk();//读入重传节点
		}else{   //SN GN 不等于0,说明是分配传输任务,不是初次连接,需要传输一个文件段
			cout << "ready to get udp port" <<endl;
		}
	}
	return true;//退出准备获取udp端口
}

bool TCPManager::SendUdpPort(){
	char msgSend[200] = "";
	sprintf(msgSend,"UDPPort %d\n",UdpPort);
	writen(client_sockfd, (void *)msgSend, strlen(msgSend));
	if(waitforACK()){
		return true;
	}else{
		return false;
	}
}

void TCPManager::GetResentBlk(){
	for(int j =0;j<BN;++j){
		ResentInfo tempInfo;
		char msgRec[200] = "";
		string temp="";
		unsigned int i =0;
		readLine(client_sockfd,msgRec,sizeof(msgRec));//开始读入重传节点
		for( ;i<strlen(msgRec) && msgRec[i]!='\n' && msgRec[i]!=' ';i++){
			temp+=msgRec[i];
		}
		tempInfo.SN = atol(temp.c_str());    //得到SN
		temp="";
		for(i++;i<strlen(msgRec) && msgRec[i]!='\n' && msgRec[i]!=' ';i++){
			temp+=msgRec[i];
		}
		tempInfo.GN = atol(temp.c_str());//得到GN
		temp="";
		for(i++;i<strlen(msgRec) && msgRec[i]!='\n' && msgRec[i]!=' ';i++){
			temp+=msgRec[i];
		}
		tempInfo.BN = atol(temp.c_str());//得到BN
		resentInfos.push_back(tempInfo);//存入vector中
	}
	char msgSend[200]="";
	strcpy(msgSend,"Get all the point!\n");
	writen(client_sockfd, (void *)msgSend, strlen(msgSend));
	waitforACK();
}
