
#ifndef _EXPORT_H_
#define _EXPORT_H_

#include <stddef.h> /* 定义了size_t */

typedef void (*export_init_fn_t)(void);
typedef void (*export_run_fn_t)(void);

/*********************注意：只有级别1 2 3 4 5会被运行 1级别最先运行************************/

#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6000000)
    //__CC_ARM是MDK
    #define EXPORT_USED
    #define EXPORT_SECTION(x)                  __attribute__((used, section(x)))

    #define EXPORT_INIT(fn, level) \
        const export_init_fn_t EXPORT_SECTION("export_init_fn_" #level) __export_init_##fn = fn;
        
    #define EXPORT_RUN(fn, level) \
        const export_run_fn_t EXPORT_SECTION("export_run_fn_" #level) __export_run_##fn = fn;
#elif defined(__ICCARM__) || defined(__ICCRX__)
    #define EXPORT_USED             __attribute__((used))
    #define EXPORT_SECTION(x)                  @ x
#elif defined(__GNUC__)
    //GCC
    #define EXPORT_USED             __attribute__((used))
    #define EXPORT_SECTION(x)                  __attribute__((section(x)))

    #define EXPORT_INIT(fn, level) \
        EXPORT_USED const export_init_fn_t __export_init_##fn EXPORT_SECTION(".export_init_fn." #level) = fn;

    #define EXPORT_RUN(fn, level) \
        EXPORT_USED const export_run_fn_t __export_run_##fn EXPORT_SECTION(".export_run_fn." #level) = fn;
/* GCC 链接脚本 在.text下
. = ALIGN(4);
PROVIDE(__export_init_start = .);
KEEP(*(SORT(.export_init_fn.*)))
PROVIDE(__export_init_end = .);

. = ALIGN(4);
PROVIDE(__export_run_start = .);
KEEP(*(SORT(.export_run_fn.*)))
PROVIDE(__export_run_end = .);
*/
#else
    #define SECTION(x)
#endif


#define INIT_EXPORT EXPORT_INIT
#define RUN_EXPORT EXPORT_RUN

void export_init(void);
#define INIT export_init()

void export_run(void);
#define RUN export_run()





#endif /* _EXPORT_H_ */
