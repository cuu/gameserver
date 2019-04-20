#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>

#define MILL_USE_PREFIX
#include <libmill.h>

#include "types.h"
#include "gamethread.h"

#define TCPBUFF 2048
#define UDPBUFF 2048

const char* remote_host = "127.0.0.1";
const int remote_port  =8081;


mill_coroutine void start_tcp_client(GameThread*gs) {
  char buf[TCPBUFF]; // one frame 

  mill_ipaddr addr = mill_ipremote(remote_host, remote_port, 0, -1);
  mill_tcpsock s = mill_tcpconnect(addr, -1);

  if(s == NULL) {
    printf("tcp connect failed,%s\n",strerror(errno));
    exit(-1);
  }
  
  size_t nbytes;
  for(;;) {
    
    nbytes = mill_tcprecvuntil(s, buf, sizeof(buf), "\n" , 1, -1);
    if(errno != 0) {
      if(errno == ENOBUFS) {
        printf("buff overflow\n");
      }
      if(errno == ECONNRESET) {
        printf("connection reset by peer!\n");
        break;
        
      }
    }else {
      buf[nbytes]='\0';
      printf("tcp client received: %s $$\n",buf);
      
    }
    
    
  }
}


mill_coroutine void start_udp_client(GameThread*gs) {

  mill_ipaddr addr = mill_iplocal("0.0.0.0", 5555, 0);
  mill_udpsock s = mill_udplisten(addr);

  mill_ipaddr outaddr = mill_ipremote(remote_host, remote_port, 0, -1);

  mill_udpsend(s, outaddr, "ping", 4);

  char buf[UDPBUFF];
  mill_ipaddr inaddr;
  size_t sz;

  for(;;) {
    sz = mill_udprecv(s, &inaddr, buf, sizeof(buf), -1);
    if(errno != 0) {
      if(errno == ENOBUFS) {
        printf("buff overflow\n");
      }
    }else {
        //printf("udp received: %s\n",buf);
        
    }
  }

}



int main(int argc,char*argv[]) {
  
  GameThread*gs=NULL;
  gs = NewGameThread();
  
  mill_go(start_tcp_client(gs));
  mill_go(start_udp_client(gs));
  
  GameThread_Run(gs);
  return 0;
}
