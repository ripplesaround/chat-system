#include <gtk/gtk.h>	// 头文件
#include <string.h>
#include <stdio.h>
static GtkWidget* entry1;
GtkWidget* window;
GtkWidget* box;
GtkWidget* box1;
GtkWidget* box2;
GtkWidget* label1;
GtkWidget* button;
GtkWidget* sep;
void search(char *username)
{
    printf("%s\n",username);
}
void on_button_clicked (GtkWidget* button,gpointer data)
{
    if((int)data==0)
    {
        gchar* searchid = gtk_entry_get_text(GTK_ENTRY(entry1));
        search((char*)searchid);
    }
}
int main(int argc,char* argv[])
{
    gtk_init(&argc,&argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window),"delete_event",G_CALLBACK(gtk_main_quit),NULL);
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

    label1 = gtk_label_new("fwxnb");
    gtk_box_pack_start(GTK_BOX(box1),label1,FALSE,FALSE,60);

    sep = gtk_hseparator_new();//分割线
    gtk_box_pack_start(GTK_BOX(box),sep,FALSE,FALSE,7);

    entry1 = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box2),entry1,TRUE,TRUE,20);
    button = gtk_button_new_with_label("  搜索  ");
    g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(on_button_clicked),(gpointer)0);
    //g_signal_connect_swapped(G_OBJECT(button),"clicked",G_CALLBACK(gtk_widget_destroy),window);
    gtk_box_pack_start(GTK_BOX(box2),button,FALSE,FALSE,10);
    gtk_widget_show(button);

    gtk_widget_show_all(window);
    gtk_main();
}