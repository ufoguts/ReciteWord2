#pragma once

//常用宏定义库

#include <cstdio>
#include <cassert>

#include <iostream>

#include <type_traits>
#include <utility>

#include <string>

#include <exception>
#include <stdexcept>



//标识符操作
#define SYMBOL_STR(id) #id//转变字符串

#define PRE_SYMBOL(id, symbol) symbol##id//添加前缀
#define SUF_SYMBOL(id, symbol) id##symbol//添加后缀



//数值运算操作
#define OPPOSITE_TO(x) ((x) = -(x))//赋相反数

#define ABS_OF(x) ((x)>0? (x):-(x))//取绝对值
#define ABS_TO(x) ((x) = ABS_OF(x))//赋绝对值

#define DIS_OF(x, y) (ABS_OF((x)-(y)))//取差值

#define MAX_OF(x, y) ((x)>(y)? (x):(y))//取最大值
#define MIN_OF(x, y) ((x)<(y)? (x):(y))//取最小值
#define MAX_TO(x, lim) ((x) = MAX_OF(x, lim))//赋最大值
#define MIN_TO(x, lim) ((x) = MIN_OF(x, lim))//赋最小值
#define HIGH_LIMIT(x, lim) MIN_TO(x, lim)//设置上限
#define LOW_LIMIT(x, lim) MAX_TO(x, lim)//设置下限
#define RANGE_OF(x, llim, hlim) ((x)<(llim)? (llim): (x)>(hlim)? (hlim):(x))//取范围
#define RANGE_LIMIT(x, llim, hlim) ((x) = RANGE_OF(x, llim, hlim))//设置范围

#define IS_IN_RANGE(x, llim, hlim) ((x)>=(llim) && (x)<=(hlim))//判断是否在范围内
#define IS_IN_ITER_RANGE(x, st, ed) ((x)>=(st) && (x)<(ed))//判断是否在范围内，以迭代器标准

#define SQUARE_OF(x) ((x)*(x))//求平方
#define SQUARE_TO(x) ((x) = SQUARE_OF(x))//赋值平方



//运算省略操作
#define LIST_GO(p, m_p) ((p) = (p)->m_p)//链表指针移动
#define LIST_CNNT(p1, m_p1, p2, m_p2) \
	(((p1)->m_p1=p2), ((p2)->m_p2=p1))//双向链表连接
#define LIST_ADD(p1, m_p1, p2, m_p2, pNew) \
	(LIST_CNNT(p1, m_p1, pNew, m_p2), LIST_CNNT(pNew, m_p1, p2, m_p2))//双向链表添加

#define ARR_SIZE(arr) (sizeof(decltype(arr))\
	/sizeof(typename std::remove_extent<decltype(arr)>::type))//求数组大小

#define ARR_SIZE_ALL(arr) (sizeof(decltype(arr))\
	/sizeof(typename std::remove_all_extents<decltype(arr)>::type))//求根数组大小

#define O_(id) PRE_SYMBOL(id, o_)//添加原始前缀
#define O_INIT(id) id(O_(id))//使用前缀版本初始化
#define O_INIT_MOVE(id) id(std::move(O_(id)))//使用前缀版本移动初始化
#define O_INIT_FORWARD(id, Ty) id(std::forward<Ty>(O_(id)))//使用前缀版本转发初始化
#define O_ASSIGN(id) (id = O_(id))//使用前缀版本赋值
#define O_ASSING_MOVE(id) (id = std::move(O_(id)))//使用前缀版本移动赋值
#define O_ASSIGN_FORWARD(id, Ty) (id = std::forward<Ty>(O_(id)))//使用前缀版本转发初始化

#define OTHER_INIT(other, mem) mem((other).mem)//使用类成员初始化
#define OTHER_INIT_MOVE(other, mem) mem(std::move((other).mem))//使用类成员移动初始化
#define OTHER_ASSIGN(other, mem) (mem = (other).mem)//使用类成员赋值
#define OTHER_ASSIGN_MOVE(other, mem) (mem = std::move((other).mem))//使用类成员移动赋值



//类操作
#define CLASS_ASSIGN(Type, other) {\
	if(this!=&(other)) {\
		this->~Type();\
		new(this) Type(other);\
	}}//拷贝赋值，注意会覆盖虚表

#define CLASS_ASSIGN_MOVE(Type, other) {\
	if(this!=&(other)) {\
		this->~Type();\
		new(this) Type(std::move(other));\
	}}//移动赋值，注意会覆盖虚表

#define TEMPLATE_ASSIGN(Type, Typename, arg) {\
	this->~Type();\
	new(this) Type(std::forward<Typename>(arg));\
	}//模板赋值，自赋值不安全，注意会覆盖虚表

#define TEMPLATE_PACKET_ASSIGN(Type, Typenames, args) {\
	this->~Type();\
	new(this) Type(std::forward<Typenames>(args)...);\
	}//模板包赋值，自赋值不安全，注意会覆盖虚表



//输出操作，不保证表达式执行次数

//采用屏幕格式化输出
#define UPRINTF_STDIN(...) (printf(__VA_ARGS__))
//采用文件格式化输出，需定义UFILE_POINTER
#define UPRINTF_FILE(...) (fprintf((UFILE_POINTER), __VA_ARGS__))
//采用屏幕和文件格式化输出，需定义UFILE_POINTER
#define UPRINTF_ALL(...) (UPRINTF_STDIN(__VA_ARGS__), UPRINTF_FILE(__VA_ARGS__))

//采用屏幕格式化刷新
#define UFFLUSH_STDIN() (fflush(stdin))
//采用文件格式化刷新，需定义UFILE_POINTER
#define UFFLUSH_FILE() (fflush((UFILE_POINTER)))
//采用屏幕和文件格式化刷新，需定义UFILE_POINTER
#define UFFLUSH_ALL() (UFFLUSH_STDIN(), UFFLUSH_FILE())

//实际输出模式
//#define UPRINTF_FFLUSH_MODE 1
#if UPRINTF_FFLUSH_MODE==1
//屏幕输出
#define UPRINTF(...) UPRINTF_STDIN(__VA_ARGS__)
#define UFFLUSH() UFFLUSH_STDIN()
#elif UPRINTF_FFLUSH_MODE==2
//文件输出
#define UPRINTF(...) UPRINTF_FILE(__VA_ARGS__)
#define UFFLUSH() UFFLUSH_FILE()
#elif UPRINTF_FFLUSH_MODE==3
//屏幕文件输出
#define UPRINTF(...) UPRINTF_ALL(__VA_ARGS__)
#define UFFLUSH() UFFLUSH_ALL()
#endif
//采用定义格式输出并刷新
#define UPRINTF_FFLUSH(...) (UPRINTF(__VA_ARGS__), UFFLUSH())


//控制符默认模板
#define CIO_TEMPLATE(oper) (oper<char, std::char_traits<char>>)

//c++输出辅助，传控制符需注明模板
inline void _COUT_STDIN_ASSIST()
{
}
template<typename Ty, typename ...Tys>
inline void _COUT_STDIN_ASSIST(Ty &&arg, Tys &&...args)
{
	std::cout <<std::forward<Ty>(arg);
	_COUT_STDIN_ASSIST(std::forward<Tys>(args)...);
}
inline void _COUT_FILE_ASSIST(std::ostream &os)
{
}
template<typename Ty, typename ...Tys>
inline void _COUT_FILE_ASSIST(std::ostream &os, Ty &&arg, Tys &&...args)
{
	os <<std::forward<Ty>(arg);
	_COUT_FILE_ASSIST(os, std::forward<Tys>(args)...);
}

//线程安全辅助，需定义COUT_GLOBAL_MUTEX
#define COUT_SAFE(exp) \
	((COUT_GLOBAL_MUTEX).lock(), (exp), (COUT_GLOBAL_MUTEX).unlock())

//c++屏幕输出
#define COUT_STDIN(...) (_COUT_STDIN_ASSIST(__VA_ARGS__))
//c++屏幕输出，线程安全，需定义COUT_GLOBAL_MUTEX
#define COUT_STDIN_S(...) (COUT_SAFE(COUT_STDIN(__VA_ARGS__)))
//c++文件输出，需定义COUT_GLOBAL_FILE
#define COUT_FILE(...) (_COUT_FILE_ASSIST((COUT_GLOBAL_FILE), __VA_ARGS__))
//c++文件输出，线程安全，需定义COUT_GLOBAL_FILE，需定义COUT_GLOBAL_MUTEX
#define COUT_FILE_S(...) (COUT_SAFE(COUT_FILE(__VA_ARGS__)))
//c++屏幕和文件输出，需定义COUT_GLOBAL_FILE
#define COUT_ALL(...) (COUT_STDIN(__VA_ARGS__), COUT_FILE(__VA_ARGS__))
//c++屏幕和文件输出，线程安全，需定义COUT_GLOBAL_FILE，需定义COUT_GLOBAL_MUTEX
#define COUT_ALL_S(...) (COUT_SAFE(COUT_ALL(__VA_ARGS__)))
//c++不输出
#define COUT_NULL(...) ()
//c++不输出，线程安全
#define COUT_NULL_S(...) ()

//c++屏幕刷新
#define CFLUSH_STDIN() (std::cout.flush())
//c++屏幕刷新，线程安全，需定义COUT_GLOBAL_MUTEX
#define CFLUSH_STDIN_S() (COUT_SAFE(CFLUSH_STDIN()))
//c++文件刷新，需定义COUT_GLOBAL_FILE
#define CFLUSH_FILE() ((COUT_GLOBAL_FILE).flush())
//c++文件刷新，线程安全，需定义COUT_GLOBAL_FILE，需定义COUT_GLOBAL_MUTEX
#define CFLUSH_FILE_S() (COUT_SAFE(CFLUSH_FILE()))
//c++屏幕和文件刷新，需定义COUT_GLOBAL_FILE
#define CFLUSH_ALL() (CFLUSH_STDIN(), CFLUSH_FILE())
//c++屏幕和文件刷新，线程安全，需定义COUT_GLOBAL_FILE，需定义COUT_GLOBAL_MUTEX
#define CFLUSH_ALL_S() (COUT_SAFE(CFLUSH_ALL()))
//c++不刷新
#define CFLUSH_NULL() ()
//c++不刷新，线程安全
#define CFLUSH_NULL_S() ()

//c++采用定义格式输出并刷新
#define COUT_FLUSH_STDIN(...) (COUT_STDIN(__VA_ARGS__), CFLUSH_STDIN())
#define COUT_FLUSH_STDIN_S(...) (COUT_SAFE(COUT_FLUSH_STDIN(__VA_ARGS__)))
#define COUT_FLUSH_FILE(...) (COUT_FILE(__VA_ARGS__), CFLUSH_FILE())
#define COUT_FLUSH_FILE_S(...) (COUT_SAFE(COUT_FLUSH_FILE(__VA_ARGS__)))
#define COUT_FLUSH_ALL(...) (COUT_ALL(__VA_ARGS__), CFLUSH_ALL())
#define COUT_FLUSH_ALL_S(...) (COUT_SAFE(COUT_FLUSH_ALL(__VA_ARGS__)))
#define COUT_FLUSH_NULL(...) ()
#define COUT_FLUSH_NULL_S(...) ()

//实际输出模式
#ifdef CFLUSH
#undef CFLUSH
#endif
//#define COUT_FLUSH_MODE 1
#if COUT_FLUSH_MODE==1
//c++屏幕文件输出
#define COUT(...) COUT_STDIN(__VA_ARGS__)
#define CFLUSH() CFLUSH_STDIN()
#define COUT_S(...) COUT_STDIN_S(__VA_ARGS__)
#define CFLUSH_S() CFLUSH_STDIN_S()
#define COUT_FLUSH(...) COUT_FLUSH_STDIN(__VA_ARGS__)
#define COUT_FLUSH_S(...) COUT_FLUSH_STDIN_S(__VA_ARGS__)
#elif COUT_FLUSH_MODE==2
//c++屏幕文件输出
#define COUT(...) COUT_FILE(__VA_ARGS__)
#define CFLUSH() CFLUSH_FILE()
#define COUT_S(...) COUT_FILE_S(__VA_ARGS__)
#define CFLUSH_S() CFLUSH_FILE_S()
#define COUT_FLUSH(...) COUT_FLUSH_FILE(__VA_ARGS__)
#define COUT_FLUSH_S(...) COUT_FLUSH_FILE_S(__VA_ARGS__)
#elif COUT_FLUSH_MODE==3
//c++屏幕文件输出
#define COUT(...) COUT_ALL(__VA_ARGS__)
#define CFLUSH() CFLUSH_ALL()
#define COUT_S(...) COUT_ALL_S(__VA_ARGS__)
#define CFLUSH_S() CFLUSH_ALL_S()
#define COUT_FLUSH(...) COUT_FLUSH_ALL(__VA_ARGS__)
#define COUT_FLUSH_S(...) COUT_FLUSH_ALL_S(__VA_ARGS__)
#elif COUT_FLUSH_MODE==4
//c++无输出
#define COUT(...) COUT_NULL(__VA_ARGS__)
#define CFLUSH() CFLUSH_NULL()
#define COUT_S(...) COUT_NULL_S(__VA_ARGS__)
#define CFLUSH_S() CFLUSH_NULL_S()
#define COUT_FLUSH(...) COUT_FLUSH_NULL(__VA_ARGS__)
#define COUT_FLUSH_S(...) COUT_FLUSH_NULL_S(__VA_ARGS__)
#endif



//断言操作

//无可转换基类异常
class _NoCatchError:
	private std::runtime_error
{
	//继承基类构造函数（除默认拷贝构造）
	using std::runtime_error::runtime_error;
public:
	//默认构造
	_NoCatchError():
		std::runtime_error("")
	{
	}
	//使用继承what
	using std::runtime_error::what;
	//省略合成5拷贝
};

//exp一定会执行一次，mes一定仅在断言失败执行一次
#define ASSERT_ELSENOT } else {//ifnot断言的对应else
#define ASSERT_ENDIF }}//if断言或ifnot断言结束

//抛出异常断言，同时打印信息
#define ASSERT_THROWLOG_ONLY(exp, mes) \
	{if(!(exp)) {COUT_FLUSH(mes); throw _NoCatchError(mes);}}//仅断言
#define ASSERT_OMIT_THROWLOG_ONLY(exp) \
	{if(!(exp)) {CFLUSH(); throw _NoCatchError();}}//无参数仅断言
#define ASSERT_THROWLOG_IFNOT(exp, mes) \
	{if(!(exp)) {{COUT_FLUSH(mes); throw _NoCatchError(mes);}//ifnot开始
#define ASSERT_OMIT_THROWLOG_IFNOT(exp) \
	{if(!(exp)) {{CFLUSH(); throw _NoCatchError();}//无参数ifnot开始
#define ASSERT_THROWLOG_IF(exp, mes) \
	{if(!(exp)) {COUT_FLUSH(mes); throw _NoCatchError(mes);} else {//if开始
#define ASSERT_OMIT_THROWLOG_IF(exp) \
	{if(!(exp)) {CFLUSH(); throw _NoCatchError();} else {//无参数if开始

//抛出异常断言，同时打印信息，线程安全
#define ASSERT_THROWLOG_ONLY_S(exp, mes) \
	{if(!(exp)) {COUT_FLUSH_S(mes); throw _NoCatchError(mes);}}//仅断言
#define ASSERT_OMIT_THROWLOG_ONLY_S(exp) \
	{if(!(exp)) {CFLUSH_S(); throw _NoCatchError();}}//无参数仅断言
#define ASSERT_THROWLOG_IFNOT_S(exp, mes) \
	{if(!(exp)) {{COUT_FLUSH_S(mes); throw _NoCatchError(mes);}//ifnot开始
#define ASSERT_OMIT_THROWLOG_IFNOT_S(exp) \
	{if(!(exp)) {{CFLUSH_S(); throw _NoCatchError();}//无参数ifnot开始
#define ASSERT_THROWLOG_IF_S(exp, mes) \
	{if(!(exp)) {COUT_FLUSH_S(mes); throw _NoCatchError(mes);} else {//if开始
#define ASSERT_OMIT_THROWLOG_IF_S(exp) \
	{if(!(exp)) {CFLUSH_S(); throw _NoCatchError();} else {//无参数if开始

//抛出异常断言
#define ASSERT_THROW_ONLY(exp, mes) \
	{if(!(exp)) {throw _NoCatchError(mes);}}//仅断言
#define ASSERT_OMIT_THROW_ONLY(exp) \
	{if(!(exp)) {throw _NoCatchError();}}//无参数仅断言
#define ASSERT_THROW_IFNOT(exp, mes) \
	{if(!(exp)) {{throw _NoCatchError(mes);}//ifnot开始
#define ASSERT_OMIT_THROW_IFNOT(exp) \
	{if(!(exp)) {{throw _NoCatchError();}//无参数ifnot开始
#define ASSERT_THROW_IF(exp, mes) \
	{if(!(exp)) {throw _NoCatchError(mes);} else {//if开始
#define ASSERT_OMIT_THROW_IF(exp) \
	{if(!(exp)) {throw _NoCatchError();} else {//无参数if开始

//抛出异常断言，线程安全
#define ASSERT_THROW_ONLY_S(exp, mes) \
	{if(!(exp)) {throw _NoCatchError(mes);}}//仅断言
#define ASSERT_OMIT_THROW_ONLY_S(exp) \
	{if(!(exp)) {throw _NoCatchError();}}//无参数仅断言
#define ASSERT_THROW_IFNOT_S(exp, mes) \
	{if(!(exp)) {{throw _NoCatchError(mes);}//ifnot开始
#define ASSERT_OMIT_THROW_IFNOT_S(exp) \
	{if(!(exp)) {{throw _NoCatchError();}//无参数ifnot开始
#define ASSERT_THROW_IF_S(exp, mes) \
	{if(!(exp)) {throw _NoCatchError(mes);} else {//if开始
#define ASSERT_OMIT_THROW_IF_S(exp) \
	{if(!(exp)) {throw _NoCatchError();} else {//无参数if开始

//调试模式断言，同时打印信息
#define ASSERT_DEBUGLOG_ONLY(exp, mes) \
	{if(!(exp)) {COUT_FLUSH(mes); assert(false&&(exp));}}//仅断言
#define ASSERT_OMIT_DEBUGLOG_ONLY(exp) \
	{if(!(exp)) {CFLUSH(); assert(false&&(exp));}}//无参数仅断言
#define ASSERT_DEBUGLOG_IFNOT(exp, mes) \
	{if(!(exp)) {{COUT_FLUSH(mes); assert(false&&(exp));}//ifnot开始
#define ASSERT_OMIT_DEBUGLOG_IFNOT(exp) \
	{if(!(exp)) {{CFLUSH(); assert(false&&(exp));}//无参数ifnot开始
#define ASSERT_DEBUGLOG_IF(exp, mes) \
	{if(!(exp)) {COUT_FLUSH(mes); assert(false&&(exp));} else {//if开始
#define ASSERT_OMIT_DEBUGLOG_IF(exp) \
	{if(!(exp)) {CFLUSH(); assert(false&&(exp));} else {//无参数if开始

//调试模式断言，同时打印信息，线程安全
#define ASSERT_DEBUGLOG_ONLY_S(exp, mes) \
	{if(!(exp)) {COUT_FLUSH_S(mes); assert(false&&(exp));}}//仅断言
#define ASSERT_OMIT_DEBUGLOG_ONLY_S(exp) \
	{if(!(exp)) {CFLUSH_S(); assert(false&&(exp));}}//无参数仅断言
#define ASSERT_DEBUGLOG_IFNOT_S(exp, mes) \
	{if(!(exp)) {{COUT_FLUSH_S(mes); assert(false&&(exp));}//ifnot开始
#define ASSERT_OMIT_DEBUGLOG_IFNOT_S(exp) \
	{if(!(exp)) {{CFLUSH_S(); assert(false&&(exp));}//无参数ifnot开始
#define ASSERT_DEBUGLOG_IF_S(exp, mes) \
	{if(!(exp)) {COUT_FLUSH_S(mes); assert(false&&(exp));} else {//if开始
#define ASSERT_OMIT_DEBUGLOG_IF_S(exp) \
	{if(!(exp)) {CFLUSH_S(); assert(false&&(exp));} else {//无参数if开始

//调试模式断言
#define ASSERT_DUBUG_ONLY(exp, mes) \
	{if(!(exp)) {assert(false&&(exp)); (mes);}}//仅断言
#define ASSERT_OMIT_DEBUG_ONLY(exp) \
	{if(!(exp)) {assert(false&&(exp));}}//无参数仅断言
#define ASSERT_DUBUG_IFNOT(exp, mes) \
	{if(!(exp)) {{assert(false&&(exp)); (mes);}//ifnot开始
#define ASSERT_OMIT_DEBUG_IFNOT(exp) \
	{if(!(exp)) {{assert(false&&(exp));}//无参数ifnot开始
#define ASSERT_DUBUG_IF(exp, mes) \
	{if(!(exp)) {assert(false&&(exp)); (mes);} else {//if开始
#define ASSERT_OMIT_DEBUG_IF(exp) \
	{if(!(exp)) {assert(false&&(exp));} else {//无参数if开始

//调试模式断言，线程安全
#define ASSERT_DUBUG_ONLY_S(exp, mes) \
	{if(!(exp)) {assert(false&&(exp)); (mes);}}//仅断言
#define ASSERT_OMIT_DEBUG_ONLY_S(exp) \
	{if(!(exp)) {assert(false&&(exp));}}//无参数仅断言
#define ASSERT_DUBUG_IFNOT_S(exp, mes) \
	{if(!(exp)) {{assert(false&&(exp)); (mes);}//ifnot开始
#define ASSERT_OMIT_DEBUG_IFNOT_S(exp) \
	{if(!(exp)) {{assert(false&&(exp));}//无参数ifnot开始
#define ASSERT_DUBUG_IF_S(exp, mes) \
	{if(!(exp)) {assert(false&&(exp)); (mes);} else {//if开始
#define ASSERT_OMIT_DEBUG_IF_S(exp) \
	{if(!(exp)) {assert(false&&(exp));} else {//无参数if开始

//打印信息断言
#define ASSERT_LOG_ONLY(exp, mes) \
	{if(!(exp)) {COUT_FLUSH(mes);}}//仅断言
#define ASSERT_OMIT_LOG_ONLY(exp) \
	{if(!(exp)) {CFLUSH();}}//无参数仅断言
#define ASSERT_LOG_IFNOT(exp, mes) \
	{if(!(exp)) {{COUT_FLUSH(mes);}//ifnot开始
#define ASSERT_OMIT_LOG_IFNOT(exp) \
	{if(!(exp)) {{CFLUSH();}//无参数ifnot开始
#define ASSERT_LOG_IF(exp, mes) \
	{if(!(exp)) {COUT_FLUSH(mes);} else {//if开始
#define ASSERT_OMIT_LOG_IF(exp) \
	{if(!(exp)) {CFLUSH();} else {//无参数if开始

//打印信息断言，线程安全
#define ASSERT_LOG_ONLY_S(exp, mes) \
	{if(!(exp)) {COUT_FLUSH_S(mes);}}//仅断言
#define ASSERT_OMIT_LOG_ONLY_S(exp) \
	{if(!(exp)) {CFLUSH_S();}}//无参数仅断言
#define ASSERT_LOG_IFNOT_S(exp, mes) \
	{if(!(exp)) {{COUT_FLUSH_S(mes);}//ifnot开始
#define ASSERT_OMIT_LOG_IFNOT_S(exp) \
	{if(!(exp)) {{CFLUSH_S();}//无参数ifnot开始
#define ASSERT_LOG_IF_S(exp, mes) \
	{if(!(exp)) {COUT_FLUSH_S(mes);} else {//if开始
#define ASSERT_OMIT_LOG_IF_S(exp) \
	{if(!(exp)) {CFLUSH_S();} else {//无参数if开始

//仅执行表达式
#define ASSERT_NOP_ONLY(exp, mes) \
	{if(!(exp)) {(mes);}}//仅断言
#define ASSERT_OMIT_NOP_ONLY(exp) \
	{if(!(exp)) {}}//无参数仅断言
#define ASSERT_NOP_IFNOT(exp, mes) \
	{if(!(exp)) {{(mes);}//ifnot开始
#define ASSERT_OMIT_NOP_IFNOT(exp) \
	{if(!(exp)) {{}//无参数ifnot开始
#define ASSERT_NOP_IF(exp, mes) \
	{if(!(exp)) {(mes);} else {//if开始
#define ASSERT_OMIT_NOP_IF(exp) \
	{if(!(exp)) {} else {//无参数if开始

//仅执行表达式，线程安全
#define ASSERT_NOP_ONLY_S(exp, mes) \
	{if(!(exp)) {(mes);}}//仅断言
#define ASSERT_OMIT_NOP_ONLY_S(exp) \
	{if(!(exp)) {}}//无参数仅断言
#define ASSERT_NOP_IFNOT_S(exp, mes) \
	{if(!(exp)) {{(mes);}//ifnot开始
#define ASSERT_OMIT_NOP_IFNOT_S(exp) \
	{if(!(exp)) {{}//无参数ifnot开始
#define ASSERT_NOP_IF_S(exp, mes) \
	{if(!(exp)) {(mes);} else {//if开始
#define ASSERT_OMIT_NOP_IF_S(exp) \
	{if(!(exp)) {} else {//无参数if开始

//实际断言模式
//#define ASSERT_ONLY_IFNOT_IF_MODE 5
#if ASSERT_ONLY_IFNOT_IF_MODE==5
//抛出异常
#define ASSERT_ONLY(exp, mes) ASSERT_THROWLOG_ONLY(exp, mes)
#define ASSERT_OMIT_ONLY(exp) ASSERT_OMIT_THROWLOG_ONLY(exp)
#define ASSERT_IFNOT(exp, mes) ASSERT_THROWLOG_IFNOT(exp, mes)
#define ASSERT_OMIT_IFNOT(exp) ASSERT_OMIT_THROWLOG_IFNOT(exp)
#define ASSERT_IF(exp, mes) ASSERT_THROWLOG_IF(exp, mes)
#define ASSERT_OMIT_IF(exp) ASSERT_OMIT_THROWLOG_IF(exp)
#define ASSERT_ONLY_S(exp, mes) ASSERT_THROWLOG_ONLY_S(exp, mes)
#define ASSERT_OMIT_ONLY_S(exp) ASSERT_OMIT_THROWLOG_ONLY_S(exp)
#define ASSERT_IFNOT_S(exp, mes) ASSERT_THROWLOG_IFNOT_S(exp, mes)
#define ASSERT_OMIT_IFNOT_S(exp) ASSERT_OMIT_THROWLOG_IFNOT_S(exp)
#define ASSERT_IF_S(exp, mes) ASSERT_THROWLOG_IF_S(exp, mes)
#define ASSERT_OMIT_IF_S(exp) ASSERT_OMIT_THROWLOG_IF_S(exp)
#elif ASSERT_ONLY_IFNOT_IF_MODE==4
//抛出异常
#define ASSERT_ONLY(exp, mes) ASSERT_THROW_ONLY(exp, mes)
#define ASSERT_OMIT_ONLY(exp) ASSERT_OMIT_THROW_ONLY(exp)
#define ASSERT_IFNOT(exp, mes) ASSERT_THROW_IFNOT(exp, mes)
#define ASSERT_OMIT_IFNOT(exp) ASSERT_OMIT_THROW_IFNOT(exp)
#define ASSERT_IF(exp, mes) ASSERT_THROW_IF(exp, mes)
#define ASSERT_OMIT_IF(exp) ASSERT_OMIT_THROW_IF(exp)
#define ASSERT_ONLY_S(exp, mes) ASSERT_THROW_ONLY_S(exp, mes)
#define ASSERT_OMIT_ONLY_S(exp) ASSERT_OMIT_THROW_ONLY_S(exp)
#define ASSERT_IFNOT_S(exp, mes) ASSERT_THROW_IFNOT_S(exp, mes)
#define ASSERT_OMIT_IFNOT_S(exp) ASSERT_OMIT_THROW_IFNOT_S(exp)
#define ASSERT_IF_S(exp, mes) ASSERT_THROW_IF_S(exp, mes)
#define ASSERT_OMIT_IF_S(exp) ASSERT_OMIT_THROW_IF_S(exp)
#elif ASSERT_ONLY_IFNOT_IF_MODE==3
//调试并打印信息
#define ASSERT_ONLY(exp, mes) ASSERT_DEBUGLOG_ONLY(exp, mes)
#define ASSERT_OMIT_ONLY(exp) ASSERT_OMIT_DEBUGLOG_ONLY(exp)
#define ASSERT_IFNOT(exp, mes) ASSERT_DEBUGLOG_IFNOT(exp, mes)
#define ASSERT_OMIT_IFNOT(exp) ASSERT_OMIT_DEBUGLOG_IFNOT(exp)
#define ASSERT_IF(exp, mes) ASSERT_DEBUGLOG_IF(exp, mes)
#define ASSERT_OMIT_IF(exp) ASSERT_OMIT_DEBUGLOG_IF(exp)
#define ASSERT_ONLY_S(exp, mes) ASSERT_DEBUGLOG_ONLY_S(exp, mes)
#define ASSERT_OMIT_ONLY_S(exp) ASSERT_OMIT_DEBUGLOG_ONLY_S(exp)
#define ASSERT_IFNOT_S(exp, mes) ASSERT_DEBUGLOG_IFNOT_S(exp, mes)
#define ASSERT_OMIT_IFNOT_S(exp) ASSERT_OMIT_DEBUGLOG_IFNOT_S(exp)
#define ASSERT_IF_S(exp, mes) ASSERT_DEBUGLOG_IF_S(exp, mes)
#define ASSERT_OMIT_IF_S(exp) ASSERT_OMIT_DEBUGLOG_IF_S(exp)
#elif ASSERT_ONLY_IFNOT_IF_MODE==2
//调试异常
#define ASSERT_ONLY(exp, mes) ASSERT_DUBUG_ONLY(exp, mes)
#define ASSERT_OMIT_ONLY(exp) ASSERT_OMIT_DEBUG_ONLY(exp)
#define ASSERT_IFNOT(exp, mes) ASSERT_DUBUG_IFNOT(exp, mes)
#define ASSERT_OMIT_IFNOT(exp) ASSERT_OMIT_DEBUG_IFNOT(exp)
#define ASSERT_IF(exp, mes) ASSERT_DUBUG_IF(exp, mes)
#define ASSERT_OMIT_IF(exp) ASSERT_OMIT_DEBUG_IF(exp)
#define ASSERT_ONLY_S(exp, mes) ASSERT_DUBUG_ONLY_S(exp, mes)
#define ASSERT_OMIT_ONLY_S(exp) ASSERT_OMIT_DEBUG_ONLY_S(exp)
#define ASSERT_IFNOT_S(exp, mes) ASSERT_DUBUG_IFNOT_S(exp, mes)
#define ASSERT_OMIT_IFNOT_S(exp) ASSERT_OMIT_DEBUG_IFNOT_S(exp)
#define ASSERT_IF_S(exp, mes) ASSERT_DUBUG_IF_S(exp, mes)
#define ASSERT_OMIT_IF_S(exp) ASSERT_OMIT_DEBUG_IF_S(exp)
#elif ASSERT_ONLY_IFNOT_IF_MODE==1
//打印信息
#define ASSERT_ONLY(exp, mes) ASSERT_LOG_ONLY(exp, mes)
#define ASSERT_OMIT_ONLY(exp) ASSERT_OMIT_LOG_ONLY(exp)
#define ASSERT_IFNOT(exp, mes) ASSERT_LOG_IFNOT(exp, mes)
#define ASSERT_OMIT_IFNOT(exp) ASSERT_OMIT_LOG_IFNOT(exp)
#define ASSERT_IF(exp, mes) ASSERT_LOG_IF(exp, mes)
#define ASSERT_OMIT_IF(exp) ASSERT_OMIT_LOG_IF(exp)
#define ASSERT_ONLY_S(exp, mes) ASSERT_LOG_ONLY_S(exp, mes)
#define ASSERT_OMIT_ONLY_S(exp) ASSERT_OMIT_LOG_ONLY_S(exp)
#define ASSERT_IFNOT_S(exp, mes) ASSERT_LOG_IFNOT_S(exp, mes)
#define ASSERT_OMIT_IFNOT_S(exp) ASSERT_OMIT_LOG_IFNOT_S(exp)
#define ASSERT_IF_S(exp, mes) ASSERT_LOG_IF_S(exp, mes)
#define ASSERT_OMIT_IF_S(exp) ASSERT_OMIT_LOG_IF_S(exp)
#elif ASSERT_ONLY_IFNOT_IF_MODE==0
//仅执行表达式
#define ASSERT_ONLY(exp, mes) ASSERT_NOP_ONLY(exp, mes)
#define ASSERT_OMIT_ONLY(exp) ASSERT_OMIT_NOP_ONLY(exp)
#define ASSERT_IFNOT(exp, mes) ASSERT_NOP_IFNOT(exp, mes)
#define ASSERT_OMIT_IFNOT(exp) ASSERT_OMIT_NOP_IFNOT(exp)
#define ASSERT_IF(exp, mes) ASSERT_NOP_IF(exp, mes)
#define ASSERT_OMIT_IF(exp) ASSERT_OMIT_NOP_IF(exp)
#define ASSERT_ONLY_S(exp, mes) ASSERT_NOP_ONLY_S(exp, mes)
#define ASSERT_OMIT_ONLY_S(exp) ASSERT_OMIT_NOP_ONLY_S(exp)
#define ASSERT_IFNOT_S(exp, mes) ASSERT_NOP_IFNOT_S(exp, mes)
#define ASSERT_OMIT_IFNOT_S(exp) ASSERT_OMIT_NOP_IFNOT_S(exp)
#define ASSERT_IF_S(exp, mes) ASSERT_NOP_IF_S(exp, mes)
#define ASSERT_OMIT_IF_S(exp) ASSERT_OMIT_NOP_IF_S(exp)
#endif
