#include "chat.h"

#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAXMSG 500            /*最大消息数*/
#define MAXUSER 20            /*用户最大数量*/
#define RW 0                  /*读写进程对共享内存区的信号量 初始值：1*/
#define MUTEX 1               /*计数器信号量                 初始值：1*/
#define W 2                   /*为了写进程优先设置的信号量   初始值：1*/
#define COUNT 3               /*读进程数量                   初始值：0*/
#define FILESEM 4             /*文件访问信号量               初始值：1*/
#define MAXLINE 4096          /*传输文件单个缓冲区大小*/

union semun{                  /*信号量处理必需的共用体*/
    int val;                  /*这个不会用*/
    struct semid_ds *buf;
    unsigned short *array;
};

typedef struct _space{
    int length;               /*消息的数量，条数*/
    Message message[MAXMSG];  /*消息，最多MAXMSG条*/
    int online[MAXUSER + 1];  /*利用缓冲区存储当前在线人数，存储的为用户id号码*/
    int fwxnp[MAXUSER + 1];   /*当前状态，0 下线 1 界面 2 单聊 3 群聊 在线状态变成一个数组*/ /*能不能下线是-1，主页面0,1-20表示id，21表示群聊？*/
}Space;                       /*共享区的全部内容*/

Space *space;                 /*共享区内存*/
int client_socket;            /*客户端套接字*/
int server_socket;            /*服务器套接字*/
char client_ip[20];           /*客户端IP的字符串表示*/
int shmid;                    /*共享内存区(多人聊天)标识符(ID)*/
int semid;                    /*信号量标识符(ID)*/
int online_num = 0;           /*线上人数*/
int total_num = 0;            /*总用户人数*/
char fwxnb[20];               /*关键变量，用于标示当前fork的用户名*/
int fork_id = 0;              /*标示当前进程名*/
int verify = 0;               /*验证*/
int quit_chat;                /*退出聊天界面的flag*/

int init_socket(int port,int addr);
/*初始化套接字，传入端口和地址，自动生成一个套接字并关联地址，监听。返回套接字。若失败，返回-1*/
void do_server();
/*子进程对客户端的具体处理，使用多线程，用两个线程分别监控客户端的输入和输出到客户端*/
void read_from();
/*接收客户端发来的字符串，存入内存区，并更新内存区序号，写互斥*/
void write_to();
/*轮询内存区，若发现有新消息，则将新消息发送给客户端*/
void getunread();
/*从客户端读取ID，返回此ID之后的未读消息*/
void exitfunc(int signal);
/*退出处理函数，捕获SIGINT信号。关闭信号量和共享内存区，保存历史聊天记录*/
void waitchild(int signal);
/*子进程退出处理函数，捕获SIGCHIL信号，防止出现僵尸进程*/
int init_sem(int rw,int mutex,int w,int count,int file);
/*设置信号量集中五个信号量的初值，失败返回-1*/
int P(int type);
/*P操作，将信号量集合中对应类型的信号量的数量-1，失败返回-1*/
int V(int type);
/*V操作，将信号量集合中对应类型的信号量的数量+1，失败返回-1*/
int sem_setval(int type,int value);
/*设置信号量type的值为value，失败返回-1*/
int client_register(User user);
/*处理用户注册请求*/
int client_login(User user);
/*处理用户登录请求*/
int client_modify(User user);
/*处理用户修改密码请求*/
int load_msg_history();
/*读取历史聊天记录，储存到全局共享内存区中。若第一次开启服务器，无历史聊天记录文件，则不做处理*/
int add_friend(char A[],char B[]);
/*更新好友关系矩阵*/
int find(char A[]);
/*负责根据account查找id*/
void init_unread_fri();
/*初始化20个未读好友申请储存列表*/
int check_unread();
/*检查当前的好友未读申请*/
int update_unread();
/*保存在聊天时收到的未读消息记录，嵌入在write_to里，每次检查*/
void return_fri();
/*检查好友矩阵，2代表需要更新*/
int docu_rece();
/*接收客户端传来的文件，缓存*/

int docu_rece(char *file_name)
{
    printf("!!!\n!!!\n!!!\n");
    char  file_buff[4096];
    FILE *fp;
    int  n;
    //接受客户端传过来的文件名


    //创建待接收文件实体
    if((fp = fopen(file_name,"wb") ) == NULL )
    {
        printf("new file create fail.\n");
        return 0;
    }
    memset(file_buff,0,sizeof(file_buff));
    //把二进制文件读取到缓冲区
    while(1){
        printf("?\n");
        n = read(client_socket, file_buff, MAXLINE);
        printf("n=%d\n",n);
        fwrite(file_buff, 1, n, fp);//将缓冲区内容写进文件
        if(n<4096)
            break;
    }
    printf("???\n???\n???\n");
    fclose(fp);
    return 1;
}

void return_fri()
{
    int fd1;                                                            /*修改关系矩阵*/
    int temp1;
    int rel[MAXUSER+1][MAXUSER+1];                                      /*好友关系矩阵*/
    fd1 = open("friend.dat",O_RDWR|O_CREAT,0660);
    temp1 = read(fd1,&rel,sizeof(rel)); 
    //printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~修改后的好友矩阵\n");
    Data data;
    Packet packet;
    Kind kind;    
    strcpy(data.message.id_from,"server");
    strcpy(data.message.id_to,fwxnb);
    strcpy(data.message.str,"already success");
    for(int xxx=1;xxx<MAXUSER+1;++xxx)
    {
        if(rel[fork_id][xxx] == 2)
        {
            printf("id(%d)给id(%d)发送没有收到，需要马上发送\n",xxx,fork_id); //B返回给A
            int fd; 
            User userinfo[MAXUSER];
            fd=open("userinfo.dat",O_RDONLY,0770);


            int usernum,a;
            read(fd,&usernum,sizeof(int));
            read(fd,&a,sizeof(int));
            read(fd,userinfo,MAXUSER*sizeof(User));
            for(int i=0;i<usernum;++i)
            {
                if(xxx == userinfo[i].user_id)
                {
                    strcpy(data.message.id_from,userinfo[i].account);
                    printf("test 找到%s\n",userinfo[i].account);
                    break;
                }   
            }
            close(fd);  


            if(build_packet(&packet,enum_friend,data) == -1)
            {   
                printf("fail to build the packet!\n");
                return ;
            }
            write(client_socket,&packet,sizeof(Packet));        /*发送包*/    
            rel[fork_id][xxx] = 3; 
        }
    }
    lseek(fd1,0,SEEK_SET);   
    write(fd1,&rel,sizeof(rel));
    close(fd1);
}

int update_unread()
{
    int fd, unread = 0;
    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%d", fork_id);
    strcat(buffer,"news.dat");   
    fd = open(buffer,O_RDWR|O_CREAT,0660);
    read(fd,&unread,sizeof(int));
    Data data;
    Packet packet;
    Kind kind;
    printf("当前进程的id：%d 系统公告unread:%d\n", fork_id,unread);
    //???
    for(int i=0;i<unread;++i)
    {
        read(fd, &data, sizeof(data));
        printf("i=%d 系统公告: from:%s  to:%s str:%s\n",i, data.message.id_from,data.message.id_to,data.message.str);
        if(build_packet(&packet,enum_chat,data) == -1)
        {
            printf("fail to build the packet!\n");
            return -1;
        }
        write(client_socket,&packet,sizeof(Packet));        /*发送包*/
        // read(client_socket,&packet,sizeof(Packet));
        // parse_packet(packet,&kind,&data，&verify););
         printf("发送包 %s\n",data.message.str);
    }        
    unread = 0;
    lseek(fd,0,SEEK_SET);
    write(fd,&unread,sizeof(int));
    close(fd);  
    return unread;     
}

int check_unread_fri()
{
    int fd;
    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%d", fork_id);
    strcat(buffer,"fri.dat");
    fd = open(buffer,O_RDWR|O_CREAT,0660);
    int unread = 0;
    read(fd,&unread,sizeof(int));
    Data data;
    Packet packet;
    Kind kind;
    printf("当前进程的id：%d 好友申请unread:%d\n", fork_id,unread);
    for(int i=0;i<unread;++i)
    {
        read(fd, &data, sizeof(data));
        printf("i=%d 未读消息: from:%s  to:%s str:%s\n",i, data.message.id_from,data.message.id_to,data.message.str);
        if(build_packet(&packet,enum_friend,data) == -1)
        {
            printf("fail to build the packet!\n");
            return -1;
        }
        write(client_socket,&packet,sizeof(Packet));        /*发送包*/
        read(client_socket,&packet,sizeof(Packet));
        parse_packet(packet,&kind,&data,&verify);
        if(!strcmp(data.message.str, "accept"))
        {
            /*此时的id_from = B，id_to是A，我要发给A*/
            printf("form:%s to:%s 收到包 str:%s \n",data.message.id_from, data.message.id_to, data.message.str);
            /*修改好友矩阵*/
            int fd1;                                                            /*修改关系矩阵*/
            int temp1;
            int rel[MAXUSER+1][MAXUSER+1];                                      /*好友关系矩阵*/
            fd1 = open("friend.dat",O_RDWR|O_CREAT,0660);
            int id_a = find(data.message.id_from);
            int id_b = find(data.message.id_to);
            temp1 = read(fd1,&rel,sizeof(rel));            
            rel[id_a][id_b] = 3; 
            rel[id_b][id_a] = 2; /*只需要给id_a发也就是*/
            ++rel[id_a][0];++rel[0][id_a];
            ++rel[id_b][0];++rel[0][id_b];
            // printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~修改后的好友矩阵\n");
            // for(int zzz=0;zzz<MAXUSER+1;++zzz)
            // {
            //     for(int xxx=0;xxx<MAXUSER+1;++xxx)
            //     {
            //         printf("%d ",rel[zzz][xxx]);
            //     }
            //     printf("\n");
            // }
            // printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
            lseek(fd1,0,SEEK_SET);
            write(fd1,&rel,sizeof(rel));
            printf("%s(id:%d) 与 %s(id:%d 已经成功牵手！\n",data.message.id_from,id_a,data.message.id_to,id_b);
            close(fd1);
            
        }
        else
        {
            /*refuse的情况*/
        }
    }

    unread = 0;
    lseek(fd,0,SEEK_SET);
    write(fd,&unread,sizeof(int));
    close(fd);  
    return unread;  
}

int find(char A[])
{
    int fd;                                                             /*查找id号码*/
    User userinfo[MAXUSER];
    int a,usernum;
    fd=open("userinfo.dat",O_RDONLY,0770);
    if(fd == -1)
    {
        printf("file \"userinfo.dat\" opened failed!\n");
        return -1;
    }
    int id_a,i;
    read(fd,&usernum,sizeof(int));
    read(fd,&a,sizeof(int));
    //printf("add_friend 里面的 usernum: %d\n",usernum);
    //printf("A: %s   B:%s\n",A,B);
    read(fd,userinfo,MAXUSER*sizeof(User));
    for(i=0;i<usernum;++i)
    {
        if(!strcmp(A, userinfo[i].account))
        {
            printf("find 里面A的id: %d\n",i+1);
            id_a = i+1;
            break;
        }
    }
    return id_a;                  
}

int init_socket(int port,int addr)
{
    struct sockaddr_in server_addr;             /*服务器地址结构*/
    int server_socket;                          /*服务器套接字*/
    server_socket=socket(AF_INET,SOCK_STREAM,0);
    if(server_socket == -1)
    {
        return -1;
    }
    server_addr.sin_port=htons(port);
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=htonl(addr);    /*套接字关联地址*/
    if(bind(server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr)) != 0)
    {
        return -1;
    }
    if(listen(server_socket,5) != 0)            /*设置最大监听数并监听*/
    {
        return -1;
    }
    return server_socket;
}

void read_from()
{
    int read_byte;
    char str[141];
    int msglength;
    Packet packet;
    Kind kind;
    Data data;
    int verify;
    FILE *fp;
    time_t t;
    int i;
    char tbuf[1024];
    while(1){
        read_byte=read(client_socket,&packet,sizeof(Packet));   /*读取消息*/
        if(read_byte == -1)
        {
            printf("Client\"%s\" reads error!\n",client_ip);
            return;
        }
        else
        {
            parse_packet(packet,&kind,&data,&verify);
            //printf("!!!kind = %d\n",kind);
            if(kind == enum_chat)
            {                                                   /*写内存区*/
                P(W);                                           /*在无写进程时进入*/
                P(RW);                                          /*互斥访问共享内存区*/
                msglength=space->length%MAXMSG;                 /*更新内存区,超过500条则覆盖前面的，写的位置永远是space->length%500*/
                space->message[msglength].id=space->length;
                strcpy(space->message[msglength].str,data.message.str);
                strcpy(space->message[msglength].id_from, data.message.id_from);
                strcpy(space->message[msglength].id_to, data.message.id_to);

                /*读包的时候要更新聊天状态*/
                /*只实现了当前状态的判断，需要把发包嵌入进去，需要写一些验证*/
                int temp = find(data.message.id_to);            /*查找id*/
                printf("fork_id %d\n",fork_id);
                printf("read_from 读到的信息里要发送给人的id：%d\n", temp); 
                if(space->fwxnp[temp] == 0 || space -> fwxnp[temp] == fork_id)
                {
                    space->fwxnp[temp] = fork_id;
                    printf("fork_id = %d, space -> fwxnp[fork_id] = %d\n",fork_id,space -> fwxnp[fork_id]);
                    if(space -> fwxnp[fork_id]!= 0 && space -> fwxnp[fork_id]!=temp)    /*B和A聊天*/
                    {
                        /*当A和B聊天后A就不能给除了B以外的人发消息了，理论上来说这个判断是不会被执行的*/
                        //V(RW);                                          /*释放共享内存区信号量*/
                        //V(W);                                           /*可以让下个写进程进入*/
                        printf("你已经和别人（id：%d)聊天了，请专一一些！\n",space -> fwxnp[fork_id]);
                        //return ;
                    }
                    else
                    {
                        printf("成功，to用户%s (id:%d)已和from用户%s (id:%d)聊天\n",data.message.id_to,temp,data.message.id_from,fork_id);
                        space -> fwxnp[fork_id] = temp;                  /*唯一标示，二者“握手”*/
                    }
                }
                else
                {
                    /*A和B聊天的时候，二者可以收到消息*/
                    printf("用户%s (id:%d)已和别人(%d)聊天，换个人试试吧\n",data.message.id_to,temp, space->fwxnp[temp]);

                }



                time(&t);
                ctime_r(&t,tbuf);
                space->length++;
                printf("Client\"%s\" has writed the message %s to \"%s\".\n",data.message.id_from,space->message[msglength].str,data.message.id_to);

                if((fp=fopen("hist.txt","a"))==NULL)
                {
                    printf("hist_txt_error\n");
                }
                else                                            /*将历史记录写入hist.txt*/
                {
                    fputs(data.message.id_from,fp);
                    fprintf(fp,"--->");
                    fputs(data.message.id_to,fp);
                    fprintf(fp,"    ");
                    fputs(tbuf,fp);
                    fputs(space->message[msglength].str,fp);
                    fprintf(fp,"\n\n");
                    fclose(fp);
                }
                V(RW);                                          /*释放共享内存区信号量*/
                V(W);                                           /*可以让下个写进程进入*/
            }
            else if(kind == enum_quitchat)
            {
                space->fwxnp[fork_id] = 0;
                quit_chat=1;
                printf("quit_read_from\n");
                return;
            }
            else if(kind == enum_docu)
            {
                printf("store docu...\n");
                printf("file_name=%s\n",data.message.str);
                docu_rece(data.message.str);
            }
            else
            {
                printf("the type of the packet reveived is error!\n");
                return;
            }
        }
    }
}

void write_to()
{
    int msglength;
    msglength=space->length;
    int count;
    Packet packet;
    int temp;
    while(1){
        if(quit_chat==1){
            printf("quit_write_to\n");
            return;
        }
        temp = update_unread();
        if(temp>0)
        {
            printf("writ_to里面 当前id：%d account:%s, undate_unread:%d\n", fork_id, fwxnb, temp);        
        }

        if(msglength < space->length){                      /*有新消息，互斥访问共享内存*/
            P(W);                                           /*无写进程等待进入时*/
            P(MUTEX);                                       /*对计数器加锁*/
            if((count=semctl(semid,COUNT,GETVAL)) == 0)
            {
                P(RW);
            }
            if(sem_setval(COUNT,count+1) == -1)     /*如果不是第一个进入，则表示已经对共享内存加锁了*/
            {
                printf("174 semaphore set value failed!\n");
            }
            V(MUTEX);                                       /*对计数器访问完毕，释放计算器信号量*/
            V(W);                                           /*释放W信号量，写进程可以进了*/
            for(;msglength<space->length;msglength++)       /*读取新消息*/
            {
                //printf("test: %s\n", fwxnb);
                //printf("id_to:%s\n",space->message[msglength%MAXMSG].id_to);
                if(!strcmp(space->message[msglength%MAXMSG].id_to,fwxnb))
                {
                    if(build_packet(&packet,enum_chat,space->message[msglength%MAXMSG]) == -1)
                    {
                        printf("180 fail to build the packet!\n");
                        return;
                    }
                    printf("消息发送人： %s 发送给：%s 消息：%s\n",space->message[msglength%MAXMSG].id_from,space->message[msglength%MAXMSG].id_to,space->message[msglength%MAXMSG].str);
                    write(client_socket,&packet,sizeof(Packet));
                }
            }
            P(MUTEX);                                       /*对计数器加锁*/
            count=semctl(semid,COUNT,GETVAL);               /*读进程访问完毕，计数器减1*/
            if(sem_setval(COUNT,count-1) == -1)
            {
                printf("189 semaphore set value failed!\n");
            }
            if(semctl(semid,COUNT,GETVAL) == 0)             /*如果是最后一个读进程，则要将共享内存区的锁解开，方便写进程进入*/
            {
                V(RW);
            }
            V(MUTEX);                                       /*计数器访问完毕，释放信号量*/
        }
        sleep(1);                                 /*每秒轮询一次*/
    }
}

void getunread()
{
    char lastid[10];
    int fromid;
    int temp;
    Packet packet;
    read(client_socket,lastid,10);                 /*客户端连接成功后发送一个ID，返回此ID以后的聊天记录*/
    for(fromid=atoi(lastid)+1,temp=fromid;fromid<space->length;fromid++)
    {
        if(build_packet(&packet,enum_chat,space->message[fromid%MAXMSG]) == -1)
        {
            printf("206 fail to build the packet!\n");
            return;
        }
        write(client_socket,&packet,sizeof(packet));
    }
    if(temp < fromid)
    {
        printf("Client\"%s\" has obtained the unread message between %d to %d.\n",client_ip,temp,fromid-1);
    }
    int fd;
    int num;
    int usernum;
    User userinfo[MAXUSER];
    fd=open("userinfo.dat",O_RDONLY,0770);           /*更新在线状态*/
    if(fd == -1)
    {
        printf("file \"userinfo.dat\" opened failed!\n");
        return ;
    }
    read(fd,&usernum,sizeof(int));                          /*读人数*/
    read(fd,userinfo,MAXUSER*sizeof(User));        /*按照人数读信息*/
}

int client_register(User user)
{
    int fd;
    int usernum;
    User userinfo[MAXUSER];
    int i;
    int j,z;
    Packet packet;
    fd=open("userinfo.dat",O_RDWR|O_CREAT,0660);     /*O_RDWR 读写打开 O_creat 若文件不存在则创建, 0660 表示权限 0代表八进制 当前用户、group和其他用户*/
    if(fd == -1)
    {
        printf("file \"userinfo.dat\" opened failed!\n");
        return -1;
    }
    P(FILESEM);
    i=read(fd,&usernum,sizeof(int));                                /*文件开头是用户数量，接下来是若干个用户的帐号密码信息*/
    z = read(fd,&j,sizeof(int));
    if(i == 0)
    {                                                               /*如果读取失败，则表示该文件第一次打开，没有信息*/
        usernum=1;
        write(fd,&usernum,sizeof(int));                             /*写入1，表示用户数量数为1*/
        j=0;
        printf(" j = %d\n",j);
        write(fd,&j,sizeof(int));
        user.user_id = usernum;                                     /*写入绝对的id号码*/
        write(fd,&user,sizeof(User));                               /*将用户结构体直接写入文件*/
        int fd1;                                                    /*初始化好友关系矩阵，加一个人初始化一行一列*/
        int rel[MAXUSER+1][MAXUSER+1];                              /*好友关系矩阵*/
        memset(rel,0,sizeof(rel));
        fd1 = open("friend.dat",O_RDWR|O_CREAT,0660);
        write(fd1,&rel,sizeof(rel));
        printf("好友矩阵初始化\n");
        if(build_packet(&packet,enum_regist,user) == -1)
        {
            printf("fail to build the packet!\n");
            return -1;
        }
        write(client_socket,&packet,sizeof(Packet));                /*给客户端发送包回应，表示注册成功*/
        printf("Client\"%s\" regists succeed with the account \"%s\".\n",client_ip,user.account);
    }
    else
    {
        read(fd,userinfo,MAXUSER*sizeof(User));
        for(i=0;i<usernum;i++)
        {
            if(!strcmp(userinfo[i].account,user.account))           /*在用户列表中找到该用户，说明已注册*/
            {
                strcpy(user.account,"");                      /*用户名写空*/
                if(build_packet(&packet,enum_regist,user) == -1)
                {
                    printf("fail to build the packet!\n");
                    return -1;
                }
                write(client_socket,&packet,sizeof(Packet));        /*给客户端发送包回应，表示注册失败*/
                printf("Client\"%s\" regists failed with the repeting account.\n",client_ip);
                close(fd);
                V(FILESEM);
                return -1;
            }
        }
        usernum++;                                                  /*跳出循环，表示可以注册该用户*/
        strcpy(userinfo[i].account,user.account);                   /*将帐号密码写入用户数组*/
        strcpy(userinfo[i].password,user.password);
        user.user_id = usernum;                                     /*写入绝对的id号码*/
        userinfo[i].user_id = usernum;
        lseek(fd,0,SEEK_SET);
        write(fd,&usernum,sizeof(int));
        read(fd,&j,sizeof(int));                                    /*将用户数组和长度写入文件*/
        write(fd,userinfo,sizeof(User)*MAXUSER);
        if(build_packet(&packet,enum_regist,user) == -1)
        {
            printf("fail to build the packet!\n");
            return -1;
        }
        write(client_socket,&packet,sizeof(Packet));
        printf("Client\"%s\" regists succeed with the account \"%s\".\n",client_ip,user.account);
    }
    close(fd);
    V(FILESEM);
    return 0;
}

int client_modify(User user)
{
    int fd;
    int usernum;
    User userinfo[MAXUSER];
    int i, j, z;
    Packet packet;
    fd=open("userinfo.dat",O_RDWR|O_CREAT,0660);
    if(fd == -1){
        printf("file \"userinfo.dat\" opened failed!\n");
        return -1;
    }
    P(FILESEM);
    i = read(fd,&usernum,sizeof(int));
    z = read(fd,&j,sizeof(int));
    if(i == 0)
    {                                                           /*文件第一次打开，没有用户注册，无法修改密码*/
        strcpy(user.account,"");
        if(build_packet(&packet,enum_modify,user) == -1)
        {
            printf("fail to build the packet!\n");
            return -1;
        }
        write(client_socket,&packet,sizeof(Packet));            /*发送包给客户端，表示无法修改密码*/
        printf("Client\"%s\" modifies failed with no account.\n",client_ip);
    }
    else
    {
        read(fd,userinfo,MAXUSER*sizeof(User));
        for(i=0;i<usernum;i++)
        {
            if(!strcmp(userinfo[i].account,user.account) && !strcmp(userinfo[i].password,user.password))
            {
                if(build_packet(&packet,enum_modify,user) == -1)
                {
                    printf("fail to build the packet!\n");
                    return -1;
                }
                write(client_socket,&packet,sizeof(Packet));    /*发送包给客户端，表示允许修改密码*/
                read(client_socket,&packet,sizeof(Packet));     /*等待客户端发送新的密码*/
                strcpy(userinfo[i].password,packet.data.userinfo.password);
                lseek(fd,0,SEEK_SET);
                write(fd,&usernum,sizeof(int));
                write(fd,&j,sizeof(int));
                write(fd,userinfo,sizeof(User)*MAXUSER);
                if(build_packet(&packet,enum_modify,user) == -1)
                {
                    printf("315 fail to build the packet!\n");
                    return -1;
                }
                write(client_socket,&packet,sizeof(Packet));
                printf("Client\"%s\" modifies succeed with the account \"%s\".\n",client_ip,user.account);
                close(fd);
                V(FILESEM);
                return 0;
            }
        }
        strcpy(user.account,"");                          /*找不到帐号和密码匹配的用户，修改密码失败*/
        if(build_packet(&packet,enum_modify,user) == -1)
        {
            printf("fail to build the packet!\n");
            return -1;
        }
        write(client_socket,&packet,sizeof(Packet));
        printf("Client\"%s\" modifies failed with no account.\n",client_ip);
    }
    close(fd);
    V(FILESEM);
    return -1;
}

int client_login(User user)
{
    int fd;
    int usernum;
    User userinfo[MAXUSER];
    int i, j, z;
    Packet packet;
    fd=open("userinfo.dat",O_RDWR|O_CREAT,0660);
    if(fd == -1)
    {
        printf("file \"userinfo.dat\" opened failed!\n");
        return -1;
    }
    P(FILESEM);
    i=read(fd,&usernum,sizeof(int));
    z = read(fd,&j,sizeof(int));
    if(i == 0)
    {                                                           /*文件第一次打开，没有用户注册，无法登录*/
        strcpy(user.account,"");
        if(build_packet(&packet,enum_login,user) == -1)
        {
            printf("fail to build the packet!\n");
            return -1;
        }
        write(client_socket,&packet,sizeof(Packet));            /*发送包给客户端，表示登录失败*/
        printf("Client\"%s\" logins failed with no account.\n",client_ip);
    }
    else
    {
        read(fd,userinfo,MAXUSER*sizeof(User));
        for(i=0;i<usernum;i++)
        {
            if(!strcmp(userinfo[i].account,user.account) && !strcmp(userinfo[i].password,user.password))
            {
                if(build_packet(&packet,enum_login,user) == -1)
                {
                    printf("fail to build the packet!\n");
                    return -1;
                }
                write(client_socket,&packet,sizeof(Packet));    /*发送包给客户端，表示登录成功*/
                printf("Client\"%s\" logins succeed with the account \"%s\".\n",client_ip,user.account);
                lseek(fd,0,SEEK_SET);                  /*文件指针回到文件头*/
                j += 1;
                printf("登入 当前在线人数： %d\n",j);
                z = read(fd,&usernum,sizeof(int));
                z = write(fd,&j,sizeof(int));
                strcpy(fwxnb, user.account);                    /*将用户名存入全局变量*/
                int id = userinfo[i].user_id;
                fork_id = id;                                   /**/
                printf("在线test: id:%d  %s",id,userinfo[i].account);
                //向客户端更新在线列表：先发送在线总人数，然后每人一个包循环发送
                space->online[id] = 1;                          /*在登录时维护在线id*/
                space->fwxnp[id] = 0;                           /*该用户状态从0 表示在主界面,也有初始化的意思*/
                printf("     %s:%d\n",fwxnb, space->fwxnp[id]);
                close(fd);
                V(FILESEM);
                return 0;
            }
        }
        strcpy(user.account,"");                          /*找不到帐号和密码匹配的用户，登录失败*/
        if(build_packet(&packet,enum_login,user) == -1)
        {
            printf("fail to build the packet!\n");
            return -1;
        }
        write(client_socket,&packet,sizeof(Packet));
        printf("Client\"%s\" logins failed with no account.\n",client_ip);
    }
    close(fd);
    V(FILESEM);
    return -1;
}

void do_server()
{
    pthread_t thIDr,thIDw;
    Packet packet;
    Kind kind;
    Data data;
    int verify;
    signal(SIGINT,SIG_DFL);                                             /*设置子进程Ctrl+C为系统默认处理*/
    read(client_socket,&packet,sizeof(Packet));                         /*读包*/
    parse_packet(packet,&kind,&data,&verify);
    int fd;
    int usernum;
    User userinfo[MAXUSER];
    int i;
    printf("kind = %d %d\n",kind,verify);
    printf("test: id%s\n",data.userinfo.account);
    switch(kind)
    {
        case enum_regist:                                               /*处理注册*/
            client_register(data.userinfo);
            return;
        case enum_modify:                                               /*处理修改密码*/
            client_modify(data.userinfo);
            return;
        case enum_login:                                                /*处理登录*/
            if(client_login(data.userinfo) == -1)
                return;
            else
                break;
        default:
            printf("the type of the packet reveived is error!\n");
            return;
    }
    k:;
    space->fwxnp[fork_id] = 0;
    //printf("second choice tag   %s:%d\n",fwxnb, space->fwxnp[fork_id]);                         /*在线状态从->1*/
    int cnt_unread_fri = check_unread_fri();
    if(cnt_unread_fri > 0)
    {
        printf("当前id：%d account:%s, cnt_unread_fri:%d\n", fork_id, fwxnb, cnt_unread_fri);
    }
    return_fri();

    strcpy(data.message.id_to,"");
    read(client_socket,&packet,sizeof(Packet));                         /*读包*/
    parse_packet(packet,&kind,&data,&verify);
    //printf("kinaad = %d\n",kind);
    switch(kind)
    {
        case enum_chat:
            /*更新聊天状态*/
            quit_chat=0;
            printf("进入chatroom  和谁聊天 %s\n",data.message.id_to);
            int temp = find(data.message.id_to);
            space->fwxnp[fork_id] = temp;
            space->fwxnp[temp] = fork_id;

            //printf("after second choice tag    %s:%d\n",fwxnb, space->fwxnp[fork_id]);                 /*在线状态从1->2*/
            pthread_create(&thIDr, NULL,(void *)read_from,NULL);
            pthread_create(&thIDw, NULL,(void *)write_to,NULL);
            pthread_join(thIDr,NULL);
            goto k;
        case enum_friend:
            //printf("friend!\n");
            if(!strcmp(data.message.id_to,"fish_in_the_pool"))
            {
                printf("heart beat\n");
                goto k;
            }/*处理心跳包*/
            printf("%s:%s\n",data.message.id_from,data.message.id_to);
            int flag=0;
            int temp1;/*收件人id号码*/
            if(strcmp(data.message.id_to,""))                     /*判断合不合法*/
            {
                flag = add_friend(data.message.id_from, data.message.id_to);
                temp1 = find(data.message.id_to);
                printf("%s的状态：%d\n",data.message.id_to,space->fwxnp[temp1]);
            }
            if(flag == 0)
            {
                printf("not found...\n");
                strcpy(data.message.str,"1");
            }
            else if(flag == 2)
            {
                printf("已经添加好友了！\n");
                strcpy(data.message.str,"2");
            }
            else if(flag == 1)
            {
                int fd;
                char buffer[10];
                snprintf(buffer, sizeof(buffer), "%d", temp1);
                strcat(buffer,"fri.dat");
                fd = open(buffer,O_RDWR|O_CREAT,0660);
                int unread = 0;
                read(fd,&unread,sizeof(int));
                printf("之前的未读消息数量：%d\n",unread);
                unread++;
                lseek(fd,0,SEEK_SET);
                write(fd,&unread,sizeof(unread));
                Data data_fri;
                strcpy(data_fri.message.id_to,data.message.id_to);
                strcpy(data_fri.message.id_from, data.message.id_from);
                strcpy(data_fri.message.str, "add_friend");
                write(fd,&data_fri,sizeof(data_fri));
                close(fd);
            }

            if(build_packet(&packet,enum_friend,data) == -1)
            {
                printf("fail to build the packet!\n");
            }
            write(client_socket,&packet,sizeof(Packet));                /*给客户端发送包回应*/
            printf("search done!\n");
            goto k;
        case enum_logout:
            printf("Client\"%s\" signs out!\n",client_ip);
            int fd;
            int usernum;
            int i,j,z;
            fd=open("userinfo.dat",O_RDWR|O_CREAT,0660);
            if(fd == -1)
            {
                printf("file \"userinfo.dat\" opened failed!\n");
                return -1;
            }
            i = read(fd,&usernum,sizeof(int));
            z = read(fd,&j,sizeof(int));
            j -= 1;
            lseek(fd,0,SEEK_SET);                              /*文件指针回到文件头*/
            printf("登出 当前在线人数： %d\n",j);
            z = read(fd,&usernum,sizeof(int));
            z = write(fd,&j,sizeof(int));
            space->fwxnp[fork_id] = 0;
            printf("   %s:%d\n",fwxnb, space->fwxnp[fork_id]);                 /*在线状态从->0*/
            return;
        default:
            printf("the type of the packet reveived is error!\n");
            return;
    }
}

int add_friend(char A[], char B[])
{
    int fd;                                                             /*查找id号码*/
    User userinfo[MAXUSER];
    int a,usernum,j;
    fd=open("userinfo.dat",O_RDONLY,0770);
    if(fd == -1)
    {
        printf("file \"userinfo.dat\" opened failed!\n");
        return -1;
    }
    int id_a, id_b, i;
    read(fd,&usernum,sizeof(int));
    read(fd,&a,sizeof(int));
    //printf("add_friend 里面的 usernum: %d\n",usernum);
    printf("A: %s   B:%s\n",A,B);
    read(fd,userinfo,MAXUSER*sizeof(User));
    for(i=0;i<usernum;++i)
    {
        if(!strcmp(A, userinfo[i].account))
        {
            printf("A的id: %d  ",i+1);
            id_a = i+1;
            break;
        }
    }                                                                   /*查找A*/
    int flag=0;
    for(i=0;i<usernum;++i)
    {
        if(!strcmp(B, userinfo[i].account))
        {
            printf("B的id: %d\n",i + 1);
            id_b = i + 1;
            flag = 1;
            break;
        }
    }                                                                   /*查找B*/
    if(flag == 0)
    {
        printf("很抱歉，没有找到好友%s,只找到了个锤子\n",B);
        return 0;
    }

    if(space->fwxnp[id_b]!= 0)                                          /*id_b和别人聊天*/
    {
        printf("id_b(%d)的状态:%d\n",id_b, space->fwxnp[id_b]);
        /*要给id_b发一个包，保存在spcae里,这样不对，会导致id_a端一直收到脏包*/
        /*要看P V！*/
        /*写入到文件中,看函数名*/
        int fd;
        char buffer[10];
        snprintf(buffer, sizeof(buffer), "%d", id_b);
        strcat(buffer,"news.dat");
        fd = open(buffer,O_RDWR|O_CREAT,0660);
        int unread = 0;
        read(fd,&unread,sizeof(int));
        printf("之前的未读消息(系统公告)数量：%d\n",unread);
        unread++;
        lseek(fd,0,SEEK_SET);
        write(fd,&unread,sizeof(unread));
        Data data_news;
        strcpy(data_news.message.id_to, B);
        strcpy(data_news.message.id_from, A);
        strcpy(data_news.message.str, "you_have_a_new_message!");
        write(fd,&data_news,sizeof(data_news));
        close(fd);        
    }


    int fd1;                                                            /*修改关系矩阵*/
    int temp1;
    int rel[MAXUSER+1][MAXUSER+1];                                      /*好友关系矩阵*/
    fd1 = open("friend.dat",O_RDWR|O_CREAT,0660);
    temp1 = read(fd1,&rel,sizeof(rel));
    close(fd);
    /*在能收到好友申请的包之后，还需要继续判断*/
    if(rel[id_a][id_b] + rel[id_b][id_a]<2)
    {
        rel[id_a][id_b]=1;  //rel[id_b][id_a] = 1;
        //++rel[id_a][0];++rel[0][id_a];
        //++rel[id_b][0];++rel[0][id_b];
        printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~修改后的好友矩阵\n");
        for(int zzz=0;zzz<MAXUSER+1;++zzz)
        {
            for(int xxx=0;xxx<MAXUSER+1;++xxx)
            {
                printf("%d ",rel[zzz][xxx]);
            }
            printf("\n");
        }
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
        lseek(fd1,0,SEEK_SET);
        write(fd1,&rel,sizeof(rel));
        printf("%s(id:%d)向%s(id:%d)成功发送好友请求\n",A,id_a,B,id_b);
        close(fd1);
        return 1;
    }
    else
    {
        close(fd1);
        printf("已经是好友（return 2）\n");
        return 2;
    }
}

void exitfunc(int signal)
{
    int fd;
    if(shmctl(shmid,IPC_RMID,0) == -1)                    /*关闭共享内存区*/
    {
        printf("shared memory closed error!\n");
    }
    if(semctl(semid,0,IPC_RMID,0) == -1)               /*关闭信号量*/
    {
        printf("semaphore closed error!\n");
    }
    fd=open("histmsg.dat",O_WRONLY|O_CREAT,0660);/*打开并保存历史聊天记录*/
    if(fd == -1)
    {
        printf("file \"histmsg.dat\" opened failed!\n");
    }
    else
    {
        int write_byte;
        write_byte=write(fd,space,sizeof(Space));               /*将聊天记录存入文件*/
        if(write_byte != sizeof(Space))
        {
            printf("the length written is incorrect!\n");
        }
        else
        {
            printf("\nHistory message has stored successfully!\nServer exit!\n");
        }
    }
    if(close(server_socket) == -1)                              /*关闭服务器套接字*/
    {
        printf("server_socket closed error!\n");
    }
    _exit(0);
}

void waitchild(int signal)
{
    wait(NULL);
}

int init_sem(int rw,int mutex,int w,int count,int file)
{
    union semun arg;
    int flag;
    arg.array=(unsigned short*)malloc(sizeof(unsigned short)*5);
    arg.array[RW]=rw;                                           /*初值为1*/
    arg.array[MUTEX]=mutex;                                     /*初值为1*/
    arg.array[W]=w;                                             /*初值为1*/
    arg.array[COUNT]=count;                                     /*初值为0*/
    arg.array[FILESEM]=file;                                    /*初值为1*/
    flag=semctl(semid,0,SETALL,arg);                   /*给5个信号量赋初值*/
    free(arg.array);
    return flag;
}

int P(int type)
{
    struct sembuf buf;
    buf.sem_num=type;
    buf.sem_op=-1;
    buf.sem_flg=SEM_UNDO;
    return semop(semid,&buf,1);
}

int V(int type)
{
    struct sembuf buf;
    buf.sem_num=type;
    buf.sem_op=1;
    buf.sem_flg=SEM_UNDO;
    return semop(semid,&buf,1);
}

int sem_setval(int type,int value)
{
    union semun arg;
    arg.val=value;
    return semctl(semid,type,SETVAL,arg);
}

int load_msg_history()
{
    int fd;
    int read_byte;
    fd=open("histmsg.dat",O_RDONLY);
    if(fd == -1)
    {
        return -2;                          /*文件不存在*/
    }
    read_byte=read(fd,space,sizeof(Space));
    close(fd);
    if(read_byte == sizeof(Space))
    {
        return 0;                           /*读取成功*/
    }
    else
    {
        return -1;                          /*文件存在但读取过程出错*/
    }
}

void init_unread()
{
    int fd,fd2,fd3;
    for(int i = 1;i<MAXUSER+1;++i)
    {
        char buffer [10],buf[10],bufff[10];
        snprintf(buffer, sizeof(buffer), "%d", i); 
        strcat(buffer,"fri.dat");
        fd = open(buffer,O_RDWR|O_CREAT,0660);
        int unread = 0, unread2 = 0, unread3 = 0;
        write(fd,&unread,sizeof(unread));        
        close(fd);
        snprintf(buf, sizeof(buf), "%d", i); 
        strcat(buf,"news.dat");  
        fd2 = open(buf,O_RDWR|O_CREAT,0660);
        write(fd2,&unread2,sizeof(unread2));
        close(fd2);   
        snprintf(bufff, sizeof(bufff), "%d", i); 
        strcat(bufff,"mes.dat");  
        fd3 = open(bufff,O_RDWR|O_CREAT,0660);
        write(fd3,&unread3,sizeof(unread3));
        close(fd3);             
    }
    printf("所有dat文件初始化已完成\n");
    return;
}


int main()
{
    struct in_addr client_addr;
    struct sockaddr_in test1;
    int len1;
    int len;
    char *addr;
    signal(SIGINT,exitfunc);                                                    /*设置函数捕获并处理Ctrl+C按下时的信号*/
    signal(SIGCHLD,waitchild);                                                  /*子进程退出后wait它防止出现僵尸进程*/
    server_socket=init_socket(MYPORT,INADDR_ANY);
    shmid=shmget(IPC_PRIVATE,sizeof(Space),IPC_CREAT|0660);            /*创建一个共享内存区*/
    if(shmid == -1)
    {
        printf("shared memeoy created failed.\n");
        return -1;
    }
    space=(Space*)shmat(shmid,NULL,0);
    if((int)space == -1)
    {
        printf("shared memeoy matched failed.\n");
        return -1;
    }
    semid=semget(IPC_PRIVATE,5,IPC_CREAT|0660);                /*创建一个有5个信号量的信号量集*/
    if(semid == -1)
    {
        printf("semaphore created failed!\n");
        return -1;
    }
    if(init_sem(1,1,1,0,1) == -1)                /*将5个信号量初始化值为1 1 1 0 1*/
    {
        printf("semaphore initilize failed!\n");
        return -1;
    }
    len=load_msg_history();                                                     /*读取历史聊天记录*/
    if(len == 0)
    {
        printf("File \"histmsg.dat\" opened succeed!\n");
        printf("Server has loaded the data from the file!\n");
    }
    else if(len == -1)
    {
        printf("File \"histmsg.dat\" opened failed!\n");
        return -1;
    }
    else
    {
        printf("File \"histmsg.dat\" is not exist.\n");
    }
    printf("Wating for connecting......\n");
    init_unread();
    while(1)
    {
        len1 = sizeof(test1);
        //printf("len1 = %d\n",len1);
        client_socket = accept(server_socket,(struct sockaddr*)&test1,&len1);   /*接收连接请求*/
        // printf("after_len1 = %d\n",len1);
        // printf("test1: %s\n",inet_ntoa(test1.sin_addr));
        // printf("test1端口: %d\n",ntohs(test1.sin_port));
        // printf("client %d\n",&client_socket);
        if(client_socket != -1)
        {
            if(fork() == 0)                                                     /*子进程进行具体处理*/
            {
                
                len=sizeof(client_addr);
                getpeername(client_socket,(struct sockaddr*)&client_addr,&len);
                strcpy(client_ip,inet_ntoa(client_addr));
                printf("Connect succeed!\n");
                printf("Client ip:%s\n",client_ip);
                do_server();
            }
            else
            {                                                                   /*父进程关闭客户端套接字，继续监听*/
                close(client_socket);
                strcpy(client_ip,"");
            }
        }
        else printf("connect failed!\n");
    }
}
