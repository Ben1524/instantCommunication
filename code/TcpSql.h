//
// Created by Stephen Curry on 5/10/2023.
//

#ifndef REACTOR_TCPSQL_H
#define REACTOR_TCPSQL_H

#include "mysql/mysql.h"
#include "iostream"
#include "error.h"
#include "string"
#include "list"

//单例模式
class TcpSql {
public:
    static void deleteInstance();
    static TcpSql* getInstance(void);


public:
    int Add(std::string user,std::string passwd);

    
    

    //return -1 有重复user/sql语句失败  return 1 无重复user 用于注册
    int Select(std::string user);
    //用于登录
    int Select(std::string user,std::string passwd);
    //修改账户属性
    int Change(std::string str);

    std::list<char*> selectAll(const std::string&table,const std::string&value,const std::string&condition); //查询所有字段


private:

    // 将其拷贝构造和赋值构造成为私有函数, 禁止外部拷贝和赋值
    TcpSql(const TcpSql &signal);
    const TcpSql &operator=(const TcpSql &signal);


    TcpSql();
    ~TcpSql();

private:
    MYSQL* mysql;
    //结果集
    MYSQL_RES *result;
    //行数
    MYSQL_ROW row;
    //自身对象
    static TcpSql* m_tcpsql;
};


#endif //REACTOR_TCPSQL_H
