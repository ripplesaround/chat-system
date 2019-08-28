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

int get_user_info(char *username,char *password,Kind kind,char  *c_ipAddr)
{
    int port =MYPORT;
    int rec_len;
    struct sockaddr_in    servaddr;
    int MAXLINE = 4096;
    char    buf[MAXLINE];
    User user;
    Packet packet;
	client_socket=init_client(MYPORT,c_ipAddr);
    //client_socket = socket(AF_INET, SOCK_STREAM, 0);
	//printf("%s\n%s\n",username,password);
	//获得服务器的socket描述字，因为描述字只依赖于ip地址，协议，端口
    if(client_socket < 0){
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
    if( send(client_socket, &packet, sizeof(packet), 0) < 0)
    {
        //printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }
    if((rec_len = recv(client_socket, buf, MAXLINE,0)) == -1) {
        perror("recv error");
        exit(1);
    }
}
