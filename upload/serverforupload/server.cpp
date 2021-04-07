#include<arpa/inet.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<iostream>
#include<string>
#include<mysql/mysql.h>
#include<sys/types.h>
#include<errno.h>
#include<cmath>
#include<stdlib.h>
#include<dirent.h>
#include<netinet/in.h>
#include<stdio.h>
#include<pthread.h>
#include<stdarg.h>
#include<unistd.h>
#include<vector>
#include<fstream>
#include<stddef.h>
using namespace std;
#define SERVER_PORT 8000
#define LENGTH_OF_LISTEN_QUEUE 20
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512
#define PATH "/mnt/cephfs-demo/"	

int main(void)
{
    // 声明并初始化一个服务器端的socket地址结构
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    // 创建socket，若成功，返回socket描述符
    int server_socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket_fd < 0)
    {
        perror("Create Socket Failed:");
        exit(1);
    }
    int opt = 1;
    setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 绑定socket和socket地址结构
    if (-1 == (bind(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))))
    {
        perror("Server Bind Failed:");
        exit(1);
    }

    // socket监听
    if (-1 == (listen(server_socket_fd, LENGTH_OF_LISTEN_QUEUE)))
    {
        perror("Server Listen Failed:");
        exit(1);
    }

    while (1)
    {
        // 定义客户端的socket地址结构
        struct sockaddr_in client_addr;
        socklen_t client_addr_length = sizeof(client_addr);

        // 接受连接请求，返回一个新的socket(描述符)，这个新socket用于同连接的客户端通信
        // accept函数会把连接到的客户端信息写到client_addr中
        int new_server_socket_fd = accept(server_socket_fd, (struct sockaddr*)&client_addr, &client_addr_length);
        if (new_server_socket_fd < 0)
        {
            perror("Server Accept Failed:");
            break;
        }

        // recv函数接收数据到缓冲区buffer中
        char buffer[BUFFER_SIZE];
        bzero(buffer, BUFFER_SIZE);
        if (recv(new_server_socket_fd, buffer, BUFFER_SIZE, 0) < 0)
        {
            perror("Server Recieve Data Failed:");
            break;
        }

        // 然后从buffer(缓冲区)拷贝到file_name中
        char file_name[FILE_NAME_MAX_SIZE + 1];
        bzero(file_name, FILE_NAME_MAX_SIZE + 1);
        strncpy(file_name, buffer, strlen(buffer) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(buffer));
        printf("%s\n", file_name);


        string path=PATH;
	    path+=file_name;
	    const char *p=path.c_str();
        // 打开文件，准备写入
        FILE* fp = fopen(p, "w");
        if (NULL == fp)
        {
            printf("File:\t%s Can Not Open To Write\n", file_name);
            exit(1);
        }

        // 从服务器接收数据到buffer中
        // 每接收一段数据，便将其写入文件中，循环直到文件接收完并写完为止
        bzero(buffer, BUFFER_SIZE);
        int length = 0;
        while ((length = recv(new_server_socket_fd, buffer, BUFFER_SIZE, 0)) > 0)
        {
            if (fwrite(buffer, sizeof(char), length, fp) < length)
            {
                printf("File:\t%s Write Failed\n", file_name);
                break;
            }
            bzero(buffer, BUFFER_SIZE);
        }

        // 接收成功后，关闭文件，关闭socket
        printf("Receive uploadingFile:\t%s  Successful!\n", file_name);
        fclose(fp);
        close(new_server_socket_fd);
    }
    close(server_socket_fd);
    return 0;
}
