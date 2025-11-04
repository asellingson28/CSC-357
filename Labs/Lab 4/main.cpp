// Exercise:
// Have an inactivity detector.
// Your program should read sentences (!) from the keyboard and print the text right away, but
// with exclamation marks at the beginning and the end of the sentence.
// If the user is not entering anything for 10 seconds (to make it easier, at least 10 seconds),
// overwrite stdin and print out “no activity detected!”. Go back into read mode and wait for
// another 10 seconds and so on.
// HowTo:
// Fork into a parent and a child.
// Have the parent in an infinite loop of reading, text updating and printing.
// Have a mechanic (shared mem?) to tell the child process if an activity happened.
// The child process is in an infinite loop as well, waiting for 10 seconds. If no activity was
// reported by the parent process, overwrite stdin and print out “no activity detected”, followed
// by restoring stdin for letting the parent process read normally from the keyboard again.
// If you are entering “quit” the program should end!
// The text above contains ALL necessary information regarding the program. Read and
// understand it carefully.
// Use functions. E.g. for updating the text


#include <string>
#include <iostream>
#include <fstream>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <signal.h>
#include "../../../../../../../Library/Developer/CommandLineTools/SDKs/MacOSX15.5.sdk/usr/include/unistd.h"
#include "../../../../../../../Library/Developer/CommandLineTools/SDKs/MacOSX15.5.sdk/usr/include/signal.h"
using namespace std;

enum class STATUS {
    Active,
    Inactive
};

struct Shared {
    volatile STATUS status;
    volatile int terminate;
};

string add_exclamations(string& s) {
    return "!" + s + "!";
}


int main(int argc, char* argv[]) {

    int fd[2];
    pipe(fd);

    int readEnd = fd[0];
    int writeEnd = fd[1];
    int stdoutdup = dup(1);
    Shared* shm = (Shared*) mmap(NULL, sizeof(Shared), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    shm->terminate = 0;
    shm->status = STATUS::Inactive;
    pid_t pid = fork();

    if (pid == 0) {
        // Child writes to p[1]. Child's stdin remains the real terminal.
        close(readEnd); // child doesn't read from the pipe
        
        // wait 10 s
        // if more than 10 s
            // overwrite stdin
            // print no activity detected
            // reinstate std in
        while(!shm->terminate) {
            for (int i = 0; i < 10; ++i) {     // check every 1s for 10s total
                sleep(1);
                if (shm->terminate) break;
                if (shm->status == STATUS::Active) { // if active, turn inactive and restart for loop
                    shm->status = STATUS::Inactive;
                    i = -1;                     // restart counting
                }
            }
            if(shm->terminate) break; // if quit detected
            if (shm->status == STATUS::Inactive) {
                const char* msg = "no activity detected!\n";
                write(STDOUT_FILENO, msg, strlen(msg));
            }

            shm->status = STATUS::Inactive;
        }
        _exit(0); // exit child cleanly
    
    } else {
        // parent
        close(writeEnd);
        string line;
        getline( std::cin, line );
        dup2(stdoutdup, writeEnd);
        close(readEnd);
        shm->status=STATUS::Active;
        while (line != "quit"){
            cout << "line recieved" << endl;
            cout << add_exclamations(line) << endl;
            getline( std::cin, line );
            shm->status=STATUS::Active;
        }
        shm->terminate = 1;
        kill(pid, SIGUSR1);
    }
    return 0;
}
