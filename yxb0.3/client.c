#include "chat.h"
int client_socket;
int init_client(int port,char *addr)
{
	int cli_socket;
	int try_time;
	struct sockaddr_in server_addr;
	
	cli_socket=socket(AF_INET,SOCK_STREAM,0);	//创建客户端套接字
	if(cli_socket==-1)return -1;

	server_addr.sin_addr.s_addr=inet_addr(addr);
	server_addr.sin_port=htons(port);
	server_addr.sin_family=AF_INET;

	try_time=0;			//如果不成功每隔一秒连接一次，最多10次
	while(try_time<10 && connect(cli_socket,(struct sockaddr*)&server_addr,sizeof(server_addr))==-1)
    {
		sleep(1);
        try_time++;
    }

	if(try_time >= 10)return -1;
	else return cli_socket;
}

int registandlogin(char *username,char *password,Kind kind,char  *c_ipAddr)
{
    int port =MYPORT;
    struct sockaddr_in servaddr;
    int MAXLINE = 4096;
    char buf[MAXLINE];
    Data data;
    Packet packet;
	client_socket=init_client(MYPORT,c_ipAddr);
    if(client_socket < 0)
    {
        printf("create socket error\n");
        exit(0);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
	strcpy(data.userinfo.account,username);
    strcpy(data.userinfo.password,password);
    //printf("%s\n%s",data.userinfo.account,data.userinfo.password);
    build_packet(&packet,kind,data);
    write(client_socket, &packet, sizeof(Packet));
    read(client_socket, &packet, sizeof(Packet));
    parse_packet(packet,&kind,&data);
    printf("kkkk:\n%s\n%s\n",data.userinfo.account,data.userinfo.password);
    if(kind==enum_regist&&(!strcmp(data.userinfo.account,"")))  return 1;
    else if(kind==enum_login&&(!strcmp(data.userinfo.account,""))) return 1;
    else return 0;
}

/*void sendmessage(char *username,char *password,Kind kind,char  *c_ipAddr)
{
    int port =MYPORT;
    struct sockaddr_in servaddr;
    int MAXLINE = 4096;
    char buf[MAXLINE];
    Data data;
    Packet packet;
	client_socket=init_client(MYPORT,c_ipAddr);
    if(client_socket < 0)
    {
        printf("create socket error\n");
        exit(0);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
	strcpy(user.account,username);
    strcpy(user.password,password);
    //printf("%s\n%s",data.userinfo.account,data.userinfo.password);
    build_packet(&packet,kind,user);
    write(client_socket, &packet, sizeof(packet));
}*/