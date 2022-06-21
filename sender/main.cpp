#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <limits>
#include <string>

#include "sender.h"

//функция для перевода int в string
std::string my_to_string(int x) {
  std::string str = "";
  for (str = ""; x; x /= 10) {
    str += (char)('0' + x % 10);
  }
  reverse(str.begin(), str.end());
  return str;
}

//функция для сравнения по убыванию
bool f(int a, int b) { return (a > b); }

//инициализация условной переменной и мьютекса
void sender::init() {
  pthread_cond_init(&con, NULL);
  pthread_mutex_init(&mut, NULL);
}
//функция для отправки данных программе №2
void sender::socketP(std::string b) {
  const char* message;
  message = b.c_str();
  int sock;
  struct sockaddr_in addr;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    std::cout << "socket error\n";
    exit(1);
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(3425);
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  while (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    std::cout
        << "Не удалось подключиться к программе 2. Повторное подключение.\n";
    sleep(2);
  }
  send(sock, message, sizeof(message), 0);

  close(sock);
}
//поток 1 получает данные и передает их в поток 2
void* sender::thread1(void) {
  const int masSize = 64;
  const int strSize = 64 + 1;
  while (true) {
    char x[strSize];
    std::cout << "Thread is waiting for data input.\n";
    std::cin.getline(x, strSize);

    std::string arr = std::string(x);
    bool p = (arr.find_first_not_of("0123456789") == std::string::npos);
    int sizeArr = arr.size();

    if (std::cin.good() && p) {
      int mas[masSize];

      for (int i = 0; i < sizeArr; i++) {
        mas[i] = (arr[i] - 48);
      }

      std::sort(mas, mas + sizeArr, f);

      //замена четных цифр на буквы KB
      std::string str;
      for (int i = 0; i < sizeArr; i++) {
        if (mas[i] % 2 == 0) {
          str = str + 'K' + 'B';
        } else {
          str += char(mas[i]);
        }
      }

      //использование общих данных одним потоком
      pthread_mutex_lock(&mut);
      buffer = str;
      pthread_mutex_unlock(&mut);
      pthread_cond_signal(&con);
    } else {
      std::cout << "Bad data.\n";
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      // cin.ignore(cin.rdbuf()->in_avail(), '\n'); // очистить текущую строку
      // ввода
    }
  }
}
//поток 2 обрабатывает данные и передает их слушателю
void* sender::thread2(void) {
  while (true) {
    int x = 0;
    std::string b;

    pthread_cond_wait(&con, &mut);

    b = buffer;
    buffer.clear();
    pthread_mutex_unlock(&mut);

    std::cout << "Buffer = " << b << std::endl;
    for (int i = 0; i < b.length(); i++) {
      if (b[i] != 'K' && b[i] != 'B') {
        x = x + (int)b[i];
      }
    }
    std::string s2 = my_to_string(x);
    socketP(s2);
  }
}

int main() {
  sender p;
  p.init();

  pthread_t t1;
  pthread_t t2;

  pthread_create(&t1, NULL, &sender::t1, &p);
  pthread_create(&t2, NULL, &sender::t2, &p);

  pthread_join(t2, NULL);
  pthread_join(t1, NULL);

  return 0;
}
