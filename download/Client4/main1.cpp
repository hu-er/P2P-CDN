/*
 * main.cpp
 *
 *  Created on: Oct 19, 2015
 *      Author: lagrange
 */
#include"ClientInclude.h"
#include"MySqlManager.h"
#include"TCPManager.h"
#include"FileManager.h"
#include"UDPManager.h"
#include"FileDataReceiver.h"
#include"GetIPAddress.h"
//创建线程锁，用于在任务分配过程中不会出错
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread_func(void *fileManager);
void OneTimeTransit(void *fileManager,FileDataReceiver *fileDataReceiver,AccessPoint accPoint_info[],int TotalAccess);//修改
void CheckTCPNode(AccessPoint accPoint_info[],int TotalAccess);//修改
void ReSentData(FileDataReceiver* fileDataReceiver,int cutTime,AccessPoint accPoint_info[],int TotalAccess);//修改
int getfile(FileManager *fileManager,AccessPoint accPoint_info[],int TotalAccess);

int main(){
        AccessPoint accPoint_info[30];  //存放可获得对应文件的节点信息
        int TotalAccess;    //一共可用的节点数
        AccessPoint accPoint_info2[30];  //存放可获得对应文件的节点信息
        int TotalAccess2;    //一共可用的节点数
        string filename2="a";
        int filesize2=0;
        int filesize=0;
        string filename="b";
	MySqlManager* mysqlManager = new MySqlManager();
	mysqlManager->MySql_Init();
	mysqlManager->getFilename(filename);
	TotalAccess=mysqlManager->MySql_Query(accPoint_info);
	if(TotalAccess == 0){
		cout <<"no resource can use!!!!"<<endl;
		return 0;
	}
        mysqlManager->getFilesize(accPoint_info[0].FileSize);
	for(int i=0;i<TotalAccess;i++){
		cout<<accPoint_info[i].FileName<<endl;
		cout<<accPoint_info[i].FilePath<<endl;
		cout<<accPoint_info[i].FileSize<<endl;
		cout<<accPoint_info[i].ipAddress<<endl;
	}
        mysqlManager->getFilename(filename2);
	TotalAccess2=mysqlManager->MySql_Query(accPoint_info2);
	if(TotalAccess2 == 0){
		cout <<"no resource can use!!!!"<<endl;
		return 0;
	}
        mysqlManager->getFilesize(accPoint_info2[0].FileSize);
        for(int i=0;i<TotalAccess2;i++){
		cout<<accPoint_info2[i].FileName<<endl;
		cout<<accPoint_info2[i].FilePath<<endl;
		cout<<accPoint_info2[i].FileSize<<endl;
		cout<<accPoint_info2[i].ipAddress<<endl;
	}
        filesize=accPoint_info[0].FileSize;
        filesize2=accPoint_info2[0].FileSize;
        FileManager *fileManager = new FileManager(filename,filesize);
        FileManager *fileManager2 = new FileManager(filename2,filesize2);

	mysqlManager->MySql_Close();
        getfile(fileManager,accPoint_info,TotalAccess);
        getfile(fileManager2,accPoint_info2,TotalAccess2);
        delete mysqlManager;
        return 0;

}

int getfile(FileManager *fileManager,AccessPoint accPoint_info[],int TotalAccess){
        //string filename="test";
        //int filesize=5;

	
	//先进行节点可用性的检查,GN SN 均等于0的传输连接,表示进行可用性的检查连接
	CheckTCPNode(accPoint_info,TotalAccess);
	int curTime=0;
	int totalTime = fileManager->GetTotalTime();
	//从1开始算传输轮次
	for(curTime =1;curTime<=totalTime;++curTime){
		FileDataReceiver *fileDataReceiver = new FileDataReceiver();//文件接受者，多个线程公用一个缓冲区
		fileDataReceiver->SetAttribute(fileManager->GetFileName(),fileManager->GetFileSize(),totalTime);
		OneTimeTransit(fileManager,fileDataReceiver,accPoint_info,TotalAccess);      //完成一轮传输
		cout << "finish onetimeTransit"<<endl;
		//接下来进行校检和写进磁盘
		if(curTime == totalTime){//最后一轮传输
			fileDataReceiver->CheckData(true);//CheckData返回需要重传的节点
			while(!fileDataReceiver->resentInfos.empty()){   //当重传结构体不为空时，说明有需要重传的数据
				ReSentData(fileDataReceiver,curTime,accPoint_info,TotalAccess);//重传数据
				fileDataReceiver->CheckData(true);//重新检查收到的文件
			}
		}else{
			fileDataReceiver->CheckData(false);
			while(!fileDataReceiver->resentInfos.empty()){   //当重传结构体不为空时，说明有需要重传的数据
				ReSentData(fileDataReceiver,curTime,accPoint_info,TotalAccess);//重传数据
				fileDataReceiver->CheckData(false);//重新检查收到的文件
			}
		}
		fileDataReceiver->WriteFile();//把数据写入文件中
		fileDataReceiver->~FileDataReceiver();//释放内存空间
	}
	return 0;
}

void *Thread_CheckNode(void* i){
	AccessPoint acc_point = *((AccessPoint*)i);
	TCPManager *tcpManager = new TCPManager(acc_point);
	if(tcpManager->SocketConnect()){
		tcpManager->SetAttribute(0,0,0);  //发送SN=0，GN=0代表是可用性查询
		int res = tcpManager->TCPNegotiate();
		if(res==1){
			cout << acc_point.ipAddress<<"can use"<<endl;
			//sleep(10);
			acc_point.Useable=true;
		}else{
			cout << acc_point.ipAddress<<" can't use"<<endl;
			acc_point.Useable=false;//此处只考虑了res = -2 的情况,直接把节点视为不可用(默认消息格式不出错)
		}
		tcpManager->TCPClose();
		//delete tcpManager;
	}else{
		cout << acc_point.ipAddress<<" can't use"<<endl;
		acc_point.Useable=false;//connect error means this node can't use
	}
	return i;
}

//函数用于检测节点可用性
void CheckTCPNode(AccessPoint accPoint_info[],int TotalAccess){
	pthread_t Thread_Check[TotalAccess];
	for(int i=0;i<TotalAccess;i++){
		pthread_create(&Thread_Check[i],NULL,Thread_CheckNode,(void*)(accPoint_info+i));
		pthread_join(Thread_Check[i],NULL);
	}
}

void OneTimeTransit(void *fileManager,FileDataReceiver *fileDataReceiver,AccessPoint accPoint_info[],int TotalAccess){
	//创建子线程，并且进行传递参数，进行TCP连接,一轮传输进行144次的socket连接
	pthread_t ThreadPool[36*4];     //每次传输36个文件段，则应该建立144个子线程
	AccessInfo accInfo[144];//通过结构体传输多个参数
	int j=0;//用于计算任务分配给哪个节点
	for(int i=0;i<144;++i){
		if(j>=TotalAccess){ j=0;}
		while(j<TotalAccess){
			if(accPoint_info[j].Useable == true){
				break;
			}else{
				++j;
				if(j == TotalAccess) {j=0;}
			}
		}
		//struct AccessInfo accInfo;
		//memset(&accInfo,0,sizeof(AccessInfo));
		accInfo[i].fileManager=(void*)fileManager;//fileManager的指针
		accInfo[i].acc_point = accPoint_info[j];//可用节点信息
		accInfo[i].fileDataRecevier = (void*)fileDataReceiver;//缓冲区所在对象的指针
		pthread_create(&ThreadPool[i],NULL,thread_func,(void*)&(accInfo[i]));//创建新的子线程
		cout << "TotalAccess is "<< TotalAccess <<" j is "<<j<<endl;
		++j;//跳到下一个节点
	}
	for(int i=0;i<144;++i){
		pthread_join(ThreadPool[i],NULL);//等待线程退出
	}
}
void *thread_func(void *accInfo){
	FileManager *fileManager = (FileManager*)((struct AccessInfo*)accInfo)->fileManager;
	FileDataReceiver *fileDataReceiver = (FileDataReceiver*)((struct AccessInfo*)accInfo)->fileDataRecevier;
	AccessPoint acc_point=((struct AccessInfo*)accInfo)->acc_point;
        //int i = ((struct AccessInfo*)accInfo)->i;//i代表第几个可用节点
	 //以下代码应该在子线程中运行，通过互斥锁进行任务分配
	TCPManager *tcpManager = new TCPManager(acc_point);
	if(tcpManager->SocketConnect()){
		pthread_mutex_lock(&mutex);//加锁，防止多个线程同时分配SN GN
		int SN = fileManager->FileDivideSeg();
		int GN = fileManager->FileDivideGroup();
		if(SN!=-1){
			tcpManager->SetAttribute(SN,GN,2049);//SN不等于-1，分配SN GN,BN为2049，表示传输整个组
		}else{
			//SN=-1 分配已经完毕，退出函数
			pthread_mutex_unlock(&mutex); //解锁
			tcpManager->TCPClose();//TCP连接结束
			return (void*)accInfo;
		}
		int res = tcpManager->TCPNegotiate();//tcp协商传输的文件段
		if(1==res){
			//cout<<"from client: negotiate success"<<endl;
			fileManager->HandOutSuccess();   //分配任务成功，更新GN SN
		}else{
			cout << "from client :negotiate fail " <<res<<endl;
			tcpManager->TCPClose();
			pthread_mutex_unlock(&mutex); //解锁
			pthread_exit((void*) accInfo);//退出子线程
			return (void*) accInfo;
		}
		pthread_mutex_unlock(&mutex); //解锁

		//以下代码是UDP传输的代码
		UDPManager *udpManager = new UDPManager();
		//配置ip地址，配置udp端口
		int port = tcpManager->GetUdpPort();
		udpManager->SetAttribute(acc_point.ipAddress,tcpManager->GetUdpPort());//配置ip地址和端口
		tcpManager->TCPClose();//TCP连接结束
		if(udpManager->UdpConnect()){
			//UDP连接成功，开始传输数据
			int segNum=0;//此次传输应该接收多少个512B的数据
			int Total_SN = (int)ceil((double)fileManager->GetFileSize() / (SEGSIZE)); //一共有多少个3M文件段
			if(SN == Total_SN){  //最后一个文件段
				int remainsize = fileManager->GetFileSize()-(SN-1)*SEGSIZE;//剩余文件大小
				segNum = (int)ceil((double)remainsize/512);//一共分成多少个512B
				segNum = (int)ceil((double)segNum/3);//每组发送多少段512B,向上取整，有可能收到一个空包
			}else{   //不是最后一个文件段
				segNum = BLKNUM;//应该接收2048个512B
			}
			//cout<< "send OK"<<endl;
			int segNow = 0;//当前接收到的个数
			fd_set readfds;
			struct timeval tv;
			int fd = udpManager->Getfd();
			int lost_count =0;
			while(segNow < segNum){//0~2047
				FD_ZERO(&readfds);
				FD_SET(fd,&readfds);
				tv.tv_sec=2;
				tv.tv_usec=0;
				select(fd+1,&readfds,NULL,NULL,&tv);//配置等待时间，让线程不阻塞
				if(FD_ISSET(fd, &readfds)){   //当前可读
					if(udpManager->UdpRecv()){ //UDP成功收到数据
						fileDataReceiver->SetFileData(udpManager->GetDataBuf());//得到缓冲区指针,把数据放到对应的数组中
						++segNow;
						lost_count =0;//读到数据重新将计数器清零
					}
				}else{//读不到数据
					//cout<< "UDP read error "<<"SN is "<<SN <<" GN is "<< GN<< "segNow is "<<segNow<<endl;
					++lost_count;
					if(lost_count == 3){
						cout << "3 time cant read !!!!!!!!!!!!!"<<endl;
						break;   //连续三次读不到数据，跳出循环
					}
				}
			}
			cout << "segNum is "<< segNum << "and segNow is "<<segNow<< " port is " <<port<<endl;
			udpManager->UdpClose();//关闭udp socket
		}
	}else{
		cout<<"TCP connect error"<<endl;
	}
	pthread_exit((void*) accInfo);//退出子线程
	return (void*) accInfo;
}

