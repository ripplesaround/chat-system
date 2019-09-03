#include <gtk/gtk.h>	// 头文件
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include"chat.h"
#include"chatting_window.h"
GtkWidget *chatting_together_window;
//发送目标用户窗口。
GtkWidget *entryname_together;
// 文本框缓冲区。
GtkTextBuffer *bufferuser_together;

GtkTextBuffer *buffers_together;

//char fname[]="/var/tmp/";
extern int client_socket;
extern char *username;
extern GMutex* mutex;
pthread_t thread_together;

void sendtouser_together(GtkButton  *button, gpointer entry)
{
	char str[250];
	Packet packet;
	Data data;
	Kind kind;
	char buf[100];
	// char *buf;	//用来存储内容。
	// buf = (char *)malloc(1024);    //向系统申请分配指定1024个字节的内存空间。
	// memset(buf, 0, 1024);    //将buf中当前位置后面的1024个字节用0替换。
	// int sendbytes;	//用来记录发送的字节数。
	const gchar  *text = gtk_entry_get_text(GTK_ENTRY(entry));	//获得行编辑entry的内容并静态建立text指针进行指定。
	const char *but = gtk_button_get_label(button);	//获得获取按钮button文本内容并静态建立but指针进行指定。
//	const gchar  *name = gtk_entry_get_text(GTK_ENTRY(entryname_together));	//获得行编辑entryname的内容并静态建立name指针进行指定。

	if(strlen(text)==0)
	{	//如果text的长度为0
		printf("不能为空\n");	//打印内容。
	}
	else
	{
		if(strcmp(but,"--发送--")==0)
		{	//比较but与"--发送--"，若相同返回0；若相同。

			strcpy(data.message.id_from,username);
			//strcpy(data.message.id_to,name);
			strcpy(data.message.str,text);
			printf("id_from:%s	text:%s\n",data.message.id_from,text);
			if(build_packet(&packet,enum_ipchat,data.message) == -1)
			{	    //打包类型为enum_ipchat的包
				printf("fail to build the packet!\n");
				return;
			}
			write(client_socket,&packet,sizeof(Packet));
			strcpy(buf,"\t\t\t\t\t\t\t\t");
			strcat(buf,data.message.id_from);
			strcat(buf,":\n\t\t\t\t\t\t\t\t\t");
			strcat(buf,data.message.str);
			strcat(buf,"\n");
			GtkTextIter start,end;    //新建保存文字在buffer中位置的结构start和end。
			gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(bufferuser_together),&start,&end);    //得到当前buffer中开始位置，结束位置的ITER。
			gtk_text_buffer_insert(GTK_TEXT_BUFFER(bufferuser_together),&end,buf,strlen(buf));    //向文本框插入文字。start，end分别为文本框文字开始位置和结束位置的iter，插入文本的长度为strlen(buf)。
		}
		// else
		// {
		// 	strcat(data.message.str,text);
		// 	if(build_packet(&packet,enum_ipchat,data) == -1)
		// 	{	    //打包类型为enum_chat的包
		// 		printf("fail to build the packet!\n");
		// 		return;
		// 	}
		// 	write(client_socket,&packet,sizeof(Packet));
		// }
		
	}
}


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

		strcpy(buf,data.message.id_from);
		strcat(buf,":\n\t");
		strcat(buf,data.message.str);
		strcat(buf,"\n");
//		if(strcmp(data.message.id_to,username)==0)
//		{    
		GtkTextIter start,end;    //新建保存文字在buffer中位置的结构start和end。
		gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(bufferuser_together),&start,&end);    //得到当前buffer中开始位置，结束位置的ITER。
		gtk_text_buffer_insert(GTK_TEXT_BUFFER(bufferuser_together),&end,buf,strlen(buf));    //向文本框插入文字。start，end分别为文本框文字开始位置和结束位置的iter，插入文本的长度为strlen(buf)。
//		}
	}
}

void quit_chatroom_together()
{
	Packet packet;
	Data data;
	if(build_packet(&packet,enum_quitchat,data) == -1)
	{	    //打包类型为enum_quitchat的包
		printf("fail to build the packet!\n");
		return;
	}
	write(client_socket,&packet,sizeof(Packet));
	pthread_cancel(thread_together);
	g_mutex_unlock(mutex);/*解锁*/
	gtk_main_quit();
}

void chatting_together_win()
{	
	g_mutex_lock(mutex);
	gtk_init(NULL, NULL);	
	//printf("zzz:%d\n",client_socket);
    // 创建顶层窗口
    chatting_together_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    // 设置窗口的标题
    gtk_window_set_title(GTK_WINDOW(chatting_together_window), "chat-together");
    // 设置窗口在显示器中的位置为居中
	gtk_window_set_position(GTK_WINDOW(chatting_together_window), GTK_WIN_POS_CENTER);
    // 设置窗口的最小大小
	gtk_widget_set_size_request(chatting_together_window, 500, 400);
    // 固定窗口的大小
	gtk_window_set_resizable(GTK_WINDOW(chatting_together_window), FALSE); 
	// "destroy" 和 gtk_main_quit 连接
	g_signal_connect(G_OBJECT(chatting_together_window), "destroy", G_CALLBACK(quit_chatroom_together), NULL);
    //创建一个固定容器
 	GtkWidget *fixed = gtk_fixed_new(); 	
	gtk_container_add(GTK_CONTAINER(chatting_together_window), fixed);
    //GtkWidget *scrolled=gtk_scrolled_window_new(NULL,NULL);
	//gtk_scrolled_window_set_policy( scrolled, NULL, GTK_POLICY_AUTOMATIC );

    GtkWidget *label_two;
	GtkWidget *label_one;
	// 创建标签
	label_one = gtk_label_new("聊天内容");
	// 将按钮放在布局容器里
	gtk_fixed_put(GTK_FIXED(fixed), label_one,20,10);	
 
	// 创建标签
	label_two = gtk_label_new("系统通知");
	gtk_fixed_put(GTK_FIXED(fixed), label_two,320,10);
 
	label_two = gtk_label_new("发送用户：");
	gtk_fixed_put(GTK_FIXED(fixed), label_two,24,295);
	
	label_two = gtk_label_new("发送内容");
	gtk_fixed_put(GTK_FIXED(fixed), label_two,24,335);

    //行编辑的创建
	entryname_together = gtk_entry_new();
	// 最大长度
	gtk_entry_set_max_length(GTK_ENTRY(entryname_together),500);
	gtk_editable_set_editable(GTK_EDITABLE(entryname_together), TRUE);
	gtk_fixed_put(GTK_FIXED(fixed), entryname_together,90,290); 
 
	GtkWidget *entry = gtk_entry_new();		
	gtk_entry_set_max_length(GTK_ENTRY(entry),500);
	gtk_editable_set_editable(GTK_EDITABLE(entry), TRUE);
	gtk_fixed_put(GTK_FIXED(fixed), entry,90,330);	
	// 创建按钮
	GtkWidget *bsend = gtk_button_new_with_label("--发送--");
	gtk_fixed_put(GTK_FIXED(fixed), bsend,325,300);
 
	GtkWidget *send_all = gtk_button_new_with_label("--发送文件--");
	gtk_fixed_put(GTK_FIXED(fixed), send_all,390,300);		

	GtkWidget *save = gtk_button_new_with_label("--保存记录--");
	gtk_fixed_put(GTK_FIXED(fixed), save,300,340);	
 
	GtkWidget *read_m = gtk_button_new_with_label("--读取记录--");
	gtk_fixed_put(GTK_FIXED(fixed), read_m,390,340);
 
	// 绑定回调函数
	g_signal_connect(bsend, "clicked", G_CALLBACK(sendtouser_together), entry);
	//g_signal_connect(send_all, "clicked", G_CALLBACK(writefile_window), entry);
	//g_signal_connect(save, "clicked", G_CALLBACK(savetxt), entry);
	//g_signal_connect(read_m, "clicked", G_CALLBACK(readtxt), entry);

 
	// 文本框聊天窗口
	GtkWidget *view = gtk_text_view_new(); 
	gtk_widget_set_size_request (view, 280, 250);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(view), FALSE);
	gtk_fixed_put(GTK_FIXED(fixed), view, 20, 30);
	// 获取文本缓冲区
	bufferuser_together=gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));     	
	
    //文本框系统公告
	GtkWidget *name_view = gtk_text_view_new(); 
	gtk_widget_set_size_request (name_view, 150, 230);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(name_view), FALSE);
  	gtk_fixed_put(GTK_FIXED(fixed), name_view, 320, 30);
	//buffernotice=gtk_text_view_get_buffer(GTK_TEXT_VIEW(name_view));
	
 
	// 显示窗口全部控件
	gtk_widget_show_all(chatting_together_window);	

	//开启线程监听收到的数据
	int res;
	res = pthread_create(&thread_together, NULL, strdeal_together, NULL);
	if (res != 0)
	{          
		exit(res);
	}
	usleep(10);
	gtk_main();

}