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

char* remote_host;
const int remote_port  = 8081;


mill_coroutine void start_tcp_client(GameThread*gs,mill_chan input) {
  char buf[TCPBUFF]; // one frame 

  mill_ipaddr addr = mill_ipremote(remote_host, remote_port, 0, -1);
  mill_tcpsock s = mill_tcpconnect(addr, -1);

  if(s == NULL) {
    printf("tcp connect failed,%s\n",strerror(errno));
    exit(-1);
  }

  mill_chs(input, int, 1);

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
      buf[nbytes-1]='\0';
      //printf("tcp client received: %s $$\n",buf);
      
      GameThread_ProcessLispCmds(gs,buf);
      
    }
    
    
  }
}


mill_coroutine void start_udp_client(GameThread*gs,mill_chan input) {

  mill_ipaddr addr = mill_iplocal("0.0.0.0", 5555, 0);
  mill_udpsock s = mill_udplisten(addr);

  mill_ipaddr outaddr = mill_ipremote(remote_host, remote_port, 0, -1);

  mill_udpsend(s, outaddr, "ping", 4);
  
  char buf[UDPBUFF];
  mill_ipaddr inaddr;
  size_t sz;
  
  gs->udpsock = s;
  gs->outaddr = outaddr;

  mill_chs(input, int, 1);

  for(;;) {
    sz = mill_udprecv(s, &inaddr, buf, sizeof(buf), -1);
    if(errno != 0) {
      if(errno == ENOBUFS) {
        printf("buff overflow\n");
      }
    }else {
      buf[sz-1]='\0';
      if(gs->state == STATE_DRAW) {
        //GameThread_ProcessLispCmds(gs,buf);
        GameThread_ProcessLispPackage(gs,buf);
      }else {
        //memset(buf,0,sz);
      }
    }
  }

}



int main(int argc,char*argv[]) {
  int opt;
  GameThread*gs=NULL;
  gs = NewGameThread();

  mill_chan ch = mill_chmake(int, 0);
  mill_chan ch2 = mill_chmake(int,0);

  int i,j;
  i = 0;
  j = 0;
  remote_host="127.0.0.1";

  while((opt = getopt(argc, argv, "h:")) != -1)
  {
    switch(opt)  
    {
      case 'h':  
        remote_host =  optarg;
      break;  
      case '?':  
        printf("unknown option: %c\n", optopt); 
        break;  
    }  
  }
  
  
  mill_go(start_tcp_client(gs,ch));
  mill_go(start_udp_client(gs,ch2));

  for(;;) {
    mill_choose {
      mill_in(ch, int, val):
        i = val;
      mill_in(ch2,int,val):
        j = val;
    mill_end
    }
    if (i > 0 && j > 0) {
      mill_chclose(ch);
      mill_chclose(ch2);
      break;
    }
  }

  GameThread_Run(gs);
  return 0;
}
