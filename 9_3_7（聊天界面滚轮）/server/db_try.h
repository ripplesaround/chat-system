#include<stdio.h>
#include<mysql.h> 
#include<string.h>
#include<stdlib.h>
#include "chat.h"


MYSQL *conn_prt;  //一个MYSQL句柄
MYSQL_RES *res;
MYSQL_ROW row;

// 插入操作
char insert_head[200] = "insert into user (account,password,user_id,user_ip) values (\'";
char insert_middle[200] = "\',\'";
char insert_comma1[5]="\',";
char insert_comma2[5]=",\'";
char insert_tail[200] = "\');";


/*初始化mysql句柄*/
void self_init() {conn_prt = mysql_init(NULL);}

//默认连接到user数据库
void self_connect()
{
		if(!mysql_real_connect(conn_prt,"localhost","root",
			"neusoft","user",0,NULL,0))
		{
			printf("failed to connect:%s\n",mysql_error(conn_prt));
			exit(0) ;
		}
		printf("connect success!\n");
}

void show_storage();

void insert_into_account(User user)
{
    //初始化
    printf("开始数据库操作\n");
	self_init();
    printf("初始化完成\n");
	self_connect();
    printf("连接数据库完成\n");

    char account1[20];
    strcpy(account1,user.account);
    printf("--%s--\n",account1);
    char password1[20];
    strcpy(password1,user.password);
    printf("--%s--\n",password1);
    char user_id3[20]="id_to_be_used";
    snprintf(user_id3, sizeof(user_id3), "%d", user.user_id); 
    printf("--%d--%s--\n",user.user_id,user_id3);
    char user_ip1[20]="Ip for user ";
    strcat(user_ip1,user_id3);
    printf("--%s--\n",user_ip1);
    //最后的命令
    char dbcommand[100];
    strcpy(dbcommand,insert_head);
    strcat(dbcommand,account1);
    strcat(dbcommand,insert_middle);
    strcat(dbcommand,password1);
    // strcat(dbcommand,insert_comma1);
    strcat(dbcommand,insert_middle);
    strcat(dbcommand,user_id3);
    strcat(dbcommand,insert_middle);
    // strcat(dbcommand,insert_comma2);
    strcat(dbcommand,user_ip1);
    strcat(dbcommand,insert_tail);
    printf("mysqlcommand:%s\n",dbcommand);
    int t = mysql_real_query(conn_prt,dbcommand,strlen(dbcommand));
    printf("--5--\n");
    if(t) printf("连接数据库失败！\n");

    //打印
    char show_accounts[30]="select * from user;";
    int tt = mysql_real_query(conn_prt,show_accounts,strlen(show_accounts));
    res=mysql_store_result(conn_prt);
    //row = mysql_fetch_row(res);
    printf("从MySQL数据库导出当前注册的所有用户信息：\n");

    while(row = mysql_fetch_row(res))
    {
        for(t = 0;t<mysql_num_fields(res);t++)
        {
            printf("%s\t",row[t]);
        }
        printf("\n");
    }

    // for (int i = 0; i < user.user_id; ++i)
    // {
    //     printf("%s\n",row[i]);
    // }
    return;
}


// void show_storage()
// {
//     char command[20]="select * from user;";
//     int t=mysql_real_query(conn_prt,dbcommand,strlen(dbcommand));
//     char res[20]=mysql_store_result(conn_prt);
//     printf("%s",res);
// }

    //打印部分
    // t = mysql_real_query(conn_prt,show_accounts,strlen(show_accounts));
    // res=mysql_store_result(conn_prt);
    // if(row==NULL)
    // {
    //     printf("暂时无人注册！!\n");
    //     return;
    // }
    // for(int i = 0;i<mysql_num_fields(res);i++)
    // {
    //     printf("%s\t",row[i]);
    // }
    // while(row = mysql_fetch_row(res))
    // {
    //     for(int i = 0;i<mysql_num_fields(res);i++)
    //     {
    //         printf("%s\t",row[i]);
    //     }
    //     printf("\n");
    // }
    // printf("\n");
    // return ;
