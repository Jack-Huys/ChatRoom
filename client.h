#include <string>
#include "common.h"

using namespace std;

// 客户端类，用来连接服务器发送和接收消息
class Client
{

public:
  // 构造函数
  Client();

  // 连接服务器
  void Connect();

  // 断开连接
  void Close();

  // 启动客户端
  void Start();

private:
  // 当前连接服务器端创建的socket
  int sock;

  // 当前进程ID
  int pid;

  // epoll_create创建后的返回值
  int ep_fd;

  // 创建管道，其中fd[0]用于父进程读，fd[1]用于子进程写
  int pipe_fd[2];

  // 表示客户端是否正常工作
  bool isClientwork;

  // 聊天信息缓冲区
  char message[MAX_SIZE];

  //用户连接的服务器 IP + port
  struct sockaddr_in server_addr;
};