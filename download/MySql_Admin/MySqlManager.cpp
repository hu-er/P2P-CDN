/*
 * MySqlManager.cpp
 *
 *  Created on: Apr 21, 2016
 *      Author: xiaoyu
 */

#include "MySqlManager.h"

MySqlManager::MySqlManager() {
	// TODO Auto-generated constructor stub

}

MySqlManager::~MySqlManager() {
	// TODO Auto-generated destructor stub
}

bool MySqlManager::MySqlInit(){
	 conn_ptr=mysql_init(NULL);
	 conn_ptr = mysql_real_connect(conn_ptr,"127.0.0.1","root","!@#Zxc123","mysql",3306,NULL,0);
	 if(!conn_ptr){
		 cout<<"connect to MySql failed!"<<endl;
		 return false;
	 }else{
		 cout<<"MySql connect Sccuess"<<endl;
		 return true;
	 }
}

void MySqlManager::MySqlClose(){
	 mysql_close(conn_ptr);
}

bool MySqlManager::MySqlQuery(string sql){
	files.count=0;
        bool flag=true;
	sql.erase(sql.size()-1,1);//去掉换行符
	cout << sql<<endl;
	int res = mysql_query(conn_ptr,sql.c_str());
	if(res){
		cout << "mysql operation fail"<<endl;
		flag = false;
	}else{
		cout << "mysql operation success" <<endl;
	}
	if(sql.find("select")!=string::npos){  //此操作是查询操作
		MYSQL_ROW sqlrow; //存放一行查询结果的字符串数组
	    MYSQL_RES *res_ptr;
		res_ptr = mysql_store_result(conn_ptr);
		int i=0;
		while((sqlrow = mysql_fetch_row(res_ptr)) && i<30){
			files.FileName[i]=sqlrow[1];
			files.FileSize[i]=sqlrow[2];
			files.FilePath[i]=sqlrow[3];
			files.ipAddress[i]=sqlrow[4];
			files.count++; 
			i++;
		}
		if (mysql_errno(conn_ptr)){
			fprintf(stderr, "Retrieve error:%s\n", mysql_error(conn_ptr));
		}
		mysql_free_result(res_ptr);  //防止内存泄漏,释放结果集内存
	}
	return flag;
}

