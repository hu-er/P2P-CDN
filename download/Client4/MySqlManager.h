/*
 * MySqlManager.h
 *
 *  Created on: Oct 19, 2015
 *      Author: lagrange
 */
#include"ClientInclude.h"

#ifndef MYSQLMANAGER_H_
#define MYSQLMANAGER_H_



class MySqlManager {
public:
	MySqlManager();
	virtual ~MySqlManager();
	bool MySql_Init();
	int MySql_Query(AccessPoint* accPoint);
	void MySql_Close();
	void getFilename(string name);
        void getFilesize(long size);
private:
	MYSQL *conn_ptr; //定义mysql连接的句柄
	MYSQL_RES *res_ptr;
	MYSQL_ROW sqlrow; //存放一行查询结果的字符串数组
	string fileName;      //查找的文件名字
	long fileSize;
	struct sockaddr_in address;
	int fd;
	ssize_t writen(void *vptr,size_t n);
	ssize_t readLine(void *vptr,size_t maxlen);
	void TCPClose();

};


#endif /* MYSQLMANAGER_H_ */
