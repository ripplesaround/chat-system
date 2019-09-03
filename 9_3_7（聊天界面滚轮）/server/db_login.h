#include<stdio.h>
#include<mysql.h> 
#include<string.h>
#include<stdlib.h>
#include "chat.h"
#include "db_try.h"

// MYSQL *conn_prter;  //一个MYSQL句柄
MYSQL_RES *reser;
MYSQL_ROW rower;

// 插入操作
char insert_header[200] = "select password from user where account =";
char insert_tailer[200] = ";";



char res_for_login[1000];
int check_your_password(User user)
{
    printf("我又来的debug了!!!\n");
    self_init();
    self_connect();
    char your_account[100];
    char your_password[100];
    strcpy(your_account,user.account);
    strcpy(your_password,user.password);
    //进入数据库比较
    char db_operation[100];
    strcpy(db_operation,insert_header);
    strcat(db_operation,your_account);
    strcat(db_operation,insert_tailer);
    printf("1-我又来的debug了!!!\n%s\n",db_operation);
    int t = mysql_real_query(conn_prt,db_operation,strlen(db_operation));
    printf("2-我又来的debug了!!!\n");
    if(t)
    {
        printf("查询失败！\n");
        return 0;
    }
    printf("3-我又来的debug了!!!\n");
    char correct_password[100];
    reser=mysql_store_result(conn_prt);
    printf("res is ok!\n");
    rower = mysql_fetch_row(reser);
    if(rower==NULL)
    {
        printf("空的!\n");
        return 0;
    }
    printf("4-我又来的debug了!!!\n");
    strcpy(correct_password,rower[0]);
    printf("5-我又来的debug了!!!\n");
    if(!strcmp(your_password,correct_password)) {printf("校验成功啦！！！！\n");return 1;}
    else return 0;
}
