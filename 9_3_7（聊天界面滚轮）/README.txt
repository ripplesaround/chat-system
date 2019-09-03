



更改步骤
1.命令行输入mysql -u root -p
        输入密码neusoft
        create database user;
        create table user(account VARCHAR(20),password VARCHAR(20),user_id CHAR(20),user_ip VARCHAR(20));
2.把db_try.h移动到server文件夹
3.修改server.c
    增加 #include "db_try.h"
    在client_register函数开始增加
        insert_into_account(user);
4.更新makefile
    用包中makefile更新原文件
4.运行客户端，新注册用户，同1开始操作进入数据库
    use user;
    select * from user;
    就可以看到user内容新增
    可以新增：用C语言在输出端打印表user
             user_ip和user_id也可以存入（目前主程序里没有初始化）