/*
 * FileManager.cpp
 *
 *  Created on: Oct 16, 2015
 *      Author: lagrange
 */

#include "FileManager.h"

FileManager::FileManager() {
	// TODO Auto-generated constructor stub
	dir=NULL;
	fileNum=0;
	dt =NULL;
	path =NULL;
	for(int i=0;i<200;i++){
		files[i].Filename="";
		files[i].Filepath="";
		files[i].Filesize=0;
	}
}

FileManager::~FileManager() {
	// TODO Auto-generated destructor stub
	dir=NULL;
	fileNum=0;
	dt=NULL;
	path =NULL;
}

void FileManager::setPath(char* path){
	this->path=path;
}

const char* FileManager::getPath(){
	return path;
}
void FileManager::SearchLocalFile(char *path,const char *file){
	struct stat s;
	DIR *dir;
	struct dirent *dt;
	char dirname[250];
	char filePath[250]="";
	memset(dirname,0,250*sizeof(char));
	strcpy(dirname,path);
	if(lstat(file,&s)<0){
		cout<<"lstat error"<<endl;
		return ;
	}
	if(S_ISDIR(s.st_mode)){ //是文件夹

		strcpy(dirname+strlen(dirname), file);
		strcpy(dirname+strlen(dirname), "/");
		if((dir = opendir(file)) == NULL){
			cout<<"opendir error"<<endl;
			return ;
		}

		if(chdir(file) < 0) {
			cout<<"chdir error"<<endl;
			return ;
		}
		while((dt = readdir(dir)) != NULL){
			if(dt->d_name[0] == '.'){
				continue;
			}
			SearchLocalFile(dirname, dt->d_name);
		}
		if(chdir("..") < 0){
			cout<<"chdir error\n"<<endl;
			return ;
		}
	}else{    //不是文件夹
		strcpy(filePath, "");
		strcat(filePath, dirname);
		strcat(filePath, file);
		if(lstat(filePath, &s) < 0){
			cout<< "lstat error"<<endl;  //验证
			return ;
		}
		//写入文件数组
		if(fileNum<200){
			files[fileNum].Filename=file;
			files[fileNum].Filepath=filePath;  //绝对路径
			files[fileNum].Filesize=(long)s.st_size;
			fileNum++;
		}
	}
	return ;
}

struct FileInfo* FileManager::getFileInfo(int i){
	if(i<fileNum){
		return &files[i];
	}else{
		return NULL;
	}
}
