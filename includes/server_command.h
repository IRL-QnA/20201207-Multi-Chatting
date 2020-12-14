#include <string.h>
#include <stdlib.h>

char *LIST = "/list";
char *KICK = "/kick";
char *BAN = "/ban";
char *TEMPBAN = "/tempban";

MessageNode cmd_message;
char cmd_message_buffer[MESSAGE_FROM_MAX_LENGTH];

bool isCommand(const MessageNode *message) {
  return message->content[0] == '/';
}

char *nextParam(char *cmd_string) {
  while (*cmd_string && *cmd_string != ' ') cmd_string++;
  while (*cmd_string && *cmd_string == ' ') cmd_string++;
  return cmd_string;
}

void listParticipants(char *buffer, const struct room_node room) {
  strcpy(buffer, "\n");
  printf("Generating list of room:[%s], total %d participants.\n", room.room_name, room.user_count);
  for (int user_idx = 0; user_idx < room.user_count; user_idx++) {
    printf("<%s>\n", room.user_list[user_idx].name);
    strcat(buffer, " * ");
    strcat(buffer, room.user_list[user_idx].name);
    strcat(buffer, "\n");
  }
}

void kickUser(const struct room_node room, const struct user_node user) {
  close(user.user_sockfd);
  return;
}

void banUser(const struct room_node room, const struct user_node user) {
  // DO SOMETHING
  kickUser(room, user);
  return;
}

void tempBanUser(const struct room_node room, const struct user_node user) {
  // DO SOMETHING
  kickUser(room, user);
  return;
}
