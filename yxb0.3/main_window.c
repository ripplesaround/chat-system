#include <gtk/gtk.h>	// 头文件
#include <string.h>
#include <stdio.h>
#include "chat.h"
static GtkWidget* entry1;
GtkWidget* window;
GtkWidget* box;
GtkWidget* box1;
GtkWidget* box2;
GtkWidget* label1;
GtkWidget* button;
GtkWidget* sep;
extern int client_socket;
//int client_socket;
char *username="fwx";
void search(char *to_username)
{
    //printf("%s\n%s\n",to_username,username);
    Kind kind=enum_friend;
    Data data;
    Packet packet;

   /* struct sockaddr_in server_addr;
	int port = 4567;
	client_socket=socket(AF_INET,SOCK_STREAM,0);	//创建客户端套接字

	server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	server_addr.sin_port=htons(port);
	server_addr.sin_family=AF_INET;
    connect(client_socket,(struct sockaddr*)&server_addr,sizeof(server_addr));*/
    printf("client:%d\n",client_socket);
    strcpy(data.message.id_from,username);
    strcpy(data.message.id_to,to_username);
    printf("%s\n%s\n",data.message.id_from,data.message.id_to);
    build_packet(&packet,kind,data);
    write(client_socket, &packet, sizeof(Packet));
    //read(client_socket, &packet, sizeof(Packet));
    //parse_packet(packet,&kind,&data);
}
void on_button_clicked_search(GtkWidget* button,gpointer data)
{
    if((int)data==0)
    {
        gchar* searchid = gtk_entry_get_text(GTK_ENTRY(entry1));
        search((char*)searchid);
    }
}
void main_win(char *user)
{
    //gtk_init(&argc,&argv);
    username=user;
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    //g_signal_connect(G_OBJECT(window),"destory",G_CALLBACK(gtk_main_quit),NULL);
    gtk_window_set_title(GTK_WINDOW(window),"Lintop");
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window),0);
    gtk_widget_set_size_request(window, 300,700);

    box = gtk_vbox_new(FALSE,0);
    gtk_container_add(GTK_CONTAINER(window),box);

    box1 = gtk_hbox_new(FALSE,20);
    gtk_box_pack_start(GTK_BOX(box),box1,FALSE,FALSE,0);

    sep = gtk_hseparator_new();//分割线
    gtk_box_pack_start(GTK_BOX(box),sep,FALSE,FALSE,7);

    box2 = gtk_hbox_new(FALSE,24);
    gtk_box_pack_start(GTK_BOX(box),box2,FALSE,FALSE,15);

    label1 = gtk_label_new(username);
    gtk_box_pack_start(GTK_BOX(box1),label1,FALSE,FALSE,60);

    sep = gtk_hseparator_new();//分割线
    gtk_box_pack_start(GTK_BOX(box),sep,FALSE,FALSE,7);

    entry1 = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box2),entry1,TRUE,TRUE,20);
    button = gtk_button_new_with_label("  搜索  ");
    g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(on_button_clicked_search),(gpointer)0);
    //g_signal_connect_swapped(G_OBJECT(button),"clicked",G_CALLBACK(gtk_widget_destroy),window);
    gtk_box_pack_start(GTK_BOX(box2),button,FALSE,FALSE,10);
    gtk_widget_show(button);

    gtk_widget_show_all(window);
    gtk_main();
}