/*
 * FileDataReceiver.h
 *
 *  Created on: Nov 11, 2015
 *      Author: lagrange
 */

#ifndef FILEDATARECEIVER_H_
#define FILEDATARECEIVER_H_
#include "ClientInclude.h"
class FileDataReceiver {
public:
	FileDataReceiver();
	virtual ~FileDataReceiver();
	void SetFileData(char *data_buffer);
	void CheckData(bool last);
	void DeCodeRaid(int segNum,int BN,int GN_lost);
	void WriteFile();
	void SetAttribute(string fileName,int fileSize,int TotalTime);
	vector<ResentInfo> resentInfos;
private:
	char filedata[36][3*2048*512];//文件存储的缓存区
	char raiddata[36][2048*512];//raid校检码的缓存区
	bool data_flag[36][3*2048];//标记文件数据是否收到
	bool raid_flag[36][2048];//标记raid数据是否收到
	string fileName;
	int fileSize;
	int TotalTime;
	bool last;//标记是否为最后一次传输
};

#endif /* FILEDATARECEIVER_H_ */
