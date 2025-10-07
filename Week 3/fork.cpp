#include <sys/mman.h> // mmap
#include <iostream>
#include <unistd.h> // pid

int main() {
    // fork does multitasking
    // example - analyzing or altering a bitmap twice as fast by splitting the image into 2
    // if fork == 0
    //          then you are in the kid


    // mmap replaces malloc

    int *i = (int*) mmap(NULL, 4, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0); // address if you know, size bytes, flag, flag, flag, flag
    int pid = fork(); // program id
    if (pid == 0) {
        std::cout<<"kid: " << getpid() << std::endl;
        *i=99; // works bc mmap
        return 0;
    }
    std::cout << "parent: " << getpid()<< std::endl;
    wait(0);
    std::cout<< *i << std::endl;

    return 0;
}