
#ifndef _TOS_H_
#define _TOS_H_

#include <stdint.h>

#include "tos_tick.h"


//超时回调函数的函数原型
typedef void (* timeout_handler)(void *arg);


//中定义定时器的结构——sys_timeo
//其中包含下一个定时器指针
//当前定时器时间//
//超时处理函数
struct timeo {
  struct timeo *next;
  uint32_t time;
  timeout_handler h;
  void *arg;
};





void tos_timeout(uint32_t msecs, timeout_handler handler, void *arg);
void tos_check_timeouts(void);
void tos_timeouts_init(void);

//查询最近定时器还剩多少tick
uint32_t tos_timeouts_sleeptime(void);
//这个不是重启某个定时器 而是将所有的定时器以现在重新计时
void tos_restart_timeouts(void);

//删除定时器 这个可以从链表中剔除定时器
void tos_untimeout(timeout_handler handler, void *arg);
//同上 只是不不比较参数
void tos_untimeout_handler(timeout_handler handler);
//想要改变某个定时时可以先删除 再重新增加

void tos_run(void);



/*****************上面是回调模式 下面是轮询函数内模式*******************************/
//循环超时，这个函数直接用在函数内部，技巧：staticTickFlag初始值回退一个间隔就可以第一次执行了
uint32_t tos_loop_timeout_ms(uint32_t* staticTickFlag, uint32_t msecs);
#define loop_timeout_ms(staticTickFlag,msecs) tos_loop_timeout_ms(staticTickFlag,msecs)

//POST API
#define     POST_FLAG_ADD_NEW          0           // 默认，添加新post，不论post列表中是否存在相同handler
#define     POST_FLAG_CLEAR_FRONT      1           // post列表中，存在相同handler, 清除之前的post
#define     POST_FLAG_CANCEL_CURRENT   2           // post列表中，存在相同handler, 取消当前post


#define post(handler,arg) tos_timeout(0,handler,arg)
#define postDelay(msecs, handler, arg) tos_timeout(msecs,handler,arg)
void postDelayEx(uint32_t msecs, timeout_handler handler, void *arg, uint32_t FLAG);
#define postDelete(handler) tos_untimeout_handler(handler)



#endif /* _TOS_H_ */
