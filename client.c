#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>

#define MILL_USE_PREFIX

#include <libmill.h>

#include "types.h"
#include "gamethread.h"

#include "ikcp.h"

#include "al_common/alhelpers.h"

char *remote_host;
const int remote_port = 8081;

mill_coroutine int kcp_output(const char *buf, int len, ikcpcb *kcp, void *user);

mill_coroutine void start_tcp_client(GameThread *gs, mill_chan input) {
    char buf[TCPBUFF]; // one frame

    char init_cmd[32];
    memset(init_cmd, 0, 32);

    mill_ipaddr addr = mill_ipremote(remote_host, remote_port, 0, -1);
    mill_tcpsock s = mill_tcpconnect(addr, -1);

    if (s == NULL) {
        printf("tcp connect failed,%s\n", strerror(errno));
        exit(-1);
    }

    mill_chs(input,
    int, 1);
    gs->tcpsock = s;

    sprintf(init_cmd, "(getres %d)\n", gs->TheUser->ID);

    mill_tcpsend(s, init_cmd, strlen(init_cmd), -1);
    if (errno != 0) { perror("tcpsend"); }
    mill_tcpflush(s, -1);
    if (errno != 0) { perror("tcpflush"); }

    printf("send %s\n", init_cmd);

    size_t nbytes;
    for (;;) {

        nbytes = mill_tcprecvuntil(s, buf, sizeof(buf), "\n", 1, -1);
        if (errno != 0) {
            if (errno == ENOBUFS) {
                printf("buff overflow\n");
            }
            if (errno == ECONNRESET) {
                printf("connection reset by peer!\n");
                break;

            }
        } else {
            buf[nbytes - 1] = '\0';
            //printf("tcp client received: %s $$\n",buf);

            GameThread_ProcessLispCmds(gs, buf);

        }


    }
}


mill_coroutine void start_udp_client(GameThread *gs, mill_chan input) {

    mill_ipaddr addr = mill_iplocal("0.0.0.0", 5555, 0);
    mill_udpsock s = mill_udplisten(addr);

    mill_ipaddr outaddr = mill_ipremote(remote_host, remote_port, 0, -1);

    mill_udpsend(s, outaddr, "ping", 4);

    char buf[UDPBUFF];
    mill_ipaddr inaddr;
    size_t sz;

    gs->udpsock = s;
    gs->outaddr = outaddr;

    mill_chs(input,
    int, 1);

    for (;;) {
        sz = mill_udprecv(s, &inaddr, buf, sizeof(buf), -1);
        if (errno != 0) {
            if (errno == ENOBUFS) {
                printf("buff overflow\n");
            }
        } else {
            buf[sz - 1] = '\0';
            if (gs->state == STATE_DRAW) {
                //GameThread_ProcessLispCmds(gs,buf);
                GameThread_ProcessIRCPackageUDP(gs, buf);
            } else {
                //memset(buf,0,sz);
            }
        }
    }

}

mill_coroutine void start_kcp_client(GameThread *gs, mill_chan input) {
    int port;
    port = 5555;

    mill_ipaddr addr = mill_iplocal("0.0.0.0", port, 0);
    mill_udpsock s = mill_udplisten(addr);
    if (s == NULL) {
        port += 1;
        addr = mill_iplocal("0.0.0.0", port, 0);
        s = mill_udplisten(addr);
        if (s == NULL) //still? no !
        {
            perror("udplisten");
            exit(-1);
        }
    }

    mill_ipaddr outaddr = mill_ipremote(remote_host, remote_port, 0, -1);

    mill_udpsend(s, outaddr, "(ping)", 4);

    char buf[UDPBUFF];
    char buf2[UDPBUFF];
    int hr;
    int conv;

    mill_ipaddr inaddr;
    size_t sz;

    gs->udpsock = s;
    gs->outaddr = outaddr;

    gs->kcp1 = ikcp_create(gs->TheUser->ID, (void *) gs);
    gs->kcp1->output = kcp_output;
    ikcp_wndsize(gs->kcp1, 256, 256);
    ikcp_nodelay(gs->kcp1, 1, 10, 2, 1);

    gs->kcp1->rx_minrto = 10;
    gs->kcp1->fastresend = 1;


    mill_chs(input,
    int, 1);

    for (;;) {
        sz = mill_udprecv(s, &inaddr, buf, sizeof(buf), -1);
        if (errno != 0) {
            if (errno == ENOBUFS) {
                printf("buff overflow\n");
            }
        } else {
            conv = ikcp_getconv(buf);
            if (conv == gs->TheUser->ID && gs->kcp1 != NULL) {
                ikcp_input(gs->kcp1, buf, sz);
                ikcp_update(gs->kcp1, iclock());

                while (1) {
                    hr = ikcp_recv(gs->kcp1, buf2, UDPBUFF);
                    if (hr > 0) {
                        buf2[hr] = '\0';
                        if (gs->state == STATE_DRAW) {
                            //printf("%s\n",buf2);
                            GameThread_ProcessLispCmds(gs, buf2);
                        } else {
                            //memset(buf2,0,hr);
                        }
                    } else if (hr < 0) {
                        break;
                    }
                }
            }
        }
    }

}

mill_coroutine int kcp_output(const char *buf, int len, ikcpcb *kcp, void *user) {
    GameThread *gs = (GameThread *) user;

    mill_udpsend(gs->udpsock, gs->outaddr, buf, len);

}

int main(int argc, char *argv[]) {

    int opt;
    GameThread *gs = NULL;
    gs = NewGameThread();

    mill_chan
    ch = mill_chmake(
    int, 0);
    mill_chan
    ch2 = mill_chmake(
    int,0);

    int i, j;
    i = 0;
    j = 0;
    remote_host = "127.0.0.1";

    while ((opt = getopt(argc, argv, "h:i:n:p:e:")) != -1) // i==ID,n==Nick, p==Password,e==email
    {
        switch (opt) {
            case 'h':
                remote_host = optarg;
                break;
            case 'i':
                GameThread_User_GetID(gs, optarg);
                break;
            case '?':
                printf("unknown option: %c\n", optopt);
                break;
        }
    }

    if (gs->TheUser->ID == 0) {
        printf("User ID error \n");
        exit(-1);
    }

    mill_go(start_tcp_client(gs, ch));
    mill_go(start_kcp_client(gs, ch2));

    for (;;) {
        mill_choose
        {
            mill_in(ch,
            int, val):
            i = val;
            mill_in(ch2,
            int,val):
            j = val;
            mill_end
        }
        if (i > 0 && j > 0) {
            mill_chclose(ch);
            mill_chclose(ch2);
            break;
        }
    }


    if(InitAL() != 0) return 1;

    GameThread_Run(gs);

    CloseAL();
    return 0;
}
