// MPI

#include <sys/mman.h>
#include <sys/mpi.h>

// named shared memeory
// shared memory p1 and p2
int main() {

    int fd = shm_open("abc", ORDWR | O_CREAT, 0777);
    ftruncate(fd, 1000); // 1000 bytes
    BYTE *data = mmap (0, 1000, PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0);
    // need to singal that P2 is done



    // matrix multiplication
    for (size_t k = 0; k < 3, k++) {
        for (size_t u = 0; u < 3; u++)
        {
            for (size_t i = 0; i < 3; i++)
            {
                R[k][u] += A[i][u] * B[k][i]
            }
            
        }
    }   
        
}