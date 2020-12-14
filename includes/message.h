#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SEPERATOR "\n"
#define MESSAGE_FROM_MAX_LENGTH 32
#define MESSAGE_CONTENT_MAX_LENGTH 1024

typedef struct message_node {
  char from[MESSAGE_FROM_MAX_LENGTH];
  char content[MESSAGE_CONTENT_MAX_LENGTH];
} MessageNode;

void clearMessage(MessageNode *message) {
  memset(message->from, '\0', MESSAGE_FROM_MAX_LENGTH);
  memset(message->content, '\0', MESSAGE_CONTENT_MAX_LENGTH);
  return;
}

void dumpMessage(char *string, int max_length, const MessageNode *message) {
  // from와 content문자열을 받아 평문을 완성하여 dst로 전달
  memset(string, '\0', max_length);
  strcat(string, message->from);
  strcat(string, SEPERATOR);
  strcat(string, message->content);
  return;
}

void parseMessage(const char *string, MessageNode *message) {
  /*
   * 메시지 평문(data)으로 부터 전송자와 메시지 내용을 추출한다.
   * ---
   * 전송자
   * <SEPERATOR>
   * 메시지
   * ---
   * 의 형태로 메시지가 오기 때문에,
   * 1. SEPERATOR를 기준으로 앞 뒤를 자르고
   * 2. SEPERATOR와 메시지의 시작 사이의 공백을 넘겨가며 메시지의 시작점을 찾는다.
   */
  char *pointer;
  pointer = strstr(string, SEPERATOR);
  if (pointer != NULL) {
    *pointer++ = '\0';
    while (*pointer == '\n' || *pointer == ' ') {
      pointer++;
    }
  }
  strncpy(message->from, string, MESSAGE_FROM_MAX_LENGTH);
  strncpy(message->content, pointer, MESSAGE_CONTENT_MAX_LENGTH);
  return;
}

void prtMessage(const MessageNode *message) {
  printf("[%s] %s\n", message->from, message->content);
  return;
}
