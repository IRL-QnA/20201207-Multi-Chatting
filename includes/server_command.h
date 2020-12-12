#include <string.h>
#include <stdlib.h>

char *LIST = "/list";
char *KICK = "/kick";
char *BAN = "/ban";
char *TEMPBAN = "/tempban";

MessageNode cmd_message;
char cmd_message_buffer[MESSAGE_FROM_MAX_LENGTH];

void listParticipants(char *buffer, const struct room_node room) {
    strcat(buffer, "*");
    for (int user_idx = 0; user_idx < room.user_count; user_idx++) {
        strcat(buffer, "\t");
        strcat(buffer, room.user_list[user_idx].name);
        strcat(buffer, "\n");
    }
}

void kickUser(const struct room_node room, const struct user_node user) {
  strcpy(cmd_message.from, "SYSTEM");
  strcpy(cmd_message.content, "You were kicked.");
  dumpMessage(&cmd_message, cmd_message_buffer, MESSAGE_FROM_MAX_LENGTH);
  write(user.user_sockfd, cmd_message_buffer, sizeof(cmd_message_buffer));
  close(user.user_sockfd);
  return;
}

void banUser(const struct room_node room, const struct user_node user) {
  strcpy(cmd_message.from, "SYSTEM");
  strcpy(cmd_message.content, "You were banned.");
  dumpMessage(&cmd_message, cmd_message_buffer, MESSAGE_FROM_MAX_LENGTH);
  // DO SOMETHING
  write(user.user_sockfd, cmd_message_buffer, sizeof(cmd_message_buffer));
  close(user.user_sockfd);
  return;
}

void tempBanUser(const struct room_node room, const struct user_node user) {
  strcpy(cmd_message.from, "SYSTEM");
  strcpy(cmd_message.content, "You were temp banned.");
  dumpMessage(&cmd_message, cmd_message_buffer, MESSAGE_FROM_MAX_LENGTH);
  // DO SOMETHING
  write(user.user_sockfd, cmd_message_buffer, sizeof(cmd_message_buffer));
  close(user.user_sockfd);
  return;
}
