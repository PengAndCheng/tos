#include <stdio.h>
#include "EXPORT.h"



/****************************************全局变量 开始***************************************************/
#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6000000)
    //__CC_ARM是MDK编译的
    //避免空段报警程序
    #define code(num)\
    static export_run_fn_t* export_run_base##num = 0;\
    static size_t export_run_count##num = 0;\
    void export_section_##num##(void){\
        /*printf("export_section_%d.\r\n",num);*/\
    }\
    EXPORT_INIT(export_section_##num##,##num##);\
    EXPORT_RUN(export_section_##num##,##num##);
    code(1);
    code(2);
    code(3);
    code(4);
    code(5);
    #undef code
#elif defined(__ICCARM__) || defined(__ICCRX__)

#elif defined(__GNUC__)
    static export_run_fn_t* export_run_base = 0;
    static size_t export_run_count = 0;
#else
    #error not supported compiler, please use command table mode
#endif
/****************************************全局变量 结束***************************************************/


/****************************************初始化 开始***************************************************/
void export_init(void){
#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6000000)
    //__CC_ARM是MDK编译的

    //避免空段报警
    #define code(num) \
    export_init_fn_t* base##num = 0;\
    size_t count##num = 0;\
    extern const int export_init_fn_##num##$$Base;\
    extern const int export_init_fn_##num##$$Limit;\
    base##num = (export_init_fn_t *)(&export_init_fn_##num##$$Base);\
    count##num = ((size_t)(&export_init_fn_##num##$$Limit) - (size_t)(&export_init_fn_##num##$$Base)) / sizeof(export_init_fn_t);\
    extern const int export_run_fn_##num##$$Base;\
    extern const int export_run_fn_##num##$$Limit;\
    export_run_base##num = (export_init_fn_t *)(&export_run_fn_##num##$$Base);\
    export_run_count##num = ((size_t)(&export_run_fn_##num##$$Limit) - (size_t)(&export_run_fn_##num##$$Base)) / sizeof(export_init_fn_t);
    code(1);
    code(2);
    code(3);
    code(4);
    code(5);
    #undef code

    #define code(num) \
    export_init_fn_t function##num##;\
    for (size_t i = 0; i < count##num##; ++i) {\
        function##num## = base##num##[i];\
        function##num##();\
    };
    code(1);
    code(2);
    code(3);
    code(4);
    code(5);
    #undef code
#elif defined(__ICCARM__) || defined(__ICCRX__)


#elif defined(__GNUC__)
    export_init_fn_t* base = 0;
    size_t count = 0;

    extern const size_t __export_init_start;
    extern const size_t __export_init_end;
    extern const size_t __export_run_start;
    extern const size_t __export_run_end;

    base = (export_init_fn_t*)(&__export_init_start);
    count = ((size_t)(&__export_init_end) - (size_t)(&__export_init_start)) / sizeof(export_init_fn_t);

    export_run_base = (export_run_fn_t*)(&__export_run_start);
    export_run_count = ((size_t)(&__export_run_end) - (size_t)(&__export_run_start)) / sizeof(export_run_fn_t);

    //只执行INIT
    export_init_fn_t function;
    for (size_t i = 0; i < count; ++i) {
        function = base[i];
        function();
    }
#else
    #error not supported compiler, please use command table mode
#endif
}
/****************************************初始化 结束***************************************************/


/****************************************运行 开始***************************************************/
void export_run(void){
#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6000000)
    #define code(num) \
    export_init_fn_t function##num##;\
    for (size_t i = 0; i < export_run_count##num##; ++i) {\
        function##num## = export_run_base##num##[i];\
        function##num##();\
    };
    code(1);
    code(2);
    code(3);
    code(4);
    code(5);
    #undef code
#elif defined(__ICCARM__) || defined(__ICCRX__)

#elif defined(__GNUC__)
    export_run_fn_t function;
    for (size_t i = 0; i < export_run_count; ++i) {
        function = export_run_base[i];
        function();
    }
#else
    #error not supported compiler, please use command table mode
#endif
}
/****************************************运行 结束***************************************************/

