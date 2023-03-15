
#include "event.h"
#include <stddef.h> /* 定义了size_t */
#include <stdio.h>

#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6000000)
    extern const size_t Event$$Base;
    extern const size_t Event$$Limit;
#elif defined(__ICCARM__) || defined(__ICCRX__)
    #pragma section="Event"
#elif defined(__GNUC__)
    extern const size_t __event_start;
    extern const size_t __event_end;
#endif



/**
 * @brief event注册表
 */
struct 
{
#if EVENT_SPEED_OPTIMIZE == 1
    //优化代码写这里
#else
    Event *base;                       /**< 表基址 */
    size_t count;                       /**< 表大小 */
#endif
} eventTable;

/**
 * @brief event初始化
 */
void eventInit(void)
{
    Event *base;
    size_t count;
#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6000000)
    base = (Event *)(&Event$$Base);
    count = ((size_t)(&Event$$Limit) - (size_t)(&Event$$Base)) / sizeof(Event);
#elif defined(__ICCARM__) || defined(__ICCRX__)
    base = (Event *)(__section_begin("Event"));
    count = ((size_t)(__section_end("Event")) - (size_t)(__section_begin("Event")))
            / sizeof(Event);
#elif defined(__GNUC__)
    base = (Event *)(&__event_start);
    count = ((size_t)(&__event_end) - (size_t)(&__event_start)) / sizeof(Event);
#else
    #error not supported compiler, please use command table mode
#endif

#if EVENT_SPEED_OPTIMIZE == 1
    //优化代码写这里
#else /** EVENT_SPEED_OPTIMIZE == 1 */
    eventTable.base = base;
    eventTable.count = count;
    //printf("&__event_start=%X, &__event_end=%X.\n",&__event_start,&__event_end);
    //端中符号__event_start并没有占用空间 求其地址为编译开始
    printf("base=0x%X, count=0x%X.\n",(int)eventTable.base,(int)eventTable.count);
#endif
}

/**
 * @brief event运行
 * @param event event
 */
static void eventRun(Event *event)
{
    if (event->paramNum < 1)
    {
        return;
    }
    void (*function)() = (void (*)())(event->param[0]);
    switch (event->paramNum)
    {
    case 1:
        function();
        break;
    case 2:
        function(event->param[1]);
        break;
    case 3:
        function(event->param[1], event->param[2]);
        break;
    case 4:
        function(event->param[1], event->param[2], event->param[3]);
        break;
    case 5:
        function(event->param[1], event->param[2], event->param[3], event->param[4]);
        break;
    case 6:
        function(event->param[1], event->param[2], event->param[3], event->param[4],
                 event->param[5]);
        break;
    case 7:
        function(event->param[1], event->param[2], event->param[3], event->param[4],
                 event->param[5], event->param[6]);
        break;
    case 8:
        function(event->param[1], event->param[2], event->param[3], event->param[4],
                 event->param[5], event->param[6], event->param[7]);
        break;
    default:
        break;
    }
}

/**
 * @brief event处理
 * @param event 事件
 */
static void eventHandler(unsigned short event)
{
#if EVENT_SPEED_OPTIMIZE == 1
    //优化代码写这里
#else
    for (size_t i = 0; i < eventTable.count; i++)
    {
        if (eventTable.base[i].event == event)
        {
            eventRun(&(eventTable.base[i]));
        }
    }
#endif
}

/**
 * @brief 广播event事件
 * @param event 事件
 */
void eventPost(unsigned short event)
{
    eventHandler(event);
}

static unsigned short eisr[EVENT_ISR_LOOPQUEUE_MAX];
static unsigned short eisr_head = 0;
static unsigned short eisr_end = 0;
int eventEmit(unsigned short event){
    //不强制添加
    int next = (eisr_end + 1)%EVENT_ISR_LOOPQUEUE_MAX;
    if (next == eisr_head) {
        return -1;
    }
    eisr[eisr_end]=event;
    eisr_end = next;
    if (eisr_end == eisr_head) {
        eisr_head = (eisr_head + 1)%EVENT_ISR_LOOPQUEUE_MAX;
    }
    return 1;
}

void eventExec(void){
    if (eisr_head != eisr_end) {
        eventHandler(eisr[eisr_head]);
        eisr_head = (eisr_head + 1)%EVENT_ISR_LOOPQUEUE_MAX;
    }
}

