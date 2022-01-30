#include <stdio.h>

int sum(int a[], int length){
    int sum = 0;
    for(int i=0; i<length; i++){
       sum+=a[i];
    }
    return sum;
}

int main()
{
    int a[10] = {5,3,-1,8,2,7,12,16,2,6};
    sum(a, 10);

    return 0;
}