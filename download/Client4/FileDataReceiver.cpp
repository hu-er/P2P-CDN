/*
 * FileDataReceiver.cpp
 *
 *  Created on: Nov 11, 2015
 *      Author: lagrange
 */

#include "FileDataReceiver.h"

FileDataReceiver::FileDataReceiver() {
	// TODO Auto-generated constructor stub
	memset(filedata,0,36*3*2048*512*sizeof(char));
	memset(raiddata,0,36*2048*512*sizeof(char));
	memset(data_flag,0,36*3*2048*sizeof(char));
	memset(raid_flag,0,36*2048*sizeof(char));
	string fileName ="";
	fileSize = 0;
	TotalTime=0;
	last = false;
}

FileDataReceiver::~FileDataReceiver() {
	// TODO Auto-generated destructor stub
	memset(filedata,0,36*3*2048*512*sizeof(char));
	memset(raiddata,0,36*2048*512*sizeof(char));
	memset(data_flag,0,36*3*2048*sizeof(char));
	memset(raid_flag,0,36*2048*sizeof(char));
	string fileName ="";
	fileSize = 0;
	TotalTime=0;
	last = false;
}

void FileDataReceiver::SetAttribute(string fileName,int fileSize,int TotalTime){
	this->fileName = fileName;
	this->fileSize = fileSize;
	this->TotalTime =TotalTime;
}
void FileDataReceiver::SetFileData(char* data_buffer){
	int BN = data_buffer[512]*100+data_buffer[513];//得到数据所在的段号,BN从0算起
	int size = data_buffer[514]*100+data_buffer[515];//得到数据段的大小
	int SN = data_buffer[516]*100+data_buffer[517];//得到SN
	int GN = data_buffer[518];//得到GN
	int offset  = 3*BN+GN-1;//得到在对应的3M文件段中为第几段512B,offset和BN从0算起,GN从1算起
	int segNum = SN % 36;//segNum为这个512B在此次传输中属于第几个3M文件段,SN从1开始
	if(segNum ==0) {segNum = 36;}//如果余数为0，说明刚好是第36个,segNum从1算起
	//下面代码把对应的数据存放在对应的地方
	if(GN == 4){   //说明此段是校检段
		int pos = BN*512;
		for(int i=0;i<size;++i){
			raiddata[segNum-1][pos + i] = data_buffer[i];
		}
		raid_flag[segNum-1][BN]=true;//标记已经收到对应校检段
	}else{   //此段不是校检段
		int pos = offset * 512;
		for(int i=0;i<size;++i){
			filedata[segNum-1][pos+i]=data_buffer[i];
		}
		data_flag[segNum-1][offset] = true;//标记已经收到对应文件段
	}
	//cout << "offset " <<offset <<" size "<<size <<" BN "<<BN << " SN " <<SN <<"GN "<<GN <<endl;
}

void FileDataReceiver::CheckData(bool last){
	//返回应该重传的节点,若只丢失一个包，则调用解码函数
	cout << "start to check data" <<endl;
	int BN_count = 3*2048;
	int SN_count =36;
	int BN_count_last=0;
	struct ResentInfo resentInfo;
	this->last = last; //标记最后一次传输
	resentInfos.clear();//清空原来的节点，为下一次检查准备
	vector<ResentInfo>(resentInfos).swap(resentInfos);
	if(last){   //是否为最后一轮传输，若是最后一轮传输，则到达的数据一般情况下小于144M，即有些标记位为false是正确的,应计算数据量
		int remainsize = fileSize-(TotalTime-1)*SEGSIZE*36;//最后一轮传输文件大小
		SN_count = (int)ceil((double)remainsize/(SEGSIZE));//一共有多少段3M
		remainsize = remainsize - (SN_count-1)*SEGSIZE;//最后一段大小
		BN_count_last = (int)ceil((double)remainsize/512);//一共分成多少个512B
		BN_count_last = (int)ceil((double)BN_count_last/3);//每组发送多少段512B,向上取整，有可能收到一个空包
		cout << "BN_count_last is" << BN_count_last<<endl;
		cout <<"fileSize is" << fileSize<<endl;
		cout << "totalTime is "<<TotalTime<<endl;
		cout <<"remainsize is"<<remainsize <<endl;
		cout <<"SN_count is" <<SN_count<<endl;
	}
	//若不是最后一轮传输，则应该传输的数据为144M，则没有丢包的情况下，每个标记位都为true
	//先查看对应的data_flag标记位，如果都为true，则可以跳过raid的查找，如果缺少了一个，并且raid位为真，则说明可以通过解码恢复
	//如果缺少了两个以上，说明需要重传，同时还要查看raid是否接收到
	for(int i=0;i<SN_count;++i){//文件段检查
		resentInfo.segNum=i+1;//登记segNum,这个segNum从1算起
		if(last && (i == SN_count-1)){   //最后一个文件段(一般情况下不足3M)
			BN_count = 3*BN_count_last;
			cout << "BN_count_last is "<<BN_count_last<<endl;
			cout <<"fileSize is" << fileSize<<endl;
		}
		for(int j=0;j<BN_count; ){   //文件块检查
			resentInfo.BN=j/3;//登记BN
			//resentInfo.BN=2049;  //直接重传文件段
			if(data_flag[i][j] && data_flag[i][j+1] && data_flag[i][j+2]){  //三个都是true，数据全部到齐
				j+=3;
				continue;
			}else if((data_flag[i][j] + data_flag[i][j+1] + data_flag[i][j+2]) == 2){//三个缺一个
				cout <<"one lost "<<"i is " <<i <<" j is "<<j<<endl;
				int GN_lost = 0;
				if(data_flag[i][j]==false){   //第一组丢失
					GN_lost=1;//登记GN
				}else if(data_flag[i][j+1] == false){  //第二组丢失
					GN_lost=2;//登记GN
				}else{          //第三组丢失
					GN_lost=3;//登记GN
				}
				if(raid_flag[i][j/3]){   //校检块没丢，做校检
					DeCodeRaid(i,j,GN_lost);
				}else{  //校检块丢了，重传数据,校检块会在后面加进去
					resentInfo.GN = GN_lost;
					resentInfos.push_back(resentInfo);//丢失数据节点推入vector中
					// break;  //一个文件块丢失，整个文件段重传
				}
			}else if((data_flag[i][j] ^ data_flag[i][j+1] ^ data_flag[i][j+2]) == true){ //三个丢两个
				cout <<"two lost "<<"i is " <<i <<" j is "<<j<<endl;
				if(raid_flag[i][j] == true){    //丢失的是二 三组
					resentInfo.GN =2;
					resentInfos.push_back(resentInfo);
					resentInfo.GN=3;
					resentInfos.push_back(resentInfo);
					//break;
				}else if(raid_flag[i][j+1] == true){//丢失的是一 三组
					resentInfo.GN =1;
					resentInfos.push_back(resentInfo);
					resentInfo.GN=3;
					resentInfos.push_back(resentInfo);
					//break;
				}else{//丢失的是一 二组
					resentInfo.GN =1;
					resentInfos.push_back(resentInfo);
					resentInfo.GN=2;
					resentInfos.push_back(resentInfo);
					//break;
				}
			}else{  //数据三个都丢失
				cout <<"three lost "<<"i is " <<i <<" j is "<<j<<endl;
				for(int k =1;k<=3;++k){
					resentInfo.GN=k;
					resentInfos.push_back(resentInfo);
				}
				//break;
			}
			if(raid_flag[i][j/3] == false){   //校检段丢失
				resentInfo.GN=4;
				resentInfos.push_back(resentInfo);
			}
			j+=3;//跳到下一组
		}
	}
	cout << "the size of the vector is" << resentInfos.size()<<endl;
}

void FileDataReceiver::DeCodeRaid(int segNum,int BN,int GN_lost){
	int pos_1=BN*512;  //segNum BN从0算起
	int pos_2=(BN+1)*512;
	int pos_3=(BN+2)*512;
	int pos_raid =(BN/3)*512;
	cout << "doing decode"<<" GN_lost is"<<GN_lost<<endl;
	if(GN_lost == 1){  //丢失第一组
		for(int i=0;i<512;++i){
			filedata[segNum][pos_1+i] = raiddata[segNum][pos_raid+i]^
										filedata[segNum][pos_2+i]^filedata[segNum][pos_3+i];
		}
		data_flag[segNum][BN] =true; //标志位置true
	}else if(GN_lost == 2){//丢失第二组
		for(int i=0;i<512;++i){
			filedata[segNum][pos_2+i] = raiddata[segNum][pos_raid+i]^
										filedata[segNum][pos_1+i]^filedata[segNum][pos_3+i];
		}
		data_flag[segNum][BN+1] =true;
	}else{
		for(int i=0;i<512;++i){//丢失第三组
			filedata[segNum][pos_3+i] = raiddata[segNum][pos_raid+i]^
										filedata[segNum][pos_1+i]^filedata[segNum][pos_2+i];
		}
		data_flag[segNum][BN+2] =true;
	}
}

void FileDataReceiver::WriteFile(){
	//个人感觉有bug，得重新考虑其正确性
	cout <<"start to write file"<<endl;
	int datasize = 36*3*2048*512; //正常情况下写入数据大小
	if(last){  //如果是最后一轮
		datasize = fileSize - (TotalTime-1)*datasize;  //求出最后一轮数据大小
	}
	//fileName += ".temp";
	cout << fileName <<endl;
	ofstream file_out(fileName.c_str(),ios::app|ios::binary|ios::out);  //打开文件
	int tempsize = 0;
	int buffersize = 512;
	for(int i=0;i<36 && (tempsize<datasize);++i){
		char *p = filedata[i];
		for(int j=0;j<3*2048 && (tempsize<datasize);++j){
			if(tempsize >=(datasize-512)){  //最后一个512B
				buffersize = datasize - tempsize;//求出末尾的文件大小
			}
			file_out.write(p,buffersize);  //每次写入buffersize B，正常情况下写入512B，当到达最后一段时，写入相应的字节数
			p+=512;
			tempsize+=512;          //计算写进去的数据量
		}
	}
}
