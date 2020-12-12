

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

// 사용자 목록에서 user_index 순번에 해당하는 유저를 pop하는 함수
struct user_node popUser(struct room_node room, int user_index) {
  struct user_node user = room.user_list[user_index];
  for (; user_index < room.user_count-1; user_index++) {
    room.user_list[user_index] = room.user_list[user_index+1];
  }
  room.user_count--;
  return user;
}

// void pushUser() {};

void findUserByName(const struct room_node room, const char* username, struct user_node *user) {
  for (int user_number = 0; user_number < room.user_count; user_number++) {
    if (strcmp(room.user_list[user_number].name, username) == 0) {
      printf("Found username: %s");
      *user = popUser(room, user_number);
      return;
    }
  }
  printf("User %s not found.\n", username);
  return;
}
