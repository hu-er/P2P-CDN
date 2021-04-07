/*
 * ClientInclude.h
 *
 *  Created on: Oct 19, 2015
 *      Author: lagrange
 */

#ifndef CLIENTINCLUDE_H_
#define CLIENTINCLUDE_H_

#include<arpa/inet.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<iostream>
#include<string>
#include<mysql/mysql.h>
#include<sys/types.h>
#include<errno.h>
#include<cmath>
#include<stdlib.h>
#include<dirent.h>
#include<netinet/in.h>
#include<stdio.h>
#include<pthread.h>
#include<stdarg.h>
#include<unistd.h>
#include<vector>
#include<fstream>
#include<stddef.h>
#include<ifaddrs.h>
#include<sys/time.h>
#include<sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include<errno.h>
#include <sstream>
#include <cstdlib>
using namespace std;

#define SEGSIZE 1024*1024*3
#define SEG_OF_GROUP 4
#define SEGNUM  36  //每轮传输多少个3M文件段
#define BLKNUM 2048 //每一次一个节点传输有多少个文件段
#define PATH "/mnt/cephfs-demo/"
struct AccessPoint{
	string FileName;
	string FilePath;
	string ipAddress;
	long FileSize;
	bool Useable;
};

struct AccessInfo{
	void *fileManager;
	AccessPoint acc_point;
	void *fileDataRecevier;
};

struct ResentInfo{
	int segNum;//在此次传输中属于第几个3M文件段，要实现重传的话要根据segNum计算SN
	int GN;
	int BN;
};

struct ResentData{    //重传结构体
	void *fileDataRecevier;
	AccessPoint acc_pointre;
	unsigned int index;//确定重传的文件块下标
};

#endif /* CLIENTINCLUDE_H_ */
