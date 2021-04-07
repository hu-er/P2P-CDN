/*
 * MySqlManager.cpp
 *
 *  Created on: Oct 15, 2015
 *      Author: lagrange
 */

#include "MySqlManager.h"
MySqlManager::MySqlManager() {
	// TODO Auto-generated constructor stub
	fd =0;
}

MySqlManager::~MySqlManager() {
	// TODO Auto-generated destructor stub
}
 bool MySqlManager::MySqlInit(){
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

 bool MySqlManager::UpdateMySql(string ipAddress,FileInfo* info){
	 char sql_update[500];
	 sprintf(sql_update,"insert into t_linkSource(fileName, linkAddr, fileSize,fullPath) "
			 	 	 	 "values('%s', '%s', %ld, '%s')\n",info->Filename.c_str(),
						 ipAddress.c_str(),info->Filesize,
						 info->Filepath.c_str());
	 cout<<sql_update<<endl;
	 writen((void*)sql_update,strlen(sql_update));
	 char msg[500];
	 readLine(msg,500);
	 cout << msg <<endl;
	 if(strcmp(msg,"OK\n")==0){
		 return true;
	 }
	 return false;
 }

 bool MySqlManager::MySqlDelete(string ipAddress){
	 string sql_delete="delete from t_linkSource where linkAddr='";
	 sql_delete=sql_delete+ipAddress+"'"+"\n";
	 char sql[500];
	 strcpy(sql,sql_delete.c_str());
	 cout <<sql<<endl;
	// int res = mysql_query(conn_ptr,sql_delete.c_str());
	 writen((void*)sql,strlen(sql));
	 usleep(1000);
	 char msg[500];
	 readLine(msg,500);
	 cout << msg <<endl;
	 if(strcmp(msg,"OK\n")==0){
		 return true;
	 }
	 return false;
 }

 void MySqlManager::MySqlClose(){
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
