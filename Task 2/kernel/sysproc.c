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

  argint(0, &n);
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
sys_channel_create(void){
  
  for(int i=0; i<NCHANNEL;i++){
    acquire(&chann[i].lockk);
    if(chann[i].is_exist == 0){
      chann[i].is_exist = 1 ;
      struct proc* my_proc= myproc();
      chann[i].my_process= my_proc->pid;
      release(&chann[i].lockk);
      return i;
    }
    release(&chann[i].lockk);
  }
  return -1;
}

uint64
sys_channel_put(void){
  int cd;
  int data;
  argint(0, &cd);
  argint(1, &data);
    if (cd >= 0 && cd < NCHANNEL){
      acquire(&chann[cd].lockk);
      while (chann[cd].is_exist==1 && chann[cd].is_full==1) {
        sleep(&chann[cd], &chann[cd].lockk);
      }
      if(chann[cd].is_exist ==1){
        chann[cd].data = data;
        chann[cd].is_full = 1; 
        wakeup(&chann[cd]);
        release(&chann[cd].lockk);
      }
      else{
        release(&chann[cd].lockk);
        return -1;
      }
    }
    else{
      return -1;
    }
    return 0;
}

uint64
sys_channel_take(void){
  int cd;
  uint64 data;
  argint(0, &cd);
  argaddr(1, &data);
  if (cd >= 0 && cd < NCHANNEL){
    acquire(&chann[cd].lockk);
    while (chann[cd].is_exist==1  && chann[cd].is_full==0) {
      sleep(&chann[cd], &chann[cd].lockk);
    }
    if(chann[cd].is_exist ==1){
      if(copyout(myproc()->pagetable, (uint64)data, (char*)&chann[cd].data, sizeof(int)) < 0) {
        release(&chann[cd].lockk);
        return -1;
      }
      chann[cd].is_full = 0; 
      wakeup(&chann[cd]);
      release(&chann[cd].lockk);
    }
    else{
      release(&chann[cd].lockk);
      return -1;
    }
  }
  else{
    return -1;
  }
  return 0;
}


uint64
sys_channel_destroy(void){
  int cd;
  argint(0, &cd);
    if (cd >= 0 && cd < NCHANNEL){
      acquire(&chann[cd].lockk);
      if(chann[cd].is_exist ==1){
        chann[cd].is_exist=0;
        chann[cd].is_full=0;
        wakeup(&chann[cd]); 
        release(&chann[cd].lockk);
      }
      else{
        release(&chann[cd].lockk);
        return -1;
    }
  }
  else{
    release(&chann[cd].lockk);
    return -1;
  }
  return 0;
}