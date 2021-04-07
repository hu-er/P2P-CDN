/*
 * ReSent.cpp
 *
 *  Created on: Nov 25, 2015
 *      Author: lagrange
 */

#include"ClientInclude.h"
#include"TCPManager.h"
#include"FileManager.h"
#include"UDPManager.h"
#include"FileDataReceiver.h"

//extern AccessPoint accPoint_info[30];  //可用节点
//extern int TotalAccess;    //一共可用的节点数

void *thread_funcRe(void *accInfo);
void ReSentData(FileDataReceiver* fileDataReceiver,int curTime,AccessPoint accPoint_info[],int TotalAccess){
	pthread_t ThreadPool[36*4];  //一次最多连接144个节点
	unsigned int count =0;
	vector<ResentInfo> &resentInfos = fileDataReceiver->resentInfos;
	struct ResentData accInfo[144];	//通过结构体传输多个参数
	for(unsigned int i=0;i<resentInfos.size();i++){
		resentInfos[i].segNum+=(curTime-1)*36;  //转为当前的SN
		cout << "resent segNum is "<<resentInfos[i].segNum<<endl;
		cout << "resent GN is "<<resentInfos[i].GN<<endl;
		cout << "resent BN is "<<resentInfos[i].BN<<endl;
		cout << "resentInfos size is" << resentInfos.size()<<endl;
	}
	while(count < resentInfos.size()){
		int j=0;//用于计算任务分配给哪个节点
		int pthread_count=0;
		for(int i=0;i<144;++i){
			if(count >= resentInfos.size()){
				break;
			}
			if(j>=TotalAccess){ j=0;}
			while(j<TotalAccess){
				if(accPoint_info[j].Useable == true){
					break;
				}else{
					++j;
					if(j == TotalAccess) {j=0;}
				}
			}

			accInfo[i].acc_pointre =accPoint_info[j];//可用节点的编号
			//cout << "now j is "<< j<<" i is"<<i<<endl;
			accInfo[i].fileDataRecevier = (void*)fileDataReceiver;//缓冲区所在对象的指针
			accInfo[i].index=count;//确定下标
			pthread_create(&ThreadPool[i],NULL,thread_funcRe,(void*)&(accInfo[i]));//创建新的子线程
			pthread_count++;
			//cout << "resent TotalAccess is "<< TotalAccess <<" j is "<<j<<endl;
			++j;//跳到下一个节点
			count+=BLKNUM;//一次最多重传2048个文件块
		}
		for(int i=0;i<pthread_count;++i){
			pthread_join(ThreadPool[i],NULL);//等待线程退出
		}
	}
}

void *thread_funcRe(void *accInfo){
	AccessPoint acc_pointre = ((struct ResentData*)accInfo)->acc_pointre;//代表可用节点
	FileDataReceiver *fileDataReceiver = (FileDataReceiver*)((struct ResentData*)accInfo)->fileDataRecevier;
	vector<ResentInfo> &resentInfos = fileDataReceiver->resentInfos;
	unsigned int index = ((ResentData*)accInfo)->index;//重传下标
	TCPManager *tcpManager = new TCPManager(acc_pointre);
	//cout << "we got this,j is " << i <<endl;
	if(tcpManager->SocketConnect()){
		int num = resentInfos.size()-index;
		if(num>BLKNUM){
			num = BLKNUM;
		}
		tcpManager->SetAttribute(0,0,num);//SN,GN为0,num此时为重传的节点数
		for(int j=0;j<num;++j){  //把重传节点传进去
			tcpManager->GetResendInfo(resentInfos[index+j].segNum,resentInfos[index+j].GN,resentInfos[index+j].BN);
		}
		int res = tcpManager->TCPNegotiate();//tcp协商传输的文件段
		if(res != 1){
			cout << "from client :resent negotiate fail " <<res<<endl;
			tcpManager->TCPClose();
			pthread_exit((void*) accInfo);//退出子线程
			return (void*) accInfo;
		}
		//以下代码是UDP传输的代码
		UDPManager *udpManager = new UDPManager();
		//配置ip地址，配置udp端口
		udpManager->SetAttribute(acc_pointre.ipAddress,tcpManager->GetUdpPort());//配置ip地址和端口
		tcpManager->TCPClose();//TCP连接结束
		if(udpManager->UdpConnect()){
			int fd = udpManager->Getfd();
			int segNow = 0;//当前接收到的个数
			int lost_count =0;
			fd_set readfds;
			struct timeval tv;
			while(segNow < num){
				FD_ZERO(&readfds);
				FD_SET(fd,&readfds);
				tv.tv_sec=2;
				tv.tv_usec=0;
				select(fd+1,&readfds,NULL,NULL,&tv);//配置等待时间，让线程不阻塞
				if(FD_ISSET(fd, &readfds)){   //当前可读
					if(udpManager->UdpRecv()){ //UDP成功收到数据
						fileDataReceiver->SetFileData(udpManager->GetDataBuf());//得到缓冲区指针,把数据放到对应的数组中
						++segNow;
						lost_count = 0;//读到数据重新将计数器清零
					}
				}else{//读不到数据
					cout<< "UDP resent read error"<<endl;
					++lost_count;
					if(lost_count == 3){
						cout << "3 time cant read !!!!!!!!!!!!!"<<endl;
						break;   //连续三次读不到数据，跳出循环
					}
				}
			}
			cout << "resent num is "<< num << "and segNow is "<<segNow <<endl;
		}
	}else{
		cout<<"TCP connect error"<<endl;
	}
	pthread_exit((void*) accInfo);//退出子线程
	return (void*) accInfo;
}
