#include "chat.h"

int build_packet(Packet *packet,Kind kind,...){
	va_list ap;			//va_list 一个指向当前参数的指针类型。
	packet->kind=kind;		
	va_start(ap,kind);		//va_start 初始化va_list的对象
	switch(kind){
		case enum_regist:
		case enum_modify:
		case enum_logout:
		case enum_login:packet->data=(Data)va_arg(ap,User);break;	//va_arg 返回当前参数
		case enum_chat:packet->data=(Data)va_arg(ap,Message);break;
		default:return -1;
	}
	va_end(ap);	//释放指针
	return 0;
}
int parse_packet(Packet packet,Kind *kind,Data *data){
	*kind=packet.kind;
	*data=packet.data;
	return 0;
}

