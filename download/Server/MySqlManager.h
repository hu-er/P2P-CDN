/*
 * MySqlManager.h
 *
 *  Created on: Oct 15, 2015
 *      Author: lagrange
 */
#include"ServerInclude.h"
#include"FileManager.h"
#ifndef MYSQLMANAGER_H_
#define MYSQLMANAGER_H_

class MySqlManager {
public:
	MySqlManager();
	virtual ~MySqlManager();
	bool MySqlInit();
	//bool SearchLocalFile();
	bool UpdateMySql(string ipAddress,FileInfo* info);
	bool MySqlDelete(string ipAddress);
	void MySqlClose();
private:
	struct sockaddr_in address;
	int fd;
	ssize_t writen(void *vptr,size_t n);
	ssize_t readLine(void *vptr,size_t maxlen);
	void TCPClose();
};

#endif /* MYSQLMANAGER_H_ */
