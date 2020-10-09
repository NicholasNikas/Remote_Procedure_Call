#include <stdio.h>

int main()
{
    int pid = fork();

    if (pid != 0)
    {
        printf("A\n");
        execlp("/bin/ls", "ls", NULL);
        printf("B\n");
    }
    else
    {
        printf("C\n");
        printf("E\n");
    }
    wait(pid);
    printf("D\n");
    return 0;
}