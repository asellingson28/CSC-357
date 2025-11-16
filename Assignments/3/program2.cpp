// #include <sys/mpi.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <cstdint> //unsigned ints
#include <iostream>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <algorithm>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>
// this is the "main", calls program one
using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 3) return 1;
    std::string prog1name = argv[1];
    int numCalls = atoi(argv[2]);
    char call[100];
    sprintf(call, "./%s", argv[1]);
    char* args[4];
    args[3]=NULL;
    args[2] = argv[2]; // par count
    args[0]=argv[1]; // ./program1
    args[1]=(char*)malloc(100);
    
    for (int i = 0; i < numCalls; i++)
    {
        cout << "Iteration " << i << endl;
        sprintf(args[1],"%d", i); // id (parcount)
        if(fork()==0){

            execv(call, args);
        }
    };
    
    // int fd = shm_open("assignment3", O_RDWR, 0777);
        
    // int flag = shm_open("assignment3flag", O_RDWR, 0777);

    // int fdata = shm_open("assignment3data", O_RDWR, 0777);
    // // int *outflag = (int *)mmap(NULL, 4, PROT_WRITE | PROT_READ, MAP_SHARED, flag, 0); 
    // // BYTE *out = (BYTE *)mmap(NULL, X, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0); // fd makes it shared
    // // BYTE *data = (BYTE *)mmap(NULL, X, PROT_READ | PROT_WRITE, MAP_SHARED, fdata, 0);


    

    // while (*outflag==0) {
        
    // }
    // close(fd);
    // close(flag);
    // shm_unlink("assignment3");
    // shm_unlink("assignment3flag");
    // shm_unlink("assignment3data");
    // munmap(out, size);
    // munmap(outflag, 4);
    // munmap(data, size);
}