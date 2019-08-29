#include <gtk/gtk.h>	// 头文件
#include <string.h>
#include <stdio.h>
#include"chat.h"
#include"chatting_window.h"
//群聊界面
GtkWidget *chatting_window;

//发送目标用户窗口。
GtkWidget *entryname;
// 文本框缓冲区。
GtkTextBuffer *bufferuser;
GtkTextBuffer *buffernotice;

GtkTextBuffer *buffers;
extern int client_socket;
//根据button的值发送消息时的自我维护。
void sendtouser(GtkButton  *button, gpointer entry){

}
//保存消息记录
void savetxt(GtkButton  *button, gpointer entry){

}
//读取消息记录
void readtxt(GtkButton  *button, gpointer entry){

}

///处理接受到的消息
void *strdeal(void *arg){
    
}

void chatting_win(int argc, char *argv[]){
    gtk_init(&argc, &argv);	
	//printf("zzz:%d\n",client_socket);
    // 创建顶层窗口
    chatting_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    // 设置窗口的标题
    gtk_window_set_title(GTK_WINDOW(chatting_window), "欢迎");
    // 设置窗口在显示器中的位置为居中
	gtk_window_set_position(GTK_WINDOW(chatting_window), GTK_WIN_POS_CENTER);
    // 设置窗口的最小大小
	gtk_widget_set_size_request(chatting_window, 860, 400);
    // 固定窗口的大小
	gtk_window_set_resizable(GTK_WINDOW(chatting_window), FALSE); 
	// "destroy" 和 gtk_main_quit 连接
	g_signal_connect(chatting_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    //创建一个固定容器
 	GtkWidget *fixed = gtk_fixed_new(); 	
	gtk_container_add(GTK_CONTAINER(chatting_window), fixed);
    
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
	gtk_widget_show_all(chatting_window);	
	
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
