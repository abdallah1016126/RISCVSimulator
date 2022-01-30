#include <stdio.h>

int gcd(int a, int b){
    
    if(a==b) return a;

    if(a>b) gcd(a-b,b);
    else gcd(a,b-a);
    
}

int main()
{
    int x =20;
    int y=4;
    gcd(x,y);

    return 0;
}
