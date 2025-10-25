#include <string>
#include <iostream>
#include <fstream>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "../../../../../../../Library/Developer/CommandLineTools/SDKs/MacOSX15.5.sdk/usr/include/unistd.h"
using namespace std;

int main() {
    const char *shm_name = "Name Shared Memory";
    int fd = shm_open(shm_name, O_RDONLY, 0);

    struct stat sb;
    fstat(fd, &sb);
    size_t size = sb.st_size;
    void *addr = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    const char* data = static_cast<const char*>(addr);

    cout << data << endl;

    munmap(addr, size);
    close(fd);
    shm_unlink(shm_name);
    return 0;
}