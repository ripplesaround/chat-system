#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "login_window.h" 
#include "chat.h"
char *str_ip;//存放ip

int main(int argc,char* argv[])
{   
    str_ip=(char*)argv[1];
    loginpage(argc,argv);
    return FALSE;
}