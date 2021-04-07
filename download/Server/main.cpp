/*
 * main.cpp
 *
 *  Created on: Oct 15, 2015
 *      Author: lagrange
 */
//#include<iostream>
#include "GetIPAddress.h"
#include "ServerInclude.h"
#include "MySqlManager.h"
#include"FileManager.h"
#include"SocketManager.h"
#include"TCPManager.h"
#include"UDPManager.h"
#include"FileDataManager.h"
bool Server_Init(){
		char filepath[250]="/mnt/cephfs-demo";
	    //建立数据库管理员，连接数据库
	    MySqlManager *mysqlManager= new MySqlManager();
	    mysqlManager->MySqlInit();

	    //建立ip地址管理员，删除原数据库中的对应ip地址所在的项，等待更新
		GetIPAddress *ipAddress= new GetIPAddress();
	    ipAddress->getIpAddress();
	    string str;
	    for(int i=0;(ipAddress->showIP(str,i))!=false;i++){
	    	cout << str <<endl;
	    	mysqlManager->MySqlDelete(str);
	    }

	    //建立文件搜索模块，搜索共享目录下的共享文件
	    FileManager *fileManager = new FileManager();
	    fileManager->setPath(filepath);   //设置共享文件路径
	    fileManager->SearchLocalFile("",fileManager->getPath());  //查找共享文件

	    struct FileInfo* info=NULL;
	    //根据不同的ip地址，更新数据库（每一个ip地址看作是一个独立的主机进行更新）
	    int insert_num=0;
	    for(int i=0;ipAddress->showIP(str,i)!=false;i++){
	    	for(int j=0;(info=fileManager->getFileInfo(j))!=NULL;j++){
	    		if(mysqlManager->UpdateMySql(str,info)){
	    			insert_num++;//插入的数据库条目数量
	    		}
	    	}
	    }

	    mysqlManager->MySqlClose();  //初始化完毕，与数据库断开连接

	    cout << "Server finished Init " << insert_num <<" files are in the database" <<endl;
	    delete ipAddress;
	    delete fileManager;
	    delete mysqlManager;
	    return true;
}

int main(){

	Server_Init();

	//以下是TCP socket代码段
	struct sockaddr_in client_address;
	int client_sockfd;
	int pid;
	SocketManager* socketManager = new SocketManager();
	socketManager->SocketInit();
	signal(SIGCLD, SIG_IGN);//父进程忽略SIGCLD软中断而不必要 wait()，避免产生僵尸进程（或者可以通过waitpid信号量不阻塞的等待子进程退出）
	while(1){
		//得到tcp socket连接的socketid 和client信息
		client_sockfd=socketManager->SocketAccept(&client_address);
		//创建子进程处理TCP socket通信
		if((pid = fork())==0){
			TCPManager *tcpManager=new TCPManager();
			tcpManager->SetAttribute(client_sockfd,client_address);
			if(tcpManager->TCPNegotiate()){
				//tcp进行协商，成功即可进行udp传输
				//cout<<"from server: negotiate success"<<endl;
				int GN=tcpManager->GetGN();
				int SN = tcpManager->GetSN();
				int BN =tcpManager->GetBN();
				string fileName = tcpManager->GetFileName();
				int fileSize = tcpManager->GetFileSize();
				if((GN==0) && (SN ==0) && (BN==0)){
					//SN GN 同时为0说明是可用性检测，并没有分配传输任务，udp端口没有分配,直接退出子进程
					socketManager->Close_Socket(client_sockfd);   //协商完毕，关闭TCPsocekt
					exit(0);//退出子进程
				}
				//开始进行udp传输,SN GN不为0
				UDPManager *udpManager = new UDPManager();
				//以下为UDP连接的代码
				if(udpManager->UdpConnect()){
					//cout << "udp bind success"<<endl;
					tcpManager->SetUdpPort(udpManager->GetUDPPort());//设置udp端口
					if(tcpManager->SendUdpPort() == false){
						cout << "Send UdpPort fail" <<endl;
						exit(0);
					}
					socketManager->Close_Socket(client_sockfd);   //协商完毕，关闭TCPsocket
					if(udpManager->UDPTest()){
						//以下开始进行文件的传输
						FileDataManager *fileDataManager = new FileDataManager();
						fileDataManager->SetAttribute(SN,GN,fileName,fileSize);//配置参数
						if(SN==0 && GN==0 && (BN!=2049) && (BN!=0)){   //需要重传
							fileDataManager->ResendData(tcpManager->resentInfos,udpManager);//把需重传的节点传进去
						}else{
							fileDataManager->DoFileProcess(udpManager);//完成读取文件和文件校检
						}
						//udpManager->UdpSend(fileDataManager);//发送数据
					}
				}
				udpManager->UdpClose();
			}
			exit(0);//退出子进程
		}else{
			//父进程直接关闭socket,继续监听
			socketManager->Close_Socket(client_sockfd);
		}
	}
	return 0;
}



