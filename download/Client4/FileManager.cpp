/*
 * FileManager.cpp
 *
 *  Created on: Oct 26, 2015
 *      Author: lagrange
 */

#include "FileManager.h"


FileManager::FileManager(string filename,int filesize)  {
	// TODO Auto-generated constructor stub
	fileName = filename;
        fileSize = filesize;
       SN = (int)ceil((double)fileSize / (SEGSIZE)); //一共有多少个3M文件段
                //每一段文件段分成4组
       GN = 1;//从1开始分组
       cur_SN=1;//从1开始分段
}

FileManager::~FileManager() {
	// TODO Auto-generated destructor stub
}

string FileManager::GetFileName(){
	return fileName;
}

int FileManager::GetFileSize(){
	return fileSize;
}
//返回文件段号 怎么向外传递返回的是最后一个文件段？
int FileManager::FileDivideSeg(){
	if(cur_SN<=SN){
		return cur_SN;
	}else{
		return -1;    //任务已经分配完毕，返回负值
	}
}

//返回组号
int FileManager::FileDivideGroup(){
	return GN;
}
 //分配任务成功，把GN+1,跳到下一个组
void FileManager::HandOutSuccess(){
	if(GN==SEG_OF_GROUP){  //前一个文件段已经分配完毕，开始下一个文件段的分配
		GN=1;
		++cur_SN;
	}else{
		++GN;
	}
}

int FileManager::GetTotalTime(){
	int totalTime = (int)ceil((float)SN / SEGNUM);  //一共要经过几轮传输
	cout << "totalTime " << totalTime<<endl;
	return totalTime;
}
