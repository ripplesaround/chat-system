#include <signal.h>
#include <stdio.h>
#include <syslog.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <arpa/inet.h>
#include <errno.h>       
#include<sys/sem.h>
#include <time.h>
#include <pthread.h>  
#define THREAD_NUMBER 30 //最大登录数
#define MYPORT 8787
#define BUFFER_SIZE 1024
struct Users{              //声明结构体Users。
	char name[50];         //昵称。
	pthread_t thread;		//声明线程ID thread。
	char buf[BUFFER_SIZE];  //消息缓冲区。
	int client_fd;         //socket端口。
	char address[20];      //地址。
	int login;         //登录标志。
}users[THREAD_NUMBER]; //结构体数组users。
 
int sem_id; //存储信号量描述符。
 
//得到当前时间并存储在nt内。
void get_now_time(char *nt){
	time_t tmpcal_ptr;   //长整型long int,适合存储日历时间类型。
	time(&tmpcal_ptr);//获取从1970-1-1,0时0分0秒到现在时刻的秒数。
	struct tm *tmp_ptr = NULL; //用来保存时间和日期的结构。
	tmp_ptr = localtime(&tmpcal_ptr);//把从1970-1-1,0时0分0秒到当前时间系统所偏移的秒数时间转换为本地时间
	sprintf(nt,"%d:%d:%d", tmp_ptr->tm_hour, tmp_ptr->tm_min, tmp_ptr->tm_sec);	 //将内容写入nt;
}
//初始化信号量。			  
int init_sem(int sem_id, int init_value){
	union semun{ 
		int val;
		struct semid_ds *buf; 
		unsigned short *array;
	};
	union semun sem_union;
	sem_union.val = init_value;
	if(semctl(sem_id, 0, SETVAL, sem_union) == -1){
		syslog(LOG_ERR, "Initialize semaphore");
		perror("Initialize semaphore");
		return -1;
	}
	return 0;
}
//删除信号量。
int del_sem(int sem_id){
	union semun{
		int val;
		struct semid_ds *buf;
		unsigned short *array;
	};
	union semun sem_union;
	if(semctl(sem_id, 1, IPC_RMID, sem_union)==-1){
		syslog(LOG_ERR, "Delete semaphore");
		perror("Delete semaphore");
		return -1;
	}
}
//p操作函数。
int sem_p(int sem_id){
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = -1;
	sem_b.sem_flg = SEM_UNDO;
	if(semop(sem_id, &sem_b, 1)==-1){
		syslog(LOG_ERR, "P operation");
		perror("P operation");
		return -1;
	}
	return 0;
}
//v操作函数。
int sem_v(int sem_id){
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = 1;
	sem_b.sem_flg = SEM_UNDO;
	if(semop(sem_id, &sem_b, 1) == -1){
		syslog(LOG_ERR, "V operation");
		perror("V operation");
		return -1;
	}
	return 0;
}
 
 
 
//将loadsend发送给所有人
void send_all(char loadsend[BUFFER_SIZE]){
	int i;//用来定位。
	for(i=0;i<THREAD_NUMBER;i++){  //遍历结构体数组users。
		if(users[i].login==1){    //如果users[i]的登录标志位login为1,即此线程已有用户使用。
			send(users[i].client_fd, loadsend, strlen(loadsend), 0); //将loadsend的所有数据由指定的 socket端口users[i].client_fd传给对方主机。
		}
	}
}
//将loadsend发送给name
void send_only(char name[50],char loadsend[BUFFER_SIZE]){
	int j;//用来定位。
	for(j=0;j<THREAD_NUMBER;j++){  //遍历结构体数组users。
		if((users[j].login==1)&&(strcmp(users[j].name,name)==0)){ //如果users[i]的登录标志位login为1,即此线程已有用户使用；且正确对比name。
			send(users[j].client_fd, loadsend, strlen(loadsend), 0); //将loadsend的所有数据由指定的 socket端口users[j].client_fd传给对方主机。	
		}
	}
}
//处理s与se_name并调用相应的函数转发。
void strdeal(char s[],char se_name[]){   
	char sign[8];
	char name[56];
	char buf[BUFFER_SIZE];
	int len;
	char send_buf[BUFFER_SIZE];
	char send_buf2[BUFFER_SIZE];
	memset(sign, 0, strlen(sign));  //将sign中当前位置后面的strlen(sign)个字节用0替换;
	memset(name, 0, strlen(name)); //将name中当前位置后面的strlen(name)个字节用0替换;
	memset(buf, 0, strlen(buf)); //将buf中当前位置后面的strlen(buf)个字节用0替换;。
	memset(send_buf, 0, strlen(send_buf)); //将send_buf中当前位置后面的strlen(send_buf)个字节用0替换;
	memset(send_buf2, 0, strlen(send_buf2));//将send_buf2中当前位置后面的strlen(send_buf)个字节用0替换;
	char nt[10]; //存储当前时间
	int i=0;
	int n=0;
	int j=0;
	for(i;i<=strlen(s);i++){
		if(n>2){
			buf[j]=s[i];
			j++;	
		}else if(n==2){
			if(len==0){
				n++;
				name[j]='\0';
				j=0;
				i--;
				continue;
			}
			name[j]=s[i];
			j++;
			len--;
		}else if(n==1){
			if(s[i]==':'){
				n++;
				name[j]='\0';
				len = atoi(name);
				j=0;
				continue;
			}
			name[j]=s[i];
			j++;
		}else{
			if(s[i]==':'){
				n++;
				sign[j]='\0';
				j=0;
				continue;
			}
			sign[j]=s[i];
			j++;
		}	
	}
	if(strcmp(sign,"All")==0){ //比较sign与"ALL"，若相同返回0；若相同。
		get_now_time(nt);  //得到当前时间并存储在nt内。
		sprintf(send_buf,"%s用户< %s >群发消息->\t\t%s:\n\t%s","User:",se_name,nt,buf);//将内容写入send_buf。
		send_all(send_buf);  //将send_buf发送给全部人。
	}else{
		get_now_time(nt); //得到当前时间并存储在nt内。
		sprintf(send_buf,"%s用户< %s >------>\t\t%s:\n\t%s","User:",se_name,nt,buf); //将内容写入send_buf。
		send_only(name,send_buf); //将send_buf发送给name。
		sprintf(send_buf2,"User: %s: say\n\t%s",nt,buf);
		send_only(se_name,send_buf2);
	}
}
 
//线程函数
void *thrd_func(void *arg)     
{
	long i = (long)arg;	
	int recvbytes;  //存储recv()返回值。
	char nt[10];  //存储当前时间。
	while(1){
		memset(users[i].buf , 0, sizeof(users[i].buf)); //将users[i].buf中当前位置后面的strlen(users[i].buf)个字节用0替换。
		if ((recvbytes = recv(users[i].client_fd, users[i].buf, BUFFER_SIZE, 0)) <= 0)    //把users[i].client_fd的接收缓冲中的数据copy到users[i].buf中，并将接收的字节数存在recvbytes；如果接收错误。
		{
			char end[100]; //用来存储发送的内容。
			memset(end, 0, 100); //将end中当前位置后面的100个字节用0替换。
			get_now_time(nt);  //得到当前时间并存储在nt内。
			sprintf(end,"%s%s%s\n用户：%s%s\n","Inform:",nt,"-通知：",users[i].name,"退出聊天室");  // 将内容写入end。
			send_all(end);			//将end发送给所有人。
			users[i].login = 0;   //将users[i]的登录标志设置为0，即未登录。
			sem_v(sem_id);        //信号量加1。
		    close(users[i].client_fd);  //关闭users[i]的socket端口。
			int n=0; //用来记录登录数。
			int j=0; //用来定位.
			for(j;j<THREAD_NUMBER;j++){ //遍历结构体数组users。
				if(users[j].login==0)   //如果users[j]的登录标志位login为0，即未登录。
					n++;	         //登录数加1。	
			}		
			printf("%s用户退出，还可以上线%d个\n",users[i].name, n);//输出内容。
			pthread_exit(0);   //结束线程
		}
		strdeal(users[i].buf,users[i].name);	 //处理users[i].buf与users[i].name并调用相应的函数转发。
	}
}
 
 
//建立一个socket连接，并将本地地址绑定到端口，且端口号与port有关。
int bindPort(unsigned short int port)
{
	int sockfd;// 用来存储套接字描述符。
    int	sendbytes;//用来记录发送的字节数。
	struct sockaddr_in my_addr; //新建结构体sockaddr_in为my_addr。	
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0))== -1)//socket()为建立使用IPV4协议的字节流套接字(TCP)； 如果建立出错。
	{
		syslog(LOG_ERR, "socket"); //将"socket"写到系统纪录中，置为错误。
		perror("socket"); //把"socket"输出到标准错误 stderr。
	}
 
	/*设置sockaddr_in 结构体中相关参数*/   
	bzero(&my_addr, sizeof(my_addr));    //置my_addr的前sizeof(my_addr)个字节为零且包括‘\0’。
	my_addr.sin_family = AF_INET;    //服务器地址族为对于TCP/IP协议的AF_INET。
	my_addr.sin_port = htons(port);    //服务器端口将小端模式改变为大端模式。
	my_addr.sin_addr.s_addr = INADDR_ANY;    //服务器地址为0.0.0.0，即任意地址。
	memset(&(my_addr.sin_zero),0,8);    //清空sin_zero的前8个字节，以保持struct sockaddr_in与struct sockaddr同样大小。
 
	/* 允许重复使用本地地址与套接字进行绑定 */
	int i = 1; //布尔型选项
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)); //对一个地址重复使用
	if (bind(sockfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)) == -1) //将本地地址my_addr与一套接口sockfd捆绑; 如果捆绑失败。
	{
		syslog(LOG_ERR, "fail to bind"); //将"fail to bind"写到系统纪录中，置为错误。
		perror("fail to bind");//把"fail to bind"输出到标准错误 stderr。
	}
 	printf("success!\n");//打印内容。
	return sockfd;
}
/*自定义处理信号sign_no*/
void my_func(int sign_no)
{	
	char loadsend[100];
	sprintf(loadsend,"%s%s","Inform:","over");   //将内容写入loadsend；	
	send_all(loadsend); //将loadsend发送给所有人；  
	printf("即将退出服务器\n"); //打印内容。
	fflush(stdout); //强制马上输出。
	sleep(1);      //休眠一秒。
	exit(0);  //正常退出。
}
int main(int argc, char *argv[])
{
	int sockfd;//用来存储socket描述符。
	int recvbytes;//用来记录接收的字节数。
	char loadsend[100];//用来存储发送的内容。。
	long i=0;  //循环来做连接
	int sendbytes;//用来记录发送的字节数。。
	int res;//用来记录建立线程时的返回值。
	struct sockaddr_in client_sockaddr;//新建结构体sockaddr_in为client_sockaddr。
	int sin_size;
	int repetition,j,n;
	sockfd = bindPort(MYPORT);//建立一个socket连接，并将本地地址绑定到端口，且端口号与port有关。
	for(i=0;i<THREAD_NUMBER;i++)//遍历结构体数组users。
		users[i].login=0;   //将users[i]的登录标志设置为0，即未登录。
	if (listen(sockfd, THREAD_NUMBER) == -1)//创建一个等待队列，在其中存放未处理的客户端连接请求；如果创建失败。
	{	
		syslog(LOG_ERR, "listen");//将"listen"写到系统纪录中，置为错误。
		perror("listen");//把"listen"输出到标准错误 stderr。
	}
	printf("Listening....\n");//打印内容。
	sem_id = semget(ftok("/", 1), 1, 0666|IPC_CREAT); //建立信号量；将信号量描述符存在sem_id。
	init_sem(sem_id, THREAD_NUMBER);  //声明信号量初值为THREAD_NUMBER。
	char nt[10]; //用来存储时间。
	openlog("daemon_syslog", LOG_PID, LOG_DAEMON);   //打开系统日志。
	signal(SIGINT, my_func);//设置信号SIGINT处理方式为myfunc。
	signal(SIGQUIT, my_func);//设置信号SIGQUIT处理方式为myfunc。
	signal(SIGTSTP,my_func);//设置信号SIGTSTP处理方式为myfunc。
	i=0;//用来定位。
	while(1){
		sem_p(sem_id); //信号量减1。
		while(i<THREAD_NUMBER){                       //遍历结构体数组users
			if(users[i].login==0)                  //如果找到未登录的users[i]。
				break;                             //结束循环体。
			i++;                                   //下一位。
			if(i==THREAD_NUMBER){                 //如果已遍历完。
				i=0;	                          //重置为初始值，重新遍历。
				}
			}
		n=0;//存储登录数。
		j=0;//用来定位。
		for(j;j<THREAD_NUMBER;j++){ //遍历结构体数组users
			if(users[j].login==1) //如果users[j]的登录标志位login为1，即已登录。
				n++;		   //已登录数加一。
		}		
		printf("已经上线%d个用户，还可以上线%d个\n",n,THREAD_NUMBER-n); //打印内容。
		if(users[i].login==0){  //确认users[i]的登录标志设置为0，即未登录。
			printf("等待第下个连接\n");	//打印内容。
			if ((users[i].client_fd = accept(sockfd,(struct sockaddr*)&client_sockaddr, &sin_size)) == -1){//等待并接收客户端的连接请求,取出第一个未处理的连接请求，创建一个新的套接字，并返回指向该套接字的文件描述符。client_fd为客户端的socket；如果创建出错。
				syslog(LOG_ERR, "accept");			//将"accept"写到系统纪录中，置为错误。	
				perror("accept");//把"accept"输出到标准错误 stderr。
			
			}
			inet_ntop(AF_INET, &client_sockaddr.sin_addr, users[i].address, sizeof(users[i].address));//将二进制地址映射为点分十进制地址。
			if ((recvbytes=recv(users[i].client_fd, users[i].name, BUFFER_SIZE, 0)) <= 0){//把users[i].client_fd的接收缓冲中的数据copy到users[i].name中，并将接收的字节数存在recvbytes；如果接收错误。
				sem_v(sem_id); //信号量加1。
				continue;		//跳出本次循环。
			};
			printf("本次连接的是用户：%s\n",users[i].name); //输出内容。
			j=0;	//用来定位，从第一个开始。
			repetition=0;//重名标志位置为0，即不重名。
			for(j=0;j<THREAD_NUMBER;j++){ //遍历结构体数组users
				if(users[j].login==1)	//如果users[j]的登录标志位login为1，即已登录。
				if(strcmp(users[i].name,users[j].name)==0){ //如果正确对比name；
					repetition=1;	//重名标志位置为1，即重名。
				}
			}
			if(repetition==1){//如果重名标志为1，即重名。
				send(users[i].client_fd, "g", strlen("g"), 0);//将"g"由指定的 socket端口users[i].client_fd传给对方主机。
				sem_v(sem_id); //信号量加1。
				continue;		//跳出本次循环。
			}else{//重名标志为0，即不重名。
				users[i].login = 1; //users[i]的登录标志位login置为1，即已登录。
				send(users[i].client_fd, "Welcome", strlen("Welcome"), 0);//将"Welcome"由指定的socket端口users[i].client_fd传给对方主机。
			}			
			//广播上线消息。
			memset(loadsend, 0, 100); //将loadsend中当前位置后面的100个字节用0替换。
			get_now_time(nt);  //得到当前时间并存储在nt内。
			sprintf(loadsend,"%s%s%s\n用户：%s%s\n","Inform:",nt,"-通知：",users[i].name,"-上线了！\n\t大家欢迎！");//将内容写入loadsend。
			send_all(loadsend);//将loadsend发送给所有人。
			res = pthread_create(&users[i].thread, NULL, thrd_func, (void*)i);  //创建线程，线程标识符存在users[i].thread。
			if (res != 0) //如果创建失败。
			{	
				syslog(LOG_ERR, "Create thread failed");//将"Create thread failed"写到系统纪录中，置为错误。	
				perror("Create thread failed");//把"Create thread failed"输出到标准错误 stderr。
				users[i].login = 0; //users[i]的登录标志位login置为0，即未登录。
				sem_v(sem_id); //信号量加1。
			}
		}
	}
	close(sockfd); //关闭storket。
	return 0;
}
