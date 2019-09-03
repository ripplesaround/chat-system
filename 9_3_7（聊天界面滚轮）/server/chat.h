#ifndef chat_h
#define chat_h

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>

#define MYPORT 4567			//约定端口
#define MAXLEN 140			//最大消息长度

typedef struct _message{
	int id;					//消息的标识符
	char id_to[20];			//消息发送给谁
	char id_from[20];	    //消息从谁发的
	char str[MAXLEN+1];		//消息正文
	char time[100];         //消息时间
	char account[20];       //用户名
}Message;					//一条消息的结构体
typedef struct _user{
	char account[20];		//账号
	char password[20];		//密码
	int user_id;			//用户id（绝对）
	char user_ip;           //用户ip（每次登陆更新）
}User;						//用户登录信息
typedef union _data{
	User userinfo;			//用户信息
	Message message;		//消息
}Data;						//数据包共用体
typedef enum _kind{
	enum_regist,enum_login,enum_logout,enum_chat,enum_modify,enum_friend,enum_docu,enum_quitchat,enum_ipchat
	//注册			    登录 		  登出		      发送消息  修改用户密码
}Kind;						//用枚举变量表示包类型
typedef struct _packet{
	Kind kind;		        //包类型
	Data data;		        //数据包
	int verify;             //脏包检测位
}Packet;					//通信协议

int build_packet(Packet *packet,Kind kind,...);
//将数据和类型打包，封装到packet中。使用变长参数，第三个参数可以任意类型
int parse_packet(Packet packet,Kind *kind,Data *data,int *verify);
//解析数据包，把类型和数据取出来



#endif
