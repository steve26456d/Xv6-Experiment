#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]){
    // for(int i = 0;i < argc;i++){
    //     printf("%ld\n",sizeof(argv[i]));
    // }
    // char* arg[3];
    // arg[0] = "echo";
    // arg[1] = "yes";
    // arg[2] = 0;
    // exec("echo",arg);
    // printf("exec error\n");
    if(argc < 2){
        fprintf(2,"xargs: no command...\n");
        exit(1);
    }
    char argk[MAXARG][8];
    char* arg[MAXARG];
    char temp;
    int i = 0;
    argk[0][0] = '\0';
    while(read(0,&temp,1)){
        if(temp == '\n'){
            // printf("3\n");
            if(fork() == 0){
                if(strlen(argk[0]) == 0){
                    fprintf(2,"xargs: no arg...\n");
                    exit(1);
                }
                int k = 0;
                for(int j = 1;j < argc;j++,k++)
                    arg[k] = argv[j];
                for(int l = 0;l <= i;l++,k++){
                    arg[k] = argk[l];
                    // printf("%s\n",argk[l]);
                }
                k++;
                arg[k] = 0;
                // for(int m = 0;m < k;m++)
                //     printf("%s\n",arg[m]);
                exec(argv[1],arg);
                fprintf(2,"exec error\n");
            }
            wait(0);
            for(int j = 0;j < MAXARG;j++)
                argk[j][0] = '\0';
            i = 0;
        }else if(temp == ' '){
            // printf("1\n");
            i++;
            if(i > MAXARG){
                fprintf(2,"xargs: exceed the maxarg...\n");
                exit(1);
            }
            argk[i][0] = '\0';
        }else{
            // printf("2\n");
            int pos = strlen(argk[i]);
            if(pos > sizeof(argk[i]) - 1){
                fprintf(2,"xargs: arg is too long...\n");
                exit(1);
            }
            *(argk[i] + pos) = temp;
            *(argk[i] + pos + 1) = '\0'; 
            //printf("%s\n", argk[i]);
        }
    }
    exit(0);
}