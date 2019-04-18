#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <libmill.h>

#include "types.h"

#define TCPBUFF 2048
#define UDPBUFF 2048

const char* remote_host = "127.0.0.1";
const int remote_port  =8080;


coroutine void start_tcp_client() {
  ipaddr addr = ipremote(remote_host, remote_port, 0, -1);
  tcpsock s = tcpconnect(addr, -1);
  
  char buf[TCPBUFF]; // one frame 
  
  size_t nbytes;
  for(;;) {
    
    nbytes = tcprecvuntil(s, buf, sizeof(buf), "\n" , 1, -1);
    if(errno != 0) {
      if(errno == ENOBUFS) {
        printf("buff overflow\n");
      }
      if(errno == ECONNRESET) {
        printf("connection reset by peer!\n");
        break;
        
      }
    }
  }
}


coroutine void start_udp_client() {

  ipaddr addr = iplocal("0.0.0.0", 5555, 0);
  udpsock s = udplisten(addr);

  ipaddr outaddr = ipremote(remote_host, remote_port, 0, -1);

  udpsend(s, outaddr, "ping", 4);

  char buf[UDPBUFF];
  ipaddr inaddr;
  size_t sz;

  for(;;) {
    sz = udprecv(s, &inaddr, buf, sizeof(buf), -1);
    if(errno != 0) {
      if(errno == ENOBUFS) {
        printf("buff overflow\n");
      }
    }
  }

}



int main(int argc,char*argv[]) {
  
  GameThread*gs=NULL;
  gs = NewGameThread();
  
  go(start_tcp_client());
  go(start_udp_client());
  
  gs.Run();
  
  return 0;
}
