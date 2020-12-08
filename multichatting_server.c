#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<stdio.h>

#define SERVER_PORT 5001

struct user_node {
	int user_sockfd;
};
struct room_node {
	char room_name[10];
	struct user_node user_list[5];
	int user_count;
};

int Socket(int family, int type, int protocol);
void Bind(int sockfd, const struct sockaddr* socketaddr, socklen_t addrlen);
void Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr* Cliaddr, socklen_t* addelen);

int main(void)
{
	struct room_node roomlist[3];

	int server_socket;
	int client_socket;

	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int addrsize, msgsize;

	char BUFF[1024] = { 0 };
	fd_set readfd;
	int maxfd;

	int room_index;
	int user_index;
	int tempsockfd;
	int temp_user_count;

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

	Bind(server_socket, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
	/*(2) \BC\AD\B9\F6 \C1\A2\BCӴ\EB\B1\E2*/
	Listen(server_socket, 0);
	FD_ZERO(&readfd);
	maxfd = server_socket;

	while (1) {
		/*(3) \BC\AD\B9\F6\BF\A1 \C1\A2\BC\D3\C7\D1 Ŭ\B6\F3\C0̾\F0Ʈ \BC\D2\C4ϱ\E2\BC\FA\C0ڸ\A6 FD_SET\BF\A1 \BC\B3\C1\A4 */

		FD_SET(server_socket, &readfd);

		for (room_index = 0; room_index < 3; room_index++) {
			for (user_index = 0; user_index < roomlist[room_index].user_count; user_index++) {
				tempsockfd = roomlist[room_index].user_list[user_index].user_sockfd;
				FD_SET(tempsockfd, &readfd);
				if (tempsockfd > maxfd)
					maxfd = tempsockfd;
			}
		}
		maxfd = maxfd + 1;

		select(maxfd, &readfd, NULL, NULL, NULL);

		/*(4) \BC\AD\B9\F6\B7\CE\C0\C7 \C1\A2\BC\D3\C0\CC \C0ִ\C2 Ŭ\B6\F3\C0̾\F0Ʈ\BF\A1 \B4\EB\C7\D1 ó\B8\AE*/

		if (FD_ISSET(server_socket, &readfd)) {
			addrsize = sizeof(struct sockaddr_in);
			client_socket = Accept(server_socket, (struct sockaddr*)&server_addr, &addrsize);

			memset(BUFF, '\0', sizeof(BUFF));
			msgsize = read(client_socket, BUFF, sizeof(BUFF));


			if (msgsize <= 0) {
				printf("Enter user Error\n");
				continue;
			}
			printf("Receive Message: %s\n", BUFF);  //Ŭ\B6\F3\C0̾\F0Ʈ\C0\C7 \C1\A2\BCӹ\E6\B9\F8ȣ\BF\CD \B4\EBȭ\B8\ED\C0\BB \C3\E2\B7\C2 


			if (BUFF[0] == '1')
			{
				printf("Login Room1 Count:%d\n", roomlist[0].user_count);

				if (roomlist[0].user_count == 5) {
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

			else if (BUFF[0] == '2') {
				if (roomlist[1].user_count == 5) {
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

			else if (BUFF[0] == '3') {
				if (roomlist[2].user_count == 5) {
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
		for (room_index = 0; room_index < 3; room_index++) {
			for (user_index = 0; user_index < roomlist[room_index].user_count; user_index++) {

				if (FD_ISSET(roomlist[room_index].user_list[user_index].user_sockfd, &readfd)) {
					memset(BUFF, '\0', sizeof(BUFF));
					tempsockfd = roomlist[room_index].user_list[user_index].user_sockfd;
					msgsize = read(tempsockfd, BUFF, sizeof(BUFF));

					if (msgsize <= 0) {

						if (user_index == roomlist[room_index].user_count) {
							close(roomlist[room_index].user_list[user_index].user_sockfd);
							roomlist[room_index].user_count--;
						}
						else if (user_index < roomlist[room_index].user_count) {
							close(roomlist[room_index].user_list[user_index].user_sockfd);

							for (temp_user_count = user_index; temp_user_count < roomlist[room_index].
								user_count; temp_user_count++) {

								roomlist[room_index].user_list[temp_user_count] =
									roomlist[room_index].user_list[temp_user_count + 1];
							}
							roomlist[room_index].user_count--;
						}
					}
					else {
						printf("Receive Message=>%s\n", BUFF, msgsize);
						for (temp_user_count = 0; temp_user_count < roomlist[room_index].user_count; temp_user_count++) {
							msgsize = strlen(BUFF);
							write(roomlist[room_index].user_list[temp_user_count].user_sockfd, BUFF, msgsize);
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
	if (result == -1) {
		printf("Socket Contructing Error\n");
		exit(0);
	}
	return result;
}

void Bind(int sockfd, const struct sockaddr* socketaddr, socklen_t addrlen) {
	int result = 0;
	result = bind(sockfd, socketaddr, addrlen);
	if (result == -1) {
		printf("Socket Binding Error\n");
		exit(0);
	}
	else {
		printf("Success Binding\n");
	}
}

void Listen(int sockfd, int backlog) {
	int result = 0;
	result = listen(sockfd, backlog);
	if (result == -1) {
		printf("Listening Error\n");
		exit(0);
	}
	else {
		printf("Success Listening\n");
	}
}
int Accept(int sockfd, struct sockaddr* cliaddr, socklen_t* addrlen) {
	int result = 0;
	result = accept(sockfd, cliaddr, addrlen);
	if (result == -1) {
		printf("Accept Error\n");
		exit(0);
	}
	else {
		printf("Success Accept\n");
	}
	return result;
}