#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#include "lim.h"

int server_q;
client clients[MAX_CLIENTS];


void signal_handler(int sig)
{
	if (server_q != -1)
	{
		msg_buff *msg = malloc(sizeof(msg_buff));
		msg->mtype = STOP;
		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			if (clients[i].key != -1)
			{
				int client_qid = msgget(clients[i].key, 0);
				msgsnd(client_qid, msg, sizeof(msg_buff), 0);
			}
		}
	}

	msgctl(server_q, IPC_RMID, NULL);
	exit(EXIT_SUCCESS);
}

void rcv_init(msg_buff *msg)
{
    int client_idx = 0;
    while (client_idx < MAX_CLIENTS && clients[client_idx].key != -1) {
        client_idx++;
    }

    if (client_idx == MAX_CLIENTS) {
        printf("SERVER | Server can only handle %d clients!\n", MAX_CLIENTS);
        msg->client_id = -1;
        return;
    }

    clients[client_idx].id = msg->client_id; // pid
    clients[client_idx].key = msg->q_key;
    msg->client_id = client_idx;

    int client_qid = msgget(clients[client_idx].key, 0);
    if (client_qid == -1) {
        printf("SERVER | Failed to obtain client queue ID for client %d!\n",
               clients[client_idx].id);
        return;
    }

    if (msgsnd(client_qid, msg, sizeof(msg_buff), 0) == -1) {
        printf("SERVER | Failed to send message to client %d!\n",
               clients[client_idx].id);
        return;
    }

    struct tm msg_time = msg->time_struct;
    printf("SERVER | [%02d:%02d:%02d] | New client: %d\n",
           msg_time.tm_hour,
           msg_time.tm_min,
           msg_time.tm_sec,
           clients[client_idx].id);
}


void rcv_list(msg_buff *msg) {
    int idx = msg->client_id;
    char list[MAX_MSG_LEN] = {0};

    // Build the list of other clients
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].key != -1 && i != idx) {
            snprintf(list + strlen(list), MAX_MSG_LEN - strlen(list), "ID: %d\n", clients[i].id);
        }
    }
    // Copy the list into the message buffer
    strcpy(msg->content, list);

    // Log the event
    struct tm msg_time = msg->time_struct;
    printf("[%d] | [%02d:%02d:%02d] | LIST\n", clients[idx].id, msg_time.tm_hour, msg_time.tm_min, msg_time.tm_sec);

    // Send the message to the client
    int client_qid = msgget(clients[idx].key, 0);
    msgsnd(client_qid, msg, sizeof(msg_buff), 0);
}


void rcv_2all(msg_buff *msg) {
    int idx = msg->client_id;

    struct tm msg_time = msg->time_struct;
    printf("[%d] | [%02d:%02d:%02d] | 2ALL\n",
           clients[idx].id,
           msg_time.tm_hour,
           msg_time.tm_min,
           msg_time.tm_sec);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (i != idx && clients[i].key != -1) {
            int client_qid = msgget(clients[i].key, 0);
            msgsnd(client_qid, msg, sizeof(msg_buff), 0);
        }
    }
}


void rcv_2one(msg_buff *msg) {
    int catcher_pid = msg->catcher_pid;
    int i = 0;

    while (i < MAX_CLIENTS && clients[i].id != catcher_pid) {
        i++;
    }

    if (i == MAX_CLIENTS) {
        printf("Can not find [%d] user\n", catcher_pid);
        return;
    }

    int client_qid = msgget(clients[i].key, 0);
    msg->client_id = clients[msg->client_id].id;
    msgsnd(client_qid, msg, sizeof(msg_buff), 0);

    struct tm msg_time = msg->time_struct;
    printf("[%d] | [%02d:%02d:%02d] | 2ONE: %d\n",
           msg->client_id,
           msg_time.tm_hour,
           msg_time.tm_min,
           msg_time.tm_sec,
           catcher_pid);
}


void rcv_stop(msg_buff *msg) {
    int client_idx = msg->client_id;  // indeks klienta, który chce się wylogować
    int client_qid = msgget(clients[client_idx].key, 0);  // identyfikator kolejki wiadomości klienta
    
    // Wylogowanie klienta
    printf("SERVER | Client [%d] left\n", clients[client_idx].id);
    clients[client_idx].id = -1;
    clients[client_idx].key = -1;
    
    // Wysłanie wiadomości zwrotnej do klienta
    msg->client_id = clients[client_idx].id;  // ustawienie identyfikatora klienta w wiadomości
    msgsnd(client_qid, msg, sizeof(msg_buff), 0);  // wysłanie wiadomości zwrotnej
}


int main()
{
	signal(SIGINT, signal_handler);

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		client *new_client = malloc(sizeof(client));
		new_client->key = -1;
		clients[i] = *new_client;
	}

	// open server q
	key_t s_key = ftok(getenv("HOME"), 1);
	if (s_key == -1)
	{
		perror("ftok");
		exit(EXIT_FAILURE);
	}

	server_q = msgget(s_key, IPC_CREAT | 0666);
	if (server_q == -1)
	{
		perror("msgget");
		exit(EXIT_FAILURE);
	}

	printf("SERVER | starting server...\n");

	msg_buff *msg = malloc(sizeof(msg_buff));
	while (1)
	{
		msgrcv(server_q, msg, sizeof(msg_buff), -6, 0);
		// printf("%ld | ", msg->mtype);

		switch (msg->mtype)
		{
		case INIT:
			rcv_init(msg);
			break;
		case STOP:
			rcv_stop(msg);
			break;
		case LIST:
			rcv_list(msg);
			break;
		case TALL:
			rcv_2all(msg);
			break;
		case TONE:
			rcv_2one(msg);
			break;
		default:
			printf("Wrong msg type [%ld]\n", msg->mtype);
		}
	}
}