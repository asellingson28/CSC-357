#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
int main() {
    int fd[2];
    pipe(fd);  // create pipe

    pid_t pid = fork();  // create child process

    if (pid == 0) { // child process
        close(fd[0]);  // close unused read end
        const char* msg = "Hello from child";
        write(fd[1], msg, 17);
        close(fd[1]);
    } else { // parent process
        close(fd[1]);  // close unused write end
        char buffer[100];
        read(fd[0], buffer, sizeof(buffer));
        std::cout << "Parent received: " << buffer << std::endl;
        close(fd[0]);
        wait(nullptr); // wait for child
    }
    return 0;
}

