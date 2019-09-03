#include <gtk/gtk.h>

void sendtouser(GtkButton  *button, gpointer entry);

//保存消息记录
void savetxt(GtkButton  *button, gpointer entry);

void readtxt(GtkButton  *button, gpointer entry);

void *strdeal(void *arg);

void chatting_win(char *friend_name);


