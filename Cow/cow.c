#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
struct {
  uint8 ref_cnt;
  struct spinlock lock;
} cows[(PHYSTOP - KERNBASE) >> 12];
// increase the reference count
void increfcnt(uint64 pa) {
  if (pa < KERNBASE) {
    return;
  }
  pa = (pa - KERNBASE) >> 12;
  acquire(&cows[pa].lock);
  ++cows[pa].ref_cnt;
  release(&cows[pa].lock);
}

// decrease the reference count
uint8 decrefcnt(uint64 pa) {
  uint8 ret;
  if (pa < KERNBASE) {
    return 0;
  }
  pa = (pa - KERNBASE) >> 12;
  acquire(&cows[pa].lock);
  ret = --cows[pa].ref_cnt;
  release(&cows[pa].lock);
  return ret;
}

// lab6
uint64 walkcowaddr(pagetable_t pagetable, uint64 va) {
  pte_t *pte;
  uint64 pa;
  char* mem;
  uint flags;

  if (va >= MAXVA)
    return 0;

  pte = walk(pagetable, va, 0);
  if (pte == 0)
      return 0;
  if ((*pte & PTE_V) == 0)
      return 0;
  if ((*pte & PTE_U) == 0)
    return 0;
  pa = PTE2PA(*pte);
  // 判断写标志位是否没有
  if ((*pte & PTE_W) == 0) {
    // pte without COW flag cannot allocate page 
    if ((*pte & PTE_COW) == 0) {
        return 0;
    }
    // 分配新物理页
    if ((mem = kalloc()) == 0) {
      return 0;
    }
    // 拷贝页表内容
    memmove(mem, (void*)pa, PGSIZE);
    // 更新标志位
    flags = (PTE_FLAGS(*pte) & (~PTE_COW)) | PTE_W;
    // 取消原映射
    uvmunmap(pagetable, PGROUNDDOWN(va), 1, 1);
    // 更新新映射
    if (mappages(pagetable, PGROUNDDOWN(va), PGSIZE, (uint64)mem, flags) != 0) {
      kfree(mem);
      return 0;
    }
    return (uint64)mem;    // COW情况下返回新物理地址
  }
  return pa;
}