#include <stdio.h>

int minimum(int a[], int length){
    int min = a[0];
    for(int i=0; i<length; i++){
        if(a[i] < min){
            min = a[i];
        }
    }
    return min;
}

int main()
{
    int a[5] = {5,3,-1,8,2};
    minimum(a, 5);

    return 0;
}