// Ellingson, Arjan
// Karyou, Michel
// Quezon, Berfredd
// Garcia, Cis

#include <string>
#include <iostream>
#include <fstream>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

int main(int argc, char* argv[]) {

    char* input = argv[1];
    char* output = argv[2];

    char* data = (char*) mmap(NULL, 1000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    // fread(data, sizeof(data), 1, inputf);
    
    int pipen1[2];
    pipe(pipen1);
    
    // int stdindup = dup(0);
    int stdoutdup = dup(1);
    
    int pipe2[2];
    pipe(pipe2);
    
    pid_t pid = fork();
    
    if (pid == 0) // child
    {
        FILE* inputf = fopen(input, "rb");
        int bytesR;
        while ((bytesR = fread(data, 1, 1000, inputf) ) > 0)
        {
            write(pipe2[1], data, bytesR); // writes to output of pipe
        }
        fclose(inputf);
        close(pipe2[1]);
        exit(0);
    }
    wait(0);
    close(pipe2[1]);
    
    if (strcmp(output, "terminal") == 0) {
        int bytesR;
        while ((bytesR = read(pipe2[0], data, 1000)) > 0)
        {
            dup2(stdoutdup, pipen1[1]); // copies behavior from stdout to pipenum
            write(pipen1[1], data, bytesR); // writes to output of pipe
            // cout << data;
        }
        
    } else {
        int bytesR;
        FILE* outfile = fopen(output, "wb");
        while ((bytesR = read(pipe2[0], data, 1000)) > 0)
        {
            dup2(fileno(outfile), pipen1[1]); // copies behavior from stdout to pipenum
            write(pipen1[1], data, bytesR); // writes to output of pipe
            cout<< bytesR << endl;
        }
        
        fclose(outfile);
    }
    wait(0);
    return 0;
}