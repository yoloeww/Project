#proma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/epoll.h>
#include "err.hpp"
#include "log.hpp"
#include "sock.hpp"

namespace epoll_ns {
    static const int dafaultport = 8888;
    static const int size = 128;
    static const int dafaultvalue = -1;
    static const int defalultnum = 64;
    using fun_t = std::function<std::string(const std::string&)>;
class EpollServer {
    public:
         EpollServer(func_t f,  uint16_t port = defaultport, int num = defalultnum)
            : func_(f), _num(num), _revs(nullptr), _port(port), _listensock(defaultvalue), _epfd(defaultvalue)
        {
        }
        void initServer() {
            // 1. 创建socket
            _listensock = Sock::Socket();
             Sock::Bind(_listensock,_port);
             Sock::Listen(_listensock);
            // 2.创建epoll模型
            _epfd = epoll_create(size);
            if (_epfd < 0) {
                logMessage(FATAL, "epoll create error: %s", strerror(errno));
                exit(EPOLL_CREATE_ERR);
            }
            // 3.添加listensock到epoll中！
           
            // typedef union epoll_data {
            // void *ptr; /* 指向用户自定义数据 */
            // int fd; /* 注册的文件描述符 */
            // uint32_t u32; /* 32-bit integer */
            // uint64_t u64; /* 64-bit integer */
            // } epoll_data_t;

            // struct epoll_event {
            // uint32_t events; /* 描述epoll事件 */
            // epoll_data_t data; /* 见上面的结构体 *
            struct epoll_event ev;
            ev.events = EPOLLIN; 
            ev.data.fd = _listensock;
            /*
            EPOLL_CTL_ADD ：注册新的fd到epfd中；
            EPOLL_CTL_MOD ：修改已经注册的fd的监听事件；
            EPOLL_CTL_DEL ：从epfd中删除一个fd；*/
            epoll_ctl(_epfd,EPOLL_CTL_ADD,_listensock,&ev); // 用户告诉内核你要帮我关心哪些文件描述符的那些事件！
            // 用户告诉内核，你需要帮我监听，（增加）_listensock 这个fd的 &ev(事情)

            // 4.申请就绪事件的空间
             _revs = new struct epoll_event[_num];

            logMessage(NORMAL, "init server success");
        }
        void HandlerEvent(int readyNum) {
            logMessage(DEBUG,"handlerEvent in");
            for (int i = 0; i < readyNum; i++) {
                uint32_t events = _revs[i].events;
                int sock = _revs[i].data.fd;

                if (sock == _listensock && (event & EPOLLIN)) {
                    std::string clientip;
                    uint16_t clientport;
                    int fd = Sock::Accept(sock,&clientip,&clientport);
                    if (fd < 0) {
                        logMessage(WARNING, "accept error");
                        continue;
                    }
                    // 获取fd成功，可以直接读取吗？？不可以，放入epoll
                     struct epoll_event ev;
                     ev.events = EPOLLIN;
                     ev.data.fd = fd;
                     epoll_ctl(_epfd,EPOLL_CTL_ADD,fd,&ev);
                }
                else if (events & EPOLLIN) {
                    char buffer[1024];
                    
                }
                else
                {
                }
            }
             logMessage(DEBUG, "HandlerEvent out");
        }
        void start() {
            int timeout = -1;
            for (;;) {
                int n = epoll_wait(_epfd,_revs,_num,timeout); // 收集在epoll监控的事件中已经发送的事件
                 switch (n)
                {
                case 0:
                    logMessage(NORMAL, "timeout ...");
                    break;
                case -1:
                    logMessage(WARNING, "epoll_wait failed, code: %d, errstring: %s", errno, strerror(errno));
                    break;
                default:
                    logMessage(NORMAL, "have event ready");
                    HandlerEvent(n);
                    break;
                }
            }
            }
        }
        ~EpollServer() {
            if (_listensock != dafaultvalue) close(_listensock)
            if(_epfd != defaultvalue) close(_epfd);
            if (_revs)
                delete[] _revs;
        }
    private:
        uint16_t _port;
        int _listensock;
        int _epfd;
        struct epoll_event *_revs;
        int _num;
        fun_t funt_;
    }
}
};