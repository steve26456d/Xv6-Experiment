#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
int primes(int fd,int prime) {
    int p[2];
    pipe(p);
    int num;
    if(fork() == 0){
        close(p[0]);
        while(read(fd,&num,4)){
            if(num % prime != 0)
                write(p[1],&num,4);
        }
        close(p[1]);
        exit(0);
    }
    close(fd);
    close(p[1]);
    return p[0];
}
int initPrime(){
    int fd[2];
    pipe(fd);
    if(!fork()){
        close(fd[0]);
        for(int i = 2;i <= 280;i++)
            write(fd[1],&i,4);
        close(fd[1]);
        exit(0);
    }
    close(fd[1]);
    return fd[0]; 
}
int main() {
    int fd = initPrime();
    int prime;
    while(read(fd,&prime,4)){
        printf("prime %d\n",prime);
        fd = primes(fd,prime);
    }
    wait(0);
    exit(0);
}
