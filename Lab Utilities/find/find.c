#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  buf[strlen(p)] = 0;
  return buf;
}
void
find(char* dir,const char *name)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(dir, O_RDONLY)) < 0){
    fprintf(2, "find: cannot open %s\n", dir);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", dir);
    close(fd);
    return;
  }

  if(strlen(dir) + 1 + DIRSIZ + 1 > sizeof buf){
    printf("find: dir too long\n");
    close(fd);
    return;
  }
  strcpy(buf, dir);
  p = buf+strlen(buf);
  *p++ = '/';
  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(de.inum == 0)
      continue;
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;
    if(stat(buf, &st) < 0){
      printf("find: cannot stat %s\n", buf);
      continue;
    }
    const char* n = fmtname(buf);
    // printf("n: %s\n",n);
    if(st.type == T_DIR && strcmp(n,".") != 0 && strcmp(n,"..") != 0)
      find(buf,name);
    else{
      if(strcmp(n,name) == 0){
        printf("%s\n",buf);
      }
    }
  }
  close(fd);
}
int main(int argc,char** argv)
{
  if(argc < 2){
    fprintf(2,"find: please input dir name and file name...\n");
    exit(0);
  } else if(argc < 3){
    fprintf(2,"find: please input file name...\n");
    exit(0);
  }
  for(int i = 2;i < argc;i++)
      find(argv[1],argv[i]);
  exit(0);
}
