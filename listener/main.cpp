#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

//программа слушатель (программа №2)
int main() {
  int sock, listener;
  struct sockaddr_in addr;
  char buf[1024];
  int bytes_read;

  listener = socket(AF_INET, SOCK_STREAM, 0);
  if (listener < 0) {
    std::cout << "socket error\n";
    exit(1);
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(3425);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    std::cout << "binding error\n";
    exit(2);
  }

  listen(listener, 1);

  while (1) {
    sock = accept(listener, NULL, NULL);
    if (sock < 0) {
      std::cout << "acceptance error\n";
      exit(3);
    }

    while (1) {
      bytes_read = recv(sock, buf, 1024, 0);
      if (bytes_read <= 0) break;
    }

    int x = atoi(buf);
    if (x > 9 && x % 32 == 0) {
      std::cout << "Данные успешно получены.\n";
    } else {
      std::cout << "Ошибка при получении данных.\n";
    }
    close(sock);
  }

  return 0;
}
