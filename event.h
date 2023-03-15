
#ifndef _EVENT_H_
#define _EVENT_H_


//目前不优化,实际小系统没有多少个事件,数组本身就是最快的 需要优化达到的目的：同一事件号对应多个注册事件 使用链表结构
#define     EVENT_SPEED_OPTIMIZE   0
//在中断中释放事件，缓存事件的循环列队大小
#define     EVENT_ISR_LOOPQUEUE_MAX   8


#ifndef SECTION
    #if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6000000)
        #define SECTION(x)                  __attribute__((used, section(x)))
    #elif defined(__ICCARM__) || defined(__ICCRX__)
        #define SECTION(x)                  @ x
    #elif defined(__GNUC__)
        #define SECTION(x)                  __attribute__((section(x)))
        /* GCC链接脚本
            PROVIDE(__event_start = .);
            KEEP(*(Event))
            PROVIDE(__event_end = .);
            //PROVID()E表示这个符号会被外部使用，也可以不加
            //还有各种对段排序的方法 自行百度
            //放在.text还是放在.data呢？
         */
    #else
        #define SECTION(x)
    #endif
#endif

/**
 * @brief 导出事件
 * @param _event 事件
 * @param _func 注册函数
 * @param ... 参数
 */
#define EVENT_EXPORT(_event, _func, ...) \
        const void *EventParam##_event##_func[] = {(void *)_func, ##__VA_ARGS__}; \
        const Event SECTION("Event") Event##_event##_func = \
        { \
            .param = EventParam##_event##_func, \
            .paramNum = sizeof(EventParam##_event##_func) / sizeof(void *), \
            .event = _event, \
        }

/**
 * @brief 导出事件(解决命名冲突)
 *        一般情况下不需要调用这个宏导出事件，当需要对同一个事件调用同一个函数时
 *        需要使用这个宏解决命名冲突
 * @param _alias 命名
 * @param _event 事件
 * @param _func 注册函数
 * @param ... 参数
 */
#define EVENT_EXPORT_ALIAS(_alias, _event, _func, ...) \
        const void *EventParam##_event##_func##_alias[] = {(void *)_func, ##__VA_ARGS__}; \
        const Event SECTION("Event") Event##_event##_func##_alias = \
        { \
            .param = EventParam##_event##_func##_alias, \
            .paramNum = sizeof(EventParam##_event##_func##_alias) / sizeof(void *), \
            .event = _event, \
        }


typedef struct 
{
    const void **param;                         /**< 参数(包括函数) */
    const unsigned char paramNum;               /**< 参数数量 */
    const unsigned short event;                 /**< 监听事件 */
} Event;


void eventInit(void);

void eventPost(unsigned short event);

//这一组可以使用在中断分离中
int eventEmit(unsigned short event);
void eventExec(void);

#endif /* #ifndef _EVENT_H_ */

