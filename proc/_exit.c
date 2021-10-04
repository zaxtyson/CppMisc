#include <unistd.h> // _exit()
#include <stdlib.h> // exit()

int main(int argc, char const *argv[])
{
    // _exit(-1);  // 255
    exit(-1); // 255
    return 0;
}
