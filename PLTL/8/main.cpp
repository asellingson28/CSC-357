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
#include "../../../../../../../Library/Developer/CommandLineTools/SDKs/MacOSX15.5.sdk/usr/include/unistd.h"
// this is the "main", calls program one
using namespace std;
typedef unsigned char BYTE;

//note:
//"ready" is a named shared array as big as par_count which needs to be initialized with zeros.
//Probably wait a second before forking the kids, so proc 0 can zero the "ready" array





int main(int argc, char *argv[]) {
    if (argc != 6) return 1;

    
    std::string prog = argv[1];
    int n = atoi(argv[2]); // numcalls
    int arg1 = atoi(argv[3]);
    int arg2 = atoi(argv[4]);
    
    int fd = shm_open("zeroarray", O_CREAT|O_RDWR, 0777);
    ftruncate(fd, (n+1)*sizeof(int));
    int *map = (int*)mmap(NULL, (n+1)*sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    // zero the shared mapping and expose it as an int array

    char call[100];
    sprintf(call, "./%s", argv[1]);
    char* args[5];
    args[0]=(char*)malloc(100);
    args[1]=(char*)malloc(100);
    args[2]=(char*)malloc(100);
    args[3]=(char*)malloc(100);
    args[4]=(char*)malloc(100);
    args[3]=argv[5];
    // sprintf(args[4], "%d", arg3);

    args[4]=NULL;
    args[0]=argv[1]; // name of prog 
    int p = (arg1 * arg2);
    sprintf(args[2], "%d", p); // num times
    args[1]=(char*)malloc(100); // allocates the memory
    for (int i = 0; i < n; i++)
    {
        sprintf(args[1],"%d", i); // assigns id

        
        if(fork()==0){
            execv(call, args);
        }
    };

    munmap(map, (n+1)*sizeof(int));
    close(fd);
    shm_unlink("zeroarray");
    return 0;
}


