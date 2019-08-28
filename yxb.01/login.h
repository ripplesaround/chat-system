#include "chat.h"
#include <gtk/gtk.h>
int init_client(int port,char *addr);

int get_user_info(char *username,char *password,Kind kind,char  *c_ipAddr);

char* sha(unsigned char input[64]);

void button_click(GtkWindow *parent);

void on_button_clicked (GtkWidget* button,gpointer data);

void mainpage();

void registpage();
