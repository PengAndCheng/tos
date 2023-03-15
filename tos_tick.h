
#ifndef _TOS_TICK_H_
#define _TOS_TICK_H_

/**********************需要移植 开始****************************/
extern unsigned int SysTick_ms;
#define tos_tick_ms SysTick_ms
#define tos_tick_s
#define tos_tick_m
#define tos_tick_h
#define tos_tick_d
#define tos_tick_y

//最常用的就这个ms API便以填写
#define tick_ms tos_tick_ms
/**********************需要移植 结束****************************/

#endif /* _TOS_TICK_H_ */
