#include "Kernel.h"


int main(){
    Kernel *p_kernel = new Kernel;
    if(p_kernel->start_server())
        while(1){
            Sleep(100);
        }
    else{
        cout<<"start server fail..."<<endl;
    }
    p_kernel->close_server();
    delete p_kernel;
//    INetMediator* p_server = new TcpServerMediator;
//    p_server->OpenNet();
//    while(1) Sleep(100);
    return 0;
}