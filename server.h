#include <string>

#include "common.h"

using namespace std;

class server
{
public:
  //构造函数
  server();
  //初始化server设置
  void init();
  //开启server服务
  void start();
  //关闭服务
  void Close();

private:
  int sendMessage(int client_fd);
  struct sockaddr_in server_addr;
  int listen_fd{0};
  int ep_fd{0};
  list<int> client_list;
};