#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <fcntl.h>
#include <map>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#define MAX_EVENTS 64


void make_non_blocking(int fd);
int main()
{
  int kq = kqueue();
  int socketFd = socket(AF_INET, SOCK_STREAM, 0);

  if (socketFd == -1){
    std::cout << "Error creating socket." << std::endl;
    return 1;
  }
  int flags = fcntl(socketFd, F_GETFL, 0);          // Lire les flags existants
  fcntl(socketFd, F_SETFL, flags | O_NONBLOCK);
  int optval = 1;
  if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
    perror("setsockopt SO_REUSEADDR");
    close(socketFd);
    exit(EXIT_FAILURE);
  }
  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverAddr.sin_port = htons(8080);
  if (bind(socketFd, (struct sockaddr *) &serverAddr, sizeof(struct sockaddr_in)) == -1){
    std::cout << "Error binding socket." << std::endl;
    return 2;
  }
  if (listen(socketFd, 5) == -1){
    std::cout << "Error listen socket." << std::endl;
    return 3;
  }
  struct sockaddr_in clientAddr;
  socklen_t clientAddrLen = sizeof(clientAddr);
  struct kevent change;
  EV_SET(&change, socketFd, EVFILT_READ, EV_ADD, 0, 0, NULL);
  if (kevent(kq, &change, 1, NULL, 0, NULL) == -1)
      return 4;
  struct kevent events[MAX_EVENTS];
  while (1) {
    int nb_result = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
    std::cout << nb_result << std::endl;
    if (nb_result == -1){
        std::cout << "Error kevent 1" << std::endl;
        break;
    }
    for (int i = 0; i < nb_result; i++) {
        int fd = events[i].ident;
        if (fd == socketFd){
            while (1){
                int client_fd = accept(socketFd, (struct sockaddr *) &clientAddr, &clientAddrLen);
                if (client_fd == -1){
                    break;
                }
                make_non_blocking(socketFd);
                EV_SET(&change, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                if (kevent(kq, &change, 1, NULL, 0, NULL) == -1) {
                    perror("kevent client");
                    close(client_fd);
                } else {
                    printf("Client connecté: fd=%d\n", client_fd);
                }
            }
        }
        else {
            char buf[1000];
            ssize_t n = read(events[i].ident, buf, 999);
            if (n <= 0) {
                // Si 0 ou erreur : le client a fermé la connexion
                printf("Client déconnecté: fd=%ld\n", events[i].ident);
                close(events[i].ident);
            }
            else {
                std::cout << buf << "requete du client "<<  std::endl;
                char response[] = "nouvelle reponse !\n";
                write(events[i].ident, response, strlen(response));
                close(events[i].ident);
            }
        }
    }
  }
  close(socketFd);
  close(kq);
  return 0;
}


void make_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl(F_GETFL)");
        return;
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl(F_SETFL)");
    }
}