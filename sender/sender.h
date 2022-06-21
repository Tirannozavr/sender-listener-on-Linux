#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

class sender {
 private:
  pthread_mutex_t mut;
  std::string buffer;  //общий буфер
  pthread_cond_t con;

 public:
  ~sender() {
    pthread_cond_destroy(&con);
    pthread_mutex_destroy(&mut);
  }
  void init();
  void socketP(std::string);
  void* thread1(void);

  void* thread2(void);
  static void* t1(void* context) { return ((sender*)context)->thread1(); }
  static void* t2(void* context) { return ((sender*)context)->thread2(); }
};
