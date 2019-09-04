#include<stdio.h>
#include<string.h>
#include<unistd.h>
#define Max 1024
char aes_str[20];
int S[16][16] =   ///s盒
{
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};
 
int S2[16][16] =    ///逆s盒
{
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

void printASCCI(char *str, int len)///输出字符串
{
    int c;
    for(int i = 0; i < len; i++)
    {
        c = (int)*str++;
        c = c & 0x000000ff;
        //printf("%c ",(c%26+97));
        aes_str[i]=(char)((c%26)+97);
    }
    printf("%s\n",aes_str);
}

///功能实现区
 
int checkKeyLen(int len) ///检查密钥长度
{
    if(len == 16)
        return 1;
    else
        return 0;
}
 
int getIntFromChar(char c) ///把一个字符化为整型
{
    int result = (int) c;
    return result & 0x000000ff;
}
int getWordFromStr(char *str) ///密钥扩展--密钥化为二进制
{
    int one = getIntFromChar(str[0]);
    one = one << 24;
    int two = getIntFromChar(str[1]);
    two = two << 16;
    int three = getIntFromChar(str[2]);
    three = three << 8;
    int four = getIntFromChar(str[3]);
    return one | two | three | four;
}
 
void splitIntToArray(int num, int array[4]) ///把一个4字节的数的第一、二、三、四个字节取出，入进一个4个元素的整型数组里面。
{
    int one = num >> 24;
    array[0] = one & 0x000000ff;
    int two = num >> 16;
    array[1] = two & 0x000000ff;
    int three = num >> 8;
    array[2] = three & 0x000000ff;
    array[3] = num & 0x000000ff;
}
int mergeArrayToInt(int array[4]) ///把数组中的第一、二、三和四元素分别作为4字节整型的第一、二、三和四字节，合并成一个4字节整型
{
    int one = array[0] << 24;
    int two = array[1] << 16;
    int three = array[2] << 8;
    int four = array[3];
    return one | two | three | four;
}
 
void leftLoop4int(int array[4], int step) ///将数组中的元素循环左移step位
{
    int temp[4];
    for(int i = 0; i < 4; i++)
        temp[i] = array[i];
 
    int index = step % 4 == 0 ? 0 : step % 4;
    for(int i = 0; i < 4; i++)
    {
        array[i] = temp[index];
        index++;
        index = index % 4;
    }
}
void rightLoop4int(int array[4], int step) ///将数组循环右移step位
{
    int temp[4];
    for(int i = 0; i < 4; i++)
        temp[i] = array[i];
 
    int index = step % 4 == 0 ? 0 : step % 4;
    index = 3 - index;
    for(int i = 3; i >= 0; i--)
    {
        array[i] = temp[index];
        index--;
        index = index == -1 ? 3 : index;
    }
}
 
int getLeft4Bit(int num) ///获取八比特的前四位，并化为十六进制
{
    int left = num & 0x000000f0;
    return left >> 4;
}
int getRight4Bit(int num) ///获取八比特的后四位，并化为十六进制
{
    return num & 0x0000000f;
}
 
int getNumFromSBox(int index) ///从s盒获取元素
{
    int row = getLeft4Bit(index);
    int col = getRight4Bit(index);
    return S[row][col];
}
int getNumFromS1Box(int index) ///从逆s盒获取元素
{
    int row = getLeft4Bit(index);
    int col = getRight4Bit(index);
    return S2[row][col];
}
 
unsigned int Rcon[10] =            ///常量轮值表
{
    0x01000000, 0x02000000,
    0x04000000, 0x08000000,
    0x10000000, 0x20000000,
    0x40000000, 0x80000000,
    0x1b000000, 0x36000000
};
 
int T(int num, int round) ///密钥扩展的T函数
{
    int numArray[4];
    splitIntToArray(num, numArray);
    leftLoop4int(numArray, 1);//字循环
 
    //字节代换
    for(int i = 0; i < 4; i++)
        numArray[i] = getNumFromSBox(numArray[i]);
 
    int result = mergeArrayToInt(numArray);
    return result ^ Rcon[round];
}
 
int w[45];
 
void extendKey(char *key) ///扩展密钥长度
{
    for(int i = 0; i < 4; i++)
        w[i] = getWordFromStr(key + i * 4);
 
    for(int i = 4, j = 0; i < 44; i++)
    {
        if( i % 4 == 0)
        {
            w[i] = w[i - 4] ^ T(w[i - 1], j);
            j++;//下一轮
        }
        else
        {
            w[i] = w[i - 4] ^ w[i - 1];
        }
    }
}
 
void convertToIntArray(char *str, int pa[4][4]) ///把字符转化为字节并按矩阵排列，中字节排序，先上后下，在左后右
{
    int k = 0;
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
        {
            pa[j][i] = getIntFromChar(str[k]);
            k++;
        }
}
 
void convertArrayToStr(int array[4][4], char *str) ///把字节转换为字符
{
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            *str++ = (char)array[j][i];
}
 
int colM[4][4] =     ///列混合所用到的矩阵
{
    2, 3, 1, 1,
    1, 2, 3, 1,
    1, 1, 2, 3,
    3, 1, 1, 2
};
 
int GFMul2(int s) ///GF上对应的值
{
    int result = s << 1;
    int a7 = result & 0x00000100;
 
    if(a7 != 0)
    {
        result = result & 0x000000ff;
        result = result ^ 0x1b;
    }
 
    return result;
}
int GFMul3(int s)
{
    return GFMul2(s) ^ s;
}
int GFMul4(int s)
{
    return GFMul2(GFMul2(s));
}
int GFMul8(int s)
{
    return GFMul2(GFMul4(s));
}
int GFMul9(int s)
{
    return GFMul8(s) ^ s;
}
int GFMul11(int s)
{
    return GFMul9(s) ^ GFMul2(s);
}
int GFMul12(int s)
{
    return GFMul8(s) ^ GFMul4(s);
}
int GFMul13(int s)
{
    return GFMul12(s) ^ s;
}
int GFMul14(int s)
{
    return GFMul12(s) ^ GFMul2(s);
}
 
int GFMul(int n, int s) ///GF上的二元运算
{
    int result;
 
    if(n == 1)
        result = s;
    else if(n == 2)
        result = GFMul2(s);
    else if(n == 3)
        result = GFMul3(s);
    else if(n == 0x9)
        result = GFMul9(s);
    else if(n == 0xb)//11
        result = GFMul11(s);
    else if(n == 0xd)//13
        result = GFMul13(s);
    else if(n == 0xe)//14
        result = GFMul14(s);
 
    return result;
}
 
 
///1.加密
void addRoundKey(int array[4][4], int round) ///轮密钥加
{
    int warray[4];
    for(int i = 0; i < 4; i++)
    {
 
        splitIntToArray(w[ round * 4 + i], warray);
 
        for(int j = 0; j < 4; j++)
        {
            array[j][i] = array[j][i] ^ warray[j];
        }
    }
}
 
void subBytes(int array[4][4]) ///字节代换
{
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            array[i][j] = getNumFromSBox(array[i][j]);
}
 
void shiftRows(int array[4][4]) ///行移位
{
    int rowTwo[4], rowThree[4], rowFour[4];
    //复制状态矩阵的第2,3,4行
    for(int i = 0; i < 4; i++)
    {
        rowTwo[i] = array[1][i];
        rowThree[i] = array[2][i];
        rowFour[i] = array[3][i];
    }
    //循环左移相应的位数
    leftLoop4int(rowTwo, 1);
    leftLoop4int(rowThree, 2);
    leftLoop4int(rowFour, 3);
 
    //把左移后的行复制回状态矩阵中
    for(int i = 0; i < 4; i++)
    {
        array[1][i] = rowTwo[i];
        array[2][i] = rowThree[i];
        array[3][i] = rowFour[i];
    }
}
 
void mixColumns(int array[4][4]) ///列混合
{
 
    int tempArray[4][4];
 
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            tempArray[i][j] = array[i][j];
 
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
        {
            array[i][j] = GFMul(colM[i][0],tempArray[0][j]) ^ GFMul(colM[i][1],tempArray[1][j])
                          ^ GFMul(colM[i][2],tempArray[2][j]) ^ GFMul(colM[i][3], tempArray[3][j]);
        }
}
 
void aes(char *p, int plen, char *key)///AES加密
{
    int keylen=strlen(key);
    int add_num=16-(plen%16);
    while(add_num--)
    {
        printf("%d  ",add_num);
        strcat(p,"a");
    }
    printf("test:%s\n",p);
    extendKey(key);//扩展密钥
    int pArray[4][4];
    for(int k = 0; k < plen; k += 16)
    {
        convertToIntArray(p + k, pArray);//变成矩阵
        addRoundKey(pArray, 0);//一开始的轮密钥加
        for(int i = 1; i < 10; i++) //前9轮
        {
            subBytes(pArray);//字节代换
            shiftRows(pArray);//行移位
            mixColumns(pArray);//列混合
            addRoundKey(pArray, i);//轮密钥加
        }
        //第10轮
        subBytes(pArray);//字节代换
        shiftRows(pArray);//行移位
        addRoundKey(pArray, 10);//轮密钥加
        convertArrayToStr(pArray, p + k);
    }
}
 
void aesStrToFile(char *key,char *password)
{
    char p[Max];
    strcpy(p,password);
    int plen;
    plen=strlen(p);
    aes(p, plen, key);//AES加密
    printASCCI(p, plen);
}
 

char* aes_password(char *password)
{

    char key[17]="123456789abcdefg";
    aesStrToFile(key,password);//用AES加密字符串，并将字符串写进文件中
    return aes_str;
}
