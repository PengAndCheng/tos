
#ifndef _TOS_PRIVATE_H_
#define _TOS_PRIVATE_H_



//断言
#define TOS_ASSERT(x,y)

//定时器内存池大小 目前算法使用的就是个简单的数组 有时间再改进成内存池算法吧
#define TIMEO_MEMORY_POOL_NODE_SIZE 16

void timeo_memory_pool_init(void);
struct timeo* timeo_malloc(void);
void timeo_free(struct timeo* timeo);

#endif /* _TOS_PRIVATE_H_ */
