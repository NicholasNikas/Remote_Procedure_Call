/**
 *  @author: Nicholas Nikas
 *  @id: 260870980
 *  @email: nicholas.nikas@mail.mcgill.ca
 *  @description: Source code file for backend of simple RPC
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "rpc.h"

#define BUFSIZE 1024

int addInts(int x, int y);
int multiplyInts(int x, int y);
float divideFloats(float x, float y);
int factorial(int x);
void RPC_Register(char command[], char param_1[], char param_2[], char result_string[], int pid, int clientfd);

int main(int argc, char *argv[])
{
    char *my_ip = argv[1];
    int my_port = atoi(argv[2]);

    int connections, rval, sockfd, pid, clientfd1, clientfd2, clientfd3, clientfd4, clientfd5;
    int clientfd_array[5] = {clientfd1, clientfd2, clientfd3, clientfd4, clientfd5};
    int pid_array[5];
    char result_string[BUFSIZE] = {0};

    Message *message = (Message *)malloc(sizeof(Message));

    if (create_server(my_ip, my_port, &sockfd) < 0)
    {
        fprintf(stderr, "Error: Can't create server\n");
        return -1;
    }

    printf("Server listening on %d:%d\n", *my_ip, my_port);

    pid = fork();

    if (pid == 0)
    {
        for (int i = 0; i < 5; i++)
        {
            pid_array[i] = fork();
            if (pid_array[i] == 0)
            {
                while (strcmp(message->command, "quit"))
                {
                    if (accept_connection(sockfd, &clientfd_array[i]) < 0)
                    {
                        fprintf(stderr, "Error: Can't accept connection to frontend\n");
                        break;
                    }
                    memset(message, 0, sizeof(message));
                    ssize_t byte_count = recv_message(clientfd_array[i], message, BUFSIZE);
                    if (byte_count <= 0)
                    {
                        fprintf(stderr, "Error: Byte count invalid\n");
                        break;
                    }
                    RPC_Register(message->command, &message->param_1, &message->param_2, &result_string, pid, clientfd_array[i]);
                    send_message(clientfd_array[i], result_string, strlen(result_string));
                }
            }
        }
    }
    free(message);
    return 0;
}

void RPC_Register(char command[], char param_1[], char param_2[], char result_string[], int pid, int clientfd)
{
    if (strcmp(command, "add") == 0)
    {
        sprintf(result_string, "%d", addInts(atoi(param_1), atoi(param_2)));
    }
    else if (strcmp(command, "multiply") == 0)
    {
        sprintf(result_string, "%d", multiplyInts(atoi(param_1), atoi(param_2)));
    }
    else if (strcmp(command, "divide") == 0)
    {
        if (atof(param_2) == 0)
        {
            fprintf(stderr, "Error: Division by zero\n");
            sprintf(result_string, "Error: Division by zero");
        }
        else
        {
            sprintf(result_string, "%f", divideFloats(atof(param_1), atof(param_2)));
        }
    }
    else if (strcmp(command, "factorial") == 0)
    {
        if (atoi(param_1) < 0 || atoi(param_1) > 20)
        {
            fprintf(stderr, "Error: Integer must be in range [0,20]\n");
            sprintf(result_string, "%s", "Error: Integer must be in range [0,20]");
        }
        else
        {
            sprintf(result_string, "%d", factorial(atoi(param_1)));
        }
    }
    else if (strcmp(command, "sleep") == 0)
    {
        sprintf(result_string, "%s", " ");
        sleep(atoi(param_1));
    }
    else if (strcmp(command, "shutdown") == 0)
    {
        kill(pid, SIGKILL);
        char *result_string = "\n we closed \n";
        send_message(clientfd, result_string, strlen(result_string));
    }
    else
    {
        sprintf(result_string, "Error: Command '%s' not found", command);
    }
}

// Math functions

int addInts(int x, int y)
{
    return x + y;
}

int multiplyInts(int x, int y)
{
    return x * y;
}

float divideFloats(float x, float y)
{
    return x / y;
}

int factorial(int x)
{
    if (x <= 1)
    {
        return 1;
    }
    else
    {
        return x * factorial(x - 1);
    }
}

// serv = RPC_Init(myIP, myPort)
// for_all_functions(name)
//     RPC_Register(serv, name, function)
//
// while (no_shutdown)
// {
//     client = accept_on_server_socket(serv)
//     serv_client(client)
// }
