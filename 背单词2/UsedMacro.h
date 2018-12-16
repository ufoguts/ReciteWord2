#pragma once

//���ú궨���

#include <cstdio>
#include <cassert>

#include <iostream>

#include <type_traits>
#include <utility>

#include <string>

#include <exception>
#include <stdexcept>



//��ʶ������
#define SYMBOL_STR(id) #id//ת���ַ���

#define PRE_SYMBOL(id, symbol) symbol##id//���ǰ׺
#define SUF_SYMBOL(id, symbol) id##symbol//��Ӻ�׺



//��ֵ�������
#define OPPOSITE_TO(x) ((x) = -(x))//���෴��

#define ABS_OF(x) ((x)>0? (x):-(x))//ȡ����ֵ
#define ABS_TO(x) ((x) = ABS_OF(x))//������ֵ

#define DIS_OF(x, y) (ABS_OF((x)-(y)))//ȡ��ֵ

#define MAX_OF(x, y) ((x)>(y)? (x):(y))//ȡ���ֵ
#define MIN_OF(x, y) ((x)<(y)? (x):(y))//ȡ��Сֵ
#define MAX_TO(x, lim) ((x) = MAX_OF(x, lim))//�����ֵ
#define MIN_TO(x, lim) ((x) = MIN_OF(x, lim))//����Сֵ
#define HIGH_LIMIT(x, lim) MIN_TO(x, lim)//��������
#define LOW_LIMIT(x, lim) MAX_TO(x, lim)//��������
#define RANGE_OF(x, llim, hlim) ((x)<(llim)? (llim): (x)>(hlim)? (hlim):(x))//ȡ��Χ
#define RANGE_LIMIT(x, llim, hlim) ((x) = RANGE_OF(x, llim, hlim))//���÷�Χ

#define SQUARE_OF(x) ((x)*(x))//��ƽ��
#define SQUARE_TO(x) ((x) = SQUARE_OF(x))//��ֵƽ��



//����ʡ�Բ���
#define LIST_GO(p, m_p) ((p) = (p)->m_p)//����ָ���ƶ�
#define LIST_CNNT(p1, m_p1, p2, m_p2) \
	(((p1)->m_p1=p2), ((p2)->m_p2=p1))//˫����������
#define LIST_ADD(p1, m_p1, p2, m_p2, pNew) \
	(LIST_CNNT(p1, m_p1, pNew, m_p2), LIST_CNNT(pNew, m_p1, p2, m_p2))//˫���������

#define ARR_SIZE(arr) (sizeof(decltype(arr))\
	/sizeof(typename std::remove_extent<decltype(arr)>::type))//�������С

#define ARR_SIZE_ALL(arr) (sizeof(decltype(arr))\
	/sizeof(typename std::remove_all_extents<decltype(arr)>::type))//��������С

#define O_(id) PRE_SYMBOL(id, o_)//���ԭʼǰ׺
#define O_INIT(id) id(O_(id))//ʹ��ǰ׺�汾��ʼ��
#define O_INIT_MOVE(id) id(std::move(O_(id)))//ʹ��ǰ׺�汾�ƶ���ʼ��
#define O_ASSIGN(id) (id = O_(id))//ʹ��ǰ׺�汾��ֵ
#define O_ASSING_MOVE(id) (id = std::move(O_(id)))//ʹ��ǰ׺�汾�ƶ���ֵ

#define OTHER_INIT(other, mem) mem((other).mem)//ʹ�����Ա��ʼ��
#define OTHER_INIT_MOVE(other, mem) mem(std::move((other).mem))//ʹ�����Ա�ƶ���ʼ��
#define OTHER_ASSIGN(other, mem) (mem = (other).mem)//ʹ�����Ա��ֵ
#define OTHER_ASSIGN_MOVE(other, mem) (mem = std::move((other).mem))//ʹ�����Ա�ƶ���ֵ



//�����
#define COPY_ASSIGN(Type, other) {\
	if(this!=&(other)) {\
		this->~Type();\
		new(this) Type(other);\
	}\
	return *this;}//������ֵ

#define MOVE_ASSIGN(Type, other) {\
	if(this!=&(other)) {\
		this->~Type();\
		new(this) Type(std::move(other));\
	}\
	return *this;}//�ƶ���ֵ

#define TEMPLATE_ASSIGN(Type, Typename, arg) {\
	this->~Type();\
	new(this) Type(std::forward<Typename>(arg));\
	return *this;}//ģ�帳ֵ�������Ը�ֵ

#define TEMPLATE_PACKET_ASSIGN(Type, Typenames, args) {\
	this->~Type();\
	new(this) Type(std::forward<Typenames>(args)...);\
	return *this;}//ģ�����ֵ�������Ը�ֵ



//�������������֤���ʽִ�д���

//������Ļ��ʽ�����
#define UPRINTF_STDIN(...) (printf(__VA_ARGS__))
//�����ļ���ʽ��������趨��UFILE_POINTER
#define UPRINTF_FILE(...) (fprintf((UFILE_POINTER), __VA_ARGS__))
//������Ļ���ļ���ʽ��������趨��UFILE_POINTER
#define UPRINTF_ALL(...) (UPRINTF_STDIN(__VA_ARGS__), UPRINTF_FILE(__VA_ARGS__))

//������Ļ��ʽ��ˢ��
#define UFFLUSH_STDIN() (fflush(stdin))
//�����ļ���ʽ��ˢ�£��趨��UFILE_POINTER
#define UFFLUSH_FILE() (fflush((UFILE_POINTER)))
//������Ļ���ļ���ʽ��ˢ�£��趨��UFILE_POINTER
#define UFFLUSH_ALL() (UFFLUSH_STDIN(), UFFLUSH_FILE())

//ʵ�����ģʽ
//#define UPRINTF_FFLUSH_MODE 1
#if UPRINTF_FFLUSH_MODE==1
//��Ļ���
#define UPRINTF(...) UPRINTF_STDIN(__VA_ARGS__)
#define UFFLUSH() UFFLUSH_STDIN()
#elif UPRINTF_FFLUSH_MODE==2
//�ļ����
#define UPRINTF(...) UPRINTF_FILE(__VA_ARGS__)
#define UFFLUSH() UFFLUSH_FILE()
#elif UPRINTF_FFLUSH_MODE==3
//��Ļ�ļ����
#define UPRINTF(...) UPRINTF_ALL(__VA_ARGS__)
#define UFFLUSH() UFFLUSH_ALL()
#endif
//���ö����ʽ�����ˢ��
#define UPRINTF_FFLUSH(...) (UPRINTF(__VA_ARGS__), UFFLUSH())


//���Ʒ�Ĭ��ģ��
#define CIO_TEMPLATE(oper) (oper<char, std::char_traits<char>>)

//c++��������������Ʒ���ע��ģ��
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

//�̰߳�ȫ�������趨��COUT_GLOBAL_MUTEX
#define COUT_SAFE(exp) \
	((COUT_GLOBAL_MUTEX).lock(), (exp), (COUT_GLOBAL_MUTEX).unlock())

//c++��Ļ���
#define COUT_STDIN(...) (_COUT_STDIN_ASSIST(__VA_ARGS__))
//c++��Ļ������̰߳�ȫ���趨��COUT_GLOBAL_MUTEX
#define COUT_STDIN_S(...) (COUT_SAFE(COUT_STDIN(__VA_ARGS__)))
//c++�ļ�������趨��COUT_GLOBAL_FILE
#define COUT_FILE(...) (_COUT_FILE_ASSIST((COUT_GLOBAL_FILE), __VA_ARGS__))
//c++�ļ�������̰߳�ȫ���趨��COUT_GLOBAL_FILE���趨��COUT_GLOBAL_MUTEX
#define COUT_FILE_S(...) (COUT_SAFE(COUT_FILE(__VA_ARGS__)))
//c++��Ļ���ļ�������趨��COUT_GLOBAL_FILE
#define COUT_ALL(...) (COUT_STDIN(__VA_ARGS__), COUT_FILE(__VA_ARGS__))
//c++��Ļ���ļ�������̰߳�ȫ���趨��COUT_GLOBAL_FILE���趨��COUT_GLOBAL_MUTEX
#define COUT_ALL_S(...) (COUT_SAFE(COUT_ALL(__VA_ARGS__)))
//c++�����
#define COUT_NULL(...) ()
//c++��������̰߳�ȫ
#define COUT_NULL_S(...) ()

//c++��Ļˢ��
#define CFLUSH_STDIN() (std::cout.flush())
//c++��Ļˢ�£��̰߳�ȫ���趨��COUT_GLOBAL_MUTEX
#define CFLUSH_STDIN_S() (COUT_SAFE(CFLUSH_STDIN()))
//c++�ļ�ˢ�£��趨��COUT_GLOBAL_FILE
#define CFLUSH_FILE() ((COUT_GLOBAL_FILE).flush())
//c++�ļ�ˢ�£��̰߳�ȫ���趨��COUT_GLOBAL_FILE���趨��COUT_GLOBAL_MUTEX
#define CFLUSH_FILE_S() (COUT_SAFE(CFLUSH_FILE()))
//c++��Ļ���ļ�ˢ�£��趨��COUT_GLOBAL_FILE
#define CFLUSH_ALL() (CFLUSH_STDIN(), CFLUSH_FILE())
//c++��Ļ���ļ�ˢ�£��̰߳�ȫ���趨��COUT_GLOBAL_FILE���趨��COUT_GLOBAL_MUTEX
#define CFLUSH_ALL_S() (COUT_SAFE(CFLUSH_ALL()))
//c++��ˢ��
#define CFLUSH_NULL() ()
//c++��ˢ�£��̰߳�ȫ
#define CFLUSH_NULL_S() ()

//c++���ö����ʽ�����ˢ��
#define COUT_FLUSH_STDIN(...) (COUT_STDIN(__VA_ARGS__), CFLUSH_STDIN())
#define COUT_FLUSH_STDIN_S(...) (COUT_SAFE(COUT_FLUSH_STDIN(__VA_ARGS__)))
#define COUT_FLUSH_FILE(...) (COUT_FILE(__VA_ARGS__), CFLUSH_FILE())
#define COUT_FLUSH_FILE_S(...) (COUT_SAFE(COUT_FLUSH_FILE(__VA_ARGS__)))
#define COUT_FLUSH_ALL(...) (COUT_ALL(__VA_ARGS__), CFLUSH_ALL())
#define COUT_FLUSH_ALL_S(...) (COUT_SAFE(COUT_FLUSH_ALL(__VA_ARGS__)))
#define COUT_FLUSH_NULL(...) ()
#define COUT_FLUSH_NULL_S(...) ()

//ʵ�����ģʽ
#ifdef CFLUSH
#undef CFLUSH
#endif
//#define COUT_FLUSH_MODE 1
#if COUT_FLUSH_MODE==1
//c++��Ļ�ļ����
#define COUT(...) COUT_STDIN(__VA_ARGS__)
#define CFLUSH() CFLUSH_STDIN()
#define COUT_S(...) COUT_STDIN_S(__VA_ARGS__)
#define CFLUSH_S() CFLUSH_STDIN_S()
#define COUT_FLUSH(...) COUT_FLUSH_STDIN(__VA_ARGS__)
#define COUT_FLUSH_S(...) COUT_FLUSH_STDIN_S(__VA_ARGS__)
#elif COUT_FLUSH_MODE==2
//c++��Ļ�ļ����
#define COUT(...) COUT_FILE(__VA_ARGS__)
#define CFLUSH() CFLUSH_FILE()
#define COUT_S(...) COUT_FILE_S(__VA_ARGS__)
#define CFLUSH_S() CFLUSH_FILE_S()
#define COUT_FLUSH(...) COUT_FLUSH_FILE(__VA_ARGS__)
#define COUT_FLUSH_S(...) COUT_FLUSH_FILE_S(__VA_ARGS__)
#elif COUT_FLUSH_MODE==3
//c++��Ļ�ļ����
#define COUT(...) COUT_ALL(__VA_ARGS__)
#define CFLUSH() CFLUSH_ALL()
#define COUT_S(...) COUT_ALL_S(__VA_ARGS__)
#define CFLUSH_S() CFLUSH_ALL_S()
#define COUT_FLUSH(...) COUT_FLUSH_ALL(__VA_ARGS__)
#define COUT_FLUSH_S(...) COUT_FLUSH_ALL_S(__VA_ARGS__)
#elif COUT_FLUSH_MODE==4
//c++�����
#define COUT(...) COUT_NULL(__VA_ARGS__)
#define CFLUSH() CFLUSH_NULL()
#define COUT_S(...) COUT_NULL_S(__VA_ARGS__)
#define CFLUSH_S() CFLUSH_NULL_S()
#define COUT_FLUSH(...) COUT_FLUSH_NULL(__VA_ARGS__)
#define COUT_FLUSH_S(...) COUT_FLUSH_NULL_S(__VA_ARGS__)
#endif



//���Բ���

//�޿�ת�������쳣
class _NoCatchError:
	private std::runtime_error
{
	//�̳л��๹�캯������Ĭ�Ͽ������죩
	using std::runtime_error::runtime_error;
public:
	//Ĭ�Ϲ���
	_NoCatchError():
		std::runtime_error("")
	{
	}
	//ʹ�ü̳�what
	using std::runtime_error::what;
	//ʡ�Ժϳ�5����
};

//expһ����ִ��һ�Σ�mesһ�����ڶ���ʧ��ִ��һ��
#define ASSERT_ELSENOT } else {//ifnot���ԵĶ�Ӧelse
#define ASSERT_ENDIF }}//if���Ի�ifnot���Խ���

//�׳��쳣���ԣ�ͬʱ��ӡ��Ϣ
#define ASSERT_THROWLOG_ONLY(exp, mes) \
	{if(!(exp)) {COUT_FLUSH(mes); throw _NoCatchError(mes);}}//������
#define ASSERT_OMIT_THROWLOG_ONLY(exp) \
	{if(!(exp)) {CFLUSH(); throw _NoCatchError();}}//�޲���������
#define ASSERT_THROWLOG_IFNOT(exp, mes) \
	{if(!(exp)) {{COUT_FLUSH(mes); throw _NoCatchError(mes);}//ifnot��ʼ
#define ASSERT_OMIT_THROWLOG_IFNOT(exp) \
	{if(!(exp)) {{CFLUSH(); throw _NoCatchError();}//�޲���ifnot��ʼ
#define ASSERT_THROWLOG_IF(exp, mes) \
	{if(!(exp)) {COUT_FLUSH(mes); throw _NoCatchError(mes);} else {//if��ʼ
#define ASSERT_OMIT_THROWLOG_IF(exp) \
	{if(!(exp)) {CFLUSH(); throw _NoCatchError();} else {//�޲���if��ʼ

//�׳��쳣���ԣ�ͬʱ��ӡ��Ϣ���̰߳�ȫ
#define ASSERT_THROWLOG_ONLY_S(exp, mes) \
	{if(!(exp)) {COUT_FLUSH_S(mes); throw _NoCatchError(mes);}}//������
#define ASSERT_OMIT_THROWLOG_ONLY_S(exp) \
	{if(!(exp)) {CFLUSH_S(); throw _NoCatchError();}}//�޲���������
#define ASSERT_THROWLOG_IFNOT_S(exp, mes) \
	{if(!(exp)) {{COUT_FLUSH_S(mes); throw _NoCatchError(mes);}//ifnot��ʼ
#define ASSERT_OMIT_THROWLOG_IFNOT_S(exp) \
	{if(!(exp)) {{CFLUSH_S(); throw _NoCatchError();}//�޲���ifnot��ʼ
#define ASSERT_THROWLOG_IF_S(exp, mes) \
	{if(!(exp)) {COUT_FLUSH_S(mes); throw _NoCatchError(mes);} else {//if��ʼ
#define ASSERT_OMIT_THROWLOG_IF_S(exp) \
	{if(!(exp)) {CFLUSH_S(); throw _NoCatchError();} else {//�޲���if��ʼ

//�׳��쳣����
#define ASSERT_THROW_ONLY(exp, mes) \
	{if(!(exp)) {throw _NoCatchError(mes);}}//������
#define ASSERT_OMIT_THROW_ONLY(exp) \
	{if(!(exp)) {throw _NoCatchError();}}//�޲���������
#define ASSERT_THROW_IFNOT(exp, mes) \
	{if(!(exp)) {{throw _NoCatchError(mes);}//ifnot��ʼ
#define ASSERT_OMIT_THROW_IFNOT(exp) \
	{if(!(exp)) {{throw _NoCatchError();}//�޲���ifnot��ʼ
#define ASSERT_THROW_IF(exp, mes) \
	{if(!(exp)) {throw _NoCatchError(mes);} else {//if��ʼ
#define ASSERT_OMIT_THROW_IF(exp) \
	{if(!(exp)) {throw _NoCatchError();} else {//�޲���if��ʼ

//�׳��쳣���ԣ��̰߳�ȫ
#define ASSERT_THROW_ONLY_S(exp, mes) \
	{if(!(exp)) {throw _NoCatchError(mes);}}//������
#define ASSERT_OMIT_THROW_ONLY_S(exp) \
	{if(!(exp)) {throw _NoCatchError();}}//�޲���������
#define ASSERT_THROW_IFNOT_S(exp, mes) \
	{if(!(exp)) {{throw _NoCatchError(mes);}//ifnot��ʼ
#define ASSERT_OMIT_THROW_IFNOT_S(exp) \
	{if(!(exp)) {{throw _NoCatchError();}//�޲���ifnot��ʼ
#define ASSERT_THROW_IF_S(exp, mes) \
	{if(!(exp)) {throw _NoCatchError(mes);} else {//if��ʼ
#define ASSERT_OMIT_THROW_IF_S(exp) \
	{if(!(exp)) {throw _NoCatchError();} else {//�޲���if��ʼ

//����ģʽ���ԣ�ͬʱ��ӡ��Ϣ
#define ASSERT_DEBUGLOG_ONLY(exp, mes) \
	{if(!(exp)) {COUT_FLUSH(mes); assert(false&&(exp));}}//������
#define ASSERT_OMIT_DEBUGLOG_ONLY(exp) \
	{if(!(exp)) {CFLUSH(); assert(false&&(exp));}}//�޲���������
#define ASSERT_DEBUGLOG_IFNOT(exp, mes) \
	{if(!(exp)) {{COUT_FLUSH(mes); assert(false&&(exp));}//ifnot��ʼ
#define ASSERT_OMIT_DEBUGLOG_IFNOT(exp) \
	{if(!(exp)) {{CFLUSH(); assert(false&&(exp));}//�޲���ifnot��ʼ
#define ASSERT_DEBUGLOG_IF(exp, mes) \
	{if(!(exp)) {COUT_FLUSH(mes); assert(false&&(exp));} else {//if��ʼ
#define ASSERT_OMIT_DEBUGLOG_IF(exp) \
	{if(!(exp)) {CFLUSH(); assert(false&&(exp));} else {//�޲���if��ʼ

//����ģʽ���ԣ�ͬʱ��ӡ��Ϣ���̰߳�ȫ
#define ASSERT_DEBUGLOG_ONLY_S(exp, mes) \
	{if(!(exp)) {COUT_FLUSH_S(mes); assert(false&&(exp));}}//������
#define ASSERT_OMIT_DEBUGLOG_ONLY_S(exp) \
	{if(!(exp)) {CFLUSH_S(); assert(false&&(exp));}}//�޲���������
#define ASSERT_DEBUGLOG_IFNOT_S(exp, mes) \
	{if(!(exp)) {{COUT_FLUSH_S(mes); assert(false&&(exp));}//ifnot��ʼ
#define ASSERT_OMIT_DEBUGLOG_IFNOT_S(exp) \
	{if(!(exp)) {{CFLUSH_S(); assert(false&&(exp));}//�޲���ifnot��ʼ
#define ASSERT_DEBUGLOG_IF_S(exp, mes) \
	{if(!(exp)) {COUT_FLUSH_S(mes); assert(false&&(exp));} else {//if��ʼ
#define ASSERT_OMIT_DEBUGLOG_IF_S(exp) \
	{if(!(exp)) {CFLUSH_S(); assert(false&&(exp));} else {//�޲���if��ʼ

//����ģʽ����
#define ASSERT_DUBUG_ONLY(exp, mes) \
	{if(!(exp)) {assert(false&&(exp)); (mes);}}//������
#define ASSERT_OMIT_DEBUG_ONLY(exp) \
	{if(!(exp)) {assert(false&&(exp));}}//�޲���������
#define ASSERT_DUBUG_IFNOT(exp, mes) \
	{if(!(exp)) {{assert(false&&(exp)); (mes);}//ifnot��ʼ
#define ASSERT_OMIT_DEBUG_IFNOT(exp) \
	{if(!(exp)) {{assert(false&&(exp));}//�޲���ifnot��ʼ
#define ASSERT_DUBUG_IF(exp, mes) \
	{if(!(exp)) {assert(false&&(exp)); (mes);} else {//if��ʼ
#define ASSERT_OMIT_DEBUG_IF(exp) \
	{if(!(exp)) {assert(false&&(exp));} else {//�޲���if��ʼ

//����ģʽ���ԣ��̰߳�ȫ
#define ASSERT_DUBUG_ONLY_S(exp, mes) \
	{if(!(exp)) {assert(false&&(exp)); (mes);}}//������
#define ASSERT_OMIT_DEBUG_ONLY_S(exp) \
	{if(!(exp)) {assert(false&&(exp));}}//�޲���������
#define ASSERT_DUBUG_IFNOT_S(exp, mes) \
	{if(!(exp)) {{assert(false&&(exp)); (mes);}//ifnot��ʼ
#define ASSERT_OMIT_DEBUG_IFNOT_S(exp) \
	{if(!(exp)) {{assert(false&&(exp));}//�޲���ifnot��ʼ
#define ASSERT_DUBUG_IF_S(exp, mes) \
	{if(!(exp)) {assert(false&&(exp)); (mes);} else {//if��ʼ
#define ASSERT_OMIT_DEBUG_IF_S(exp) \
	{if(!(exp)) {assert(false&&(exp));} else {//�޲���if��ʼ

//��ӡ��Ϣ����
#define ASSERT_LOG_ONLY(exp, mes) \
	{if(!(exp)) {COUT_FLUSH(mes);}}//������
#define ASSERT_OMIT_LOG_ONLY(exp) \
	{if(!(exp)) {CFLUSH();}}//�޲���������
#define ASSERT_LOG_IFNOT(exp, mes) \
	{if(!(exp)) {{COUT_FLUSH(mes);}//ifnot��ʼ
#define ASSERT_OMIT_LOG_IFNOT(exp) \
	{if(!(exp)) {{CFLUSH();}//�޲���ifnot��ʼ
#define ASSERT_LOG_IF(exp, mes) \
	{if(!(exp)) {COUT_FLUSH(mes);} else {//if��ʼ
#define ASSERT_OMIT_LOG_IF(exp) \
	{if(!(exp)) {CFLUSH();} else {//�޲���if��ʼ

//��ӡ��Ϣ���ԣ��̰߳�ȫ
#define ASSERT_LOG_ONLY_S(exp, mes) \
	{if(!(exp)) {COUT_FLUSH_S(mes);}}//������
#define ASSERT_OMIT_LOG_ONLY_S(exp) \
	{if(!(exp)) {CFLUSH_S();}}//�޲���������
#define ASSERT_LOG_IFNOT_S(exp, mes) \
	{if(!(exp)) {{COUT_FLUSH_S(mes);}//ifnot��ʼ
#define ASSERT_OMIT_LOG_IFNOT_S(exp) \
	{if(!(exp)) {{CFLUSH_S();}//�޲���ifnot��ʼ
#define ASSERT_LOG_IF_S(exp, mes) \
	{if(!(exp)) {COUT_FLUSH_S(mes);} else {//if��ʼ
#define ASSERT_OMIT_LOG_IF_S(exp) \
	{if(!(exp)) {CFLUSH_S();} else {//�޲���if��ʼ

//��ִ�б��ʽ
#define ASSERT_NOP_ONLY(exp, mes) \
	{if(!(exp)) {(mes);}}//������
#define ASSERT_OMIT_NOP_ONLY(exp) \
	{if(!(exp)) {}}//�޲���������
#define ASSERT_NOP_IFNOT(exp, mes) \
	{if(!(exp)) {{(mes);}//ifnot��ʼ
#define ASSERT_OMIT_NOP_IFNOT(exp) \
	{if(!(exp)) {{}//�޲���ifnot��ʼ
#define ASSERT_NOP_IF(exp, mes) \
	{if(!(exp)) {(mes);} else {//if��ʼ
#define ASSERT_OMIT_NOP_IF(exp) \
	{if(!(exp)) {} else {//�޲���if��ʼ

//��ִ�б��ʽ���̰߳�ȫ
#define ASSERT_NOP_ONLY_S(exp, mes) \
	{if(!(exp)) {(mes);}}//������
#define ASSERT_OMIT_NOP_ONLY_S(exp) \
	{if(!(exp)) {}}//�޲���������
#define ASSERT_NOP_IFNOT_S(exp, mes) \
	{if(!(exp)) {{(mes);}//ifnot��ʼ
#define ASSERT_OMIT_NOP_IFNOT_S(exp) \
	{if(!(exp)) {{}//�޲���ifnot��ʼ
#define ASSERT_NOP_IF_S(exp, mes) \
	{if(!(exp)) {(mes);} else {//if��ʼ
#define ASSERT_OMIT_NOP_IF_S(exp) \
	{if(!(exp)) {} else {//�޲���if��ʼ

//ʵ�ʶ���ģʽ
//#define ASSERT_ONLY_IFNOT_IF_MODE 5
#if ASSERT_ONLY_IFNOT_IF_MODE==5
//�׳��쳣
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
//�׳��쳣
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
//���Բ���ӡ��Ϣ
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
//�����쳣
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
//��ӡ��Ϣ
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
//��ִ�б��ʽ
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