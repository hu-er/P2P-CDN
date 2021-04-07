/*
 * MySqlManager.cpp
 *
 *  Created on: Oct 19, 2015
 *      Author: lagrange
 */

#include "MySqlManager.h"


MySqlManager::MySqlManager() {
	// TODO Auto-generated constructor stub
	res_ptr=NULL;
	sqlrow = NULL;
	fd = 0;
	fileName="";
	fileSize=0;
}

MySqlManager::~MySqlManager() {
	// TODO Auto-generated destructor stub
	conn_ptr=NULL;
	res_ptr=NULL;
	sqlrow = NULL;
	fd = 0;
	fileName="";
	fileSize=0;
}

bool MySqlManager::MySql_Init(){
		fd=socket(AF_INET,SOCK_STREAM,0);
		address.sin_family=AF_INET;
		address.sin_addr.s_addr=inet_addr("192.168.56.110");
		address.sin_port=htons(8086);
		int res = connect(fd,(struct sockaddr*)&address,sizeof(address));
		if(0==res){
			cout <<"connect to mysql"<<endl;
			return true;
		}else{
			return false;
		}
}

int MySqlManager::MySql_Query(AccessPoint* accPoint){
	char sql[500];
	int result=0;
	sprintf(sql,"select id, fileName, fileSize,  fullPath,"
			    " linkAddr from t_linkSource "
			    "where fileName ='%s'\n",fileName.c_str());
	cout << sql ;
	writen(sql,strlen(sql));
	char msg[500]={0};
	while(readLine(msg,500)&&strcmp(msg,"get result finish\n")!=0){
		msg[strlen(msg)-1] ='\0'; //去掉换行符
		cout<<msg<<endl;
		accPoint->FileName=msg;
		readLine(msg,500);
		msg[strlen(msg)-1] ='\0'; //去掉换行符
		cout<<msg<<endl;
		accPoint->FileSize=atol(msg);
		readLine(msg,500);
		msg[strlen(msg)-1] ='\0'; //去掉换行符
		cout<<msg<<endl;
		accPoint->FilePath=msg;
		readLine(msg,500);
		msg[strlen(msg)-1] ='\0'; //去掉换行符
		cout<<msg<<endl;
		accPoint->ipAddress=msg;
		accPoint->Useable=true;
		accPoint++; result++;
	}
	//int res = mysql_query(conn_ptr,sql);
	//if(res){   //查找错误
	//	printf("SELECT error:%s\n", mysql_error(conn_ptr));
	//}else{
		//把结果存放到MYSQL_RES结构体中
	//	res_ptr = mysql_store_result(conn_ptr);
	//	while(sqlrow = mysql_fetch_row(res_ptr)){
	//		accPoint->FileName=sqlrow[1];
		//	accPoint->FileSize=atol(sqlrow[2]);
	//		accPoint->FilePath=sqlrow[3];
	//		accPoint->ipAddress=sqlrow[4];
	//		accPoint->Useable=true;
	//		accPoint++;
	//		result++;     //找到结果的个数加一
		//}
	//	if (mysql_errno(conn_ptr)){
	//		fprintf(stderr, "Retrieve error:%s\n", mysql_error(conn_ptr));
	//	}
	//	mysql_free_result(res_ptr);  //防止内存泄漏,释放结果集内存
	//}
	return result;
}

void MySqlManager::getFilename(string name){
	this->fileName=name;
}

void MySqlManager::getFilesize(long size){
        this->fileSize=size;
}

void MySqlManager::MySql_Close(){
	TCPClose();
}

ssize_t MySqlManager::writen(void *vptr,size_t n){
	size_t nleft,nwritten;
	char *ptr;
	ptr=(char*)vptr;
	nleft=n;
	while (nleft>0) {
		if ((nwritten=write(fd, ptr, nleft))<=0) {
			if (errno==EINTR) { nwritten=0; }
			else { return -1; }
		}
		nleft-=nwritten; ptr+=nwritten;
	}
	return n;
}

ssize_t MySqlManager::readLine(void *vptr,size_t maxlen){
	ssize_t rc;
	size_t n;
	char c,*ptr;
	ptr=(char*)vptr;
	for (n=1; n<maxlen; n++) {
		again:
		if ((rc=read(fd,&c,1))==1) {
			*ptr++=c;
			if (c=='\n') { break; }  //readover
		}
		else if (rc==0) {
			if (n==1) { return 0; }  //EOF no data read
			else { break; }  //EOF some data read
		}
		else {
			if (errno==EINTR) { goto again; }
			return -1;  //error
		}
	}
	*ptr=0;
	return n;
}

void MySqlManager::TCPClose(){
	char msg[100] = "finish\n";
	writen(msg,strlen(msg));
	close(fd);
}
