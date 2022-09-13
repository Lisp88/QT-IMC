QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS +=\
    -lws2_32

INCLUDEPATH +=\
    ./Net\
    ./Mediator

SOURCES += \
    DialogLogin.cpp \
    DialogMain.cpp \
    Mediator/INetMediator.cpp \
    Mediator/TcpClientMediator.cpp \
    \#Mediator/TcpServerMediator.cpp \
    \#Mediator/UdpMediator.cpp \
    Net/INet.cpp \
    Net/TcpClient.cpp \
    \#Net/TcpServer.cpp \
    \#Net/UdpNet.cpp \
    chatdialog.cpp \
    kernel.cpp \
    main.cpp \
    userchildwidget.cpp

HEADERS += \
    DialogLogin.h \
    DialogMain.h \
    Mediator/INetMediator.h \
    Mediator/TcpClientMediator.h \
    \#Mediator/TcpServerMediator.h \
    \#Mediator/UdpMediator.h \
    Net/INet.h \
    Net/Packdef.h \
    Net/TcpClient.h \
    \#Net/TcpServer.h \
    \#Net/UdpNet.h \
    chatdialog.h \
    kernel.h \
    userchildwidget.h

FORMS += \
    DialogLogin.ui \
    DialogMain.ui \
    chatdialog.ui \
    userchildwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
