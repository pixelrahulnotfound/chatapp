#include<iostream>
#include "protocol.h"
#include<string>
#include<unordered_map>
#include<unordered_set>
#include<thread>
#include<mutex> 
#include<unistd.h>
#include "socket..h"
#include "banner.h"
constexpr int PORT = 5555;
constexpr size_t MAX_INBUF = 64 * 1024;
constexpr size_t OUTBUF_SIZE = 8192;

struct Client{
socket_t socket;  
std::string username;
std:: string inbuff;
char outbuff[OUTBUF_SIZE];
size_t outlen=0;
bool joined=false;

}  
;
std:: unordered_map<socket_t,Client> clients;
std:: unordered_set<std::string> usernames;

void set_unblocking(socket_t sock){
    #ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);
    #else

 int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    #endif
}
bool queue_send(Client& client,const char* data, size_t len){

    if(client.outlen+len>OUTBUF_SIZE) return false;

    memccpy(client.outbuff+client.outlen,data,len);
    client.outlen+=len;
    return true;
}

bool flush_send(Client & client){
if(client.outlen==0) return true;
 int n = send(client.socket, client.outbuff, (int)client.outlen, 0);
if(n>0){
    size_t remaining=client.outlen-n;


    for(size_t i=0; i<remaining;++i){
        client.outbuff[i]=client.outbuff[i+n];
    }
    client.outlen=remaining;

}
#ifdef _WIN32
    if (WSAGetLastError() == WSAEWOULDBLOCK)
        return true;
#else
    if (errno == EAGAIN || errno == EWOULDBLOCK)
        return true;
#endif
    return false;
}

void send_line(Client& c,const std::string msg){

  queue_send(c,msg.c_str(),msg.size());
  queue_send(c,"\n",1);


}

void broadcast(const std::string& msg) { for (auto& [socket, c] : clients) send_line(c, msg); }
bool recv_data(Client& client){

    char buff[4096];
    while(true){
    int n=recv(client.socket,buff,sizeof(buff),0);
    if(n>0){
        client.buffer.append(buff,n);
        if(client.buffer.size()>1024*64)
            return false;
            continue;

    }
    if(n==0) return false;

    #ifdef _WIN32
     if (WSAGetLastError()==WSAEWOULDBLOCK) return true;
    #else
    if(errno==EAGAIN || errno==WSAEWOULDBLOCK) return true;



    #endif
    return false;
    
}
}
