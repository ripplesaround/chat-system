#include<stdio.h>
#include<string.h>
char str[20]="";
char sha1[70]="";
void UInt32toStr(unsigned long n)
{
  char buf[40] = "";
  strcpy(str,"");
  unsigned int i = 0;
  unsigned int len = 0;
  unsigned long long temp = n < 0 ? -n: n;  // temp为n的绝对值
  while(temp)
  {
      //printf("%x\n",temp % 16);
      if(temp % 16 < 10) buf[i++] = (temp % 16) + '0';  //把temp的每一位上的数存入buf
      else buf[i++]=(temp % 16)+'a'-10;
      temp = temp / 16;
  }
 
  len = n < 0 ? ++i: i;  //如果n是负数，则多需要一位来存储负号
  str[i] = 0;            //末尾是结束符0
  while(1)
  {
    i--;
    if (buf[len-i-1] ==0){
        break;
    }
    str[i] = buf[len-i-1];  //把buf数组里的字符拷到字符串
  }
}
void creat_w(unsigned char input[64],unsigned long w[80])
{
   int i,j;unsigned long temp,temp1;
   for(i=0;i<16;i++)
          {
             j=4*i;
             w[i]=((long)input[j])<<24 |((long)input[1+j])<<16|((long)input[2+j])<<8|((long)input[3+j])<<0;
 
          }
   for(i=16;i<80;i++)
         {
             w[i]=w[i-16]^w[i-14]^w[i-8]^w[i-3];
             temp=w[i]<<1;
             temp1=w[i]>>31;
             w[i]=temp|temp1;
 
         }
}
char ms_len(long a,char intput[64])
{
    unsigned long temp3,p1;  int i,j;
    temp3=0;
    p1=~(~temp3<<8);
    for(i=0;i<4;i++)
       {
          j=8*i;
          intput[63-i]=(char)((a&(p1<<j))>>j);
 
       }
 
}
char* sha(unsigned char input[64])
{
   unsigned long H0=0x67452301,H1=0xefcdab89,H2=0x98badcfe,H3=0x10325476,H4=0xc3d2e1f0;
   unsigned long A,B,C,D,E,temp,temp1,temp2,temp3,k,f;int i,flag;unsigned long w[80];
   long x;int n;
   //printf("input message:\n");
   //scanf("%s",input);
   n=strlen((char*)input);
   if(n<57)
          {
                 x=n*8;
                 ms_len(x,(char*)input);
                 if(n==56)
                     for(i=n;i<60;i++)
                     input[i]=0;
                 else
                    {
                     input[n]=128;
                     for(i=n+1;i<60;i++)
                     input[i]=0;
                    }
 
          }
 
   creat_w(input,w);
   /*for(i=0;i<80;i++)
   printf("%lx,",w[i]);*/
   printf("\n");
   A=H0;B=H1;C=H2;D=H3;E=H4;
   for(i=0;i<80;i++)
         {
               flag=i/20;
               switch(flag)
                  {
                   case 0: k=0x5a827999;f=(B&C)|(~B&D);break;
                   case 1: k=0x6ed9eba1;f=B^C^D;break;
                   case 2: k=0x8f1bbcdc;f=(B&C)|(B&D)|(C&D);break;
                   case 3: k=0xca62c1d6;f=B^C^D;break;
                  }
               /*printf("%lx,%lx\n",k,f); */
               temp1=A<<5;
               temp2=A>>27;
               temp3=temp1|temp2;
               temp=temp3+f+E+w[i]+k;
               E=D;
               D=C;
 
               temp1=B<<30;
               temp2=B>>2;
               C=temp1|temp2;
               B=A;
               A=temp;
               //printf("%lx,%lx,%lx,%lx,%lx\n",A,B,C,D,E);
         }
   H0=H0+A;
   H1=H1+B;
   H2=H2+C;
   H3=H3+D;
   H4=H4+E;
   //printf("yes\n");
   UInt32toStr(H0);
   strcat(sha1,str);
   UInt32toStr(H1);
   strcat(sha1,str);
   UInt32toStr(H2);
   strcat(sha1,str);
   UInt32toStr(H3);
   strcat(sha1,str);
   UInt32toStr(H4);
   strcat(sha1,str);
   printf("%s\n",sha1);
   return sha1;
   //strcpy(s0,str);
   //s1=UInt32toStr(H1);
   //printf("%s\n%s\n",s0,s1);
}
