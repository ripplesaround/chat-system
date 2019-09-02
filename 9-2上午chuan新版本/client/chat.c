#include "chat.h"

int build_packet(Packet *packet,Kind kind,...){
	va_list ap;			//va_list 一个指向当前参数的指针类型。
	packet->kind=kind;
	packet->verify=23333;	
	va_start(ap,kind);		//va_start 初始化va_list的对象
	switch(kind){
		case enum_regist:
		case enum_modify:
		case enum_logout:
		case enum_login:packet->data=(Data)va_arg(ap,User);break;	//va_arg 返回当前参数
		case enum_chat:packet->data=(Data)va_arg(ap,Message);break;
		case enum_friend:packet->data=(Data)va_arg(ap,Message);break;
		case enum_docu:packet->data=(Data)va_arg(ap,Message);break;
		case enum_quitchat:break;
		case enum_ipchat:packet->data=(Data)va_arg(ap,Message);break;
		default:return -1;
	}
	va_end(ap);	//释放指针
	return 0;
}
int parse_packet(Packet packet,Kind *kind,Data *data,int *verify){
	*kind=packet.kind;
	*data=packet.data;
	*verify=packet.verify;
	return 0;
}

