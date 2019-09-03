#include <gtk/gtk.h>	// 头文件
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include"chat.h"
GtkWidget *chatting_window_together;
pthread_t thread;
//发送目标用户窗口。
// 文本框缓冲区。
GtkTextBuffer *bufferuser;

GtkTextBuffer *buffers;
extern int client_socket;
extern char *username;
extern GMutex* mutex;
//根据button的值发送消息时的自我维护。
void sendtouser_together(GtkButton  *button, gpointer entry)
{
	char str[250];
	Packet packet;
	Data data;
	Kind kind;
	char buf[100];
	GtkTextIter start,end;
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(buffers),&start,&end);
	gchar* text=gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffers),&start,&end,FALSE);/*获得文本框缓冲区文本*/
	g_print("%s\n",text);
	//printf("%s\n",buffers);
	if(strlen(text)==0)
	{	//如果text的长度为0
		printf("不能为空\n");	//打印内容。
	}
	else
	{
		strcpy(data.message.id_from,username);
		strcpy(data.message.id_to,"all");
		strcpy(data.message.str,text);
		printf("id_to:%s	id_from:%s	text:%s\n",data.message.id_to,data.message.id_from,text);
		if(build_packet(&packet,enum_chat,data) == -1)
		{	    //打包类型为enum_chat的包
			printf("fail to build the packet!\n");
			return;
		}
		write(client_socket,&packet,sizeof(Packet));
		strcpy(buf,data.message.id_from);
		strcat(buf,":\n\t");
		strcat(buf,data.message.str);
		strcat(buf,"\n");
		//printf()	
		GtkTextIter start,end;    //新建保存文字在buffer中位置的结构start和end。
		gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(bufferuser),&start,&end);/*获得缓冲区开始和结束位置的Iter*/
		gtk_text_buffer_insert(GTK_TEXT_BUFFER(bufferuser),&end,buf,strlen(buf));/*插入文本到缓冲区*/
	}
}
//保存消息记录
// void savetxt(GtkButton  *button, gpointer entry)
// {
// 	struct flock lock1;    //新建结构体flock为lock1。
// 	lock1.l_whence = SEEK_SET;    //相对位移量的起点设置为文件头0。
// 	lock1.l_start = 0;    //设置位移量。
// 	lock1.l_len = 0;    //设置加锁区域的长度。
// 	//以上三个设置可以为整个文件加锁。
// 	lock1.l_type = F_WRLCK;    //初始化l_type为写入锁。
// 	lock1.l_pid = -1;    //初始化l_pid。
// 	struct flock lock2;    //新建结构体flock为lock2。
// 	lock2.l_whence = SEEK_SET;    //相对位移量的起点设置为文件头0。
// 	lock2.l_start = 0;    //设置位移量。
// 	lock2.l_len = 0;    //设置加锁区域的长度。
// 	//以上三个设置可以为整个文件加锁。
// 	lock2.l_type = F_RDLCK;    //初始化l_type为读取锁。
// 	lock2.l_pid = -1;    //初始化l_pid。
// 	int src_file, dest_file;//用来存储源文件与目标文件的描述符。
// 	unsigned char buff[1024];//用来存储内容。
// 	int real_read_len;//用来存储真实读取的字节数。
// 	char txt_name[60];//用来存储文本记录名。
// 	sprintf(txt_name,"%s%s","./msgsave_",username);	//将内容写入buf。
// 	src_file = open(fname, O_RDONLY);//打开fname，并将文件描述符存在src_file。
// 	dest_file = open(txt_name,O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);//打开txt_name，并将文件描述符存在dest_file。
// 	if (src_file< 0 || dest_file< 0)//如果源文件与目标文件打开失败。
// 	{
// 		return;
// 	}	
// 	fcntl(dest_file, F_SETLKW, &lock1);//以阻塞的方式为dest_file设置结构为lock1的文件锁。
// 	fcntl(src_file, F_SETLKW, &lock2);//以阻塞的方式为src_file设置结构为lock2的文件锁。
// 	while ((real_read_len = read(src_file, buff, sizeof(buff))) > 0)//读取源文件sec_file的sizeof(buff)个字节的数据并存在buff中，并记录真实读取的字节数。
// 	{
// 		write(dest_file, buff, real_read_len);//向dest_fie写入buff内real_read_len字节的数据。
// 	}
// 	fcntl(dest_file, F_UNLCK, &lock1);//给dest_file解锁。
// 	fcntl(src_file, F_UNLCK, &lock2);//给src_file解锁。
// 	close(dest_file);//关闭文件描述符dest_file。
// 	close(src_file);//关闭文件描述符src_file。
// }
//读取消息记录
// void readtxt(GtkButton  *button, gpointer entry)
// {

// }

///处理接受到的消息
void *strdeal_together(void *arg)
{
	while(1)
	{
		Packet packet;
		Data data;
		Kind kind;
		int verify;
		char buf[100];
		do
		{
			if(read(client_socket, &packet, sizeof(Packet))<0)
			{
				perror("fail to recv");    //把"fail to recv"输出到标准错误stderr。
				close(client_socket);    //关闭socket端口。
				exit(1);
			}
			parse_packet(packet,&kind,&data,&verify);
		}while(verify!=23333);

		printf("test:%s\n",data.message.str);
		strcpy(buf,data.message.id_from);
		strcat(buf,":\n\t");
		strcat(buf,data.message.str);
		strcat(buf,"\n");
		GtkTextIter start,end;    //新建保存文字在buffer中位置的结构start和end。
		gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(bufferuser),&start,&end);/*获得缓冲区开始和结束位置的Iter*/
		gtk_text_buffer_insert(GTK_TEXT_BUFFER(bufferuser),&end,buf,strlen(buf));/*插入文本到缓冲区*/
	}
}
void quit_chatroom_together()
{
	Packet packet;
	Data data;
	if(build_packet(&packet,enum_quitchat,data) == -1)
	{	    //打包类型为enum_chat的包
		printf("fail to build the packet!\n");
		return;
	}
	write(client_socket,&packet,sizeof(Packet));
	pthread_cancel(thread);
	g_mutex_unlock (mutex);/*解锁*/
	gtk_main_quit();
}

void chatting_win_together()
{
	GtkWidget *entry;
    g_mutex_lock(mutex);/*上锁*/
    printf("lock\n");
	gtk_init(NULL, NULL);	
	//printf("zzz:%d\n",client_socket);
    // 创建顶层窗口
    chatting_window_together = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    // 设置窗口的标题
    gtk_window_set_title(GTK_WINDOW(chatting_window_together), "群聊");
    // 设置窗口在显示器中的位置为居中
	gtk_window_set_position(GTK_WINDOW(chatting_window_together), GTK_WIN_POS_CENTER);
    // 设置窗口的最小大小
	gtk_widget_set_size_request(chatting_window_together, 650, 800);
    // 固定窗口的大小
	gtk_window_set_resizable(GTK_WINDOW(chatting_window_together), FALSE); 
	// "destroy" 和 gtk_main_quit 连接
	g_signal_connect(G_OBJECT(chatting_window_together), "destroy", G_CALLBACK(quit_chatroom_together), NULL);
    //创建一个固定容器
 	GtkWidget *fixed = gtk_fixed_new(); 	
	gtk_container_add(GTK_CONTAINER(chatting_window_together), fixed);

	// g_signal_connect(G_OBJECT(chatting_window_together),1000,800);
    //GtkWidget *scrolled=gtk_scrolled_window_new(NULL,NULL);
	//gtk_scrolled_window_set_policy( scrolled, NULL, GTK_POLICY_AUTOMATIC );

	GtkWidget *label_one;
	GtkWidget *label_two;
	GtkWidget *label_three;
	PangoFontDescription *pattern;
	pattern = pango_font_description_from_string("Simsun 15");
	// 创建标签
	label_one = gtk_label_new("聊天内容");
	// 将按钮放在布局容器里
	gtk_fixed_put(GTK_FIXED(fixed), label_one,45,20);
	gtk_widget_modify_font(label_one,pattern);	
 
	// 创建标签
//	label_two = gtk_label_new("系统通知");
//	gtk_fixed_put(GTK_FIXED(fixed), label_two,655,20);
//	gtk_widget_modify_font(label_two,pattern);
//
//	label_three = gtk_label_new("在线人员");
//	gtk_fixed_put(GTK_FIXED(fixed), label_three,655,417);
//	gtk_widget_modify_font(label_three,pattern);

	pattern = pango_font_description_from_string("Simsun 20");
 	//创建聊天框
    GtkWidget *chat_view=gtk_text_view_new();
    gtk_widget_set_size_request(chat_view,560,130);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(chat_view),TRUE);
    gtk_fixed_put(GTK_FIXED(fixed),chat_view,45,625);
    gtk_widget_modify_font(chat_view,pattern);
	buffers = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat_view));
// 创建按钮
    GtkWidget *bsend = gtk_button_new_with_label("--发送--");
    gtk_fixed_put(GTK_FIXED(fixed), bsend, 505, 570);
    gtk_widget_set_size_request(bsend,100,40);
//    gtk_widget_modify_font(bsend,pattern);

    // GtkWidget *read_m = gtk_button_new_with_label("--读取记录--");
    // gtk_fixed_put(GTK_FIXED(fixed), read_m, 175, 570);
    // gtk_widget_set_size_request(read_m,100,40);
 
	// 绑定回调函数
	g_signal_connect(bsend, "clicked", G_CALLBACK(sendtouser_together), entry);
	//g_signal_connect(send_all, "clicked", G_CALLBACK(writefile_window), entry);
	//g_signal_connect(save, "clicked", G_CALLBACK(savetxt), entry);
	//g_signal_connect(read_m, "clicked", G_CALLBACK(readtxt), entry);

 
	// 文本框聊天窗口
	GtkWidget *view = gtk_text_view_new(); 
	gtk_widget_set_size_request (view, 560,500);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(view), FALSE);
	gtk_fixed_put(GTK_FIXED(fixed), view, 45, 55);
	gtk_widget_modify_font(view,pattern);
	// 获取文本缓冲区
	bufferuser=gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));     	
	
    //文本框系统公告
//	GtkWidget *name_view = gtk_text_view_new();
//	gtk_widget_set_size_request (name_view, 300, 300);
//	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(name_view), FALSE);
//  	gtk_fixed_put(GTK_FIXED(fixed), name_view, 655, 55);
//	gtk_widget_modify_font(name_view,pattern);
//	//buffernotice=gtk_text_view_get_buffer(GTK_TEXT_VIEW(name_view));
//
//	GtkWidget *name_chat = gtk_text_view_new();
//	gtk_widget_set_size_request (name_chat, 300, 300);
//	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(name_chat), FALSE);
//  	gtk_fixed_put(GTK_FIXED(fixed), name_chat, 655, 457);
//	gtk_widget_modify_font(name_chat,pattern);
 
 
	// 显示窗口全部控件
	gtk_widget_show_all(chatting_window_together);	
	
	int sendbytes, res;

	//开启线程监听收到的数据
	res = pthread_create(&thread, NULL, strdeal_together, NULL);
	if (res != 0)
	{          
		exit(res);
	}
	usleep(10);
	gtk_main();
}

