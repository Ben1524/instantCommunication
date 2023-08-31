//
// Created by Stephen Curry on 5/10/2023.
//

#include "TcpSql.h"
#include "command.h"

TcpSql* TcpSql::m_tcpsql = new (std::nothrow) TcpSql();

TcpSql *TcpSql::getInstance(void) {
    return m_tcpsql;
}

void TcpSql::deleteInstance() {
    if(m_tcpsql)
    {
        delete m_tcpsql;
        m_tcpsql = nullptr;
    }
}

TcpSql::TcpSql() {

    this->mysql = mysql_init(nullptr);
    if(mysql == nullptr)
    {
        std::cout<<"error mysql_init"<<std::endl;
        return;
    }
    // 实例化mysql对象
    mysql = mysql_real_connect(mysql, DATABASEHOST, DATABASEUSER, DATABASEPWD,
                               DATABASE, 3306, nullptr, 0);

    if(mysql == NULL)
    {
        printf("mysql_real_connect() error\n");
        return;
    }
}

TcpSql::~TcpSql() {
    mysql_close(mysql);
}

int TcpSql::Add(std::string user,std::string passwd) {

    std::string string_sql="INSERT INTO qq_login ( `user`, `passwd`) values(""'"+user+"'"",""'"+passwd+"'"")";
    int ret = mysql_query(this->mysql,string_sql.c_str());
    if(ret != 0)
    {
        std::cout<<"mysql_query error,"<<mysql_errno(mysql)<<std::endl;
        //回滚事务
        mysql_rollback(mysql);
        return mysql_errno(mysql);
    }
    //提交事务
    mysql_commit(mysql);
    return 1;
}

int TcpSql::Select(std::string user) {

    //判断MYSQL 1062 错误码，不用再判断账户重复
    std::string string_sql="SELECT USER FROM qq_login;";

    int ret = mysql_query(mysql, string_sql.c_str());
    if(ret != 0)
    {
        std::cout<<mysql_errno(mysql)<<std::endl;
        std::cout<<"select mysql_query erron,"<<mysql_errno(mysql)<<std::endl;

        mysql_rollback(mysql);
        return -1;
    }

    result = mysql_store_result(mysql);
    //获取列数
    //int num = mysql_num_fields(result);

    while ((row = mysql_fetch_row(result)))  //遇到最后一行，则中止循环
    {
        if(row[0] == user) //row是一个数组，row[0]是第一列，元素是char*类型
        {
            std::cout<<"username 重复"<<std::endl;
            return -1;
        }
    }
    return 1;
}

int TcpSql::Select(std::string user, std::string passwd) {

    std::string string_sql="SELECT name,password FROM user;";

    int ret = mysql_query(mysql, string_sql.c_str()); //执行sql语句,返回0表示成功
    if(ret != 0)
    {
        std::cout<<mysql_errno(mysql)<<std::endl;
        std::cout<<"select mysql_query erron"<<std::endl;
        //回滚
        mysql_rollback(mysql);
        return -1;
    }

    result = mysql_store_result(mysql); //获取结果集
    //获取列数
    //mysql_num_fields(result);

    while ((row = mysql_fetch_row(result)))  //遇到最后一行，则中止循环
    {
        if(row[0] == user&&row[1] == passwd)
        {
            std::cout<<"身份验证成功"<<std::endl;
            return 1;
        }
        if(row[0] == user&&row[1] != passwd)
        {
            std::cout<<"密码错误"<<std::endl;
            return 2;
        }
    }
    return -1;
}

int TcpSql::Change(std::string str) { // 
    return 1;
}

std::list<char *> TcpSql::selectAll(const std::string &table, const std::string &value, const std::string &condition)
{
    std::string string_sql="SELECT";
    string_sql+=value+"FROM "+table+condition+";";
    std::list<char*> list;
    int ret = mysql_query(mysql, string_sql.c_str());
    if(ret != 0)
    {
        std::cout<<mysql_errno(mysql)<<std::endl;
        std::cout<<"select mysql_query erron"<<std::endl;
        //回滚
        mysql_rollback(mysql);
        return list;
    }

    result = mysql_store_result(mysql); //获取结果集
    //获取列数
    //mysql_num_fields(result);

    while ((row = mysql_fetch_row(result)))  //遇到最后一行，则中止循环
    {
        list.push_back(row[0]);
    }
    // 释放结果集
    mysql_free_result(result);
    return list;
}

