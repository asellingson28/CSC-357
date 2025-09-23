struct myStruct{
    char i;
    float y; // takes up 8 BYTES because of overflow in memory
    // called padding
};

void fct(myStruct *p){
    p->i = 99; // accessing the address, not the instance passed
};

int main()
{
    // char = 1 byte
    // int = 4 bytes
    // short = 2 bytes
    // float = 4 bytes
    // double = 4-8 bytes (varies by os)

    int a[10]; // consecutive location in memory (this creates an array of 10 ints)
    // &a[0] (address of a0) = a (starting pointer of a) 
    // *a = 88 <-> a[2] = 88

    *(a+2) = 99; // (plus 2 means 2*(size of int) ) 
    //INTERNALLY:  *(a + i * sizeof(DT))
    // same as 
    a[2] = 99;
     // offsetting an integer will off set it by 2 (or however many) bytes of a data type



    return 0;
}
