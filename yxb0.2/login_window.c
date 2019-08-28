#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "chat.h"
static GtkWidget* entry1;
static GtkWidget* entry2;
static GtkWidget* entry3;
static GtkWidget* entry4;
static GtkWidget* entry5;
GtkWidget* window;
GtkWidget* window1;
GtkWidget* box;
GtkWidget* box1;
GtkWidget* box2;
GtkWidget* box3;
GtkWidget* label1;
GtkWidget* label2;
GtkWidget* button;
GtkWidget* sep;
GtkWidget* hbox;
extern char* str_ip;
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
void error_pop_unconsistent(GtkWindow *parent)
{
    GtkWidget *dialog;
    GtkWidget *label;
    GtkWidget *image;
    GtkWidget *hbox;
    dialog = gtk_dialog_new_with_buttons("error",parent,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
    gtk_dialog_set_has_separator(GTK_DIALOG(dialog),FALSE);
    label = gtk_label_new("两次输入密码不一致");
    hbox = gtk_hbox_new(FALSE,5);
    gtk_container_set_border_width(GTK_CONTAINER(hbox),10);
    gtk_box_pack_start_defaults(GTK_BOX(hbox),label);
    gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox),hbox);
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}
void error_pop_login(GtkWindow *parent)
{
    GtkWidget *dialog;
    GtkWidget *label;
    GtkWidget *image;
    GtkWidget *hbox;
    dialog = gtk_dialog_new_with_buttons("error",parent,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
    gtk_dialog_set_has_separator(GTK_DIALOG(dialog),FALSE);
    label = gtk_label_new("用户名或者密码错误");
    hbox = gtk_hbox_new(FALSE,5);
    gtk_container_set_border_width(GTK_CONTAINER(hbox),10);
    gtk_box_pack_start_defaults(GTK_BOX(hbox),label);
    gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox),hbox);
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}
void error_pop_resiger(GtkWindow *parent)
{
    GtkWidget *dialog;
    GtkWidget *label;
    GtkWidget *image;
    GtkWidget *hbox;
    dialog = gtk_dialog_new_with_buttons("error",parent,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
    gtk_dialog_set_has_separator(GTK_DIALOG(dialog),FALSE);
    label = gtk_label_new("注册失败，用户名已存在");
    hbox = gtk_hbox_new(FALSE,5);
    gtk_container_set_border_width(GTK_CONTAINER(hbox),10);
    gtk_box_pack_start_defaults(GTK_BOX(hbox),label);
    gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox),hbox);
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}
void on_button_clicked (GtkWidget* button,gpointer data)
{
    int flag;
    if((int)data == 1)
    {
        registpage();
    }
    else if ((int)data == 2)
    {
        gchar* username = gtk_entry_get_text(GTK_ENTRY(entry4));
        gchar* password = gtk_entry_get_text(GTK_ENTRY(entry5));
        //char *pass;
        //strcpy(pass,password);
        //pass=sha((char*)password);
        flag=registandlogin((char*)username,(char*)password,enum_login,str_ip);
        g_print("用户名是：%s",username);
        g_print("\n");
        g_print("密码是：%s\n",password);
        //g_print("密码是：%s\n",pass);
        if(flag) error_pop_login(window1);
        else
        {
            gtk_widget_hide_all(window1);
            chatting_win();
        }
    }
    else if((int)data == 3)
    {
        gchar* username = gtk_entry_get_text(GTK_ENTRY(entry1));
        gchar* password = gtk_entry_get_text(GTK_ENTRY(entry2));
        gchar* passagain = gtk_entry_get_text(GTK_ENTRY(entry3));
        //printf("%s\n%s\n%s\n",username,password,passagain);
        //printf("%d\n",strcmp(password,passagain));
        if(strcmp(password,passagain)==0)
        {
            //char *pass;
            //strcpy(pass,password);
            //pass=sha((char*)password);
            flag=registandlogin((char*)username,(char*)password,enum_regist,str_ip);
            printf("%d\n",flag);
            g_print("用户名是：%s\n",username);
            g_print("密码是：%s\n",password);
            if(flag) error_pop_resiger(window);
            else gtk_widget_hide_all(window);
        }
        else
        {
            printf("error\n");
            error_pop_unconsistent(window);
            gtk_entry_set_text(entry3, "");
        }
    }
}
void loginpage()
{   
    //设置窗口
    window1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window1),"destroy",G_CALLBACK(gtk_main_quit),NULL);
    gtk_window_set_title(GTK_WINDOW(window1),"Lintop");
    gtk_window_set_position(GTK_WINDOW(window1),GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window1),0);
    gtk_widget_set_size_request(window1, 500,350);
    box = gtk_vbox_new(FALSE,0);
    gtk_container_add(GTK_CONTAINER(window1),box);
    hbox = gtk_hbox_new(TRUE, 0);       // 创建水平布局容器
    //gtk_container_add(GTK_BOX(box), hbox); // 把水平布局容器放入窗口
    gtk_box_pack_start(GTK_BOX(box),hbox,FALSE,FALSE,25);
    box1 = gtk_hbox_new(FALSE,20);
    gtk_box_pack_start(GTK_BOX(box),box1,FALSE,FALSE,0);
    box2 = gtk_hbox_new(FALSE,24);
    gtk_box_pack_start(GTK_BOX(box),box2,FALSE,FALSE,25);
    sep = gtk_hseparator_new();//分割线
    gtk_box_pack_start(GTK_BOX(box),sep,FALSE,FALSE,7);
    box3 = gtk_hbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(box),box3,TRUE,TRUE,7);
	// 下面借助GdkPixbuf来修改图片的大小，注意接口以gdk_开头，不属于控件类
	// 创建pixbuf，需要占用资源，使用完，需要人为释放
	GdkPixbuf *src = gdk_pixbuf_new_from_file("./neutalk.jpg", NULL);// 读取原图片	
	GdkPixbuf *dst = gdk_pixbuf_scale_simple(src, 500, 120, GDK_INTERP_BILINEAR);	// 修改图片大小(500, 120), 保存在dst
	GtkWidget *image_two = gtk_image_new_from_pixbuf(dst);	// 通过pixbuf创建图片控件
	g_object_unref(src);	// pixbuf使用完，需要人为释放资源
	g_object_unref(dst);
	gtk_container_add(GTK_CONTAINER(hbox), image_two);	// 添加到布局里 
 
    label1 = gtk_label_new("用户名：");
    entry4 = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box1),label1,FALSE,FALSE,50);
    gtk_box_pack_start(GTK_BOX(box1),entry4,TRUE,TRUE,40);
 
    label2 = gtk_label_new("密    码：");
    entry5 = gtk_entry_new();
    /*设置输入文本不可见*/
    gtk_entry_set_visibility(GTK_ENTRY(entry5),FALSE);
    gtk_box_pack_start(GTK_BOX(box2),label2,FALSE,FALSE,50);
    gtk_box_pack_start(GTK_BOX(box2),entry5,TRUE,TRUE,40);

    button = gtk_button_new_with_label("  注册  ");
    g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(on_button_clicked),(gpointer)1);
    gtk_box_pack_start(GTK_BOX(box3),button,FALSE,FALSE,120);
    gtk_widget_show(button);

    button = gtk_button_new_with_label("  登陆  ");
    g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(on_button_clicked),(gpointer)2);
    g_signal_connect_swapped(G_OBJECT(button),"clicked",G_CALLBACK(gtk_widget_destroy),window1);
    //g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(on_button_clicked),(gpointer)2);
    gtk_box_pack_start(GTK_BOX(box3),button,FALSE,FALSE,30);
    gtk_widget_show(button);
    gtk_widget_show_all(window1);
    gtk_main();
}

void registpage()
{
    GtkWidget* label3;
    GtkWidget* box4;
    //设置窗口
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    //g_signal_connect(G_OBJECT(window),"delete_event",G_CALLBACK(gtk_main_quit),NULL);
    gtk_window_set_title(GTK_WINDOW(window),"Lintop");
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window),0);
    gtk_widget_set_size_request(window, 500,350);
    box = gtk_vbox_new(FALSE,0);
    gtk_container_add(GTK_CONTAINER(window),box);
    hbox = gtk_hbox_new(TRUE, 0);       // 创建水平布局容器
    //gtk_container_add(GTK_BOX(box), hbox); // 把水平布局容器放入窗口
    gtk_box_pack_start(GTK_BOX(box),hbox,FALSE,FALSE,15);
    box1 = gtk_hbox_new(FALSE,20);
    gtk_box_pack_start(GTK_BOX(box),box1,FALSE,FALSE,0);
    box2 = gtk_hbox_new(FALSE,24);
    gtk_box_pack_start(GTK_BOX(box),box2,FALSE,FALSE,15);
    box4 = gtk_hbox_new(FALSE,20);
    gtk_box_pack_start(GTK_BOX(box),box4,FALSE,FALSE,0);
    sep = gtk_hseparator_new();//分割线
    gtk_box_pack_start(GTK_BOX(box),sep,FALSE,FALSE,7);
    box3 = gtk_hbox_new(FALSE,0);
    gtk_box_pack_start(GTK_BOX(box),box3,TRUE,TRUE,7);
	// 下面借助GdkPixbuf来修改图片的大小，注意接口以gdk_开头，不属于控件类
	// 创建pixbuf，需要占用资源，使用完，需要人为释放
	GdkPixbuf *src = gdk_pixbuf_new_from_file("./neutalk.jpg", NULL);// 读取原图片	
	GdkPixbuf *dst = gdk_pixbuf_scale_simple(src, 500, 120, GDK_INTERP_BILINEAR);	// 修改图片大小(500, 120), 保存在dst
	GtkWidget *image_two = gtk_image_new_from_pixbuf(dst);	// 通过pixbuf创建图片控件
	g_object_unref(src);	// pixbuf使用完，需要人为释放资源
	g_object_unref(dst);
	gtk_container_add(GTK_CONTAINER(hbox), image_two);	// 添加到布局里 
 
    label1 = gtk_label_new("用户名：");
    entry1 = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box1),label1,FALSE,FALSE,50);
    gtk_box_pack_start(GTK_BOX(box1),entry1,TRUE,TRUE,40);
 
    label2 = gtk_label_new("密    码：");
    entry2 = gtk_entry_new();
    /*设置输入文本不可见*/
    gtk_entry_set_visibility(GTK_ENTRY(entry2),FALSE);
    gtk_box_pack_start(GTK_BOX(box2),label2,FALSE,FALSE,50);
    gtk_box_pack_start(GTK_BOX(box2),entry2,TRUE,TRUE,40);

    label3 = gtk_label_new("重复密码：");
    entry3 = gtk_entry_new();
    /*设置输入文本不可见*/
    gtk_entry_set_visibility(GTK_ENTRY(entry3),FALSE);
    gtk_box_pack_start(GTK_BOX(box4),label3,FALSE,FALSE,43);
    gtk_box_pack_start(GTK_BOX(box4),entry3,TRUE,TRUE,40);

    button = gtk_button_new_with_label("  确认  ");
    g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(on_button_clicked),(gpointer)3);
    g_signal_connect_swapped(G_OBJECT(button),"clicked",G_CALLBACK(gtk_widget_destroy),window);
    gtk_box_pack_start(GTK_BOX(box3),button,FALSE,FALSE,240);
    gtk_widget_show(button);
    gtk_widget_show_all(window);
    gtk_main();
} 
