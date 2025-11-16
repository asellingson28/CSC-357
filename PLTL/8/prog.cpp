// #include <sys/mpi.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
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
#include <sys/stat.h>
// this is the "main", calls program one
using namespace std;


typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;
typedef unsigned char BYTE;


void synch(int par_id, int par_count, int* ready)
	{

	int synchid = ready[par_count] + 1;
	ready[par_id] = synchid;
	int breakout = 0;
	while (1)
		{

		breakout = 1;
		for (int i = 0; i < par_count; i++)
			{

			if (ready[i] < synchid)
				{
				breakout = 0;
				break;
				}
			}
		if (breakout == 1)
			{
			ready[par_count] = synchid;
			break;
			}
		}
	}


int main(int argc, char *argv[]) { 
    cout << "hello" << endl;
    char* progname = argv[0]; // prog name
    int parID = atoi(argv[1]); // par id
    int numcalls = atoi(argv[2]); // numcalls
    char* word = argv[3];

    int fd = shm_open("zeroarray", O_RDWR, 0777);
    // ftruncate(fd, 4096);

    int *map = (int*) mmap(NULL, (numcalls+1)*sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    cout << "hi"<<endl;
    memset(map, 0, (numcalls+1)*sizeof(int));
    cout << "bye"<<endl;
    int *zeroarray = reinterpret_cast<int*>(map);
    int *ready = zeroarray; // use 'ready' when calling synch


    for (int i = 0; i < numcalls; i++)
    {
        /* code */
        // gather();
        synch(parID, numcalls, map);

        cout << "I am child: " << parID << " and the word is " << word << " and this is itteration " << i << endl;
    }
    // cout << "test" << endl;
    munmap(map, (numcalls+1)*sizeof(int));
    close(fd);
    return 0;
}


