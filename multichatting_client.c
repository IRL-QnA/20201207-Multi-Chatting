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
{
  struct sockaddr_in connect_addr;
  int connect_fd;
  int msgsize;

  char name[MAX] = {0};
  char sender[MAX] = {0};
  char room_number;
  char msg[1024] = {0};

  fd_set readfd;
  int temp_index;

  printf("****************************************************\n");
  printf("************Welcome to Chatting Browser*************\n");
  printf("****************************************************\n");

  printf(" Enter User Name=> ");
  memset(name, '\0', sizeof(name));

  fgets(name, 30, stdin);
  name[strlen(name)] = '\0';
  while (1)
  {

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

  memset(msg, '\0', sizeof(msg));
  msg[0] = room_number;

  for (temp_index = 0; temp_index < strlen(name); temp_index++)
  {
    msg[temp_index + 1] = name[temp_index];
  }

  connect_fd = Socket(AF_INET, SOCK_STREAM, 0);
  memset(&connect_addr, 0, sizeof(connect_addr));
  connect_addr.sin_family = AF_INET;
  connect_addr.sin_port = htons(atoi(argv[2]));
  connect_addr.sin_addr.s_addr = inet_addr(argv[1]);

  Connect(connect_fd, (struct sockaddr *)&connect_addr, sizeof(connect_addr));

  msgsize = sizeof(msg);
  write(connect_fd, msg, msgsize);
  memset(msg, '\0', sizeof(msg));

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

  FD_ZERO(&readfd);

  while (1)
  {

    FD_SET(0, &readfd);
    FD_SET(connect_fd, &readfd);

    select(connect_fd + 1, &readfd, NULL, NULL, NULL);

    if (FD_ISSET(connect_fd, &readfd))
    {
      memset(msg, '\0', sizeof(msg));

      msgsize = read(connect_fd, msg, sizeof(msg));
      if (msgsize <= 0)
        continue;

      msg[sizeof(msg) - 1] = '\0';

      sscanf(msg, "%s", sender);
      char *body = strstr(msg, "\n");

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

    if (FD_ISSET(0, &readfd))
    {

      memset(msg, '\0', sizeof(msg));

      strcpy(msg, name);
      msg[strlen(name)] = '\n';

      fflush(stdin);
      fgets(msg + strlen(name) + 1, 1024, stdin);
      msg[strlen(msg)] = '\0';

      if (strstr(msg + strlen(name) + 1, CHANGE_NAME) != NULL)
      {
        printf("Write your change name: ");
        memset(name, '\0', sizeof(name));

        fgets(name, 30, stdin);
        name[strlen(name)] = '\0';
      }

      if (strstr(msg, EXIT) != NULL)
      {
        sprintf(msg, "Good Bye~!!\n %s is exit.", name);
        write(connect_fd, msg, msgsize);
        close(connect_fd);
        exit(0);
      }

      if (strstr(msg, HELP) != NULL)
      {
        printf("***************************************************\n");
        printf("*  write 'ChangeName' on your chatting window *\n");
        printf("*  you can change your name                       *\n");
        printf("*  write 'Exit' on your chatting window             *\n");
        printf("*  you can exit your browser                         *\n");
        printf("***************************************************\n");
      }

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
