#include <iostream>
using namespace std;
// build : cmd shift b
int main() {
    int a = 5;
    cout << "a: " << a << '\n';
    int *pa = &a;
    cout << "*pa: " << *pa << '\n';
    cout << "pa: " << pa << '\n';  
    int &b = a; // read as int b is a reference to a
    b = 10; // b is 10 thus a is 10 
    cout << "b:" << b << '\n';
    // TOODO
    cout << "a:" << a << '\n';
    cout << "b:" << b << '\n';


    // // passing an address to a function changes the value directly
    // char text[] = "Titanic";

    // char *p = text;

    // char c = *(p + 1);

    // printf("%c",c);


}