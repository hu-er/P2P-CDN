/*
 * FileManager.h
 *
 *  Created on: Oct 26, 2015
 *      Author: lagrange
 */

#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_
#include "ClientInclude.h"


class FileManager {
public:
	FileManager(string filename,int filesize);
	virtual ~FileManager();
	string GetFileName();
	int FileDivideSeg();
	int FileDivideGroup();
	int GetFileSize();
	void HandOutSuccess();
	int GetTotalTime();
private:
	int SN;
	int GN;
	string fileName;
	int fileSize;
	int cur_SN;
};

#endif /* FILEMANAGER_H_ */
