#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>


#define SERVER_PORT 5001
#define MAX_NAME_LENGTH 30
#define ETERNITY 999999999

struct user_node
{
  char name[MAX_NAME_LENGTH];
  int user_sockfd;
};

struct room_node
{
  char room_name[10];
  struct user_node user_list[5];
  int user_count;
};

struct ban_node
{
  char name[MAX_NAME_LENGTH];
  time_t expires;
};

int Socket(int family, int type, int protocol);
void Bind(int sockfd, const struct sockaddr *socketaddr, socklen_t addrlen);
void Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr *Cliaddr, socklen_t *addelen);

int main(void)
{
  struct room_node roomlist[3];

  int server_socket;
  int client_socket;

  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  int addrsize, msgsize;

  char BUFF[1024] = {0};
  fd_set readfd;
  int maxfd;

  int room_index;
  int user_index;
  int tempsockfd;
  int temp_user_count;

  time_t current_time;
  struct ban_node banned_user_list[20]; // 최대 20명까지만 밴 가능함에 유의
  int banned_user_count = 0;

  strcpy(roomlist[0].room_name, "room1");
  strcpy(roomlist[1].room_name, "room2");
  strcpy(roomlist[2].room_name, "room3");

  roomlist[0].user_count = 0;
  roomlist[1].user_count = 0;
  roomlist[2].user_count = 0;

  /*(1) \BC\AD\B9\F6 \B1⺻\BC\B3\C1\A4*/
  server_socket = Socket(AF_INET, SOCK_STREAM, 0);

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERVER_PORT);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  memset(&(server_addr.sin_zero), 0, 8);

  Bind(server_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
  /*(2) \BC\AD\B9\F6 \C1\A2\BCӴ\EB\B1\E2*/
  Listen(server_socket, 0);
  FD_ZERO(&readfd);
  maxfd = server_socket;

  while (1)
  {
    current_time = time(NULL);

    // 기간 만료된 밴 풀어주기
    for (temp_user_count = 0; temp_user_count < banned_user_count; temp_user_count++) {
      if (banned_user_list[temp_user_count].expires < current_time) {
        while (temp_user_count < banned_user_count-1) {
          banned_user_list[temp_user_count] = banned_user_list[temp_user_count+1];
          temp_user_count++;
        }
        break;
      }
    }

    /*(3) \BC\AD\B9\F6\BF\A1 \C1\A2\BC\D3\C7\D1 Ŭ\B6\F3\C0̾\F0Ʈ \BC\D2\C4ϱ\E2\BC\FA\C0ڸ\A6 FD_SET\BF\A1 \BC\B3\C1\A4 */

    FD_SET(server_socket, &readfd);

    for (room_index = 0; room_index < 3; room_index++)
    {
      for (user_index = 0; user_index < roomlist[room_index].user_count; user_index++)
      {
        tempsockfd = roomlist[room_index].user_list[user_index].user_sockfd;
        FD_SET(tempsockfd, &readfd);
        if (tempsockfd > maxfd)
          maxfd = tempsockfd;
      }
    }
    maxfd = maxfd + 1;

    select(maxfd, &readfd, NULL, NULL, NULL);

    /*(4) \BC\AD\B9\F6\B7\CE\C0\C7 \C1\A2\BC\D3\C0\CC \C0ִ\C2 Ŭ\B6\F3\C0̾\F0Ʈ\BF\A1 \B4\EB\C7\D1 ó\B8\AE*/

    if (FD_ISSET(server_socket, &readfd))
    {
      addrsize = sizeof(struct sockaddr_in);
      client_socket = Accept(server_socket, (struct sockaddr *)&server_addr, &addrsize);

      memset(BUFF, '\0', sizeof(BUFF));
      msgsize = read(client_socket, BUFF, sizeof(BUFF));

      if (msgsize <= 0)
      {
        printf("Enter user Error\n");
        continue;
      }
      printf("Receive Message: %s\n", BUFF); //Ŭ\B6\F3\C0̾\F0Ʈ\C0\C7 \C1\A2\BCӹ\E6\B9\F8ȣ\BF\CD \B4\EBȭ\B8\ED\C0\BB \C3\E2\B7\C2

      if (BUFF[0] == '1')
      {
        printf("Login Room1 Count:%d\n", roomlist[0].user_count);

        // 밴된 유저인지 검사
        bool is_banned = false;

        for (temp_user_count = 0; temp_user_count < banned_user_count; temp_user_count++) {
          if (strcmp(banned_user_list[temp_user_count].name, BUFF+1) != NULL) {
            is_banned = true;
            break;
          }
        }

        if (is_banned) {
          printf("Banned User, login refused.\n");
          strcpy(BUFF, "You were banned.");
          write(client_socket, BUFF, strlen(BUFF));
          close(client_socket);
          continue;
        }

        if (roomlist[0].user_count == 5)
        {
          strcpy(BUFF, "User Count Overflow Error");
          write(client_socket, BUFF, strlen(BUFF));
          // \BF\C0\B7\F9\B8\C3\C1\F6\B8\A6 Ŭ\B6\F3\C0̾\F0Ʈ\BF\A1 \BC۽\C5
          close(client_socket);
          continue;
        }

        roomlist[0].user_list[roomlist[0].user_count].user_sockfd = client_socket;
        roomlist[0].user_count++;
        strcpy(BUFF, "Connect OK");
        write(client_socket, BUFF, strlen(BUFF)); // \C1\A2\BC\D3 \BC\BA\B0\F8 \B8޽\C3\C1\F6\B8\A6 Ŭ\B6\F3\C0̾\F0Ʈ\BF\A1 \BC۽\C5
      }

      else if (BUFF[0] == '2')
      {
        if (roomlist[1].user_count == 5)
        {
          strcpy(BUFF, "User Count Overflow Error");
          write(client_socket, BUFF, strlen(BUFF));
          close(client_socket);
          continue;
        }
        roomlist[1].user_list[roomlist[1].user_count].user_sockfd = client_socket;
        roomlist[1].user_count++;
        strcpy(BUFF, "Connect OK");
        write(client_socket, BUFF, strlen(BUFF));
      }

      else if (BUFF[0] == '3')
      {
        if (roomlist[2].user_count == 5)
        {
          strcpy(BUFF, "User Count Overflow Error");
          write(client_socket, BUFF, strlen(BUFF));
          close(client_socket);
          continue;
        }
        roomlist[2].user_list[roomlist[2].user_count].user_sockfd = client_socket;
        roomlist[2].user_count++;
        strcpy(BUFF, "Connect OK");
        write(client_socket, BUFF, strlen(BUFF));
      }
      continue;
    }

    /*(4) \B0\A2\B9濡 \BC\D3\C7\D1 \B8\F0\B5\E7 \C0\AF\C0\FA\B5\E9\C0\C7 \BC\D2\C4\CF \B1\E2\BC\FA\C0ڿ\A1 \BA\AFȭ\B8\A6 \B0\A8\C1\F6\C7Ͽ\A9
		\C7ش\E7\BC\F6\BDŵ\A5\C0\CC\C5Ϳ\A1 \B4\EB\C7\D1 \C0\C0\B4\E4\C0\BB \B0\B0\C0\BA\B9濡 \C0ִ\C2 \C0\AF\C0\FA\BF\A1 \C7ѿ\A1\BC\AD \BC۽\C5*/
    for (room_index = 0; room_index < 3; room_index++)
    {
      for (user_index = 0; user_index < roomlist[room_index].user_count; user_index++)
      {

        if (FD_ISSET(roomlist[room_index].user_list[user_index].user_sockfd, &readfd))
        {
          // 메시지를 받는 부분
          memset(BUFF, '\0', sizeof(BUFF));

          tempsockfd = roomlist[room_index].user_list[user_index].user_sockfd;
          msgsize = read(tempsockfd, BUFF, sizeof(BUFF));

          // 전송자 이름을 추출
          sscanf(BUFF, "%s", roomlist[room_index].user_list[user_index].name);

          // 메시지가 없다면 처리
          if (msgsize <= 0)
          {

            if (user_index == roomlist[room_index].user_count)
            {
              close(roomlist[room_index].user_list[user_index].user_sockfd);
              roomlist[room_index].user_count--;
            }
            else if (user_index < roomlist[room_index].user_count)
            {
              close(roomlist[room_index].user_list[user_index].user_sockfd);

              for (temp_user_count = user_index; temp_user_count < roomlist[room_index].user_count; temp_user_count++)
              {

                roomlist[room_index].user_list[temp_user_count] =
                    roomlist[room_index].user_list[temp_user_count + 1];
              }
              roomlist[room_index].user_count--;
            }
          }

          // 메시지가 있다면 처리
          else
          {
            printf("Receive Message=>%s\n===END===\n", BUFF, msgsize);

            // 방장이 "/ls"를 보내면 참여자 리스트 전송
            char *cmd_list = strstr(BUFF, "/list"); // list participants
            char *cmd_kick = strstr(BUFF, "/kick");  // kick
            char *cmd_ban = strstr(BUFF, "/ban");

            if (cmd_list != NULL)
            {
              // BUFF => 참여자 리스트
              printf("Show participants\n");
              Participants(roomlist, room_index, BUFF);
              write(roomlist[room_index].user_list[user_index].user_sockfd, BUFF, strlen(BUFF));
            }
            if (user_index == 0 && cmd_kick != NULL)
            {
              char kickWho_name[MAX_NAME_LENGTH];
              int kickWho_idx;
              memset(kickWho_name, '\0', sizeof(kickWho_name));

              sscanf(strstr(cmd_kick, " ")+1, "%s", kickWho_name);
              printf("Kicking user : %s\n", kickWho_name);

              kickWho_idx = GetUserindex(roomlist, room_index, kickWho_name);
              if (kickWho_idx >= 0)
              {
                strcpy(BUFF, "SYSTEM\nKICKED");
                write(roomlist[room_index].user_list[kickWho_idx].user_sockfd, BUFF, sizeof(BUFF));
                close(roomlist[room_index].user_list[kickWho_idx].user_sockfd);

                roomlist[room_index].user_count--;
                for (int user_number = kickWho_idx; user_number < roomlist[room_index].user_count; user_number++)
                {
                  roomlist[room_index].user_list[user_number] = roomlist[room_index].user_list[user_number + 1];
                }
              }
            }
            // 밴
            if (user_index == 0 && cmd_ban != NULL)
            {
              char target_name[MAX_NAME_LENGTH];
              int target_index;
              memset(target_name, '\0', sizeof(target_name));

              sscanf(strstr(cmd_ban, " ")+1, "%s", target_name);
              printf("Banning user : %s\n", target_name);

              strcpy(banned_user_list[banned_user_count].name, target_name);
              banned_user_list[banned_user_count].expires = ETERNITY;
              banned_user_count++;

              target_index = GetUserindex(roomlist, room_index, target_name);
              if (target_index >= 0)
              {
                strcpy(BUFF, "SYSTEM\nKICKED");
                write(roomlist[room_index].user_list[target_index].user_sockfd, BUFF, sizeof(BUFF));
                close(roomlist[room_index].user_list[target_index].user_sockfd);

                roomlist[room_index].user_count--;
                for (int user_number = target_index; user_number < roomlist[room_index].user_count; user_number++)
                {
                  roomlist[room_index].user_list[user_number] = roomlist[room_index].user_list[user_number + 1];
                }
              }
            }
            // 일반 메세지가 왔을 때
            else
            {
              // 방에 있는 모든 참가자들에게 메시지 전달
              for (temp_user_count = 0; temp_user_count < roomlist[room_index].user_count; temp_user_count++)
              {
                msgsize = strlen(BUFF);
                write(roomlist[room_index].user_list[temp_user_count].user_sockfd, BUFF, msgsize);
              }
            }
          }
        }
      }
    }
  }
  return 0;
}

int Socket(int family, int type, int protocol)
{
  int result = 0;
  result = socket(family, type, protocol);
  if (result == -1)
  {
    printf("Socket Contructing Error\n");
    exit(0);
  }
  return result;
}

void Bind(int sockfd, const struct sockaddr *socketaddr, socklen_t addrlen)
{
  int result = 0;
  result = bind(sockfd, socketaddr, addrlen);
  if (result == -1)
  {
    printf("Socket Binding Error\n");
    exit(0);
  }
  else
  {
    printf("Success Binding\n");
  }
}

void Listen(int sockfd, int backlog)
{
  int result = 0;
  result = listen(sockfd, backlog);
  if (result == -1)
  {
    printf("Listening Error\n");
    exit(0);
  }
  else
  {
    printf("Success Listening\n");
  }
}
int Accept(int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen)
{
  int result = 0;
  result = accept(sockfd, cliaddr, addrlen);
  if (result == -1)
  {
    printf("Accept Error\n");
    exit(0);
  }
  else
  {
    printf("Success Accept\n");
  }
  return result;
}

int GetUserindex(const struct room_node roomlist[], const int room_number, const char *username)
{
  for (int user_number = 0; user_number < roomlist[room_number].user_count; user_number++)
  {
    if (strcmp(roomlist[room_number].user_list[user_number].name, username) == 0)
    {
      printf("Found username: %s\n", username);
      return user_number;
    }
  }
  printf("User %s not found.\n", username);
  return -1;
}

void Participants(const struct room_node roomlist[], const int room_number, char *buffer)
{
  memset(buffer, '\0', sizeof(buffer));

  // 클라이언트에서 입력받는 방식을 "전송자\n메시지" 형식으로 했기 때문에, 내 이름을 추가해줌.
  strcat(buffer, "SYSTEM\nParticipants:\n*"); // 방장 이름 왼쪽에 별표시
  for (int user_number = 0; user_number < roomlist[room_number].user_count; user_number++)
  {
    strcat(buffer, roomlist[room_number].user_list[user_number].name);
    strcat(buffer, "\n");
  }

  printf("Built String:\n%s\n", buffer);
  return;
}
