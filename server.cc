#include <iostream>

#include "server.h"

using namespace std;

server::server()
{
  //初始化server地址和端口
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERVER_PORT);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
}

void server::init()
{
  cout << "init server******************************\n";

  //获取套接字
  listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd < 0)
  {
    perror("listen_fd error");
    exit(-1);
  }
  // 配置listen_fd的TIME_WAIT时可复用
  int on = 1;
  int res = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
  if (res == -1)
  {
    perror("set sock reuse addr:");
    exit(-1);
  }
  //bind
  if (bind(listen_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    perror("bind fail error");
    exit(-1);
  }
  //监听
  int ret = listen(listen_fd, 10);
  if (ret < 0)
  {
    perror("listen error");
    exit(-1);
  }
  cout << "start litening............." << endl;

  //在内核中创建事件表
  ep_fd = epoll_create(EPOLL_SIZE);
  if (ep_fd < 0)
  {
    perror("ep_fd error");
    exit(-1);
  }
  //往事件表里添加监听事件
  addfd(ep_fd, listen_fd, true);
}

//close
void server::Close()
{
  //关闭socket
  close(listen_fd);
  //关闭epoll监听
  close(ep_fd);
}

//发送广播消息给所有客户端
int server::sendMessage(int client_fd)
{
  //buf接受新消息
  //message保存格式化消息
  char buf[MAX_SIZE], message[MAX_SIZE];
  memset(buf, 0, sizeof(buf));
  memset(message, 0, sizeof(message));
  //接受新消息
  int len = recv(client_fd, buf, sizeof(buf), 0);
  cout << "<client" << client_fd << "> :" << buf << endl;
  if (len == 0)
  {
    close(client_fd);
    client_list.remove(client_fd);
    cout << "用户 " << client_fd << " 已退出聊天室" << endl;
    cout << "聊天室现在总共有" << client_list.size() << "位用户" << endl;
  }
  else
  {
    if (client_list.size() == 1)
    {
      send(client_fd, "only you....", 12, 0);
      return len;
    }
    //格式化发送的消息内容到message
    sprintf(message, "<Client %d> say >>>>  %s", client_fd, buf);
    list<int>::iterator iter; //迭代器遍历list
    for (iter = client_list.begin(); iter != client_list.end(); iter++)
    {
      if (*iter != client_fd)
      {
        send(*iter, message, sizeof(message), 0);
      }
    }
  }
  return len;
}
//开启服务
void server::start()
{
  //epoll 事件队列
  static struct epoll_event events[EPOLL_SIZE];
  init();
  while (1)
  {
    //epoll_events_count表示就绪事件的数目
    int epoll_events_count = epoll_wait(ep_fd, events, sizeof(events), -1);
    // 函数声明:int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout)
    // 功能：该函数用于轮询I/O事件的发生；
    // @epfd：由epoll_create生成的epoll专用的文件描述符；
    // @epoll_event：用于回传代处理事件的数组；
    // @maxevents：每次能处理的事件数；
    // @timeout：等待I/O事件发生的超时值；
    // 成功：返回发生的事件数；失败：-1
    if (epoll_events_count < 0)
    {
      perror("epoll fail");
      break;
    }
    cout << "epoll_events_count = " << epoll_events_count << endl;

    //处理这些就绪事件
    for (int i = 0; i < epoll_events_count; i++)
    {
      int sock_fd = events[i].data.fd;
      if (sock_fd == listen_fd)
      {
        struct sockaddr_in client_addr;
        // (struct sockaddr *)&client_addr, (socklen_t *)sizeof(client_addr)
        int client_fd = accept(sock_fd, NULL, NULL);
        if (client_fd < 0)
        {
          perror("accept fail");
          exit(-1);
        }
        cout << "client connection from: "
             << inet_ntoa(client_addr.sin_addr) << ":"
             << ntohs(client_addr.sin_port) << ", client_fd = "
             << client_fd << endl;
        addfd(ep_fd, client_fd, true);
        //用标准库模板list储存连接的fd
        client_list.push_back(client_fd);
        cout << "聊天室里新增用户" << client_fd << endl;
        cout << "现在总共有： " << client_list.size() << "位用户在聊天室" << endl;

        char buf[MAX_SIZE];
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "欢迎来到在线聊天室! 你的ID是: <Client %d>", client_fd);
        int ret = send(client_fd, buf, sizeof(buf), 0);
        if (ret < 0)
        {
          perror("send error");
          Close();
          exit(-1);
        }
      }
      else
      {
        int ret = sendMessage(sock_fd);
        if (ret < 0)
        {
          perror("error");
          Close();
          exit(-1);
        }
      }
    }
  }
  Close();
}