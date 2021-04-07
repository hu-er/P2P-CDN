/*
 * Compress.h
 *
 *  Created on: Apr 26, 2016
 *      Author: xiaoyu
 */

#ifndef COMPRESS_H_
#define COMPRESS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef long long	 INT64;		//¶¨Òå64Î»ÓÐ·ûºÅÕûÊýÀàÐÍ
typedef unsigned long long	UINT64;		//¶¨Òå64Î»ÎÞ·ûºÅÕûÊýÀàÐÍ

#define ESC			256				//×ªÒå·û
#define ESC0(idx)	for(i=ESC+N;i>0;a[idx][i]--,i>>=1);
									//×ªÒå·ûÍ³¼ÆÖµÇå0
#define ESC1(idx)	for(i=ESC+N;i>0;a[idx][i]++,i>>=1);
#define MAXBUF		519		//»º³åÇø´óÐ¡
#define N			257				//×Ö·û¸öÊý
#define N1			128*128			//¶Ñ¸öÊý

class Compress {
public:
	Compress();
	virtual ~Compress();

	void encBit();								//µ÷ÕûÉÏÏÂÏÞ£¬²¢Êä³ö±ÈÌØ
	void encode(unsigned idx,unsigned x);		//±àÂë
	void encode0(unsigned idx, unsigned x);		//¼Ç·Ö°å±àÂë
	//unsigned inBit();							//ÊäÈë±ÈÌØ
	unsigned inChr();							//ÊäÈë×Ö·û
	void init();								//³õÊ¼»¯
	void outBit(unsigned b);					//Êä³ö±ÈÌØ
	void outChr(unsigned c);					//Êä³ö×Ö·û
	void update(unsigned idx, unsigned x);		//¸üÐÂÍ³¼ÆÖµ
	void zip(char* data_buffer);									//Ñ¹Ëõ
	unsigned char bufOut[MAXBUF];
	int data_size;
private:
	unsigned short a[N1][N*2];			//Í³¼Æ¶Ñ
	unsigned short b[N];				//ÁÙÊ±¼Ç·Ö°å
	unsigned H,L,M;						//ÉÏÏÞ¡¢ÏÂÏÞ¡¢ÖÐÖµ¡¢²îÖµ
	unsigned char bufIn[MAXBUF];		//ÊäÈë»º³åÇø
	//unsigned char bufOut[MAXBUF];		//Êä³ö»º³åÇø
	unsigned bufBit;					//Î»»º³åÇø
	int pIn,pOut,pBit;					//ÊäÈë»º³åÇø¡¢Êä³ö»º³åÇø¡¢Î»»º³åÇøÖ¸Õë
	int pIdx;							//Í³¼Æ¶ÑË÷ÒýÖ¸Õë

};

#endif /* COMPRESS_H_ */
