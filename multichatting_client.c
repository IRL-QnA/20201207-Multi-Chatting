#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>

#include "includes/message.h"

#define myport 5000

char *CHANGE_NAME = "ChangeName";
char *EXIT = "Exit";
char *HELP = "Help";

int Socket(int family, int type, int protocol);
void Connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);

int main(int argc, char *argv[])
{ // argc: \C0?\C2\C7\D2\C0\CE\C0\DA\C0\C7 \B0\B9\BC\F6, argv \C0\CE\C0?\A6\B4\E3\B4\C2 \B9��
  struct sockaddr_in connect_addr;
  int connect_fd;
  int buffersize;

  char room_number;
  char user_name[MESSAGE_FROM_MAX_LENGTH] = {0};
  char buffer[MESSAGE_CONTENT_MAX_LENGTH] = {0};
  MessageNode message;

  fd_set readfd;

  printf("****************************************************\n");
  printf("************Welcome to Chatting Browser*************\n");
  printf("****************************************************\n");

  printf(" Enter User Name=> ");
  fgets(user_name, MESSAGE_CONTENT_MAX_LENGTH, stdin);
  while (1)
  {
    /*\C1\A2\BC?\E6\B9\F8? \C0?\C2*/
    printf(" Enter Room Number(ex=> 1or 2 or 3)=>");
    room_number = fgetc(stdin);
    fgetc(stdin);

    if (room_number != '1' && room_number != '2' && room_number != '3')
    {
      printf(" Incorrect Room Number\n");
      continue;
    }
    else
    {
      break;
    }
  }

  // @Hepheir ���� ������
  memset(buffer, '\0', sizeof(buffer)); // flush
  buffer[0] = room_number;           // 0: ���ȣ
  strcat(buffer, user_name);

  /*(1) ?\B6\F3\C0?\F0? \B1?\BC\B3\C1\A4*/
  connect_fd = Socket(AF_INET, SOCK_STREAM, 0);
  memset(&connect_addr, 0, sizeof(connect_addr));
  connect_addr.sin_family = AF_INET;
  connect_addr.sin_port = htons(atoi(argv[2]));
  connect_addr.sin_addr.s_addr = inet_addr(argv[1]);

  /*\BC\AD\B9\F6\C1\A2\BC\D3*/
  Connect(connect_fd, (struct sockaddr *)&connect_addr, sizeof(connect_addr));

  buffersize = sizeof(buffer);
  write(connect_fd, buffer, buffersize);
  memset(buffer, '\0', sizeof(buffer));

  /*?\C6\C3 \BC\AD\B9\F6\BF\A1 \B4\EB\C7\D1 \C1\A2\BC\D3 \B0\E1\B0\FA ?\C0\CE*/
  read(connect_fd, buffer, sizeof(buffer));

  if (!strcmp(buffer, "Connect OK"))
  {
    printf("************Server Connection Success*************\n");
    printf("************ Start Chatting Program  *************\n");
  }
  if (!strcmp(buffer, "User Count Overflow Error"))
  {
    printf("%s\n", buffer);
    exit(1);
  }

  /*\BC\F6\BD\C5 \B5\A5\C0\CC\C5\CD\C0\C7 \B5\BF\BD\C3?\B8\AE*/
  FD_ZERO(&readfd);

  while (1)
  {

    /*(2) \BC\F6\BD?\A5\C0\CC\C5\CD \B9\D7 \BC?\C5 \B5\A5\C0\CC\C5\CD ?\B8\AE*/
    FD_SET(0, &readfd);
    FD_SET(connect_fd, &readfd);

    select(connect_fd + 1, &readfd, NULL, NULL, NULL);

    // �������� �޽����� �������
    if (FD_ISSET(connect_fd, &readfd))
    {
      memset(buffer, '\0', sizeof(buffer));

      buffersize = read(connect_fd, buffer, sizeof(buffer));
      if (buffersize <= 0) continue;

      clearMessage(&message);
      parseMessage(buffer, &message);

      if (strcmp(message.from, "SYSTEM") == 0)
      {
        // TODO : 명령에 따른 처리 (킥, 밴, 템밴 등)
      }

      prtMessage(&message);
    }

    // ���� �޽��� ������ ���
    if (FD_ISSET(0, &readfd))
    {
      memset(buffer, '\0', sizeof(buffer));
      clearMessage(&message);

      strncpy(message.from, user_name, MESSAGE_FROM_MAX_LENGTH);
      fgets(message.content, MESSAGE_CONTENT_MAX_LENGTH, stdin);

      if (strstr(message.content, CHANGE_NAME) != NULL)
      {
        printf("Write your change name: ");
        memset(user_name, '\0', sizeof(user_name));
        fgets(user_name, MESSAGE_FROM_MAX_LENGTH, stdin);
        sprintf(message.content, "Change name to: %s", user_name);
        dumpMessage(buffer, MESSAGE_CONTENT_MAX_LENGTH, &message);
        write(connect_fd, buffer, buffersize);
      }
      else if (strstr(message.content, EXIT) != NULL)
      {
        sprintf(message.content, "Good Bye~!!\n %s is exit.", user_name);
        dumpMessage(buffer, MESSAGE_CONTENT_MAX_LENGTH, &message);
        write(connect_fd, buffer, buffersize);
        close(connect_fd);
        exit(0);
      }

      /*\B5\B5\BF\F2\B8\BB*/
      if (strstr(buffer, HELP) != NULL)
      {
        printf("***************************************************\n");
        printf("*  write 'ChangeName' on your chatting window *\n");
        printf("*  you can change your name                       *\n");
        printf("*  write 'Exit' on your chatting window             *\n");
        printf("*  you can exit your browser                         *\n");
        printf("***************************************************\n");
      }

      else {
        dumpMessage(buffer, MESSAGE_CONTENT_MAX_LENGTH, &message);
        write(connect_fd, buffer, strlen(buffer));
      }
    }
  }
  return 0;
}

void Connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen)
{
  int result = 0;
  result = connect(sockfd, servaddr, addrlen);
  if (result < 0)
  {
    printf("Connecting Error\n");
    printf("%d \n", result);
    exit(1);
  }
  else
  {
    printf("Success Connecting\n");
  }
}

int Socket(int family, int type, int protocol)
{
  int result = 0;
  result = socket(family, type, protocol);
  if (result == -1)
  {
    printf("Socket Contructing Error \n");
    exit(0);
  }
  return result;
}
