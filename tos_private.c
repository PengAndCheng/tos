

#include "tos.h"
#include "tos_private.h"

#include <string.h>

struct timeo_memory_node{
    int flag; //使用标记
    struct timeo t;
};

//自己实现内存池
static struct timeo_memory_node timeo_memory_pool[TIMEO_MEMORY_POOL_NODE_SIZE];

static inline void timeo_memory_node_rest(struct timeo_memory_node* node){
    node->flag =0;
    //memset(node,0,sizeof(struct timeo));
}

void timeo_memory_pool_init(void){
    for (int i = 0; i < TIMEO_MEMORY_POOL_NODE_SIZE; ++i) {
        timeo_memory_node_rest(&timeo_memory_pool[i]);
    }
}
struct timeo* timeo_malloc(void){
    for (int i = 0; i < TIMEO_MEMORY_POOL_NODE_SIZE; ++i) {
        if (timeo_memory_pool[i].flag == 0) {
            timeo_memory_pool[i].flag = 1;
            return &timeo_memory_pool[i].t;
        }
    }
    return 0;
}

void timeo_free(struct timeo* timeo){
    for (int i = 0; i < TIMEO_MEMORY_POOL_NODE_SIZE; ++i) {
        if (&timeo_memory_pool[i].t == timeo) {
            timeo_memory_node_rest(&timeo_memory_pool[i]);
        }
    }
}




