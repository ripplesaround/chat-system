#include "chat.h"
#include <gtk/gtk.h>
int init_client(int port,char *addr);

int registandlogin(char *username,char *password,Kind kind,char  *c_ipAddr);

char* aes_password(char *password);

void button_click(GtkWindow *parent);

void on_button_clicked (GtkWidget* button,gpointer data);

void loginpage(int argc,char* argv[]);

void registpage();
