#include <iostream>
#include <list>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SERVER_PORT 8080
#define SERVER_IP "10.10.0.5"

#define MAX_SIZE 1024
#define EPOLL_SIZE 5000

//注册新的fd到epollfd中
//参数enable_et是否启动边沿触发模式

static void addfd(int epollfd, int fd, bool enable_et)
{
  struct epoll_event event;
  event.data.fd = fd;
  event.events = EPOLLIN;
  if (enable_et)
  {
    event.events = EPOLLIN | EPOLLET;
  }
  epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
  // 函数声明：int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
  // 功能：用于控制某个文件描述符上的事件，可以注册事件，修改事件，删除事件。
  // @epfd：由epoll_create生成的epoll专用的文件描述符；
  // @op：要进行的操作，EPOLL_CTL_ADD注册、EPOLL_CTL_MOD修改、EPOLL_CTL_DEL删除；
  // @fd：关联的文件描述符；
  // @event：指向epoll_event的指针；
  // 成功：0；失败：-1
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
  //设置socket为非阻塞
  printf("fd added to epoll success\n");
}