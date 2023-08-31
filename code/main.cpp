#include "TcpServer.h"

int main(int argc ,char ** argv)
{
    int port = 12345;
    if(argc >= 2){
        port = atoi(argv[1]);
    }

    std::cout<<"服务器初始化"<<std::endl;
    TcpServer serv(port);
    serv.run();

    return 0;
}
