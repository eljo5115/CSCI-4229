#include <stdio.h>
#include <sys/types.h> 
#include <unistd.h> 

int main(){
    int pid1 = fork();
    int pid2 = fork();
    printf("Hello\n");
    if(!pid1 && !pid2){
        printf("World\n");
    }
    return 0;
}