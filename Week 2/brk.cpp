#include <unistd.h>
int main() {
    void *PB = sbrk(0);
    void *p = sbrk(32); // p is still the address where PB is 
    p = sbrk(0); // new p address
    brk(PB); 
    p = sbrk(0);

    return 0;

}