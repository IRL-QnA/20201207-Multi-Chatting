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

#define myport 3000
#define MAX 30

char *CHANGE_NAME = "ChangeName";
char *EXIT = "Exit";
char *HELP = "Help";

int Socket(int family, int type, int protocol);
void Connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);

int main(int argc, char *argv[])
{ // argc: \C0?\C2\C7\D2\C0\CE\C0\DA\C0\C7 \B0\B9\BC\F6, argv \C0\CE\C0?\A6\B4\E3\B4\C2 \B9迭
  struct sockaddr_in connect_addr;
  int connect_fd;
  int msgsize;

  char name[MAX] = {0};
  char sender[MAX] = {0}; // 전송자
  char room_number;
  char msg[1024] = {0};

  fd_set readfd;
  int temp_index;

  printf("****************************************************\n");
  printf("************Welcome to Chatting Browser*************\n");
  printf("****************************************************\n");

  printf(" Enter User Name=> ");
  memset(name, '\0', sizeof(name));

  /*\BB\E7\BF\EB\C0\DA \B4\EB?\B8\ED \C0?\C2?\B8\AE*/
  fgets(name, 30, stdin);
  name[strlen(name)] = '\0';
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

  // @Hepheir 내가 접속한
  memset(msg, '\0', sizeof(msg)); // flush
  msg[0] = room_number;           // 0: 방번호

  for (temp_index = 0; temp_index < strlen(name); temp_index++)
  {
    msg[temp_index + 1] = name[temp_index]; // 1~쭉 이름
  }

  /*(1) ?\B6\F3\C0?\F0? \B1?\BC\B3\C1\A4*/
  connect_fd = Socket(AF_INET, SOCK_STREAM, 0);
  memset(&connect_addr, 0, sizeof(connect_addr));
  connect_addr.sin_family = AF_INET;
  connect_addr.sin_port = htons(atoi(argv[2]));
  connect_addr.sin_addr.s_addr = inet_addr(argv[1]);

  /*\BC\AD\B9\F6\C1\A2\BC\D3*/
  Connect(connect_fd, (struct sockaddr *)&connect_addr, sizeof(connect_addr));

  msgsize = sizeof(msg);
  write(connect_fd, msg, msgsize);
  memset(msg, '\0', sizeof(msg));

  /*?\C6\C3 \BC\AD\B9\F6\BF\A1 \B4\EB\C7\D1 \C1\A2\BC\D3 \B0\E1\B0\FA ?\C0\CE*/
  read(connect_fd, msg, sizeof(msg));

  if (!strcmp(msg, "Connect OK"))
  {
    printf("************Server Connection Success*************\n");
    printf("************ Start Chatting Program  *************\n");
  }
  if (!strcmp(msg, "User Count Overflow Error"))
  {
    printf("%s\n", msg);
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

    // 서버에서 메시지를 받은경우
    if (FD_ISSET(connect_fd, &readfd))
    {
      memset(msg, '\0', sizeof(msg));

      msgsize = read(connect_fd, msg, sizeof(msg));
      if (msgsize <= 0)
        continue;

      // 혹시모를 대비
      msg[sizeof(msg)-1] = '\0';

      // 이름과 내용을 파싱
      sscanf(msg, "%s", sender);
      char* body = strstr(msg, "\n");

      if (strcmp(sender, "SYSTEM") == 0)
      {
        if (body != NULL && strstr(body, "KICKED") != NULL)
        {
          printf("Oops, You were kicked from the room T.T\n");
          close(connect_fd);
          exit(0);
        }
      }

      if (body != NULL)
      {
        printf("[%s]: ", sender);
        puts(body);
      }
    }

    // 내가 메시지 보내는 경우
    if (FD_ISSET(0, &readfd))
    {

      memset(msg, '\0', sizeof(msg));

      // 사용자 메시지 이전에 이름을 먼저 삽입
      strcpy(msg, name);
      msg[strlen(name)] = '\n';

      fgets(msg+strlen(name)+1, 1024, stdin); // 1 더하는 이유는 '\n'도 1칸을 차지해서.
      msg[strlen(msg)] = '\0';

      /*\C0?\A7\BA\AF\B0\E6*/
      if (strstr(msg+strlen(name)+1, CHANGE_NAME) != NULL)
      {
        printf("Write your change name: ");
        memset(name, '\0', sizeof(name));

        /*\BB\E7\BF\EB\C0\DA \B4\EB?\B8\ED \C0?\C2?\B8\AE*/
        fgets(name, 30, stdin);
        name[strlen(name)] = '\0';
      }

      /*\C1\BE\B7\E1*/
      if (strstr(msg, EXIT) != NULL)
      {
        sprintf(msg, "Good Bye~!!\n %s is exit.", name);
        write(connect_fd, msg, msgsize);
        close(connect_fd);
        exit(0);
      }

      /*\B5\B5\BF\F2\B8\BB*/
      if (strstr(msg, HELP) != NULL)
      {
        printf("***************************************************\n");
        printf("*  write 'ChangeName' on your chatting window *\n");
        printf("*  you can change your name                       *\n");
        printf("*  write 'Exit' on your chatting window             *\n");
        printf("*  you can exit your browser                         *\n");
        printf("***************************************************\n");
      }

      // 메시지 전송
      msgsize = strlen(msg);
      write(connect_fd, msg, msgsize);
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
