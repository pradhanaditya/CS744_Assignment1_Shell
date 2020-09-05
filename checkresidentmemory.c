#include <unistd.h>

int main(int argc, char *argv[])
{
    char *psArgs[] = {"ps", "-o", "rss=", argv[1], NULL};
    char *environ[] = {NULL};

    execve("/bin/ps", psArgs, environ);

    return 0;
}