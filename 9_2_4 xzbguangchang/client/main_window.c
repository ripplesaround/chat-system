#include <gtk/gtk.h>	// 头文件
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <glib.h>
#include "chat.h"
extern int client_socket;

GMutex *mutex;/*返回一个新的互斥锁*/
//GMutex *mutex_together;//room_mutex
//int client_socket;
char *username="fwx";
char file_name[30]="friend_list";
//g_thread_m
GtkWidget *window;
GtkWidget *vbox;
GtkWidget *hbox1;
GtkWidget *hbox2;
GtkWidget *hbox3;
GtkWidget *vbox1;
GtkWidget *vbox1_0;
GtkWidget *vbox1_1;
GdkPixbuf *pix;
GdkPixbuf *pixnew;
GtkWidget *image;
GtkWidget *label1;
GtkWidget *search_friend;
GtkButton *button;
GtkWidget *frame;
GtkWidget *scrolled;
FILE *fp;
//char *id;
pthread_t thIDr,thIDw;
void write_friend(GtkWidget* button,int flag)
{
    Kind kind;
    Data data;
    Packet packet;
    if((int)flag==1) strcpy(data.message.str,"accept");
    else if((int)flag==2) strcpy(data.message.str,"refuse");
    kind = enum_friend;
    build_packet(&packet,kind,data);
    write(client_socket, &packet, sizeof(Packet));
}
void pop_friend(GtkWindow *parent,int flag,char *id_to)
{
    gdk_threads_enter(); 
    GtkWidget *dialog;
    GtkWidget *button;
    GtkWidget *label;
    GtkWidget *hbox;
    Packet packet;
    Data data;
    if(flag==1)
    {
        dialog = gtk_dialog_new_with_buttons("error",parent,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
        label = gtk_label_new("该人不存在");
    }
    else if(flag==0)
    {
        dialog = gtk_dialog_new_with_buttons("TeliTalk",parent,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
        label = gtk_label_new("好友请求已发送");
    }
    else if(flag==2)
    {
        GtkWidget *dialog;
        gint result;
        dialog = gtk_dialog_new_with_buttons("好友请求",NULL,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
        gtk_dialog_set_default_response(GTK_DIALOG(dialog),GTK_RESPONSE_OK);
        char friend_str[50];
        strcpy(friend_str,id_to);
        strcat(friend_str,"想添加您为好友");
        label = gtk_label_new(friend_str);
        gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox),label);
        gtk_widget_show_all(dialog);
        result = gtk_dialog_run(GTK_DIALOG(dialog));
        switch(result)
        {
            case GTK_RESPONSE_OK:
                strcpy(data.message.str,"accept");
                g_print("ok\n");
                update_new(id_to);
                gtk_widget_show_all(window);
                fp=fopen(file_name,"a+");
                fputs(id_to,fp);
                fprintf(fp,"\n");
                fclose(fp);
                break;
            case GTK_RESPONSE_CANCEL:
                strcpy(data.message.str,"refuse");
                g_print("cancel is press!\n");
                break;
            default:
                g_print("something wrong!\n");
                break;
        }
        strcpy(data.message.id_from,username);
        strcpy(data.message.id_to,id_to);
        build_packet(&packet,enum_friend,data);
        write(client_socket, &packet, sizeof(Packet));
        gtk_widget_destroy(dialog);
        gdk_threads_leave(); 
        return ;
    }
    else if (flag == 3)
    {
        dialog = gtk_dialog_new_with_buttons("TeliTalk",parent,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
        label = gtk_label_new("对方已添加您为好友");
        update_new(id_to);
        gtk_widget_show_all(window);
        fp=fopen(file_name,"a+");
        fputs(id_to,fp);
        fprintf(fp,"\n");
        fclose(fp);
    }
    else if (flag == 4)
    {
        dialog = gtk_dialog_new_with_buttons("TeliTalk",parent,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
        label = gtk_label_new("对方拒绝添加您为好友");
    }
    gtk_dialog_set_has_separator(GTK_DIALOG(dialog),FALSE);
    hbox = gtk_hbox_new(FALSE,5);
    gtk_container_set_border_width(GTK_CONTAINER(hbox),10);
    gtk_box_pack_start_defaults(GTK_BOX(hbox),label);
    gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox),hbox);
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    gdk_threads_leave();  
}
void pop_message(GtkWindow *parent,int flag,char *id_from)
{
    gdk_threads_enter(); 
    GtkWidget *dialog;
    GtkWidget *button;
    GtkWidget *label;
    GtkWidget *hbox;
    Packet packet;
    Data data;
    if(flag==0)
    {
        dialog = gtk_dialog_new_with_buttons("TeliTalk",parent,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
        //gtk_dialog_set_default_response(GTK_DIALOG(dialog),GTK_RESPONSE_OK);
        char message_str[50];
        strcpy(message_str,id_from);
        strcat(message_str,"给您发送了消息"); 
        label = gtk_label_new(message_str);
    }
    gtk_dialog_set_has_separator(GTK_DIALOG(dialog),FALSE);
    hbox = gtk_hbox_new(FALSE,5);
    gtk_container_set_border_width(GTK_CONTAINER(hbox),10);
    gtk_box_pack_start_defaults(GTK_BOX(hbox),label);
    gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox),hbox);
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    gdk_threads_leave();
}
void write_heart()
{
    while(1)
    {
        Kind kind;
        Data data;
        Packet packet;
        strcpy(data.message.id_to,"fish_in_the_pool");
        kind = enum_friend;
        build_packet(&packet,kind,data);
        write(client_socket, &packet, sizeof(Packet));
        //printf("123\n");
        sleep(5);
    }
}
void search(char *to_username)
{
    //printf("%s\n%s\n",to_username,username);
    Kind kind=enum_friend;
    Data data;
    Packet packet;
    strcpy(data.message.id_from,username);
    strcpy(data.message.id_to,to_username);
    if(!strcmp(data.message.id_to,data.message.id_from)) 
    {
        GtkWidget *dialog;
        GtkWidget *label;
        GtkWidget *hbox;
        dialog = gtk_dialog_new_with_buttons("error",window,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
        label = gtk_label_new("不能添加自己");
        gtk_dialog_set_has_separator(GTK_DIALOG(dialog),FALSE);
        hbox = gtk_hbox_new(FALSE,5);
        gtk_container_set_border_width(GTK_CONTAINER(hbox),10);
        gtk_box_pack_start_defaults(GTK_BOX(hbox),label);
        gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox),hbox);
        gtk_widget_show_all(dialog);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    build_packet(&packet,kind,data);
    write(client_socket, &packet, sizeof(Packet));
}
void on_button_clicked_search(GtkWidget* button,gpointer data)
{
    gchar* searchid = gtk_entry_get_text(GTK_ENTRY(search_friend));
    search((char*)searchid);
}
void on_button_clicked_chat(GtkWidget* button,char *friend_name)
{
    Kind kind=enum_chat;
    Data data;
    Packet packet;

    strcpy(data.message.id_to,friend_name);
    build_packet(&packet,kind,data);
    write(client_socket, &packet, sizeof(Packet));
    pthread_cancel(thIDw);
    chatting_win(friend_name);
    pthread_create(&thIDw, NULL,(void *)write_heart,NULL);
}
//chat_together

void on_button_clicked_chat_together(GtkWidget* button,char *friend_name)
{
    Kind kind=enum_ipchat;
    Data data;
    Packet packet;
    strcpy(data.message.id_from,username);
    build_packet(&packet,kind,data);
    write(client_socket, &packet, sizeof(Packet));
    pthread_cancel(thIDw);
    chatting_together_win();
    pthread_create(&thIDw, NULL,(void *)write_heart,NULL);
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
    int verify;
    Packet packet;
    while(1)
    {
        g_mutex_lock(mutex);/*上锁*/
        //g_mutex_lock(mutex_together);
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
        printf("tes!!!t: %s\n",data.message.str);
        if(kind==enum_friend&&(!strcmp(data.message.str,"1"))) pop_friend(window,1,data.message.id_from);
        else if(kind==enum_friend&&(!strcmp(data.message.str,"add_friend"))) pop_friend(window,2,data.message.id_from);
        else if(kind==enum_friend&&(!strcmp(data.message.str,"already success"))) pop_friend(window,3,data.message.id_from);
        else if(kind==enum_friend&&(!strcmp(data.message.str,"refuse"))) pop_friend(window,4,data.message.id_from);
        else if(kind==enum_friend) pop_friend(window,0,data.message.id_from);
        else if(kind==enum_chat&&(!strcmp(data.message.str,"new_message")))pop_message(window,0,data.message.id_from);
        g_mutex_unlock(mutex);/*解锁*/
        //g_mutex_unlock(mutex_together);
        sleep(1);
    }
}
void set_background(GtkWidget *widget,int w,int h,gchar *path)
{
    gtk_widget_set_app_paintable(widget,TRUE);
    gtk_widget_realize(widget);
    gtk_widget_queue_draw(widget);

    GdkPixbuf *src_pixbuf=gdk_pixbuf_new_from_file(path,NULL);
    GdkPixbuf *dst_pixbuf=gdk_pixbuf_scale_simple(src_pixbuf,w,h,GDK_INTERP_BILINEAR);
    GdkPixmap *pixmap=NULL;
    gdk_pixbuf_render_pixmap_and_mask(dst_pixbuf,&pixmap,NULL,128);
    gdk_window_set_back_pixmap(widget->window,pixmap,FALSE);

    g_object_unref(src_pixbuf);
    g_object_unref(dst_pixbuf);
    g_object_unref(pixmap);
}
void personal_id(char *id)
{
    hbox1=gtk_hbox_new(FALSE,30);
    gtk_widget_set_size_request(hbox1,100,150);//width,height
    gtk_box_pack_start(GTK_BOX(vbox),hbox1,FALSE,FALSE,0);
    pix=gdk_pixbuf_new_from_file("portrait.png",NULL);
    pixnew=gdk_pixbuf_scale_simple(pix,100,100,GDK_INTERP_BILINEAR);//图像对象
    image=gtk_image_new_from_pixbuf(pixnew);
    gtk_box_pack_start(GTK_BOX(hbox1),image,FALSE,FALSE,10);
    GtkWidget *vbox1_0=gtk_vbox_new(FALSE,0);
    label1=gtk_label_new(id);
    PangoFontDescription *font_desc=pango_font_description_from_string("Serif 15");//设置字体
    pango_font_description_set_size(font_desc,30*PANGO_SCALE);//设置标签字大小
    gtk_widget_modify_font(label1,font_desc);
    pango_font_description_free(font_desc);
    gtk_box_pack_start(GTK_BOX(vbox1_0),label1,FALSE,FALSE,50);
    gtk_box_pack_start(GTK_BOX(hbox1),vbox1_0,FALSE,FALSE,0);
}
void update_new(char *friend)//新建联系人
{
    GtkWidget *hbox4_0=gtk_hbox_new(FALSE,10);//每一个联系人，box内部控件间距离
    gtk_widget_set_size_request(hbox4_0,0,100);//width,height
    gtk_box_pack_start(GTK_BOX(vbox1_1),hbox4_0,FALSE,FALSE,0);
    
    //联系人头像图片
    GdkPixbuf *pix0;
    pix0=gdk_pixbuf_new_from_file("friends.png",NULL);
    GdkPixbuf *pixnew0;
    pixnew0=gdk_pixbuf_scale_simple(pix0,60,60,GDK_INTERP_BILINEAR);
    GtkWidget *image0;
    image0=gtk_image_new_from_pixbuf(pixnew0);
    gtk_box_pack_start(GTK_BOX(hbox4_0),image0,FALSE,FALSE,20);

    //联系人id
    //GtkWidget *vbox4_0=gtk_vbox_new(FALSE,0);
    GtkWidget *label4_0;

    // gtk_widget_modify_font(label4_0,font_desc);
    // pango_font_description_free(font_desc);
    // gtk_box_pack_start(GTK_BOX(vbox4_0),label4_0,FALSE,FALSE,20);
    // gtk_box_pack_start(GTK_BOX(hbox4_0),vbox4_0,FALSE,FALSE,0);

    GtkButton *button0=gtk_button_new_with_label(friend);
    g_signal_connect(G_OBJECT(button0),"clicked",G_CALLBACK(on_button_clicked_chat),friend);
    gtk_button_set_relief(button0,GTK_RELIEF_NONE);
    PangoFontDescription *font_desc=pango_font_description_from_string("Serif 15");//设置字体
    pango_font_description_set_size(font_desc,15*PANGO_SCALE);//设置标签字大小
    label4_0=gtk_bin_get_child(GTK_WIDGET(button0));
    gtk_widget_modify_font(GTK_WIDGET(label4_0),font_desc);

    gtk_container_add(GTK_CONTAINER(hbox4_0),button0);
    gtk_widget_show_all(window);

}

void main_win(char *user)
{
    mutex = g_mutex_new();
    //g_mutex_unlock(mutex);
    //mutex_together=g_mutex_new();
    g_thread_create((GThreadFunc)read_from, NULL, FALSE, NULL);
    pthread_create(&thIDw, NULL,(void *)write_heart,NULL);
    //pthread_join(thIDr,NULL);
    username=user;
    gdk_threads_init();
    gtk_init(NULL,NULL);
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window),"联系人列表");
    gtk_widget_set_size_request(window,400,950);
    set_background(window,400,950,"background.jpg");
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(destroy_logout),NULL);
    
    //总box框架&个人id栏
    vbox=gtk_vbox_new(FALSE,0);
    gtk_container_add(GTK_CONTAINER(window),vbox);
    personal_id(user);

    //搜索好友栏
    hbox2=gtk_hbox_new(FALSE,0);//box内部控件间距离
    gtk_widget_set_size_request(hbox2,300,40);
    gtk_box_pack_start(GTK_BOX(vbox),hbox2,FALSE,FALSE,0);
    search_friend=gtk_entry_new();
    gtk_entry_set_max_length(search_friend,150);
    gtk_box_pack_start(GTK_BOX(hbox2),search_friend,FALSE,FALSE,20);
    button=gtk_button_new_with_label("search");
    g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(on_button_clicked_search),NULL);
    gtk_box_pack_start(GTK_BOX(hbox2),button,FALSE,FALSE,0);
    GtkWidget *new_friend=gtk_button_new_with_label("好友申请");
    gtk_box_pack_start(GTK_BOX(hbox2),new_friend,FALSE,FALSE,50);


    //群聊接口
    hbox3=gtk_hbox_new(FALSE,0);
    gtk_widget_set_size_request(hbox3,300,50);
    gtk_box_pack_start(GTK_BOX(vbox),hbox3,FALSE,FALSE,20);
    GtkWidget *button1;
    button1=gtk_button_new_with_label("xzb广场入口");
    // gtk_button_set_relief(button1,GTK_RELIEF_NONE);
    gtk_box_pack_start(GTK_BOX(hbox3),button1,FALSE,FALSE,100);
    g_signal_connect(G_OBJECT(button1),"clicked",G_CALLBACK(on_button_clicked_chat_together),NULL);
    
    //好友列表
    vbox1=gtk_vbox_new(FALSE,0);
    gtk_widget_set_size_request(vbox1,0,0);
    gtk_box_pack_start(GTK_BOX(vbox),vbox1,TRUE,TRUE,0);
    frame=gtk_frame_new("好友列表");
    gtk_container_add(GTK_CONTAINER(vbox1),frame);
    vbox1_0=gtk_vbox_new(FALSE,0);
    gtk_widget_set_size_request(vbox1_0,0,0);
    gtk_container_add(GTK_CONTAINER(frame),vbox1_0);

    //增加滚轮
    scrolled=gtk_scrolled_window_new(NULL,NULL);
    gtk_container_set_border_width(GTK_CONTAINER(scrolled),10);
    gtk_scrolled_window_set_policy(scrolled,GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(vbox1_0),scrolled);
    vbox1_1=gtk_vbox_new(FALSE,0);
    gtk_widget_set_size_request(vbox1_1,350,2000);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled),vbox1_1);
    //
    char buff[30];
    strcat(file_name,"_");
    strcat(file_name,username);
    strcat(file_name,".txt");
    fp=fopen(file_name,"a+");
    printf("%s\n",file_name);
    while(1)
    {
        fscanf(fp,"%s",buff);
        if(feof(fp)) break;
        printf("%s\n",buff);
        update_new(buff);
    }
    fclose(fp);
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
