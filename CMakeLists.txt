cmake_minimum_required(VERSION 3.16)
project(Reactor)

set(CMAKE_CXX_STANDARD 17)



#链接sql文件
set(INC_DIR /usr/include/mysql/)
set(LINK_DIR /usr/lib/x86_64-linux-gnu/)
include_directories(${INC_DIR})
include_directories(/home/cxk/spdlog/include)#导入日志库头文件
link_directories(${LINK_DIR})
link_libraries(mysqlclient)

add_executable(Reactor code/TcpTool.cpp code/TcpTool.h code/Epoller.cpp code/TcpTool.cpp
        code/main.cpp code/TcpServer.cpp code/TcpServer.h code/TcpSql.cpp code/TcpSql.h
        code/Cache.cpp code/Cache.h code/Log.cpp code/Log.h code/Log.h code/ThreadPool.cpp
        code/command.h timer.cpp timer.h Json.cpp Json.h)


target_link_libraries(Reactor
        mysqlclient
        pthread
        /home/cxk/spdlog/build/libspdlog.a
        )

set(CMAKE_BUILD_TYPE Debug)
