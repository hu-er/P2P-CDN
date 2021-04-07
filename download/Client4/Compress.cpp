#include"Compress.h"

Compress::Compress() {
	// TODO Auto-generated constructor stub
	pIn=0;
	pOut=0;
	pBit=0;
	data_size=0;
}

Compress::~Compress() {
	// TODO Auto-generated destructor stub
}

void Compress::decBit() {
	unsigned register i;
/*
if (len-iCount>=chkpoint && len-iCount<chkpoint+16) {
printf("%8x: %x %x %x\n",pOut,H,L,M);
}
*/
	for (i=H^L; (i&0x80000000)==0; H=(H<<1)|1,L<<=1,M=(M<<1)|inBit(),i=(i<<1)|1);
	if (H-L<0x20000) { for (L=0x80000000,i=H^L; (i&0x80000000)==0; H=(H<<1)|1,L<<=1,M=(M<<1)|inBit(),i=(i<<1)|1); }
	  //Ö÷¶¯Ìá¸ßÏÂÏÞ£¬·ÀÖ¹ÉÏÏÂÏÞÈÝ²îÇ÷ÓÚÒ»ÖÂ
/*
if (len-iCount>=chkpoint && len-iCount<chkpoint+16) {
printf("        : %x %x %x\n",H,L,M);
}
*/
}

unsigned Compress::decode(unsigned idx) {
	unsigned register i,j,s;
	UINT64 DH,DL;
	DL=(((UINT64)M-L+1)*a[idx][1]-1)/((UINT64)H-L+1);  //±ØÐëÏÈ±£´æÔÚDL£¬·ñÔò¿ÉÄÜÒòÎª¾«¶ÈÎÊÌâ¶ªÊ§Êý¾Ý
	for (i=1,s=(unsigned)DL; i<N; j=(s>=a[idx][i*2]),s-=j*a[idx][i*2],i=i*2+j); j=i-N;  //»ñÈ¡×Ö·û
	for (s=0; i>1; s+=a[idx][i-1]*(i&1),i>>=1);  //¶¨Î»
	DH=((UINT64)H-L+1)*(s+a[idx][j+N])/a[idx][1]-1; DL=((UINT64)H-L+1)*s/a[idx][1];
	H=L+(unsigned)DH; L+=(unsigned)DL;  //¸üÐÂÉÏÏÂÏÞ
	decBit();
	return j;
}

unsigned Compress::decode0(unsigned idx) {
	unsigned register i,s;
	for (i=s=0; i<N-1; b[i]=(a[idx][i+N]==0),s+=b[i],i++);  //ÉèÖÃÁÙÊ±¼Ç·Ö°å
	if (s==1) {  //Ö»Ê£ÏÂ1¸ö×Ö·û£¬²»±Ø½âÂë
		ESC0(idx);
		for (i=0; b[i]==0; i++);
	}
	else {  //¼Ç·Ö°å½âÂë
		UINT64 DH,DL;
		DL=(((UINT64)M-L+1)*s-1)/((UINT64)H-L+1);  //±ØÐëÏÈ±£´æÔÚDL£¬·ñÔò¿ÉÄÜÒòÎª¾«¶ÈÎÊÌâ¶ªÊ§Êý¾Ý
		for (i=0,b[ESC]=s,s=(unsigned)DL+1; s>0; s-=b[i],i++);  //¶¨Î»£¬»ñµÃiÖµ
		i--;
		DH=((UINT64)H-L+1)*(DL+1)/b[ESC]-1; DL=((UINT64)H-L+1)*DL/b[ESC];
		H=L+(unsigned)DH; L+=(unsigned)DL;  //¸üÐÂÉÏÏÂÏÞ
		decBit();
	}
	return i;
}




unsigned Compress::inBit() {
	if (pBit==0) { pBit=8; bufBit=inChr(); }
	pBit--;
	return (bufBit>>pBit)&1;
}

unsigned Compress::inChr() {
	unsigned c;
	//if (pIn==sizeof(bufIn)) {
	//	if (fread(bufIn,1,sizeof(bufIn),fpIn)<1) { printf("Fail reading file"); fclose(fpIn); fclose(fpOut); exit(1); }
	//	pIn=0;
	//}
	c=bufIn[pIn]; pIn++;
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
	bufOut[pOut]=(unsigned char)c; pOut++;
	//if (pOut==sizeof(bufOut)) { fwrite(bufOut,1,sizeof(bufOut),fpOut); pOut=0; }
	data_size++;
}

void Compress::unzip(char* data_buffer) {
	strcpy((char*)bufIn,data_buffer);
	data_size=0;
	pIn=0;
	pOut=0;
	pBit=0;
	pIdx=0;
	init();
	unsigned i;
	for (i=M=0; i<32; M=(M<<1)|inBit(),i++);
	for (; data_size<519;) {
		unsigned x=decode(pIdx);
		if (x==ESC) { x=decode0(pIdx); }
		update(pIdx,x); pIdx=((pIdx&0x7f)<<7)|(x&0x1f)|((x>>1)&0xe0);  //Ë÷ÒýÖÐÆÁ±Î´óÐ¡Ð´
		outChr(x);
		//if ((iCount&0x1fffff)==0) { printf("%c%5.2f",13,(double)(len.__pos-iCount.__pos)*100/len.__pos); }
/*
if (len-iCount>=chkpoint && len-iCount<chkpoint+16) {
printf("%I64x\n",len-iCount);
}.
*/
	}
	//if (pOut>0) { fwrite(bufOut,1,pOut,fpOut); }  //Çå¿ÕÊä³ö»º³åÇø
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


