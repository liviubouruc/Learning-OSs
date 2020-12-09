#include <unistd.h>

int main()
{
        write(1, "Hello, World!", sizeof("Hello, World!"));
        return 0;
}