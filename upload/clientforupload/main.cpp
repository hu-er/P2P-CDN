#include"UploadInclude.h"
#include"GetIPAddress.h"
int filecut();
int CEPHNum;
string filename;
string CEPH_ONE_ADDRESS = "192.168.56.102";
string CEPH_TWO_ADDRESS = "192.168.55.113";
string CEPH_THREE_ADDRESS = "192.168.54.122";
int getcephnum(string str);
int uploadtoserver(string ipaddr, string filename);

int main() {
    cout << "Please enter the file name you want to upload:";
    cin >> filename;
    cout << endl;
    string tempstr = "";
    char strnum[4];
    char tempname[30];
    int filecutResult = filecut();
    if (filecutResult == -1)
    {
        cout << "The file cannot be opened. Please try again!" << endl;
        return 0;
    }
    GetIPAddress* ipAddress = new GetIPAddress();
    ipAddress->getIpAddress();
    string str;
    for (int i = 0; (ipAddress->showIP(str, i)) != false; i++) {
        cout << str << endl;
        getcephnum(str);
    }
    cout << "CEPHNum=" << CEPHNum << endl;

    sprintf(strnum, "%d", CEPHNum);
    tempstr = string(strnum) + filename;
    string command = "mv ";
    command += "/home/cephu/clientforupload/";
    command += tempstr;
    command += " ";
    command += "/mnt/cephfs-demo/";
    system((char*)command.c_str());
    if (CEPHNum == 0)
    {
        sprintf(strnum, "%d", 1);
        tempstr = string(strnum) + filename;
        strcpy(tempname,tempstr.c_str());
        uploadtoserver(CEPH_TWO_ADDRESS, tempstr);
        unlink(tempname);
        sprintf(strnum, "%d", 2);
        tempstr = string(strnum) + filename;
        uploadtoserver(CEPH_THREE_ADDRESS, tempstr);
        strcpy(tempname,tempstr.c_str());
        unlink(tempname);
    }
    else if (CEPHNum == 1)
    {
        sprintf(strnum, "%d", 0);
        tempstr = string(strnum) + filename;
        strcpy(tempname,tempstr.c_str());
        uploadtoserver(CEPH_ONE_ADDRESS, tempstr);
        unlink(tempname);
        sprintf(strnum, "%d", 2);
        tempstr = string(strnum) + filename;
        uploadtoserver(CEPH_THREE_ADDRESS, tempstr);
        strcpy(tempname,tempstr.c_str());
        unlink(tempname);
    }
    else if (CEPHNum == 2)
    {
        sprintf(strnum, "%d", 0);
        tempstr = string(strnum) + filename;
        strcpy(tempname,tempstr.c_str());
        uploadtoserver(CEPH_ONE_ADDRESS, tempstr);
        unlink(tempname);
        sprintf(strnum, "%d", 1);
        tempstr = string(strnum) + filename;
        uploadtoserver(CEPH_TWO_ADDRESS, tempstr);
        strcpy(tempname,tempstr.c_str());
        unlink(tempname);
    }
    return 0;
}


int filecut() {
    fstream pf, pf1;
    string tempstr = "";
    int number = 0;
    char strnum[4];
    ostringstream oss;
    pf.open(filename.c_str(), ios::in | ios::binary);

    if (!pf)
    {
        return -1;
    }

    int temp = pf.tellg();
    pf.seekg(0, ios_base::end);
    long flen = pf.tellg();
    pf.seekg(temp);

    cout << "file size is: " << flen << " byte" << endl;

    int n = 3;  // 文件要分为多少分，为了保证最有一个文件最小，故+1；
    int size = ceil(flen / 3);
    int sizefinal = flen - size * 2;
    cout << "number of file is: " << n << endl;


    //
    // 先分n-1的文件
    char* databuf = new char[size];
    char* databuf1 = new char[sizefinal];

    for (int i = 0; i < n - 1; i++)
    {
        number = i;
        sprintf(strnum, "%d", number);
        tempstr = string(strnum) + filename;
        pf1.open(tempstr.c_str(), ios::out | ios::binary);
        pf.read(databuf, size * sizeof(char));
        pf1.write(databuf, size * sizeof(char));
        pf1.close();
        cout << "file :" << tempstr << endl;

    }
    delete[] databuf;
    sprintf(strnum, "%d", 2);
    tempstr = string(strnum) + filename;
    pf1.open(tempstr.c_str(), ios::out | ios::binary);

    pf.read(databuf1, sizefinal * sizeof(char));
    pf1.write(databuf1, sizefinal * sizeof(char));
    pf1.close();
    cout << "file :" << tempstr << endl;
    pf.close();

    return 0;

}

int getcephnum(string str) {
    if (str.find("192.168.56") != string::npos)
        CEPHNum = 0;
    else if (str.find("192.168.55") != string::npos)
        CEPHNum = 1;
    else if (str.find("192.168.54") != string::npos)
        CEPHNum = 2;
}

int uploadtoserver(string ipaddr, string filename)
{
    char file_name[FILE_NAME_MAX_SIZE + 1];
    bzero(file_name, FILE_NAME_MAX_SIZE + 1);
    strcpy(file_name,filename.c_str());
    char IPADDR[30];
    strcpy(IPADDR,ipaddr.c_str());
    // 声明并初始化一个客户端的socket地址结构
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htons(INADDR_ANY);
    client_addr.sin_port = htons(0);

    // 创建socket，若成功，返回socket描述符
    int client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket_fd < 0)
    {
        perror("Create Socket Failed:");
        exit(1);
    }

    // 绑定客户端的socket和客户端的socket地址结构 非必需
    if (-1 == (bind(client_socket_fd, (struct sockaddr*)&client_addr, sizeof(client_addr))))
    {
        perror("Client Bind Failed:");
        exit(1);
    }

    // 声明一个服务器端的socket地址结构，并用服务器那边的IP地址及端口对其进行初始化，用于后面的连接
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, IPADDR, &server_addr.sin_addr) == 0)
    {
        perror("Server IP Address Error:");
        exit(1);
    }
    server_addr.sin_port = htons(SERVER_PORT);
    socklen_t server_addr_length = sizeof(server_addr);

    // 向服务器发起连接，连接成功后client_socket_fd代表了客户端和服务器的一个socket连接
    if (connect(client_socket_fd, (struct sockaddr*)&server_addr, server_addr_length) < 0)
    {
        perror("Can Not Connect To Server IP:");
        exit(0);
    }


    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    strncpy(buffer, file_name, strlen(file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name));

    // 向服务器发送buffer中的数据
    if (send(client_socket_fd, buffer, BUFFER_SIZE, 0) < 0)
    {
        perror("Send File Name Failed:");
        exit(1);
    }


    // 打开文件并读取文件数据
    FILE* fp = fopen(file_name, "r");
    if (NULL == fp)
    {
        printf("File:%s Not Found\n", file_name);
    }
    else
    {
        bzero(buffer, BUFFER_SIZE);
        int length = 0;
        // 每读取一段数据，便将其发送给客户端，循环直到文件读完为止
        while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
        {
            if (send(client_socket_fd, buffer, length, 0) < 0)
            {
                printf("Send File:%s Failed./n", file_name);
                break;
            }
            bzero(buffer, BUFFER_SIZE);
        }

        // 关闭文件
        fclose(fp);
        printf("File:%s Transfer Successful!\n", file_name);
    }
    close(client_socket_fd);
    return 0;
}
