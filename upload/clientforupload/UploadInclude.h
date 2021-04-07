/*
 * ServerInclude.h
 *
 *  Created on: Oct 15, 2015
 *      Author: lagrange
 */

#ifndef SERVERINCLUDE_H_
#define SERVERINCLUDE_H_
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netdb.h>
#include<ifaddrs.h>
#include<unistd.h>
#include<string.h>
#include<iostream>
#include<string>
#include<mysql/mysql.h>
#include<sys/types.h>
#include<errno.h>
#include<sys/time.h>
#include<cmath>
#include<stdlib.h>
#include<dirent.h>
#include<sys/stat.h>
#include<netinet/in.h>
#include<vector>
#include<stdio.h>
#include <fstream>
#include<stddef.h>
#include <signal.h>
#include <sys/wait.h>
#include <sstream>
#include <cstdlib>
using namespace std;

#define SERVER_PORT 8000
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512

#endif /* SERVERINCLUDE_H_ */
