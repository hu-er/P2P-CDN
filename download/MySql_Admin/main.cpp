#include"MySqlInclude.h"
#include"MySqlManager.h"
#include"TCPManager.h"
bool Init(){


	return true;
}

int main(){
	TCPManager* tcpManager =new  TCPManager();
	MySqlManager* mysqlManager =new MySqlManager();
	struct sockaddr_in client_address;
	int client_sockfd;
	int pid;
	signal(SIGCLD, SIG_IGN);//父进程忽略SIGCLD软中断而不必要 wait()，避免产生僵尸进程（或者可以通过waitpid信号量不阻塞的等待子进程退出）
	if(!mysqlManager->MySqlInit()){
		cout << "connect to mysql fail"<<endl;
		exit(0);
	}
	tcpManager->SocketInit();  //设置tcp socket

	while(1){
		client_sockfd = tcpManager->SocketAccept(&client_address);
		if((pid = fork())==0){     //子进程
			while(1){
				char msg[500];
				tcpManager->readLine(msg,500);
				cout <<msg<<endl;
				if(strcmp(msg,"finish\n")==0){
					break;
				}
				bool flag = mysqlManager->MySqlQuery(msg);
				string message = msg;
				if(flag){
					if(message.find("select")!=string::npos){    //查询操作,准备发送节点和文件信息
						cout << "send file info"<<endl;


						for(int i =0;i<mysqlManager->files.count;i++){
							char msg[500];
							string temp;
							temp= mysqlManager->files.FileName[i]+'\n';
							strcpy(msg,temp.c_str());
							tcpManager->writen(msg,strlen(msg));
							temp= mysqlManager->files.FileSize[i]+'\n';
							strcpy(msg,temp.c_str());
							tcpManager->writen(msg,strlen(msg));
							temp= mysqlManager->files.FilePath[i]+'\n';
							strcpy(msg,temp.c_str());
							tcpManager->writen(msg,strlen(msg));
							temp= mysqlManager->files.ipAddress[i]+'\n';
							strcpy(msg,temp.c_str());
							tcpManager->writen(msg,strlen(msg));
						}
						char finish[100] ="get result finish\n";
						tcpManager->writen(finish,strlen(finish));  //通告完成
					}else{    //不是查询操作
						char ack[5] = "OK\n";
						tcpManager->writen(ack,strlen(ack));
					}
				}
			}
			tcpManager->Close_Socket(client_sockfd);
			//exit(0);
		}else{
			tcpManager->Close_Socket(client_sockfd);
		}
	}
	return 0;
}
