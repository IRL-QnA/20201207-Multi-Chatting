#define ROOM_NAME_MAX_LENGTH 10

typedef struct room_node {
  char room_name[ROOM_NAME_MAX_LENGTH];
  struct user_node user_list[5];
  int user_count;
} RoomNode;

void initRoom(RoomNode *room, const char *room_name) {
    room->user_count = 0;
    setRoomName(room, room_name);
}

void setRoomName(RoomNode *room, const char *string) {
    strncpy(room->room_name, string, ROOM_NAME_MAX_LENGTH);
}
