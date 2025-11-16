// #include <sys/mpi.h>
#include <sys/mman.h>
#include <iostream>
#include <stdlib.h>
#include "det.h"
#include <string.h>
#include <fcntl.h>
#include <cstdint> //unsigned ints
#include <iostream>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <vector>
#include <random>
using namespace std;
typedef unsigned char BYTE;

vector<vector<int>> create_matrix(int n, int minVal = 0, int maxVal = 9) {
    vector<vector<int>> matrix(n, vector<int>(n)); // more common nested array

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            matrix[i][j] = minVal + rand() % (maxVal - minVal + 1);
        }
    }
    return matrix;
}


//note:
//"ready" is a named shared array as big as par_count which needs to be initialized with zeros.
//Probably wait a second before forking the kids, so proc 0 can zero the "ready" array
void synch(int par_id, int par_count, int* ready) {
    int synchid = ready[par_count] + 1;
    ready[par_id] = synchid;
    int breakout = 0;
    while (1) {
        breakout = 1;
        for (int i = 0; i < par_count; i++) {
            if (ready[i] < synchid) { breakout = 0; break; }
        }
        if (breakout == 1) {
            ready[par_count] = synchid;
            break;
        }
    }
}




inline int idx(int n, int r, int c) { return r*n + c; }

int* A_ptr(int* base) {
    int n = base[0];
    return base + 3;
}

int* B_ptr(int* base) {
    int n = base[0];
    return base + 3 + n*n;
}

int* M_ptr(int* base) {
    int n = base[0];
    return base + 3 + 2*n*n;
}

void print_matrix(const int* mat, int n, string name) {
    cout << "\n" << name << ":\n";
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            cout << mat[idx(n, r, c)] << "\t";
        }
        cout << "\n";
    }
}

void multiply_into(int* D, const int* L, const int* R,
                          int n, int par_id, int par_count) {
    for (int row = par_id; row < n; row += par_count) {
        int* Dout = D + row*n;
        const int* Lrow = L + row*n;
        for (int col = 0; col < n; ++col) {
            int sum = 0;
            for (int k = 0; k < n; ++k)
                sum += Lrow[k] * R[k*n + col];
            Dout[col] = sum;
        }
    }
}
int main(int argc, char *argv[]) { 
    // if (argc != 4) return 1;
    // cout << "test" << endl;
    char* progname = argv[0]; // prog name
    int par_id = atoi(argv[1]); // par id
    int par_count = atoi(argv[2]); // numcalls
    
    const int MATRIX_SIZE = 2; // changes matrix size
    const char* SHM_NAME = "matrix";
    size_t ints_needed = 3 + 3*MATRIX_SIZE*MATRIX_SIZE;
    size_t bytes = ints_needed * sizeof(int);

    int fd;
    int* map;
    const char* READY_NAME = "ready_array";                
    size_t ready_ints = (par_count + 1);                   
    size_t ready_bytes = ready_ints * sizeof(int);         

    int fd_ready;                                          
    int* ready;    
    if (par_count > MATRIX_SIZE) {
        cerr << "Error: par_count (" << par_count 
            << ") cannot exceed MATRIX_SIZE (" << MATRIX_SIZE << ")." << endl;
        return 1;
    }
    // map[0] = MATRIXSIZE
    // map[1] = spare (ready?) 
    // map[3] = spare (num completed rows?)
    // map[4...?] A
    // map[?...?] B
    // map[?...?] M
    // vector<vector<int>> A_vec;
    // vector<vector<int>> B_vec;

    // first call inits the shared mem and creates matrix
    if (par_id == 0) {
        fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0777);
        ftruncate(fd, bytes);
        map = (int*)mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        map[0] = MATRIX_SIZE;

        // generate A,B locally then copy into shm
        srand(time(0) ^ getpid() ^ par_id);
        vector<vector<int>> A_vec = create_matrix(MATRIX_SIZE);
        vector<vector<int>> B_vec = create_matrix(MATRIX_SIZE);

        int n = map[0];
        int* A = A_ptr(map);
        int* B = B_ptr(map);
        int* M = M_ptr(map);

        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) {
                A[idx(n,r,c)] = A_vec[r][c];
                B[idx(n,r,c)] = B_vec[r][c];
                M[idx(n,r,c)] = 0; 
            }
        }
    } else {
        while (true) {
            fd = shm_open(SHM_NAME, O_RDWR, 0777);
            if (fd != -1) break;                    // success → exit loop

            usleep(1000); // wait for creator
        }

        map = (int*)mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    }
    // synch(par_id, par_count, ready);

    //create ready array
    if (par_id == 0) {
        fd_ready = shm_open(READY_NAME, O_CREAT | O_RDWR, 0777);
        ftruncate(fd_ready, ready_bytes);
        ready = (int*)mmap(NULL, ready_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd_ready, 0);
        memset(ready, 0, ready_bytes);  // zero all par_count slots + generation slot
    } else {
        while (true) {
            fd_ready = shm_open(READY_NAME, O_RDWR, 0777);
            if (fd_ready != -1) break;
            usleep(1000);
        }
        ready = (int*)mmap(NULL, ready_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd_ready, 0);
    }
    synch(par_id, par_count, ready);

    int* As = A_ptr(map);
    int* Bs = B_ptr(map);
    int* Ms = M_ptr(map);
    int n = map[0];
    // M = A * B
    multiply_into(Ms, As, Bs, n, par_id, par_count);
    synch(par_id, par_count, ready);
    print_matrix(As, MATRIX_SIZE, "A");
    print_matrix(Bs, MATRIX_SIZE, "B");
    print_matrix(Ms, MATRIX_SIZE, "M");
    // B = M * A   
    multiply_into(Bs, Ms, As, n, par_id, par_count);
    synch(par_id, par_count, ready);
    print_matrix(As, MATRIX_SIZE, "A");
    print_matrix(Bs, MATRIX_SIZE, "B");
    print_matrix(Ms, MATRIX_SIZE, "M");
    
    // M = B * A   (uses the updated B)
    multiply_into(Ms, Bs, As, n, par_id, par_count);
    synch(par_id, par_count, ready);
    print_matrix(As, MATRIX_SIZE, "A");
    print_matrix(Bs, MATRIX_SIZE, "B");
    print_matrix(Ms, MATRIX_SIZE, "M");

    if (par_id == 0) {
        float M_f[n * n];
        for (int i = 0; i < n * n; ++i) {
            M_f[i] = (float)Ms[i];
        }
        float detM = det(M_f, n, n);
        printf("det(M) = %.2f\n", detM);
        munmap(map, bytes);
        close(fd);
        shm_unlink(SHM_NAME);

        munmap(ready, ready_bytes);
        close(fd_ready);
        shm_unlink(READY_NAME);
    } else {
        munmap(map, bytes);
        close(fd);

        munmap(ready, ready_bytes);
        close(fd_ready);
    }
    return 0;

}



// gather();
// m=A*B;
// gather();
// B=M*A;
// gather();
// m=B*A;
// gather print m
