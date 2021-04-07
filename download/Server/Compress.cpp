/*
 * Compress.cpp
 *
 *  Created on: Apr 26, 2016
 *      Author: xiaoyu
 */

#include "Compress.h"

Compress::Compress() {
	// TODO Auto-generated constructor stub
	pIn=0;
	pOut=0;
	pBit=0;
	data_size=0;
	pIdx=0;
	memset(a,0,N1*N*2);
	memset(b,0,N);
}

Compress::~Compress() {
	// TODO Auto-generated destructor stub
}

void Compress::encBit() {
	unsigned register i;
/*
if (len-iCount>=chkpoint && len-iCount<chkpoint+16) {
printf("%8x: %x %x\n",pIn,H,L);
}
*/
	for (i=H^L; (i&0x80000000)==0; outBit(L>>31),H=(H<<1)|1,L<<=1,i=(i<<1)|1);
	if (H-L<0x20000) { for (L=0x80000000,i=H^L; (i&0x80000000)==0; outBit(L>>31),H=(H<<1)|1,L<<=1,i=(i<<1)|1); }
	  //Ö÷¶¯Ìá¸ßÏÂÏÞ£¬·ÀÖ¹ÉÏÏÂÏÞÈÝ²îÇ÷ÓÚÒ»ÖÂ
/*
if (len-iCount>=chkpoint && len-iCount<chkpoint+16) {
printf("        : %x %x\n",H,L);
}
*/
}

void Compress:: encode(unsigned idx, unsigned x) {
	unsigned register i,s;
	UINT64 DH,DL;
	for (i=x+N,s=0; i>1; s+=a[idx][i-1]*(i&1),i>>=1);  //¶¨Î»
	DH=((UINT64)H-L+1)*(s+a[idx][x+N])/a[idx][1]-1; DL=((UINT64)H-L+1)*s/a[idx][1];
	H=L+(unsigned)DH; L+=(unsigned)DL;  //¸üÐÂÉÏÏÂÏÞ
	encBit();
}

void Compress::encode0(unsigned idx, unsigned x) {
	unsigned register i,s;
	for (i=s=0; i<N-1; b[i]=(a[idx][i+N]==0),s+=b[i],i++);  //ÉèÖÃÁÙÊ±¼Ç·Ö°å
	if (s==1) { ESC0(idx); }  //Ö»Ê£ÏÂ1¸ö×Ö·û£¬²»±Ø±àÂë
	else {  //¼Ç·Ö°å±àÂë
		UINT64 DH,DL;
		for (b[ESC]=s,i=s=0; i<x; s+=b[i],i++);  //¶¨Î»
		DH=((UINT64)H-L+1)*(s+1)/b[ESC]-1; DL=((UINT64)H-L+1)*s/b[ESC];
		H=L+(unsigned)DH; L+=(unsigned)DL;  //¸üÐÂÉÏÏÂÏÞ
		encBit();
	}
}

/*unsigned Compress::inBit() {
	if (pBit==0) { pBit=8; bufBit=inChr(); }
	pBit--;
	return (bufBit>>pBit)&1;
}
*/
unsigned Compress::inChr() {
	unsigned c = 0;
	//if (pIn==sizeof(bufIn)) {
	//	if (fread(bufIn,1,sizeof(bufIn),fpIn)<1) { printf("Fail reading file"); fclose(fpIn); fclose(fpOut); exit(1); }
	//	pIn=0;
	//}
	if(pIn<519){
		c=bufIn[pIn]; pIn++;
	}
	return c;
}

void Compress::init() {
	unsigned register i;
	unsigned idx;
	for (idx=0; idx<N1; idx++) {
		for (i=0; i<N*2; a[idx][i]=0,i++);
		ESC1(idx);
	}
	pIn=0; pOut=pBit=0; pIdx=('\r'<<7)|'\n';
	H=0xffffffff; L=0;
}

void Compress::outBit(unsigned b) {
	bufBit=(bufBit<<1)|b; pBit++;
	if (pBit==8) { pBit=0; outChr(bufBit); }
}

void Compress::outChr(unsigned c) {
	if(pOut<519){
		bufOut[pOut]=(unsigned char)c; pOut++;
		//if (pOut==sizeof(bufOut)) { fwrite(bufOut,1,sizeof(bufOut),fpOut); pOut=0; }
		data_size++;
	}
}

void Compress::update(unsigned idx, unsigned x) {
	unsigned register i;
	if (a[idx][1]==0xfffe) {  //ÇåÏ´£¬ÉÏÏÞÖµÔ¤Áô1£¬ÒÔ·ÀÁÙÊ±±àÂë×ªÒå·ûÊ±Òç³ö
		if (a[idx][N+ESC]==1) { for (i=N; i<N+ESC; a[idx][i]>>=1,i++); }
		else {
			unsigned register j=0;
			for (i=N; i<N+ESC; a[idx][i]>>=1,j|=(a[idx][i]==0),i++);
			a[idx][i]=j;
		}
		for (i=N-1; i>0; a[idx][i]=a[idx][i*2]+a[idx][i*2+1],i--);
	}
	for (i=x+N; i>0; a[idx][i]++,i>>=1);
/*
if (len-iCount>=chkpoint && len-iCount<chkpoint+16) {
printf("%I64x: %x\n",len-iCount,x);
}
*/
}

void Compress::zip(char* data_buffer) {
	for(int i=0;i<519;i++){
		bufIn[i]=data_buffer[i];
	}
	pIn=0;
	pOut=0;
	pBit=0;
	data_size=0;
	pIdx=0;
	init();
	for (int iCount=0; iCount<519; iCount++) {
		unsigned x=inChr();
		if (a[pIdx][x+N]>0) { encode(pIdx,x); }
		else { encode(pIdx,ESC); encode0(pIdx,x); }
		update(pIdx,x); pIdx=((pIdx&0x7f)<<7)|(x&0x1f)|((x>>1)&0xe0);  //Ë÷ÒýÖÐÆÁ±Î´óÐ¡Ð´
		//if ((iCount&0x1fffff)==0) { printf("%c%5.2f",13,(double)(len.__pos-iCount.__pos)*100/len.__pos); }
	}
	H=L; encBit();  //Êä³öÊ£ÓàÎ»
	if (pBit>0) {  //Çå¿ÕÎ»»º³åÇø
		unsigned i;
		for (i=pBit; i<8; outBit(0),i++);
	}
	//if (pOut>0) { fwrite(bufOut,1,pOut,fpOut); }  //Çå¿ÕÊä³ö»º³åÇø
}
