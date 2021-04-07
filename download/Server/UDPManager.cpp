/*
 * UDPManager.cpp
 *
 *  Created on: Nov 3, 2015
 *      Author: lagrange
 */

#include "UDPManager.h"

UDPManager::UDPManager() {
	// TODO Auto-generated constructor stub
	fd = 0;
	memset(&address,0,sizeof(sockaddr_in));
	UdpPort =0;
	//com = new Compress();
	//com->init();
}

UDPManager::~UDPManager() {
	// TODO Auto-generated destructor stub
	fd = 0;
	memset(&address,0,sizeof(sockaddr_in));
	UdpPort =0;
}

bool UDPManager::UdpConnect(){
	int nRecvBufLen = 100*1024;
	fd = socket(AF_INET,SOCK_DGRAM,0);
	//设置缓冲区大小为100k
	setsockopt(fd,SOL_SOCKET,SO_SNDBUF,&nRecvBufLen,sizeof(int));
	address.sin_family=AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	SetUDPPort();  //生成udp端口
	address.sin_port = htons(UdpPort);
	//等待建立连接
	int count =0;
	while(bind(fd,(struct sockaddr *)&address,sizeof(address))!=0 && count <30){  //多次执行bind函数，防止bind不成功
		cout << "Udp connect error" << endl;
		cout << "UdpPort is "<< UdpPort <<endl;
		UdpPort = rand()%26530+9000;//udp port: 9000-35530  重新产生udp端口
		address.sin_port=htons(UdpPort);
		++count;
	}
	if(count<30){
		cout << "bind return 0 " << UdpPort<<endl;
		return true;
	}else{
		return false;
	}
}

void UDPManager::SetUDPPort(){
	srand(getpid());  //随机生成UDP port
	UdpPort=rand()%26530+9000;
}

unsigned int UDPManager::GetUDPPort(){
	return UdpPort;
}

bool UDPManager::UDPTest(){
	char msg[100]="";
	unsigned int len = sizeof(address);
	//bzero(&client_address,sizeof(client_address));
	recvfrom(fd,msg,sizeof(msg),0,(sockaddr*)&address,&len);
	if(strcmp(msg,"OK\n")==0){
		//cout <<"msg is "<< msg<<endl;
		//cout << "Connect success " << UdpPort<<endl;
		return true;
	}else{
		return false;
	}

}
void UDPManager::UdpClose(){
	close(fd);
}

/*bool UDPManager::UdpSend(FileDataManager *fileDataManager){
	//发送UDP数据
	int segNum,rest;//rest还没有用
	fileDataManager->GetSendInfo(&segNum,&rest);
	cout << "segNum is "<<segNum<<endl;
	for(int i=0;i<segNum;i++){
		while(sendto(fd,(void*)(fileDataManager->data_buffer[i]),519,0,(sockaddr*)&address,sizeof(address))==-1){
			usleep(1000);//等待发送完毕
		}
		//int BN = fileDataManager->data_buffer[i][512]*100+fileDataManager->data_buffer[i][513];//得到数据所在的段号,BN从0算起
		//int size = fileDataManager->data_buffer[i][514]*100+fileDataManager->data_buffer[i][515];//得到数据段的大小
		//cout <<" size "<<size <<" BN "<<BN <<' '<<fileDataManager->SN<<' '<<fileDataManager->GN<<endl;
		usleep(9000);
	}
	return true;
}
*/
void UDPManager::UdpSendBlk(char* data_buffer){
	//char* compress_data[519];
	//com->zip(data_buffer);
	//cout << "zip success, data_size is " <<com->data_size <<endl;
	while(sendto(fd,(void*)data_buffer,519,0,(sockaddr*)&address,sizeof(address))==-1){
		usleep(1000);//等待发送完毕
	}
	//cout <<"send success" <<endl;
	usleep(5000);
}
