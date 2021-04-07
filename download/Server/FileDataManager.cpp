/*
 * FileDataManager.cpp
 *
 *  Created on: Nov 5, 2015
 *      Author: lagrange
 */

#include "FileDataManager.h"
FileDataManager::FileDataManager() {
	// TODO Auto-generated constructor stub
	SN=0;
	GN=0;
	Total_SN=0;
	filename="";
	filesize=0;
	memset(data_buffer,0,2048*519*sizeof(char));
	segNum=0;
	rest =0;
}

FileDataManager::~FileDataManager() {
	// TODO Auto-generated destructor stub
	SN=0;
	GN=0;
	Total_SN=0;
	filename="";
	filesize=0;
	memset(data_buffer,0,2048*519*sizeof(char));
	segNum=0;
	rest =0;
}

void FileDataManager::SetAttribute(int SN,int GN,string filename,int filesize){
	this->SN=SN;
	this->GN=GN;
	this->filename=filename;
	this->filesize=filesize;
	Total_SN = (int)ceil((double)filesize / (SEGSIZE)); //一共有多少个3M文件段
}


void FileDataManager::DoFileProcess(UDPManager* udpManager){    //处理文件的事情
	if(GN == SEG_OF_GROUP) { 	//SEG_OF_GROUP = 4 如果得到的是校检段
		if(DoRaidCode(udpManager)){    //调用校验函数
			cout << "Get RAID block,and RAID success" << endl;
		}
	}else{
		GetDataInFile(udpManager);//不是校验段，直接得到对应的文件块
	}
}

void FileDataManager::GetSendInfo(int *segNum,int *rest){
	*segNum = this->segNum;//segNum-1 才是填充的位置
	*rest=this->rest;
}

bool FileDataManager::GetDataInFile(UDPManager* udpManager){
	ifstream file_in(filename.c_str(),ios::in|ios::binary);//二进制方式打开文件
	int offset = (SN-1)*1024*1024*3+(GN-1)*512;//计算文件指针位置
	file_in.seekg(offset,ios::beg);//移动文件指针
	if(SN == Total_SN){   //最后一个文件段
		//segNum-1 代表一直填充到第几个文件段(segNum=0代表整个数据段都是空白)，rest代表最后一个文件段的大小
		int remainsize = filesize - (SN-1)*1024*1024*3;//剩余文件大小
		segNum = (int)ceil((double)remainsize/512);//一共分成多少个512B
		segNum = (int)ceil((double)segNum/3);//每组发送多少段512B,向上取整，有可能发送一个空包
		for(int i = 0;i<segNum;++i){
			file_in.read(data_buffer[i],512);
			rest = file_in.gcount();//读到的文件字节数小于512，说明已经读到了文件尾部
			if(!file_in.eof() || rest>0){  //如果移动指针后还没到文件末尾,并且输入流正常
				data_buffer[i][512] = (char)(i/100);  //用两个字节存储段号i
				data_buffer[i][513] = (char)(i%100);
				data_buffer[i][514] = (char)(rest/100);//用两个字节存储数据段大小
				data_buffer[i][515] = (char)(rest%100);
				data_buffer[i][516] = (char)(SN/100);
				data_buffer[i][517] = (char)(SN%100);//存放SN
				data_buffer[i][518] = (char)GN;//存放GN
				udpManager->UdpSendBlk(data_buffer[i]);
				file_in.seekg(512*2,ios::cur);//移到下一个位置
			}else{  //已经到文件结尾
				//cout << "rest is "<<rest <<"GN is "<<GN <<endl;
				cout << "reach the end of file,the segNum is  "<<segNum<< "i is "<<i<<"GN is"<<GN<<endl;
				memset(data_buffer[i],0,515*sizeof(char));//填充一个空包
				data_buffer[i][512] = (char)(i/100);  //用两个字节存储段号i
				data_buffer[i][513] = (char)(i%100);
				data_buffer[i][514] = 0;//用两个字节存储数据段大小
				data_buffer[i][515] = 0;
				data_buffer[i][516] = (char)(SN/100);
				data_buffer[i][517] = (char)(SN%100);//存放SN
				data_buffer[i][518] = (char)GN;//存放GN
				udpManager->UdpSendBlk(data_buffer[i]);
			}
		}
	}else{      //不是最后一个文件段,文件指针每次移动512B
		segNum=2048;
		rest =512;
		for(int j=0;j<2048;j++){
			file_in.read(data_buffer[j],512);//读取数据
			data_buffer[j][512] = (char)(j/100);  //用两个字节存储段号j
			data_buffer[j][513] = (char)(j%100);
			data_buffer[j][514] = (char)(512/100);//用两个字节存储数据段大小
			data_buffer[j][515] = (char)(512%100);
			data_buffer[j][516] = (char)(SN/100);
			data_buffer[j][517] = (char)(SN%100);
			data_buffer[j][518] = (char)GN;//存放GN
			udpManager->UdpSendBlk(data_buffer[j]);
			file_in.seekg(512*2,ios::cur);//移到下一个位置
		}
	}
	file_in.close();
	return true;
}

bool FileDataManager::DoRaidCode(UDPManager* udpManager){
	char buffer[3][512];
	memset(buffer,0,3*512*sizeof(char));//初始化缓存区
	ifstream file_in(filename.c_str(),ios::in|ios::binary);//二进制方式打开文件
	int offset = (SN-1)*1024*1024*3;//计算文件指针位置
	file_in.seekg(offset,ios::beg);//移动文件指针
	if(SN == Total_SN){   //最后一个文件段，校检段大小一定是512B的整倍数
		int remainsize = filesize - (SN-1)*1024*1024*3;//剩余文件大小
		segNum = (int)ceil((double)remainsize/512);//一共分成多少个512B
		segNum = (int)ceil((double)segNum/3);//每组发送多少段512B,向上取整，有可能发送一个空包
		for(int i = 0,j=0;i<segNum; ){
			if(!file_in.eof()){  //如果移动指针后还没到文件末尾
				file_in.read(buffer[j],512);
				rest = file_in.gcount();//读到的文件字节数小于512，说明已经读到了文件尾部
				if(j==2){
					j=-1;//因为后面还得++j
					for(int k=0;k<512;k++){
						data_buffer[i][k]=buffer[0][k]^buffer[1][k]^buffer[2][k];//按位异或
					}
					data_buffer[i][512] = (char)(i/100);  //用两个字节存储段号i
					data_buffer[i][513] = (char)(i%100);
					data_buffer[i][514] = (char)(512/100);//用两个字节存储数据段大小
					data_buffer[i][515] = (char)(512%100);
					data_buffer[i][516] = (char)(SN/100);
					data_buffer[i][517] = (char)(SN%100);
					data_buffer[i][518] = (char)GN;//存放GN
					udpManager->UdpSendBlk(data_buffer[i]);
					memset(buffer,0,3*512*sizeof(char));//初始化缓存区
					++i;//跳到下一段
				}
				++j;
			}else{  //已经到文件结尾
				cout << "reach the end of file,the segNum is  "<<segNum<< "i is "<<i<<"GN is"<<GN<<"j is"<<j<<endl;
				for(int k=0;k<512;k++){
					data_buffer[i][k]=buffer[0][k]^buffer[1][k]^buffer[2][k];//按位异或
				}
				data_buffer[i][512] = (char)(i/100);  //用两个字节存储段号i
				data_buffer[i][513] = (char)(i%100);
				data_buffer[i][514] = (char)(512/100);//用两个字节存储数据段大小
				data_buffer[i][515] = (char)(512%100);
				data_buffer[i][516] = (char)(SN/100);
				data_buffer[i][517] = (char)(SN%100);//存放SN
				data_buffer[i][518] = (char)GN;//存放GN
				udpManager->UdpSendBlk(data_buffer[i]);
				++i;//跳出循环
			}
		}
	}else{     //不是最后一个文件段
		segNum=2048;
		rest =512;
		for(int i=0;i<2048;i++){
			for(int j=0;j<3;j++){
				file_in.read(buffer[j],512);//读取数据
			}
			for(int k=0;k<512;k++){
				data_buffer[i][k]=buffer[0][k]^buffer[1][k]^buffer[2][k];//按位异或
			}
			data_buffer[i][512] = (char)(i/100);  //用两个字节存储段号i
			data_buffer[i][513] = (char)(i%100);
			data_buffer[i][514] = (char)(512/100);//用两个字节存储数据段大小
			data_buffer[i][515] = (char)(512%100);
			data_buffer[i][516] = (char)(SN/100);
			data_buffer[i][517] = (char)(SN%100);//存放SN
			data_buffer[i][518] = (char)GN;//存放GN
			udpManager->UdpSendBlk(data_buffer[i]);
		}
	}
	file_in.close();
	return true;
}

void FileDataManager::ResendData(vector<ResentInfo> &resentInfos,UDPManager* udpManager){
	ifstream file_in(filename.c_str(),ios::in|ios::binary);//二进制方式打开文件
	segNum =resentInfos.size();  //先把发送的数据量设好
	for(unsigned int i=0;i<resentInfos.size();++i){
		int SN_resent = resentInfos[i].SN;
		int GN_resent = resentInfos[i].GN;
		int BN_resent = resentInfos[i].BN;
		cout <<"resent SN is " <<SN_resent<<" GN is "<<GN_resent<<" BN is "<<BN_resent<<endl;
		if(GN_resent == SEG_OF_GROUP){   //传输的是校检段
			int offset = (SN_resent-1)*1024*1024*3+3*BN_resent;//计算文件指针位置
			file_in.seekg(offset,ios::beg);//移动文件指针
			char buffer[3][512];
			memset(buffer,0,3*512*sizeof(char));//初始化缓存区
			for(int j =0;j<3;++j){
				file_in.read(buffer[j],512);
				rest = file_in.gcount();
				if(file_in.eof()){  //读到了文件末尾
					break;
				}
			}
			for(int k=0;k<512;k++){
				data_buffer[i][k]=buffer[0][k]^buffer[1][k]^buffer[2][k];//按位异或
			}
			data_buffer[i][512] = (char)(BN_resent/100);  //用两个字节存储段号BN
			data_buffer[i][513] = (char)(BN_resent%100);
			data_buffer[i][514] = (char)(512/100);//用两个字节存储数据段大小
			data_buffer[i][515] = (char)(512%100);
			data_buffer[i][516] = (char)(SN_resent/100);
			data_buffer[i][517] = (char)(SN_resent%100);//存放SN
			data_buffer[i][518] = (char)GN_resent;//存放GN
			udpManager->UdpSendBlk(data_buffer[i]);
		}else{   //传输的不是校检段
			int offset = (SN_resent-1)*1024*1024*3+3*BN_resent+(GN_resent-1)*512;//计算文件指针位置
			file_in.seekg(offset,ios::beg);//移动文件指针
			file_in.read(data_buffer[i],512);
			rest = file_in.gcount();
			data_buffer[i][512] = (char)(BN_resent/100);  //用两个字节存储段号BN
			data_buffer[i][513] = (char)(BN_resent%100);
			data_buffer[i][514] = (char)(rest/100);//用两个字节存储数据段大小
			data_buffer[i][515] = (char)(rest%100);
			data_buffer[i][516] = (char)(SN_resent/100);
			data_buffer[i][517] = (char)(SN_resent%100);//存放SN
			data_buffer[i][518] = (char)GN_resent;//存放GN
			udpManager->UdpSendBlk(data_buffer[i]);
		}
	}
}
