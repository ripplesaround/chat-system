#include <gtk/gtk.h>	// 头文件
#include <string.h>
#include <stdio.h>
#include <netinet/in.h> //定义数据结构sockaddr_in
#include <sys/socket.h> //定义socket函数以及数据结构
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <netdb.h>
#include<sys/sem.h>
#include <unistd.h>
#include <pthread.h> 
//#include <uuid/uuid.h>
GtkWidget *window;   //登录窗口
GtkWidget *home;	//主窗口
int clientfd,b_file;
struct sockaddr_in clientaddr; 
char user_name[50];
char fname[]="/var/tmp/";
int sem_id; 
 
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
		perror("Initialize semaphore");    //把"Initialize semaphore"输出到标准错误stderr。
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
		perror("Delete semaphore");    //把"Delete semaphore"输出到标准错误stderr。
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
		perror("P operation");    //把"P operation"输出到标准错误stderr。
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
		perror("V operation");    //把"V operation"输出到标准错误stderr。
		return -1;
	}
	return 0;
}
 
 
//处理登录
void deal_pressed(GtkWidget *button, gpointer entry){
	int sendbytes;
	char *buff;
	struct hostent *host;
	char wel[]="Welcome";
	host = gethostbyname("127.0.0.1");   //本地地址，可改为服务器端口地址。
	buff = (char *)malloc(9);    //向系统申请分配指定9个字节的内存空间。
	
	const gchar  *text = gtk_entry_get_text(GTK_ENTRY(entry));
	if(strlen(text)==0){
		printf("不能为空\n");         // 提示 不能为空
	}
	else{	
		if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{
			perror("fail to create socket");    //把"fail to create socket"输出到标准错误stderr。
			exit(1);
		}
		bzero(&clientaddr, sizeof(clientaddr));    //置clientaddr的前sizeof(clientaddr)个字节为零且包括‘\0’。
		clientaddr.sin_family = AF_INET;    //服务器地址族为对于TCP/IP协议的AF_INET。
		clientaddr.sin_port = htons((uint16_t)atoi("8787"));    //服务器端口将小端模式改变为大端模式。
		clientaddr.sin_addr = *((struct in_addr *)host->h_addr);    //服务器地址为0.0.0.0，即任意地址。
	
		if (connect(clientfd, (struct sockaddr *)&clientaddr, sizeof(struct sockaddr)) == -1)
		{
			perror("fail to connect");    //把"fail to connect"输出到标准错误stderr。
			exit(1);
		}
		if ((sendbytes = send(clientfd, text, strlen(text), 0)) == -1)
		    {
			perror("fail to send");    //把"fail to send"输出到标准错误stderr。
			exit(1);
		    }
 
		if (recv(clientfd, buff, 7, 0) == -1)
		{
			perror("fail to recv");    //把"fail to recv"输出到标准错误stderr。
			exit(1);
		}
		if(strcmp(buff,wel)==0){
			strcpy(user_name,text);
			gtk_widget_destroy(window);		
		}else{
			//  弹窗 提醒 提示 昵称重复
			GtkWidget *dialog;
			dialog = gtk_message_dialog_new((gpointer)window,
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_ERROR,
			GTK_BUTTONS_OK,
			"该用户已登陆，请勿重复登陆，拒绝登录！");
			gtk_window_set_title(GTK_WINDOW(dialog), "拒绝");
			gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);	
			close(clientfd);
		}
	}
}
//登录界面
void login(int argc,char *argv[]){
	// 初始化
	gtk_init(&argc, &argv);		
	// 创建顶层窗口
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	// 设置窗口的标题
	gtk_window_set_title(GTK_WINDOW(window), "登录");
	// 设置窗口在显示器中的位置为居中
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	// 设置窗口的最小大小
	gtk_widget_set_size_request(window, 300, 200);
	// 固定窗口的大小
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE); 
	// "destroy" 和 gtk_main_quit 连接
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	//创建一个固定容器
 	GtkWidget *fixed = gtk_fixed_new(); 	
	// 将布局容器放窗口中
	gtk_container_add(GTK_CONTAINER (window), fixed);
	// 创建标签
	GtkWidget *label_one = gtk_label_new("请输入昵称");
	// 将按钮放在布局容器里
	gtk_fixed_put(GTK_FIXED(fixed), label_one,120,30);
	// 行编辑的创建
	GtkWidget *entry = gtk_entry_new();
	//设置最大长度
	gtk_entry_set_max_length(GTK_ENTRY(entry),50);
	// 设置行编辑允许编辑
	gtk_editable_set_editable(GTK_EDITABLE(entry), TRUE);
 
	gtk_fixed_put(GTK_FIXED(fixed), entry,70,60); 
	// 创建按钮
	GtkWidget *button = gtk_button_new_with_label("  登录  "); 
    
	gtk_fixed_put(GTK_FIXED(fixed), button,130,110);
	//绑定点击事件
	g_signal_connect(button, "pressed", G_CALLBACK(deal_pressed), entry);  
	// 显示窗口全部控件
	gtk_widget_show_all(window);
	//启动主循环
 	gtk_main();	
}
//发送目标用户窗口。
GtkWidget *entryname;
// 文本框缓冲区。
GtkTextBuffer *bufferuser;
GtkTextBuffer *buffernotice;
//保存信息记录buf。
void savelinetxt(char buf[]){
	struct flock lock3;    //新建结构体flock为lock3。
	lock3.l_whence = SEEK_SET;    //相对位移量的起点设置为文件头0。
	lock3.l_start = 0;    //相对位移量。
	lock3.l_len = 0;    //设置加锁区域的长度。
	//以上三个设置可以为整个文件加锁。
	lock3.l_type = F_WRLCK;	//初始化l_type为写入锁。
	lock3.l_pid = -2;	//初始化l_pid。
	b_file = open(fname,O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);    //打开fname，并将文件描述符存在b_file。
	fcntl(b_file, F_SETLKW, &lock3);    //以阻塞的方式为b_file设置结构为lock3的文件锁。
	write(b_file, buf, strlen(buf));    //向b_file写入buf。
	fcntl(b_file, F_UNLCK, &lock3);    //给b_file解锁。
	close(b_file);    //关闭文件描述符b_file。
	
}
//根据button的值发送消息时的自我维护。
void sendtouser(GtkButton  *button, gpointer entry){
	char *buf;	//用来存储内容。
	buf = (char *)malloc(1024);    //向系统申请分配指定1024个字节的内存空间。
	memset(buf, 0, 1024);    //将buf中当前位置后面的1024个字节用0替换。
	int sendbytes;	//用来记录发送的字节数。
	const gchar  *text = gtk_entry_get_text(GTK_ENTRY(entry));	//获得行编辑entry的内容并静态建立text指针进行指定。
	const char *but = gtk_button_get_label(button);	//获得获取按钮button文本内容并静态建立but指针进行指定。
	if(strlen(text)==0){	//如果text的长度为0
		printf("不能为空\n");	//打印内容。
		return;
	}else{
		if(strcmp(but,"--发送--")==0){	//比较but与"--发送--"，若相同返回0；若相同。
			const gchar  *name = gtk_entry_get_text(GTK_ENTRY(entryname));	//获得行编辑entryname的内容并静态建立name指针进行指定。
			if(strlen(name)==0){	//如果name的长度为0
					printf("name为空。\n");	//打印内容。
					return;
			}
			sprintf(buf,"User:%d:%s%s\n",strlen(name),name,text); 	//将内容写入buf。
			if ((sendbytes = send(clientfd, buf, strlen(buf), 0)) == -1)	//将buf由指定的socket端口clientfd传给对方主机并将发送的字节数存进sendbytes；如果发送失败。
			{
				perror("fail to send");    //把"fail to send"输出到标准错误stderr。       
			}
			return ;		
		}else{
			sprintf(buf,"%s%s\n","All::",text);	//将内容写入buf。
			if ((sendbytes = send(clientfd, buf, strlen(buf), 0)) == -1)	//将buf由指定的socket端口clientfd传给对方主机并将发送的字节数存进sendbytes；如果发送失败。
			{
				perror("fail to send");	//把"fail to send"输出到标准错误 stderr。
			}
			return ;
		}
		
	}
}
//保存消息记录
void savetxt(GtkButton  *button, gpointer entry){
	struct flock lock1;    //新建结构体flock为lock1。
	lock1.l_whence = SEEK_SET;    //相对位移量的起点设置为文件头0。
	lock1.l_start = 0;    //设置位移量。
	lock1.l_len = 0;    //设置加锁区域的长度。
	//以上三个设置可以为整个文件加锁。
	lock1.l_type = F_WRLCK;    //初始化l_type为写入锁。
	lock1.l_pid = -1;    //初始化l_pid。
	struct flock lock2;    //新建结构体flock为lock2。
	lock2.l_whence = SEEK_SET;    //相对位移量的起点设置为文件头0。
	lock2.l_start = 0;    //设置位移量。
	lock2.l_len = 0;    //设置加锁区域的长度。
	//以上三个设置可以为整个文件加锁。
	lock2.l_type = F_RDLCK;    //初始化l_type为读取锁。
	lock2.l_pid = -1;    //初始化l_pid。
	int src_file, dest_file;//用来存储源文件与目标文件的描述符。
	unsigned char buff[1024];//用来存储内容。
	int real_read_len;//用来存储真实读取的字节数。
	char txt_name[60];//用来存储文本记录名。
	sprintf(txt_name,"%s%s","./msgsave_",user_name);	//将内容写入buf。
	src_file = open(fname, O_RDONLY);//打开fname，并将文件描述符存在src_file。
	dest_file = open(txt_name,O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);//打开txt_name，并将文件描述符存在dest_file。
	if (src_file< 0 || dest_file< 0)//如果源文件与目标文件打开失败。
	{
		return;
	}	
	fcntl(dest_file, F_SETLKW, &lock1);//以阻塞的方式为dest_file设置结构为lock1的文件锁。
	fcntl(src_file, F_SETLKW, &lock2);//以阻塞的方式为src_file设置结构为lock2的文件锁。
	while ((real_read_len = read(src_file, buff, sizeof(buff))) > 0)//读取源文件sec_file的sizeof(buff)个字节的数据并存在buff中，并记录真实读取的字节数。
	{
		write(dest_file, buff, real_read_len);//向dest_fie写入buff内real_read_len字节的数据。
	}
	fcntl(dest_file, F_UNLCK, &lock1);//给dest_file解锁。
	fcntl(src_file, F_UNLCK, &lock2);//给src_file解锁。
	close(dest_file);//关闭文件描述符dest_file。
	close(src_file);//关闭文件描述符src_file。
}
GtkTextBuffer *buffers;
//读取消息记录
void readtxt(GtkButton  *button, gpointer entry){
	FILE *dest_file;
	char txt_name[60];
	sprintf(txt_name,"%s%s","./msgsave_",user_name);
	GtkTextIter start,end;
	char StrLine[1024];
	if((dest_file=fopen(txt_name,"r"))==NULL){
		return;	
	}
	while(!feof(dest_file)){
		memset(StrLine,0,1024);
		fgets(StrLine,1024,dest_file);
		gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(buffers),&start,&end);
		gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffers),&end,StrLine,strlen(StrLine));//将消息放入聊天信息框
	}
	fclose(dest_file);
}
///处理接受到的消息
void *strdeal(void *arg){  
	char sign[10];
	char buf[1024];
	char s[1024];
	struct flock lock;    //新建结构体flock为lock1
	lock.l_whence = SEEK_SET;    //相对位移量的起点设置为文件头0。
	lock.l_start = 0;    //设置位移量。
	lock.l_len = 0;    //设置加锁区域的长度。
	lock.l_type = F_WRLCK;    //初始化l_type为写入锁。
	lock.l_pid = -2;    //初始化l_pid
	while(1){
		memset(s, 0, strlen(s));    //将s中当前位置后面的strlen(s)个字节用0替换。
		memset(sign, 0, strlen(sign));    //将sign中当前位置后面的strlen(sign)个字节用0替换。
		memset(buf, 0, strlen(buf));    //将buf中当前位置后面的strlen(buf)个字节用0替换。
		if(recv(clientfd, s, 1024, 0) <= 0)    //把clientfd的接收缓冲中的1024字节的数据copy到s中;如果接收错误。
		    {
			perror("fail to recv");    //把"fail to recv"输出到标准错误stderr。
			close(clientfd);    //关闭socket端口。
			exit(1);
		    }
		int i=0;
		int n=0;
		int j=0;
		for(i;i<strlen(s);i++){
			if(n==1){
				buf[j]=s[i];
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
		if(strcmp(sign,"User")==0){    //比较sign与"User"，若相同返回0；若相同。
			b_file = open(fname,O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);    //打开fname，并将文件描述符存在b_file。
			fcntl(b_file, F_SETLKW, &lock);    //以阻塞的方式为b_file设置结构为lock的文件锁。		
			write(b_file, buf, strlen(buf));    //向b_file写入buf。
			fcntl(b_file, F_UNLCK, &lock);    //给b_file解锁。
			close(b_file);    //关闭文件描述符b_file。
			GtkTextIter start,end;    //新建保存文字在buffer中位置的结构start和end。
			sem_p(sem_id);    //信号量减1。
			gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(bufferuser),&start,&end);    //得到当前buffer中开始位置，结束位置的ITER。
			gtk_text_buffer_insert(GTK_TEXT_BUFFER(bufferuser),&start,buf,strlen(buf));    //向文本框插入文字。start，end分别为文本框文字开始位置和结束位置的iter，插入文本的长度为strlen(buf)。
			sem_v(sem_id);    //信号量加1。
		}else{    //若sign与"User"不同。
			GtkTextIter start,end;    //新建保存文字在buffer中位置的结构start和end。
			gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(buffernotice),&start,&end);
			if(strcmp(buf,"over")==0){   // 收到over说明服务停止，所以程序退出  
				strcpy(buf,"服务停止，程序即将退出\n");    //将"服务停止，程序即将退出\n"复制进buf。 
				sem_p(sem_id);    //信号量减1。
	     		gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffernotice),&start,buf,strlen(buf));    //向文本框插入文字。start，end分别为文本框文字开始位置和结束位置的iter，插入文本的长度为strlen(buf)。
				sem_v(sem_id);    //信号量加1。
				sleep(2);    //休眠2秒。
				close(clientfd);    //关闭socket端口。
				unlink(fname);  //删除fname。 
				exit(0);
			}else{
				sem_p(sem_id);    //信号量减1。
				gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffernotice),&start,buf,strlen(buf));    //向文本框插入文字。start，end分别为文本框文字开始位置和结束位置的iter，插入文本的长度为strlen(buf)。
				sem_v(sem_id);    //信号量加1。
			}
		}	
	}	
}
//主界面
void homepage(int argc,char *argv[]){
	char *buf;
	buf = (char *)malloc(1024);
	memset(buf, 0, 1024);
	pid_t pid;
	// 初始化
 	gtk_init(&argc, &argv);		
	// 创建顶层窗口
	home = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	// 设置窗口的标题
	gtk_window_set_title(GTK_WINDOW(home), "欢迎");
	// 设置窗口在显示器中的位置为居中
	gtk_window_set_position(GTK_WINDOW(home), GTK_WIN_POS_CENTER);
	// 设置窗口的最小大小
	gtk_widget_set_size_request(home, 820, 400);
	// 固定窗口的大小
	gtk_window_set_resizable(GTK_WINDOW(home), FALSE); 
	// "destroy" 和 gtk_main_quit 连接
	g_signal_connect(home, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	//创建一个固定容器
 	GtkWidget *fixed = gtk_fixed_new(); 	
	gtk_container_add(GTK_CONTAINER(home), fixed);
 
	GtkWidget *label_two;
	GtkWidget *label_one;
	// 创建标签
	label_one = gtk_label_new("聊天内容：");
	// 将按钮放在布局容器里
	gtk_fixed_put(GTK_FIXED(fixed), label_one,20,10);	
 
	// 创建标签
	label_two = gtk_label_new("系统通知：");
	gtk_fixed_put(GTK_FIXED(fixed), label_two,320,10);
 
	label_two = gtk_label_new("发送用户：");
	gtk_fixed_put(GTK_FIXED(fixed), label_two,24,295);
	
	label_two = gtk_label_new("发送内容：");
	gtk_fixed_put(GTK_FIXED(fixed), label_two,24,335);
 
	label_two = gtk_label_new("读取消息记录：");
	gtk_fixed_put(GTK_FIXED(fixed), label_two,500,10);
 
	// 行编辑的创建
	entryname = gtk_entry_new();
	// 最大长度
	gtk_entry_set_max_length(GTK_ENTRY(entryname),500);
	gtk_editable_set_editable(GTK_EDITABLE(entryname), TRUE);
	gtk_fixed_put(GTK_FIXED(fixed), entryname,90,290); 
 
	GtkWidget *entry = gtk_entry_new();		
	gtk_entry_set_max_length(GTK_ENTRY(entry),500);
	gtk_editable_set_editable(GTK_EDITABLE(entry), TRUE);
	gtk_fixed_put(GTK_FIXED(fixed), entry,90,330);	
	// 创建按钮
	GtkWidget *bsend = gtk_button_new_with_label("--发送--");
	gtk_fixed_put(GTK_FIXED(fixed), bsend,325,300);
 
	GtkWidget *send_all = gtk_button_new_with_label("--群发--");
	gtk_fixed_put(GTK_FIXED(fixed), send_all,390,300);
	
	GtkWidget *save = gtk_button_new_with_label("--保存记录--");
	gtk_fixed_put(GTK_FIXED(fixed), save,300,340);	
 
	GtkWidget *read_m = gtk_button_new_with_label("--读取记录--");
	gtk_fixed_put(GTK_FIXED(fixed), read_m,390,340);		
 
	// 绑定回调函数
	g_signal_connect(bsend, "pressed", G_CALLBACK(sendtouser), entry);
	g_signal_connect(send_all, "pressed", G_CALLBACK(sendtouser), entry);
	g_signal_connect(save, "pressed", G_CALLBACK(savetxt), entry);
	g_signal_connect(read_m, "pressed", G_CALLBACK(readtxt), entry);
 
	// 文本框聊天窗口
	GtkWidget *view = gtk_text_view_new(); 
	gtk_widget_set_size_request (view, 280, 250);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(view), FALSE);
	gtk_fixed_put(GTK_FIXED(fixed), view, 20, 30);
	// 获取文本缓冲区
	bufferuser=gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));     	
	
        //文本框系统公告
	GtkWidget *name_view = gtk_text_view_new(); 
	gtk_widget_set_size_request (name_view, 150, 230);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(name_view), FALSE);
  	gtk_fixed_put(GTK_FIXED(fixed), name_view, 320, 30);
	buffernotice=gtk_text_view_get_buffer(GTK_TEXT_VIEW(name_view));
	
 
 
	GtkWidget *viewtxt = gtk_text_view_new(); 
	gtk_widget_set_size_request (viewtxt, 300, 350);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(viewtxt), FALSE);
	gtk_fixed_put(GTK_FIXED(fixed), viewtxt, 500, 30);
	
	buffers=gtk_text_view_get_buffer(GTK_TEXT_VIEW(viewtxt)); 
 
 
	// 显示窗口全部控件
	gtk_widget_show_all(home);	
	
	int sendbytes, res;
	pthread_t thread;
	// 开启线程监听收到的数据
	res = pthread_create(&thread, NULL, strdeal, NULL);
	if (res != 0)
	{          
		exit(res);
	}
	usleep(10);
	gtk_main();
}
//主函数
int main(int argc,char *argv[])
{	
	
	//uuid_t uuid;//UUID含义是通用唯一识别码 (Universally Unique Identifier)，这 是一个软件建构的标准,这里用来标记每一个客户端
	char str[36];
	//uuid_generate(uuid);
	//uuid_unparse(uuid, str);//生成的UUID放在参数uuid里面。此时得到的结果是一个8位数的16进制数。在UUID生成函数的过程中经过了一些处理，才生成的是8位的16进制数，原因在于，在它生成的过程中，本来生成的是32位的长整型，结果经过uuid_parse进行转换变成8位的16进制数。相反，我们有uuid_unparse函数，可以反向将16进制数转换为32位的整型。
	strcat(fname,str);
	sem_id = semget(ftok("/", 'a'), 1, 0666|IPC_CREAT);
 
	init_sem(sem_id, 1);
 
	login(argc,argv);
 
	homepage(argc,argv);
	
	//unlink(fname);   //删除临时储存消息记录文件
	return 0;
}