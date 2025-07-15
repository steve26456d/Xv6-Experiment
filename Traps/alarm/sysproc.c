#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;
  backtrace();
  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64 
sys_sigreturn(void)
{
  struct proc* p = myproc();
  p->trapframe->ra = p->alarmsaved[0];
  p->trapframe->sp = p->alarmsaved[1];
  p->trapframe->gp = p->alarmsaved[2];
  p->trapframe->tp = p->alarmsaved[3];
  p->trapframe->t0 = p->alarmsaved[4];
  p->trapframe->t1 = p->alarmsaved[5];
  p->trapframe->t2 = p->alarmsaved[6];
  p->trapframe->s0 = p->alarmsaved[7];
  p->trapframe->s1 = p->alarmsaved[8];
  p->trapframe->a0 = p->alarmsaved[9];
  p->trapframe->a1 = p->alarmsaved[10];
  p->trapframe->a2 = p->alarmsaved[11];
  p->trapframe->a3 = p->alarmsaved[12];
  p->trapframe->a4 = p->alarmsaved[13];
  p->trapframe->a5 = p->alarmsaved[14];
  p->trapframe->a6 = p->alarmsaved[15];
  p->trapframe->a7 = p->alarmsaved[16];
  p->trapframe->s2 = p->alarmsaved[17];
  p->trapframe->s3 = p->alarmsaved[18];
  p->trapframe->s4 = p->alarmsaved[19];
  p->trapframe->s5 = p->alarmsaved[20];
  p->trapframe->s6 = p->alarmsaved[21];
  p->trapframe->s7 = p->alarmsaved[22];
  p->trapframe->s8 = p->alarmsaved[23];
  p->trapframe->s9 = p->alarmsaved[24];
  p->trapframe->s10 = p->alarmsaved[25];
  p->trapframe->s11 = p->alarmsaved[26];
  p->trapframe->t3 = p->alarmsaved[27];
  p->trapframe->t4 = p->alarmsaved[28];
  p->trapframe->t5 = p->alarmsaved[29];
  p->trapframe->t6 = p->alarmsaved[30];
  p->trapframe->epc = p->alarmsaved[31];
  p->isalarmrt = 1;
  return p->trapframe->a0;
}

uint64 
sys_sigalarm(void)
{
  uint64 addr;
  int n;
  argaddr(1, &addr);
  argint(0,&n);
  struct proc* p = myproc();
  p->alarminterval = n;
  p->handler = addr;
  // printf("n: %d, addr: %p\n",n,(void*)addr);
  if(n == 0 && addr == 0)
    p->isalarm = 0;
  else{
    p->isalarm = 1;
  }
  return 0;
}