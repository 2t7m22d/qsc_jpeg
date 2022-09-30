#include <stdio.h>
#include <math.h>

#define PI 3.1415926

int pixel_R[256][256];
int pixel_G[256][256];
int pixel_B[256][256]; /*bin文件像素记录*/ 

float pixel_Y[256][256];
float pixel_Cr[256][256];
float pixel_Cb[256][256]; /*储存转换为YCrCb颜色空间后信息*/ 

float DCT_Y[256][256];
float DCT_Cr[256][256];
float DCT_Cb[256][256];  /*储存DCT转化后像素点信息*/ 

int QY[8][8]={{16,11,10,16,24,40,51,61},
              {12,12,14,19,26,58,60,55},
              {14,13,16,24,40,57,69,56},
              {14,17,22,29,51,87,80,62},
              {18,22,37,56,68,109,103,77},
              {24,35,55,64,81,104,113,92},
              {49,64,78,87,103,121,120,101},
              {72,92,95,98,112,100,103,99},
			 };
int QC[8][8]={{17,18,24,47,99,99,99,99},
              {18,21,26,66,99,99,99,99},
              {24,26,56,99,99,99,99,99},
              {47,66,99,99,99,99,99,99},
              {99,99,99,99,99,99,99,99},
              {99,99,99,99,99,99,99,99},
              {99,99,99,99,99,99,99,99},
              {99,99,99,99,99,99,99,99},
             };   /*初始化量化表*/ 
             
int Q_Y[256][256];
int Q_Cr[256][256];
int Q_Cb[256][256];  /*储存量化后信息*/ 

int Zigzag_Y[1024][64];
int Zigzag_Cr[1024][64];
int Zigzag_Cb[1024][64];  /*储存Zigzag排列*/ 

int RLC_Y[1024][64];
int RLC_Cr[1024][64];
int RLC_Cb[1024][64];
int flag_Y[1024][64];
int flag_Cr[1024][64];
int flag_Cb[1024][64];
/*储存游程编码,其中flag标记应为变长编码部分*/

int DC_Y_lcnt[16]={00,01,05,01,01,01,01,01,01,00,00,00,00,00,00,00};  /*lcnt表示该编码表,对应编码长度下被编码数字数量*/
int DC_Y_code[13]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,256};  /*记录被编码数字顺序,其中256作为数组末尾标记*/ 
int DC_C_lcnt[16]={00,03,01,01,01,01,01,01,01,01,01,00,00,00,00,00};
int DC_C_code[13]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,256};
int AC_Y_lcnt[16]={0x00,0x02,0x01,0x03,0x03,0x02,0x04,0x03,0x05,0x05,0x04,0x04,0x00,0x00,0x01,0x7D};
int AC_Y_code[251]={0x01,0x02,0x03,0x00,0x04,0x11,0x05,0x12,0x21,0x31,0x41,0x06,0x13,0x51,0x61,0x07,0x22,0x71,0x14,0x32,0x81,0x91,0xA1,0x08,0x23,0x42,0xB1,0xC1,
0x15,0x52,0xD1,0xF0,0x24,0x33,0x62,0x72,0x82,0x09,0x0A,0x16,0x17,0x18,0x19,0x1A,0x25,0x26,0x27,0x28,0x29,0x2A,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x43,0x44,0x45,
0x46,0x47,0x48,0x49,0x4A,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x83,0x84,0x85,
0x86,0x87,0x88,0x89,0x8A,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,
0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xF1,0xF2,0xF3,0xF4,
0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,256};
int AC_C_lcnt[16]={0x00,0x02,0x01,0x02,0x04,0x04,0x03,0x04,0x07,0x05,0x04,0x04,0x00,0x01,0x02,0x77};
int AC_C_code[251]={0x00,0x01,0x02,0x03,0x11,0x04,0x05,0x21,0x31,0x06,0x12,0x41,0x51,0x07,0x61,0x71,0x13,0x22,0x32,0x81,0x08,0x14,0x42,0x91,0xA1,0xB1,0xC1,0x09,
0x23,0x33,0x52,0xF0,0x15,0x62,0x72,0xD1,0x0A,0x16,0x24,0x34,0xE1,0x25,0xF1,0x17,0x18,0x19,0x1A,0x26,0x27,0x28,0x29,0x2A,0x35,0x36,0x37,0x38,0x39,0x3A,0x43,0x44,
0x45,0x46,0x47,0x48,0x49,0x4A,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x82,0x83,
0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,
0xB9,0xBA,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xF2,0xF3,0xF4,
0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,256};

char Huffman_value[17]; /*保存huffman编码后字符串*/ 
char OC[17]; /*保存反码值*/ 


float C(int x){
	if(x==0)
	return 0.7071;
	else
	return 1;	
}   
/*离散余弦变换中补偿系数*/ 

float Sum_Y(int u,int v,int left,int up){
	float sum=0;
	int i,j;
	for(i=left;i<left+8;i++){
		for(j=up;j<up+8;j++){
			sum=pixel_Y[i][j]*cos((2*(i%8)+1)*(u%8)*PI/16)*cos((2*(j%8)+1)*(v%8)*PI/16)+sum;
		}
	}
	return sum;
}   
/*计算离散余弦变换中Sum部分,下同，分别为计算Cr和Cb部分*/ 

float Sum_Cr(int u,int v,int left,int up){
	float sum=0;
	int i,j;
	for(i=left;i<left+8;i++){
		for(j=up;j<up+8;j++){
			sum=pixel_Cr[i][j]*cos((2*(i%8)+1)*(u%8)*PI/16)*cos((2*(j%8)+1)*(v%8)*PI/16)+sum;
		}
	}
	return sum;
}

float Sum_Cb(int u,int v,int left,int up){
	float sum=0;
	int i,j;
	for(i=left;i<left+8;i++){
		for(j=up;j<up+8;j++){
			sum=pixel_Cb[i][j]*cos((2*(i%8)+1)*(u%8)*PI/16)*cos((2*(j%8)+1)*(v%8)*PI/16)+sum;
		}
	}
	return sum;
}

void F(int u,int v){
	int i=u+8,j=v+8;
	for(;u<i;u++){
	for(;v<j;v++){
			DCT_Y[u][v]=0.25*C(u%8)*C(v%8)*Sum_Y(u,v,i-8,j-8);
			DCT_Cr[u][v]=0.25*C(u%8)*C(v%8)*Sum_Cr(u,v,i-8,j-8);
			DCT_Cb[u][v]=0.25*C(u%8)*C(v%8)*Sum_Cb(u,v,i-8,j-8);
		}
	}
	return;
}   
/*计算离散余弦变换*/ 

void quantization(int left,int up){
	int i,j;
	for(i=0;i<8;i++){
		for(j=0;j<8;j++){
			Q_Y[left+i][up+j]=(int)(DCT_Y[left+i][up+j]/QY[i][j]);
			Q_Cr[left+i][up+j]=(int)(DCT_Cr[left+i][up+j]/QC[i][j]);
			Q_Cb[left+i][up+j]=(int)(DCT_Cb[left+i][up+j]/QC[i][j]);
		}
	}
} 
/*计算量化*/
 
void Zigzag(int left,int up,int num){
	int i=0,j=0,count=0;
	Zigzag_Y[num][count]=Q_Y[left+i][up+j]-Zigzag_Y[num-1][count];
	Zigzag_Cr[num][count]=Q_Cr[left+i][up+j]-Zigzag_Cr[num-1][count];
	Zigzag_Cb[num][count]=Q_Cb[left+i][up+j]-Zigzag_Cb[num-1][count];   /*对第一个直流系数做差分*/ 
	j++;count++;
	while(!(i==7&&j==7)){
		Zigzag_Y[num][count]=Q_Y[left+i][up+j];
		Zigzag_Cr[num][count]=Q_Cr[left+i][up+j];
		Zigzag_Cb[num][count]=Q_Cb[left+i][up+j];
		count++;
		if(j%2==0&&(i==0||i==7)){
			j++;
		}else if(i%2==1&&(j==0||j==7)){
			i++;
		}else{
			if((i+j)%2==0){
				i--;j++;
			}else{
				i++;j--;
			}
		}
	}
}   
/*做Zigzag排列*/
 
int judge_EOB_Y(int i,int j){
	int n;
	for(n=j;n<64;n++){
		if(Zigzag_Y[i][n]!=0) return 1;
	}
	return 0;
}
/*判断Zigzag数组是否到末尾全是0部分，其中为Y部分代码，下面都一样的，分别编码Cr和Cb*/ 

int judge_EOB_Cr(int i,int j){
	int n;
	for(n=j;n<64;n++){
		if(Zigzag_Cr[i][n]!=0) return 1;
	}
	return 0;
}

int judge_EOB_Cb(int i,int j){
	int n;
	for(n=j;n<64;n++){
		if(Zigzag_Cb[i][n]!=0) return 1;
	}
	return 0;
}

int search(int x,int *temp){
	int i=0;
	while((*temp)!=256){
		if((*temp)==x) return i;
		temp++;
	}
	return 256;
}
/*找到数组temp中的对应数x*/ 

void add1(int bit){
	if(Huffman_value[bit-1]=='0'){
		Huffman_value[bit-1]=='1';
	}else{
		add1(bit-1);
		Huffman_value[bit-1]='0';
	}
	return;
}
/*二进制进位*/ 

void binary(int value,int bit){
	int i;
	for(i=0;i<bit;i++){
		Huffman_value[i]='0';
	}
	Huffman_value[bit]='\0';
	while(value>0){
		add1(bit);
		value--;
	}
	return;
}
/*将十进制转化为二进制*/ 

void huffman_print(int code,int *lcnt,FILE *fp){
	int end=0,i,j,begin,huffman_begin=0,bit,huffman_value;
	char value[17];
	code++;
	for(i=0;i<16;i++){
		if(end>=code){
			begin=end-lcnt[i-1];
			for(j=1;j<i-1;j++){
					huffman_begin=huffman_begin+lcnt[j]-1;
					huffman_begin=(huffman_begin+1)*2;
				}
		    huffman_value=huffman_begin+(code-begin-1);
		    bit=i;
		    binary(huffman_value,bit);
		    fprintf(fp,"%s ",Huffman_value);
		    return;
			}else{
			end=end+lcnt[i];
	 	    }
	} 
}
/*将给定的code值转化成huffman值打印到文件上*/ 

int bit(int x){
	int cnt=0;
	if(x==0||x==1){
		return 1;
	}
	while(x!=0){
		x=x/2;
		cnt=cnt+1;
	}
	return cnt;
}
/*计算给定数的二进制数的位数*/

void RLC_print(int x,FILE *fp){
	int i=0;
	binary(x,bit(x));
	while(Huffman_value[i]!='\0'){
		if(Huffman_value[i]=='0'){
			Huffman_value[i]=='1';
		}else{
			Huffman_value[i]=='0';
		}
		i++;
	}
	fprintf(fp,"%s ",Huffman_value);
}
/*输出给定数的反码*/ 
 
int main(){
	FILE *fp;
	fp=fopen("demo.bin","rb");

	int i,j,*R,*G,*B;
	float (*Y)[256],(*Cr)[256],(*Cb)[256];
	
	R=&pixel_R[0][0];
	G=&pixel_G[0][0];
	B=&pixel_B[0][0];
	Y=pixel_Y;
	Cr=pixel_Cr;
	Cb=pixel_Cb;
	
	for(i=0;i<256;i++){
		for(j=0;j<256;j++){
		fread(R,1,1,fp);
		fread(G,1,1,fp);
		fread(B,1,1,fp);
		R++;B++;G++;
		}
	}   /*读入bin文件*/ 
	fclose(fp);  
	
	for(i=0;i<256;i++){
		for(j=0;j<256;j++){
			pixel_Y[i][j]=0.299*pixel_R[i][j]+0.587*pixel_G[i][j]+0.144*pixel_B[i][j]-128;
			pixel_Cr[i][j]=0.5*pixel_R[i][j]-0.4187*pixel_G[i][j]-0.0813*pixel_B[i][j];
			pixel_Cb[i][j]=-0.1687*pixel_R[i][j]-0.3313*pixel_G[i][j]+0.5*pixel_B[i][j];
		}
	}  /*转换为YCrCb颜色空间,并将数值减少128*/
	
	int  left,up;  /*定义矩阵边界*/ 
	
	for(left=0;left<256;left=left+8){
		for(up=0;up<256;up=up+8){
			F(left,up);
		}
	}  /*划分矩阵,计算离散余弦变换*/ 
	
	for(left=0;left<256;left=left+8){
		for(up=0;up<256;up=up+8){
			quantization(left,up);
		}
	} /*划分矩阵,计算量化*/
	
	int num=0;  /*记录是第几个zigzag排列*/ 
	
	Zigzag_Y[0][0]=Q_Y[0][0];
	Zigzag_Cr[0][0]=Q_Cr[0][0];
	Zigzag_Cb[0][0]=Q_Cb[0][0];  /*存储第一个矩阵的第一个直流系数*/ 
	
	for(up=0;up<256;up=up+8){
		for(left=0;left<256;left=left+8){
			Zigzag(left,up,num);
			num++;
		}
	} /*划分矩阵，做Zigzag排列*/
	 
  /*FILE *fp_Y,*fp_Cr,*fp_Cb;
	fp_Y=fopen("Zigzag_Y.txt","w");
	fp_Cr=fopen("Zigzag_Cr.txt","w");
	fp_Cb=fopen("Zigzag_Cb.txt","w");
	
	for(i=0;i<1024;i++){
		for(j=0;j<64;j++){
			fprintf(fp_Y,"%d ",Zigzag_Y[i][j]);
		}
	}
	
	for(i=0;i<1024;i++){
		for(j=0;j<64;j++){
			fprintf(fp_Cr,"%d ",Zigzag_Cr[i][j]);
		}
	}
	
	for(i=0;i<1024;i++){
		for(j=0;j<64;j++){
			fprintf(fp_Cb,"%d ",Zigzag_Cb[i][j]);
		}
	}
	
	fclose(fp_Y);fclose(fp_Cr);fclose(fp_Cb);
	输出数据，验证数据*/ 
	
	int m=0,n=0;

    for(i=0;i<1024;i++){
		num=0;
		for(j=0;j<64;j++){
			if(j==0){
				RLC_Y[m][n]=Zigzag_Y[i][j];
				n++;
				continue;
			}
			if(Zigzag_Y[i][j]==0){
				num++;
				if(num==16&&judge_EOB_Y(i,j)){
					num=0;
					RLC_Y[m][n]=15;n++;
					RLC_Y[m][n]=0;n++;
				}else if(!(judge_EOB_Y(i,j))){
					RLC_Y[m][n]=0;RLC_Y[m][n+1]=0;
					break;
				}
			}else{
				RLC_Y[m][n]=num;
				num=0;n++;
				RLC_Y[m][n]=Zigzag_Y[i][j];
                flag_Y[m][n]=1;
				n++;
			}
		}
		m++;
	}    /*游程编码，标记应变长编码,其中为Y部分代码，下面都一样的，分别编码Cr和Cb*/  
	
	m=0;n=0;
	
	for(i=0;i<1024;i++){
		num=0;
		for(j=0;j<64;j++){
			if(j==0){
				RLC_Cr[m][n]=Zigzag_Cr[i][j];
				n++;
				continue;
			}
			if(Zigzag_Cr[i][j]==0){
				num++;
				if(num==16&&judge_EOB_Cr(i,j)){
					num=0;
					RLC_Cr[m][n]=15;n++;
					RLC_Cr[m][n]=0;n++;
				}else if(!(judge_EOB_Cr(i,j))){
					RLC_Cr[m][n]=0;RLC_Cr[m][n+1]=0;
					break;
				}
			}else{
				RLC_Cr[m][n]=num;
				num=0;n++;
				RLC_Cr[m][n]=Zigzag_Cr[i][j];
                flag_Cr[m][n]=1;
				n++;
			}
		}
		m++;
	}
	
	m=0;n=0;
	
	for(i=0;i<1024;i++){
		num=0;
		for(j=0;j<64;j++){
			if(j==0){
				RLC_Cb[m][n]=Zigzag_Cb[i][j];
				n++;
				continue;
			}
			if(Zigzag_Cb[i][j]==0){
				num++;
				if(num==16&&judge_EOB_Cb(i,j)){
					num=0;
					RLC_Cb[m][n]=15;n++;
					RLC_Cb[m][n]=0;n++;
				}else if(!(judge_EOB_Cb(i,j))){
					RLC_Cb[m][n]=0;RLC_Cb[m][n+1]=0;
					break;
				}
			}else{
				RLC_Cb[m][n]=num;
				num=0;n++;
				RLC_Cb[m][n]=Zigzag_Cb[i][j];
                flag_Cb[m][n]=1;
				n++;
			}
		}
		m++;
	}
	
	FILE *fp_txt;
	fp_txt=fopen("binary.txt","w");   /*将处理结果先输入binary.txt，再将binary.txt转化为二进制文件（未实现）*/ 
	int *temp_array;
	int code;  /*记录在编码表的顺序*/ 

    for(i=0;i<1024;i++){
    	for(j=0;j<64;j++){
    		if(j==0){
    			temp_array=DC_Y_code;
    		    code=search(RLC_Y[i][j],temp_array);
    		    temp_array=DC_Y_lcnt;
    		    huffman_print(code,temp_array,fp_txt); 
			}else{
				if(flag_Y[i][j]==0){
			    temp_array=AC_Y_code;
				code=search(RLC_Y[i][j]+bit(RLC_Y[i+1][j+1]),temp_array);
				temp_array=AC_Y_lcnt;
				huffman_print(code,temp_array,fp_txt);
				RLC_print(RLC_Y[i+1][j+1],fp_txt);
				}
			}
		}
		for(j=0;j<64;j++){
    		if(j==0){
    			temp_array=DC_C_code;
    		    code=search(RLC_Cr[i][j],temp_array);
    		    temp_array=DC_C_lcnt;
    		    huffman_print(code,temp_array,fp_txt); 
			}else{
				if(flag_Cr[i][j]==0){
			    temp_array=AC_C_code;
				code=search(RLC_Cr[i][j]+bit(RLC_Cr[i+1][j+1]),temp_array);
				temp_array=AC_C_lcnt;
				huffman_print(code,temp_array,fp_txt);
				RLC_print(RLC_Cr[i+1][j+1],fp_txt);
				}
			}
		}
		for(j=0;j<64;j++){
    		if(j==0){
    			temp_array=DC_C_code;
    		    code=search(RLC_Cb[i][j],temp_array);
    		    temp_array=DC_C_lcnt;
    		    huffman_print(code,temp_array,fp_txt); 
			}else{
				if(flag_Cb[i][j]==0){
			    temp_array=AC_C_code;
				code=search(RLC_Cb[i][j]+bit(RLC_Cb[i+1][j+1]),temp_array);
				temp_array=AC_C_lcnt;
				huffman_print(code,temp_array,fp_txt);
				RLC_print(RLC_Cb[i+1][j+1],fp_txt);
				}
			}
		}
	}/*霍夫曼编码并输出为文本文档*/
	
	return 0;
}
