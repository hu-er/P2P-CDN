/*
 * FileDataManager.h
 *
 *  Created on: Nov 5, 2015
 *      Author: lagrange
 */

#ifndef FILEDATAMANAGER_H_
#define FILEDATAMANAGER_H_
#include"ServerInclude.h"

#include"UDPManager.h"
#define SEGSIZE 1024*1024*3
#define SEG_OF_GROUP 4
#define SEGNUM  36  //每轮传输多少个3M文件段

class FileDataManager {
public:
	FileDataManager();
	virtual ~FileDataManager();
	void SetAttribute(int SN,int GN,string filename,int filesize);
	bool GetDataInFile(UDPManager* udpManager);
	bool DoRaidCode(UDPManager* udpManager);
	void  DoFileProcess(UDPManager* udpManager);
	void GetSendInfo(int *segNum,int *rest);
	char data_buffer[2048][519];   //1M数据有2048组512B，每次发送512B
	void ResendData(vector<ResentInfo> &resentInfos,UDPManager* udpManager);
private:
	int SN;
	int GN;
	int Total_SN;
	string filename;
	int filesize;
	int segNum;  //代表此次发送应该发几个512B数据，如果不是最后一段，则为2048
	ssize_t rest;// 代表最后一个512B的空间中，存放的数据量，若不是最后一段，则为512B
};

#endif /* FILEDATAMANAGER_H_ */
