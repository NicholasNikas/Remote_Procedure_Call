/**
 *  @author: Nicholas Nikas
 *  @id: 260870980
 *  @email: nicholas.nikas@mail.mcgill.ca
 *  @description: Source code file for frontend of simple RPC
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "rpc.h"

#define BUFSIZE 1024

void print_prompt();
void parse_line(Message *message, char user_input[]);

int main(int argc, char *argv[])
{
    char *my_ip = argv[1];
    int my_port = atoi(argv[2]);
    int sockfd;
    char user_input[BUFSIZE] = {0};
    char server_msg[BUFSIZE] = {0};
    Message *message = (Message *)malloc(sizeof(Message));

    if (connect_to_server(my_ip, my_port, &sockfd) < 0)
    {
        fprintf(stderr, "Error: Can't connect to server\n");
        return -1;
    }

    while (strcmp(user_input, "quit\n"))
    {
        print_prompt();

        memset(user_input, 0, sizeof(user_input));
        memset(server_msg, 0, sizeof(server_msg));

        // read user input from command line
        fgets(user_input, BUFSIZE, stdin);

        if (strcmp(user_input, "exit\n") == 0)
        {
            printf("Shut frontend\n");
            break;
        }

        parse_line(message, user_input);

        // send the input to server
        send_message(sockfd, message, BUFSIZE);

        // if (strcmp(user_input, "shutdown\n") == 0 || strcmp(user_input, "quit\n") == 0)
        // {
        //     printf("Bye!\n");
        //     break;
        // }

        // receive a msg from the server
        ssize_t byte_count = recv_message(sockfd, server_msg, sizeof(server_msg));
        if (byte_count <= 0)
        {
            fprintf(stderr, "Error: Byte count invalid\n");
            break;
        }
        printf("%s\n", server_msg);
    }
    free(message);
    return 0;
}

void parse_line(Message *message, char user_input[])
{
    char *token = strtok(user_input, " ");
    int index = 1;

    while (token != NULL)
    {
        if (index == 1)
        {
            // parse command
            strcpy(message->command, token);
            index++;
            token = strtok(NULL, " ");
        }
        else if (index == 2)
        {
            // parse first paramter
            strcpy(message->param_1, token);
            index++;
            token = strtok(NULL, " ");
        }
        else if (index == 3)
        {
            // parse second parameter
            strcpy(message->param_2, token);
            index++;
            token = strtok(NULL, " ");
        }
    }
}

void print_prompt()
{
    printf(">> ");
}

// backend = RPC_Connect(backendIP, backendPort)
// while (no_exit) {
//     print_prompt();
//     line = read_line();
//     cmd = parse_line(line);
//     RPC_Call(backend, cmd.name, cmd.args);
// }
// RPC_Close(backend);
