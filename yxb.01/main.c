#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "login.h" 
#include "chat.h"
char *str_ip;//存放ip

int main(int argc,char* argv[])
{   
    str_ip=(char*)argv[1];
    //printf("%s\n",str);
    gtk_init(&argc,&argv);//初始化
    mainpage();
    return FALSE;
}