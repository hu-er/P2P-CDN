/*
 * MySqlManager.h
 *
 *  Created on: Apr 21, 2016
 *      Author: xiaoyu
 */
#include"MySqlInclude.h"
#ifndef MYSQLMANAGER_H_
#define MYSQLMANAGER_H_


struct file_info{
	string FileName[30];
	string FileSize[30];
	string FilePath[30];
	string ipAddress[30];
	int count;
};
class MySqlManager {
public:
	MySqlManager();
	virtual ~MySqlManager();
	bool MySqlInit();
	void MySqlClose();
	bool MySqlQuery(string sql);
	file_info files;
private:
    MYSQL *conn_ptr;
	MYSQL_FIELD *fd;

};


#endif /* MYSQLMANAGER_H_ */
