#include <gtk/gtk.h>	// 头文件
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include "chat.h"
static GtkWidget* entry1;
GtkWidget* window;
GtkWidget* box;
GtkWidget* box1;
GtkWidget* box2;
GtkWidget* box3;
GtkWidget* label1;
GtkWidget* button_forsearch;//
GtkWidget* button_forchat_together;//
GtkWidget* sep;
extern int client_socket;
//int client_socket;
char *username="fwx";
void pop_friend(GtkWindow *parent,int flag)
{
    GtkWidget *dialog;
    GtkWidget *label;
    GtkWidget *image;
    GtkWidget *hbox;
    if(flag==1)
    {
        dialog = gtk_dialog_new_with_buttons("error",parent,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
        label = gtk_label_new("该人不存在");
    }
    else if(flag==0)
    {
        dialog = gtk_dialog_new_with_buttons("Lintop",parent,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
        label = gtk_label_new("好友请求已发送");
    }
    else if(flag==2)
    {
        dialog = gtk_dialog_new_with_buttons("error",parent,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
        label = gtk_label_new("不能添加自己");
    }
    gtk_dialog_set_has_separator(GTK_DIALOG(dialog),FALSE);
    hbox = gtk_hbox_new(FALSE,5);
    gtk_container_set_border_width(GTK_CONTAINER(hbox),10);
    gtk_box_pack_start_defaults(GTK_BOX(hbox),label);
    gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox),hbox);
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}
void search(char *to_username)
{
    //printf("%s\n%s\n",to_username,username);
    Kind kind=enum_friend;
    Data data;
    Packet packet;
    strcpy(data.message.id_from,username);
    strcpy(data.message.id_to,to_username);
    build_packet(&packet,kind,data);
    write(client_socket, &packet, sizeof(Packet));
    read(client_socket, &packet, sizeof(Packet));
    read(client_socket, &packet, sizeof(Packet));
    parse_packet(packet,&kind,&data);
    if(!strcmp(data.message.id_to,data.message.id_from)) pop_friend(window,2);
    if(kind==enum_friend&&(!strcmp(data.message.str,"1"))) pop_friend(window,1);
    else pop_friend(window,0);
}
void on_button_clicked_search(GtkWidget* button,gpointer data)
{
    gchar* searchid = gtk_entry_get_text(GTK_ENTRY(entry1));
    search((char*)searchid);
}
void on_button_clicked_chat_together(GtkWidget* button,gpointer gdata)
{
    Kind kind=enum_chat;
    Data data;
    Packet packet;
    build_packet(&packet,kind,data);
    write(client_socket, &packet, sizeof(Packet));
    chatting_win();
}
void destroy_logout()
{
    Kind kind=enum_logout;
    Data data;
    Packet packet;
    strcpy(data.userinfo.account,username);
    build_packet(&packet,kind,data);
    write(client_socket, &packet, sizeof(Packet));
    gtk_main_quit();
}
void read_from()
{
    Kind kind;
    Data data;
    Packet packet;
    read(client_socket, &packet, sizeof(Packet));
    parse_packet(packet,&kind,&data);

}
void main_win(char *user)
{
    //gtk_init(&argc,&argv);
    pthread_t thIDr,thIDw;
    pthread_create(&thIDr, NULL,(void *)read_from,NULL);
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

    box3 = gtk_hbox_new(FALSE,24);
    gtk_box_pack_start(GTK_BOX(box),box3,FALSE,FALSE,15);

    label1 = gtk_label_new(username);
    gtk_box_pack_start(GTK_BOX(box1),label1,FALSE,FALSE,60);

    sep = gtk_hseparator_new();//分割线
    gtk_box_pack_start(GTK_BOX(box),sep,FALSE,FALSE,7);

    entry1 = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box2),entry1,TRUE,TRUE,20);
    button_forsearch = gtk_button_new_with_label("  搜索  ");
    button_forchat_together = gtk_button_new_with_label("  群聊  ");

    g_signal_connect(G_OBJECT(button_forsearch),"clicked",G_CALLBACK(on_button_clicked_search),NULL);
    g_signal_connect(G_OBJECT(button_forchat_together),"clicked",G_CALLBACK(on_button_clicked_chat_together),NULL);
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(destroy_logout),NULL);
    gtk_box_pack_start(GTK_BOX(box2),button_forsearch,FALSE,FALSE,10);
    gtk_box_pack_start(GTK_BOX(box3),button_forchat_together,TRUE,TRUE,10);
    gtk_widget_show(button_forsearch);
    gtk_widget_show(button_forchat_together);
    gtk_widget_show_all(window);
    gtk_main();
}