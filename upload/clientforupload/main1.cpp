#include"UploadInclude.h"
#include"GetIPAddress.h"
int filecut();
int CEPHNum;
string filename;
int getcephnum(string str);

int main() {
    cout << "Please enter the file name you want to upload:";
    cin >> filename;
    cout << endl;
    string tempstr = "";
    char strnum[4];
    char tempname[50];
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
    strcpy(tempname,tempstr.c_str());
    if (CEPHNum == 0)
    {
        rename(tempname, "/mnt/rbd-demo");
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
