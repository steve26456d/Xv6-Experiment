#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
int main(int argc,char** argv)
{
    char buff[2];
    int p1[2];
    int p2[2];
    pipe(p1);
    pipe(p2);
    if(fork() == 0){
	    read(p1[1],buff,1);
	    close(p1[1]);
	    printf("%d: received ping\n",getpid());
	    write(p2[0],buff,1);
	    close(p2[0]);
	    exit(0);
    }
    else{
	    write(p1[0],buff,1);
	    close(p1[0]);
    }
    wait(0);
    read(p2[1],buff,1);
    close(p2[1]);
    printf("%d: received pong\n",getpid());
    exit(0);
}
