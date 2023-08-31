//
// Created by Stephen Curry on 5/8/2023.
//


#include "TcpTool.h"
#include "../timer.h"
#include "../Json.h"
#include "command.h"
using namespace byjson;
using namespace by;

int TcpTool::read() {
    char buf[BUFFMAXSIZE];
    memset(buf,0,sizeof(buf));
    // 先读取前5个字节，获取消息长度
    int ret = readn(buf,5); // 标志位+4字节的消息长度
    if(ret==-1)
    {
        return -1;
    }
    else if(ret >0)
    {
        int len = atoi(buf+1); // 获取消息长度
        len= ntohl(len); // 转换为主机字节序

        LOG_INFO_CONSLE("发送的消息长度为{}",len)

        memset(buf+1,0,sizeof(buf)); // 第一个字节为标志位，保留
        ret = readn(buf+1,len); // 读取消息
        if(ret==-1)
        {
            LOG_WARN_CONSLE("读取消息失败")
            return -1;
        }
        else if(ret >0)
        {
            LOG_INFO_CONSLE("读取消息成功:{}",buf)
            dowork(buf); //处理消息
        }
    }

    return ret;
}
int TcpTool::sendn(const char *buffer, const size_t n, int fd){
    int remain, len, idx;
    remain = (n == 0) ? strlen(buffer) : n; // 判断发送长度正不正确
    idx = 0; // 已发送的长度
    while (remain > 0) { // 循环发送，直到发送完毕，避免消息过长，一次发送不完
        if ((len = send(fd,buffer + idx, remain,0)) <= 0) // 发送失败
            return 0;
        idx += len; // 已发送的长度
        remain -= len; // 剩余的长度
    }
    LOG_INFO_CONSLE("send {} to traget_fd {}",buffer,fd)
    return 1;
}

int TcpTool::getFd() {
    return m_fd;
}

void TcpTool::dowork(const char *buf) { //处理消息
    // 标志位+mes

    switch (buf[0]) { //判断消息类型
        case REGISTER://注册
        {
            LOG_INFO_CONSLE("USER REGISTER")
            enroll(buf+1); //处理注册消息
            break;
        }

        case LOGIN://登录
        {
            LOG_INFO_CONSLE()
            login(buf+1); //处理登录消息
            break;
        }
        case GROUP://`GROUP`+`|`+`聊天室名字`+`|`+`发送方用户名`+`|`+`mess`
            transfer(buf); //处理转发消息
            break;
        case SINGLE://`SINGLE`+`|`+`接收方姓名`+`|`+`发送方姓名`+`|`+`mes`
            singleChat(buf+2);
            break;
        default:
            break;
    }
}

void TcpTool::enroll(const char buf[BUFFMAXSIZE]) {// 处理注册消息,注册之后进行登录
    auto info = splitStr(buf,"\t"); //拆分消息,消息格式为 mes##1##UID@@info
    if(info.size()>1)
    {
        int ret=tcpsql->Add(info[0], info[1]) ;
        if (ret== 1)//成功
        {
            sendn("REGISTER SUCCESS",0,this->m_fd);
        }
        else{
            sendn("REGISTER REPEAT",0,this->m_fd);
        }
    }
}

void TcpTool::login(const char buf[BUFFMAXSIZE])
{
    auto info = splitStr(buf,"\t");
    if(info.size()>1)
    {
        int ret=tcpsql->Select(info[0], info[1]);
        if (ret==1)//成功
        {
            /*存放UID 和 fd和登录时间*/
            cache->addUser(info[0],this->m_fd,timelineTimer::getCurrentTimeStr());
            this->Uid = info[0];
            // todo:发送登录成功消息
            sendn("LOGIN SUCCESS",0,this->m_fd);
            sendNewUserForInfo(this->m_fd); //发送初始化信息
        }
        else if(ret==2)
        {
            sendn("PASSWORD ERROR",0,this->m_fd);
        }
        else
        {
            sendn("NEED REGISTER",0,this->m_fd);
        }
    }
}

// 洪泛转发消息
void TcpTool::transfer(const char buf[BUFFMAXSIZE])
{
    //std::map<std::string,user>UID_map; // 保存在线用户的fd,用于转发消息
    std::vector<int> fds=cache->getFdList();
    for(auto fd:fds)
    {
        sendn(buf,0,fd);
    }
}

//发送在线用户给客户端
void TcpTool::send_user() {
   /* JSON json;
    vector<string> hobbies;
    for(auto iter:cache->UID_map)
    {
        hobbies.push_back(iter.first);
    }
    json.addArray("hobbies", hobbies);// 添加json数组
    string users_info = "json##"+json.pack();
    for(auto iter:cache->UID_map)
    {
        sendn(users_info.data(),0,iter.second.fd); //发送给每个用户
    }*/
}

std::vector<std::string> TcpTool::splitStr(const std::string& src, const std::string& delimiter) {
    std::vector<std::string> vetStr;

    // 入参检查
    // 1.原字符串为空或等于分隔符，返回空 vector
    if (src == "" || src == delimiter) {
        return vetStr;
    }
    // 2.分隔符为空返回单个元素为原字符串的 vector
    if (delimiter == "") {
        vetStr.push_back(src);
        return vetStr;
    }

    std::string::size_type startPos = 0;
    auto index = src.find(delimiter); // 查找第一个分隔符对应的下标，返回值为 std::string::size_type 类型
    while (index != std::string::npos) { // 对应的迭代器不是末尾
        auto str = src.substr(startPos, index - startPos);
        if (str != "") {
            vetStr.push_back(str);
        }
        startPos = index + delimiter.length();
        index = src.find(delimiter, startPos);
    }
    // 取最后一个子串
    auto str = src.substr(startPos);
    if (str != "") {
        vetStr.push_back(str);
    }

    return vetStr;
}

std::string TcpTool::getUid()
{
    return this->Uid;
}

void TcpTool::sendNewUserForInfo(int fd)
{
    Json json;
    // todo :发送好友列表
    json["type"] = "newUser";
    json["onlineUserInfo"] =cache->getOnlineUser();
    json["offlineUserInfo"] = cache->getOfflineUser();
    Json arr;
    std::list<char*> list = tcpsql->selectAll("chatRoomName","name","");
    for(auto iter:list)
    {
        arr.append(*iter);
    }
    json["chatRoomNames"]=arr;
    std::string str{JSON};
    str+=json.toString();
    std::cout<<"Json\n"<<str<<std::endl;
    sendn(str.c_str(),sizeof(str),fd);
    json.clear();
    arr.clear();
}

// 单发消息
//`接收方姓名`+`|`+`发送方姓名`+`|`+`mes`
void TcpTool::singleChat(const char *buf)
{
    auto info = splitStr(buf,"|");
    int fd=cache->getUserFd(info[0]);
    //`SINGLE`+`|`+`发送方姓名`+`|`+`mes` 发送给接收方
    std::string str{SINGLE};
    str+="|";
    str+=info[1];
    str+="|";
    str+=info[2];
    sendn(str.c_str(),sizeof(str),fd);
}

int TcpTool::readn(char *buffer, const size_t n)
{
    int left = n; //剩余未读取的字节数
    int nread = 0; //已经读取的字节数
    char *buf = buffer; //缓冲区指针
    while (left > 0)
    {
        nread = recv(m_fd, buf, left,0);
        if (nread < 0)
        {
            if (errno == EINTR)
                continue;
            else
                return -1;
        }
        else if (nread == 0)
        {
            return 0;
        }
        left -= nread;
        buf += nread;
    }
    return n;
}
