cmake_minimum_required(VERSION 3.22)
project(IMC)

set(CMAKE_CXX_STANDARD 14)
include_directories("C:\\Program Files\\MySQL\\MySQL Server 5.7\\include")
link_libraries(ws2_32)
link_directories("lib")
add_executable(IMC main.cpp
        Mediator/INetMediator.cpp Mediator/INetMediator.h Mediator/UdpMediator.cpp Mediator/UdpMediator.h Mediator/TcpServerMediator.cpp Mediator/TcpServerMediator.h
        Net/Packdef.h Net/UdpNet.cpp Net/UdpNet.h Net/INet.cpp Net/INet.h Net/TcpServer.cpp Net/TcpServer.h
        Mysql/CMySql.cpp Mysql/CMySql.h Kernel.cpp Kernel.h)
target_link_libraries(IMC libmysql)