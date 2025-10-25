#include <string>
#include <iostream>
#include <fstream>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include "../../../../../../../Library/Developer/CommandLineTools/SDKs/MacOSX15.5.sdk/usr/include/unistd.h"
using namespace std;

int main() {
    string line;
    getline( std::cin, line );
    int fd = shm_open("Name Shared Memory", O_CREAT | O_RDWR, 0777);
    size_t len = line.size() + 1;
    ftruncate(fd, len);
    char *out = (char *) mmap(NULL, len, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    memcpy(out, line.c_str(), len);
    close(fd);
    // shm_unlink("Name Shared Memory");
    munmap(out, len);
    return 0;
}