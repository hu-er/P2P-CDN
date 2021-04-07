/*
 * FileManager.h
 *
 *  Created on: Oct 16, 2015
 *      Author: lagrange
 */
#include"ServerInclude.h"
#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

struct FileInfo{
	string Filename;
	string Filepath;
	long Filesize;
};

class FileManager {
public:
	FileManager();
	virtual ~FileManager();
	void SearchLocalFile(char *path, const char *file);
	void CheckFile();
	void setPath(char* path);
	const char* getPath();
	struct FileInfo* getFileInfo(int i);
private:
	char* path;
	struct stat s;
	DIR *dir;
	struct dirent *dt;
	struct FileInfo files[200];
	//vector<FileInfo> files;
	int fileNum;
};

#endif /* FILEMANAGER_H_ */
