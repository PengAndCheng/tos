
#include "tos.h"
#include "tos_private.h"

/**********************目前适用32位 且tick满32位自动溢出 源码中NULL直接用0*******************************/

//最大定时单位ms
#define TOS_UINT32_MAX 0xffffffff
//查询最近的超时还剩多少tick时 没有定时器就返回这个值
#define TOS_TIMEOUTS_SLEEPTIME_INFINITE 0xFFFFFFFF

#define TOS_MAX_TIMEOUT  0x7fffffff
//检查计时器的到期时间是否大于时间，并关注uint32_t循环 因为uint32_t会溢出 这样设计就是比较大小吧 小减大会得负数 负数高位为1
#define TIME_LESS_THAN(t, compare_to) ( (((uint32_t)((t)-(compare_to))) > TOS_MAX_TIMEOUT) ? 1 : 0 )


//全局定时器链表头
static struct timeo *next_timeout;
//这个还不知道意图
static uint32_t current_timeout_due_time;


static void tos_timeout_abs(uint32_t abs_time, timeout_handler handler, void *arg)
{
  struct timeo *timeout, *t;

  //创建一个定时器
  timeout = (struct timeo *)timeo_malloc();

  if (timeout == 0) {
    TOS_ASSERT("sys_timeout: timeout != NULL, pool MEMP_SYS_TIMEOUT is empty", timeout != NULL);
    return;
  }

  timeout->next = 0;
  timeout->h = handler;
  timeout->arg = arg;
  timeout->time = abs_time;

  if (next_timeout == 0) {
    next_timeout = timeout;
    return;
  }
  if (TIME_LESS_THAN(timeout->time, next_timeout->time)) {
    timeout->next = next_timeout;
    next_timeout = timeout;
  } else {
    for (t = next_timeout; t != 0; t = t->next) {
      if ((t->next == 0) || TIME_LESS_THAN(timeout->time, t->next->time)) {
        timeout->next = t->next;
        t->next = timeout;
        break;
      }
    }
  }
}

//定时器添加
void tos_timeout(uint32_t msecs, timeout_handler handler, void *arg)
{
    uint32_t next_timeout_time;

    TOS_ASSERT("Timeout time too long, max is LWIP_UINT32_MAX/4 msecs", msecs <= (TOS_UINT32_MAX / 4));

    next_timeout_time = (uint32_t)(tos_tick_ms + msecs); //下一次超时tick

    tos_timeout_abs(next_timeout_time, handler, arg);
}

//定时器检查及超时操作 将最小时间放到链表头来的设计就是为了快速遍历
void tos_check_timeouts(void)
{
  uint32_t now;

  now = tos_tick_ms;

  do {
    struct timeo *tmptimeout;
    timeout_handler handler;
    void *arg;

    tmptimeout = next_timeout;
    if (tmptimeout == 0) {
      return;
    }

    if (TIME_LESS_THAN(now, tmptimeout->time)) {
      return;
    }

    //能运行到这里 超时已经过期
    next_timeout = tmptimeout->next;
    handler = tmptimeout->h;
    arg = tmptimeout->arg;
    current_timeout_due_time = tmptimeout->time;

    timeo_free(tmptimeout);
    if (handler != 0) {
      handler(arg);
    }

    //重复此操作，直到调用所有过期计时器
  } while (1);
}

//查询最近定时器还剩多少tick
uint32_t tos_timeouts_sleeptime(void)
{
    uint32_t now;

    if (next_timeout == 0) {
        return TOS_TIMEOUTS_SLEEPTIME_INFINITE;
    }
    now = tos_tick_ms;
    if (TIME_LESS_THAN(next_timeout->time, now)) {
        return 0;
    } else {
        uint32_t ret = (uint32_t)(next_timeout->time - now);
        TOS_ASSERT("invalid sleeptime", ret <= TOS_MAX_TIMEOUT);
        return ret;
  }
}

//这个不是重启某个定时器 而是将所有的定时器以现在重新计时
void tos_restart_timeouts(void)
{
  uint32_t now;
  uint32_t base;
  struct timeo *t;

  if (next_timeout == 0) {
    return;
  }

  now = tos_tick_ms;
  base = next_timeout->time;

  for (t = next_timeout; t != 0; t = t->next) {
    t->time = (t->time - base) + now;
  }
}


void tos_untimeout(timeout_handler handler, void *arg)
{
  struct timeo *prev_t, *t;

  if (next_timeout == 0) {
    return;
  }

  for (t = next_timeout, prev_t = 0; t != 0; prev_t = t, t = t->next) {
    if ((t->h == handler) && (t->arg == arg)) {
      //将定时器从链表中删除
      if (prev_t == 0) {
        next_timeout = t->next;
      } else {
        prev_t->next = t->next;
      }
      timeo_free(t);
      return;
    }
  }
  return;
}

void tos_untimeout_handler(timeout_handler handler)
{
  struct timeo *prev_t, *t;

  if (next_timeout == 0) {
    return;
  }

  for (t = next_timeout, prev_t = 0; t != 0; prev_t = t, t = t->next) {
    if ((t->h == handler)) {
      //将定时器从链表中删除
      if (prev_t == 0) {
        next_timeout = t->next;
      } else {
        prev_t->next = t->next;
      }
      timeo_free(t);
      return;
    }
  }
  return;
}



void tos_timeouts_init(void)
{
    timeo_memory_pool_init();
    next_timeout = 0;
}


static int tos_is_init = 0;
void tos_run(void){
    if (tos_is_init == 0) {
        tos_is_init = 1;
        tos_timeouts_init();
    }

    tos_check_timeouts();
}










uint32_t tos_loop_timeout_ms(uint32_t* staticTickFlag, uint32_t msecs){
    uint32_t nowTick = tos_tick_ms;
    uint32_t overflowTick = *staticTickFlag + msecs;
    if (TIME_LESS_THAN(nowTick,overflowTick))
    {
        return 0;
    }else {
        *staticTickFlag = nowTick;
        return 1;
    }
}


void postDelayEx(uint32_t msecs, timeout_handler handler, void *arg, uint32_t FLAG){
    if (FLAG == POST_FLAG_CLEAR_FRONT) {
        tos_untimeout_handler(handler);
        tos_timeout(msecs,handler,arg);
    }else if (FLAG == POST_FLAG_CANCEL_CURRENT) {
        //啥也不干
    }else {
        //默认
        tos_timeout(msecs,handler,arg);
    }
}
