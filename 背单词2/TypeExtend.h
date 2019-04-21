#pragma once

//标准库类型扩展操作

#include <cassert>
#include <ctime>
#include <cmath>
#include <clocale>
#include <cstdint>

#include <sstream>

#include <type_traits>
#include <limits>
#include <algorithm>
#include <utility>
#include <tuple>
#include <functional>
#include <iterator>

#include <string>

#include <exception>
#include <stdexcept>
#include <ratio>

#include <chrono>
#include <mutex>




//整形快速计算幂次
inline constexpr long long _TypeExtendIntPower(long long base, int power)
{
	return power<0 ? 0
		: power==0 ? 1
		: (power&1)==0 ? _TypeExtendIntPower(base*base, power>>1)
		: base*_TypeExtendIntPower(base*base, power>>1);
}

//推算比例表示精度，即比例扩大底数的几次幂可以大于等于基准比例
template<typename Ratio, typename BaseRatio,
	typename CompareRatio= std::ratio<1>
> inline constexpr typename std::enable_if<
	std::ratio_greater_equal<Ratio, CompareRatio>::value, int
>::type _TypeExtendRatioPrecision()
{
	return 0;
}
template<typename Ratio, typename BaseRatio,
	typename CompareRatio= std::ratio<1>
> inline constexpr typename std::enable_if<
	!std::ratio_greater_equal<Ratio, CompareRatio>::value, int
>::type _TypeExtendRatioPrecision()
{
	return _TypeExtendRatioPrecision<std::ratio_multiply<Ratio, BaseRatio>,
		BaseRatio, CompareRatio>()+1;
}



//类型别名扩展

struct timeval;


//duration扩展
namespace std
{
namespace chrono
{
typedef duration<long, ratio<3600*24>> days;
typedef duration<long, ratio<3600*24*7>> weeks;
}
}



//************
//基础类型扩展
//************

//无用占位类型
template<int N>
class BlankType
{
};



//测试参数是否合法类
template<typename ...>
struct ParamValidTester
{
	typedef void type;
};


//先去除引用再去除常数易变属性
template<typename Ty>
struct RemoveCVRef
{
	using type = typename std::remove_cv<typename std::remove_reference<Ty>::type>::type;
};

//前者去除引用常数易变后是否为后者
template<typename TySrc, typename TyDst>
struct IsRemoveCVRefSame:
	std::is_same<typename RemoveCVRef<TySrc>::type, TyDst>
{
};


//测试是否为迭代器
template<class Ty, class= void>
struct IsIterator:
	std::false_type
{
};
template<class Ty>
struct IsIterator<Ty, typename ParamValidTester<
	typename std::iterator_traits<Ty>::iterator_category>::type
>: std::true_type
{
};



//提取变量的常引用
template<typename Ty>
const Ty &ConstRef(const Ty &value)
{
	return value;
}



//根据提供来源类型的引用、常数、易变属性，对目的类型附加同样的属性
template<typename TySrc, typename TyDst>
struct TypeAttachAttribute
{
	using type = TyDst;
};
//常属性特化
template<typename TySrc, typename TyDst>
struct TypeAttachAttribute<const TySrc, TyDst>
{
	using type = typename std::add_const<
		typename TypeAttachAttribute<TySrc, TyDst>>::type;
};
//易变属性特化
template<typename TySrc, typename TyDst>
struct TypeAttachAttribute<volatile TySrc, TyDst>
{
	using type = typename std::add_volatile<
		typename TypeAttachAttribute<TySrc, TyDst>>::type;
};
//左值引用属性特化
template<typename TySrc, typename TyDst>
struct TypeAttachAttribute<TySrc &, TyDst>
{
	using type = typename std::add_lvalue_reference<
		typename TypeAttachAttribute<TySrc, TyDst>>::type;
};
//右值引用属性特化
template<typename TySrc, typename TyDst>
struct TypeAttachAttribute<TySrc &&, TyDst>
{
	using type = typename std::add_rvalue_reference<
		typename TypeAttachAttribute<TySrc, TyDst>>::type;
};



//整数序列模板
template<size_t ...c_idx_s>
struct IndexSequence
{
};
template<>
struct IndexSequence<>
{
	using value_type = size_t;
	static constexpr size_t size()
	{
		return 0;
	}
};
template<int c_idx, int ...c_rest_s>
struct IndexSequence<c_idx, c_rest_s...>
{
	using value_type = size_t;
	static constexpr size_t size()
	{
		return IndexSequence<c_rest_s...>::size()+1;
	}
};

//整数模板添加元素函数
template<size_t c_new, size_t ...c_idx_s>
inline IndexSequence<c_idx_s..., c_new> AddIndexSqeuence(IndexSequence<c_idx_s...>)
{
	return IndexSequence<c_rest_s..., c_idx>();
}

//生成顺序递增整数模板辅助类
template<size_t c_size>
struct _MakeIndexSequenceAssist
{
	using type = decltype(AddIndexSqeuence<c_size-1>(
		typename _MakeIndexSequenceAssist<c_size-1>::type()));
};
template<>
struct _MakeIndexSequenceAssist<0>
{
	using type = IndexSequence<>;
};
//生成顺序递增整数模板类
template<size_t c_size>
using MakeIndexSequence = typename _MakeIndexSequenceAssist<c_size>::type;



//模板定长整形
template<bool c_bSigned, int c_size>
struct TemplateInt
{
};
template<>
struct TemplateInt<true, 1>
{
	using type = int8_t;
};
template<>
struct TemplateInt<false, 1>
{
	using type = uint8_t;
};
template<>
struct TemplateInt<true, 2>
{
	using type = int16_t;
};
template<>
struct TemplateInt<false, 2>
{
	using type = uint16_t;
};
template<>
struct TemplateInt<true, 4>
{
	using type = int32_t;
};
template<>
struct TemplateInt<false, 4>
{
	using type = uint32_t;
};
template<>
struct TemplateInt<true, 8>
{
	using type = int64_t;
};
template<>
struct TemplateInt<false, 8>
{
	using type = uint64_t;
};



//存储某类型，值类型
template<typename Ty>
struct TypeHold
{
	Ty hold;
public:
	//构造
	TypeHold(const Ty &o_hold):
		hold(o_hold)
	{
	}
	TypeHold(Ty &&o_hold):
		hold(std::move(o_hold))
	{
	}
};
//存储某类型，左值引用特化
template<typename Ty>
struct TypeHold<Ty &>
{
	Ty &hold;
public:
	//构造
	TypeHold(Ty &o_hold):
		hold(o_hold)
	{
	}
};
//存储某类型，右值引用特化
template<typename Ty>
struct TypeHold<Ty &&>
{
	Ty &&hold;
public:
	//构造
	TypeHold(Ty &&o_hold):
		hold(std::move(o_hold))
	{
	}
};



//存储某值类型，支持void
template<typename Ty>
struct ValueHold
{
	Ty hold;
public:
	ValueHold()
	{
	}
	//模板构造
	template<typename ...Ty_S>
	explicit ValueHold(Ty_S &&...args):
		hold(std::forward<Ty_S>(args)...)
	{
	}
};
//void特化
template<>
struct ValueHold<void>
{
};



//**********************
//类型成员或辅助函数扩展
//**********************

//c风格字符串转化字符串，重载其他类型
inline std::string SzToStr(const char *sz)
{
	return std::string(sz);
}
inline std::string SzToStr(char *sz)
{
	return std::string(sz);
}
template<typename Ty>
inline Ty &&SzToStr(Ty &&arg)
{
	return std::forward<Ty>(arg);
}



//整形比较辅助宏
#define _IS_INT_TYPE(Ty1, Ty2) \
 (std::is_integral<Ty1>::value && std::is_integral<Ty2>::value)
#define _INT_SIGN_UNSIGN(Ty1, Ty2) \
	((std::is_integral<Ty1>::value && std::is_signed<Ty1>::value)\
	&& std::is_unsigned<Ty2>::value)

//整形类型安全判断小于
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_INT_SIGN_UNSIGN(Ty1, Ty2), bool
>::type IntLT(Ty1 num1, Ty2 num2)
{
	return (num1<0) ? (true)
		: (static_cast<typename std::make_unsigned<Ty1>::type>(num1)<num2);
}
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_INT_SIGN_UNSIGN(Ty2, Ty1), bool
>::type IntLT(Ty1 num1, Ty2 num2)
{
	return (num2<0) ? (false)
		: (num1<static_cast<typename std::make_unsigned<Ty2>::type>(num2));
}
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_IS_INT_TYPE(Ty1, Ty2)
	&& !_INT_SIGN_UNSIGN(Ty1, Ty2) && !_INT_SIGN_UNSIGN(Ty2, Ty1), bool
>::type IntLT(Ty1 num1, Ty2 num2)
{
	return num1<num2;
}

//整形类型安全判断等于
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_INT_SIGN_UNSIGN(Ty1, Ty2), bool
>::type IntEQ(Ty1 num1, Ty2 num2)
{
	return (num1<0) ? (false)
		: (static_cast<typename std::make_unsigned<Ty1>::type>(num1)==num2);
}
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_INT_SIGN_UNSIGN(Ty2, Ty1), bool
>::type IntEQ(Ty1 num1, Ty2 num2)
{
	return (num2<0) ? (false)
		: (num1==static_cast<typename std::make_unsigned<Ty2>::type>(num2));
}
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_IS_INT_TYPE(Ty1, Ty2)
	&& !_INT_SIGN_UNSIGN(Ty1, Ty2) && !_INT_SIGN_UNSIGN(Ty2, Ty1), bool
>::type IntEQ(Ty1 num1, Ty2 num2)
{
	return num1==num2;
}

//整形类型安全判断小于等于
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_IS_INT_TYPE(Ty1, Ty2), bool
>::type IntLTE(Ty1 num1, Ty2 num2)
{
	return IntLT(num1, num2) || IntEQ(num1, num2);
}

//整形类型安全判断大于
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_IS_INT_TYPE(Ty1, Ty2), bool
>::type IntGT(Ty1 num1, Ty2 num2)
{
	return !(IntLT(num1, num2) || IntEQ(num1, num2));
}

//整形类型安全判断大于等于
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_IS_INT_TYPE(Ty1, Ty2), bool
>::type IntGTE(Ty1 num1, Ty2 num2)
{
	return !IntLT(num1, num2);
}

//整形类型安全判断不等于
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_IS_INT_TYPE(Ty1, Ty2), bool
>::type IntNEQ(Ty1 num1, Ty2 num2)
{
	return !IntEQ(num1, num2);
}
#undef _IS_INT_TYPE
#undef _INT_SIGN_UNSIGN



//浮点数比较辅助宏
#define _FLOAT_DEFAULT_ULP 10
#define _IS_FLOAT_TYPE(Ty1, Ty2) \
	(std::is_floating_point<Ty1>::value && std::is_floating_point<Ty2>::value)
#define _FLOAT_COMMON_EPSILON(Ty1, Ty2) \
	(std::max(std::numeric_limits<Ty1>::epsilon(), std::numeric_limits<Ty2>::epsilon()))
#define _FLOAT_COMMON_MIN(Ty1, Ty2) \
	(std::max(std::numeric_limits<Ty1>::min(), std::numeric_limits<Ty2>::min()))
#define _FLOAT_ABS(num) ((num)>=0? (num):-(num))

//浮点数比较相等，保证逻辑关系
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_IS_FLOAT_TYPE(Ty1, Ty2), bool
>::type FloatEQ(Ty1 num1, Ty2 num2, bool bAbs= false, int ulp= _FLOAT_DEFAULT_ULP)
{
	return bAbs ?
		(_FLOAT_ABS(num1-num2)<_FLOAT_COMMON_EPSILON(Ty1, Ty2)*ulp)
		:
		(_FLOAT_ABS(num1-num2)<_FLOAT_COMMON_EPSILON(Ty1, Ty2)
		*(_FLOAT_ABS(num1)+_FLOAT_ABS(num2))*ulp
		|| _FLOAT_ABS(num1-num2)<_FLOAT_COMMON_MIN(Ty1, Ty2)*ulp);
}

//浮点数比较小于，保证逻辑关系
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_IS_FLOAT_TYPE(Ty1, Ty2), bool
>::type FloatLT(Ty1 num1, Ty2 num2, bool bAbs= false, int ulp= _FLOAT_DEFAULT_ULP)
{
	return num1<num2 && !FloatEQ(num1, num2, bAbs, ulp);
}

//浮点数比较小于等于，保证逻辑关系
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_IS_FLOAT_TYPE(Ty1, Ty2), bool
>::type FloatLTE(Ty1 num1, Ty2 num2, bool bAbs= false, int ulp= _FLOAT_DEFAULT_ULP)
{
	return FloatLT(num1, num2, bAbs, ulp) || FloatEQ(num1, num2, bAbs, ulp);
}

//浮点数比较大于，保证逻辑关系
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_IS_FLOAT_TYPE(Ty1, Ty2), bool
>::type FloatGT(Ty1 num1, Ty2 num2, bool bAbs= false, int ulp= _FLOAT_DEFAULT_ULP)
{
	return !(FloatLT(num1, num2, bAbs, ulp) || FloatEQ(num1, num2, bAbs, ulp));
}

//浮点数比较大于等于，保证逻辑关系
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_IS_FLOAT_TYPE(Ty1, Ty2), bool
>::type FloatGTE(Ty1 num1, Ty2 num2, bool bAbs= false, int ulp= _FLOAT_DEFAULT_ULP)
{
	return !FloatLT(num1, num2, bAbs, ulp);
}

//浮点数比较不等于，保证逻辑关系
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_IS_FLOAT_TYPE(Ty1, Ty2), bool
>::type FloatNEQ(Ty1 num1, Ty2 num2, bool bAbs= false, int ulp= _FLOAT_DEFAULT_ULP)
{
	return !FloatEQ(num1, num2, bAbs, ulp);
}


//浮点数下转化整数
template<typename Ty>
inline typename std::enable_if<std::is_floating_point<Ty>::value, Ty
>::type FloatFloor(Ty num, bool bAbs= false, int ulp= _FLOAT_DEFAULT_ULP)
{
	Ty res = std::round(num);
	if(FloatEQ(num, res, bAbs, ulp))
		return res;
	return std::floor(num);
}

//浮点数上转化整数
template<typename Ty>
inline typename std::enable_if<std::is_floating_point<Ty>::value, Ty
>::type FloatCeil(Ty num, bool bAbs= false, int ulp= _FLOAT_DEFAULT_ULP)
{
	Ty res = std::round(num);
	if(FloatEQ(num, res, bAbs, ulp))
		return res;
	return std::ceil(num);
}
#undef _FLOAT_DEFAULT_ULP
#undef _IS_FLOAT_TYPE
#undef _FLOAT_COMMON_EPSILON
#undef _FLOAT_COMMON_MIN
#undef _FLOAT_ABS



//string的累加操作，使用左移运算符
template<typename Ele, typename Ty>
inline std::string &operator <<(std::basic_string<Ele> &str, Ty &&arg)
{
	return str += std::forward<Ty>(arg);
}



//检查是否为tuple类
template<typename Ty>
struct TypeIsTuple:
	std::false_type
{
};
template<typename ...Ty_S>
struct TypeIsTuple<std::tuple<Ty_S...>>:
	std::true_type
{
};


//tuple类型转换扩展辅助函数
template<typename TyDst, typename TySrc, size_t ...c_idx_s>
inline TyDst _TupleCastAssist(TySrc &&src, IndexSequence<c_idx_s...>)
{
	return TyDst(std::get<c_idx_s>(std::forward<TySrc>(src))...);
}
//tuple类型转换扩展，直接完美转发以支持所有的引用情况
//源类型需要能被get函数拆解，目的类型需要能接受源类型拆解后的构造函数
template<typename TyDst, typename TySrc>
inline TyDst TupleCast(TySrc &&src)
{
	return _TupleCastAssist<TyDst>(std::forward<TySrc>(src), MakeIndexSequence<
		std::tuple_size<typename std::remove_reference<TySrc>::type>::value>());
}



//hash的特例化
namespace std
{
//对pair特例化，没有交换不变性
template<typename Ty1, typename Ty2>
struct hash<std::pair<Ty1, Ty2>>
{
	typedef size_t result_type;
	typedef std::pair<Ty1, Ty2> argument_type;
	result_type operator()(const argument_type &pr) const
	{
		return std::hash<result_type>()(std::hash<Ty1>()(pr.first))
			^ std::hash<Ty2>()(pr.second);
	}
};

//对tuple特例化，没有交换不变性
template<typename ...Tys>
struct hash<std::tuple<Tys...>>
{
	typedef size_t result_type;
	typedef std::tuple<Tys...> argument_type;
	result_type operator()(const argument_type &tup) const
	{
		return Assist<sizeof...(Tys)>(tup);
	}
private:
	template<size_t c_index>
	static result_type Assist(const std::tuple<Tys...> &tup)
	{
		return std::hash<typename std::tuple_element<c_index-1, std::tuple<Tys...>>::type>
			()(std::get<c_index-1>(tup))
			^ std::hash<result_type>()(Assist<c_index-1>(tup));
	}
	template<>
	static result_type Assist<1>(const std::tuple<Tys...> &tup)
	{
		return std::hash<typename std::tuple_element<0, std::tuple<Tys...>>::type>
			()(std::get<0>(tup));
	}
	template<>
	static result_type Assist<0>(const std::tuple<Tys...> &tup)
	{
		return 0;
	}
};
}


//各类迭代器hash类，尾后迭代器解引用不安全
template<typename Ty>
struct IteratorHash
{
	typedef size_t result_type;
	typedef Ty argument_type;
	result_type operator()(argument_type it) const
	{
		return std::hash<decltype(&*it)>()(&*it);
	}
};



//各类迭代器比较函数，尾后迭代器解引用不安全
template<typename Ty>
inline bool IteratorLT(Ty it1, Ty it2)
{
	return &*it1<&*it2;
}
template<typename Ty>
inline bool IteratorEQ(Ty it1, Ty it2)
{
	return &*it1==&*it2;
}


//各类迭代器比较类，尾后迭代器解引用不安全
template<typename Ty>
struct IteratorLess
{
	typedef bool result_type;
	typedef Ty first_argument_type;
	typedef Ty second_argument_type;
	result_type operator()(first_argument_type it1, second_argument_type it2) const
	{
		return IteratorLT(it1, it2);
	}
};
template<typename Ty>
struct IteratorEuqalTo
{
	typedef bool result_type;
	typedef Ty first_argument_type;
	typedef Ty second_argument_type;
	result_type operator()(first_argument_type it1, second_argument_type it2) const
	{
		return IteratorEQ(it1, it2);
	}
};



//字符数字判断
inline constexpr bool CharIsNum(char ch)
{
	return ch>='0' && ch<='9';
}
inline constexpr bool CharIsNotNum(char ch)
{
	return !CharIsNum(ch);
}
//字符字母判断
inline constexpr bool CharIsLet(char ch)
{
	return (ch>='a' && ch<='z') || (ch>='A' && ch<='Z');
}
inline constexpr bool CharIsNotLet(char ch)
{
	return !CharIsLet(ch);
}
//字符数字字母判断
inline constexpr bool CharIsNumLet(char ch)
{
	return (ch>='0' && ch<='9')
		|| (ch>='a' && ch<='z') || (ch>='A' && ch<='Z');
}
inline constexpr bool CharIsNotNumLet(char ch)
{
	return !CharIsNumLet(ch);
}
//字符标识符判断
inline constexpr bool CharIsId(char ch)
{
	return (ch>='0' && ch<='9')
		|| (ch>='a' && ch<='z') || (ch>='A' && ch<='Z')
		|| (ch=='_');
}
inline constexpr bool CharIsNotId(char ch)
{
	return !CharIsId(ch);
}
//字符十六进制判断
inline constexpr bool CharIsHex(char ch)
{
	return (ch>='0' && ch<='9')
		|| (ch>='a' && ch<='f') || (ch>='A' && ch<='F')
		|| (ch=='_');
}
inline constexpr bool CharIsNotHex(char ch)
{
	return !CharIsHex(ch);
}
//字符ascii判断
inline constexpr bool CharIsAscii(char ch)
{
	return ch>=0 && ch<=127;
}
inline constexpr bool CharIsNotAscii(char ch)
{
	return !CharIsAscii(ch);
}
//字符广义标识符判断
inline constexpr bool CharIsBroadId(char ch)
{
	return CharIsId(ch) || CharIsNotAscii(ch);
}
inline constexpr bool CharIsNotBroadId(char ch)
{
	return !CharIsBroadId(ch);
}
//字符空白符判断
inline constexpr bool CharIsBlank(char ch)
{
	return ch==' ' || ch=='\t' || ch=='\r' || ch=='\n'
		|| ch=='\v' || ch=='\f';
}
inline constexpr bool CharIsNotBlank(char ch)
{
	return !CharIsBlank(ch);
}


//数字字符转化数字
inline int CharToNum(char ch)
{
	if(ch>='0' && ch<='9')
		return ch-'0';
	else
		return -1;
}
//字母字符转化数字
inline int CharToLet(char ch)
{
	if(ch>='a' && ch<='z')
		return ch-'a';
	else if(ch>='A' && ch<='Z')
		return ch-'A';
	else
		return -1;
}
//十六进制字符转化数字
inline int CharToHex(char ch)
{
	if(ch>='0' && ch<='9')
		return ch-'0';
	else if(ch>='a' && ch<='z')
		return ch-'a'+10;
	else if(ch>='A' && ch<='Z')
		return ch-'A'+10;
	else
		return -1;
}


//数字转化数字字符
inline char NumToChar(int i)
{
	if(i>=0 && i<=9)
		return i+'0';
	else
		return 0;
}
//数字转化大写字母字符
inline char LetToUppChar(int i)
{
	if(i>=0 && i<=25)
		return i+'A';
	else
		return 0;
}
//数字转化小写字母字符
inline char LetToLowChar(int i)
{
	if(i>=0 && i<=25)
		return i+'a';
	else
		return 0;
}
//数字转化大写十六进制字符
inline char HexToUppChar(int i)
{
	if(i>=0 && i<=9)
		return i+'0';
	else if(i>=10 && i<=15)
		return i+'A';
	else
		return 0;
}
//数字转化小写十六进制字符
inline char HexToLowChar(int i)
{
	if(i>=0 && i<=9)
		return i+'0';
	else if(i>=10 && i<=15)
		return i+'a';
	else
		return 0;
}



//************
//实用类型扩展
//************

//传递异常类
class CatchError:
	public std::runtime_error
{
	//继承基类构造函数（除默认拷贝构造）
	using std::runtime_error::runtime_error;
public:
	//默认构造
	CatchError():
		std::runtime_error("")
	{
	}
	//继承基类赋值
	using std::runtime_error::operator =;
	//省略合成5拷贝
	//省略继承what
};

//传递异常类扩展
template<int c_n>
class CatchErrorEx:
	virtual public CatchError
{
	//继承基类构造函数（除默认拷贝构造）
	using CatchError::CatchError;
public:
	//默认构造
	CatchErrorEx()
		= default;
	//拷贝
	CatchErrorEx(const CatchErrorEx &other)
		= default;
	CatchErrorEx &operator =(const CatchErrorEx &other)
		= default;
	//移动
	CatchErrorEx(CatchErrorEx &&other) noexcept
		= default;
	CatchErrorEx &operator =(CatchErrorEx &&other) noexcept
		= default;
	//继承基类赋值
	using CatchError::operator =;
	//省略合成析构
	//省略继承what
};

//传递类型异常类
template<typename Ty>
class TypeError:
	virtual public CatchError
{
public:
	Ty value;//存储值
public:
	//仅参数构造，基类参数传递
	template<typename ...Tys>
	explicit TypeError(const Ty &o_value, Tys &&...args):
		CatchError(std::forward<Tys>(args)...), value(o_value)
	{
	}
	template<typename ...Tys>
	explicit TypeError(Ty &&o_value, Tys &&...args):
		CatchError(std::forward<Tys>(args)...), value(std::move(o_value))
	{
	}
	//拷贝
	TypeError(const TypeError &other)
		= default;
	TypeError &operator =(const TypeError &other)
		= default;
	//移动
	TypeError(TypeError &&other) noexcept
		= default;
	TypeError &operator =(TypeError &&other) noexcept
		= default;
	//值类型赋值运算符重载
	TypeError &operator =(const Ty &o_value)
	{
		value = o_value;
		return *this;
	}
	//值引用转换
	operator Ty &()
	{
		return value;
	}
	operator const Ty &() const
	{
		return value;
	}
	//继承基类赋值
	using CatchError::operator =;
	//省略合成析构
	//省略继承what
};

//传递类型异常类扩展
template<typename Ty, int c_n>
class TypeErrorEx:
	public TypeError<Ty>, public CatchErrorEx<c_n>
{
	//继承TypeError基类构造函数（除默认拷贝构造）
	using TypeError::TypeError;
public:
	//拷贝
	TypeErrorEx(const TypeErrorEx &other)
		= default;
	TypeErrorEx &operator =(const TypeErrorEx &other)
		= default;
	//移动
	TypeErrorEx(TypeErrorEx &&other) noexcept
		= default;
	TypeErrorEx &operator =(TypeErrorEx &&other) noexcept
		= default;
	//继承所有基类赋值
	using TypeError<Ty>::operator =;
	using CatchErrorEx<c_n>::operator =;
	//省略合成析构
	//省略继承what、value、引用转换
};



//计时器类，Rep计时变量类型，Period计时单位
template<typename Rep= double, typename Period= std::ratio<1>>
class Timer
{
private:
	std::chrono::steady_clock::time_point tmptStart;//记录起始时间点
	Rep recordCount;//存储记录时间段

public:
	//构造函数，bStart是否开始计时
	explicit Timer(bool bStart= false)
	{
		if(bStart)
			Start();
	}
	//开始计时
	void Start()
	{
		tmptStart = std::chrono::steady_clock::now();
	}
	//记录时间并返回
	Rep Record()
	{
		recordCount = std::chrono::duration_cast<typename std::chrono::duration<Rep, Period>>
			(std::chrono::steady_clock::now()-tmptStart).count();
		return recordCount;
	}
	//获取上次时间
	Rep Get()
	{
		return tmduRecord;
	}
};



//保存参数包类
template<typename ...Ty_S>
class ArgsTuple:
	public std::tuple<Ty_S...>
{
	//继承基类构造函数（除默认拷贝构造）
	using std::tuple<Ty_S...>::tuple;
public:
	//默认构造
	ArgsTuple()
		= default;
	//继承基类赋值
	using std::tuple<Ty_S...>::operator =;
	//省略合成5拷贝
};
//继承非成员函数
//tuple_cat, std::get(std::tuple), std::swap(std::tuple)
//operator==, operator!=, operator<, operator<=, operator>, operator>=


//tuple转化对应的ArgsTuple类辅助类型
template<typename Ty>
struct TupleToArgsTuple
{
};
template<typename ...Ty_S>
struct TupleToArgsTuple<std::tuple<Ty_S...>>
{
	using type = ArgsTuple<Ty_S...>;
};
template<typename Ty>
struct TupleToArgsTuple<const Ty>
{
	using type = typename std::add_const<TupleToArgsTuple<Ty>>::type;
};
template<typename Ty>
struct TupleToArgsTuple<volatile Ty>
{
	using type = typename std::add_volatile<TupleToArgsTuple<Ty>>::type;
};
template<typename Ty>
struct TupleToArgsTuple<Ty &>
{
	using type = typename std::add_lvalue_reference<TupleToArgsTuple<Ty>>::type;
};
template<typename Ty>
struct TupleToArgsTuple<Ty &&>
{
	using type = typename std::add_rvalue_reference<TupleToArgsTuple<Ty>>::type;
};

//ArgsTuple转化对应的tuple类辅助类型
template<typename Ty>
struct ArgsTupleToTuple
{
};
template<typename ...Ty_S>
struct ArgsTupleToTuple<ArgsTuple<Ty_S...>>
{
	using type = std::tuple<Ty_S...>;
};
template<typename Ty>
struct ArgsTupleToTuple<const Ty>
{
	using type = typename std::add_const<ArgsTupleToTuple<Ty>>::type;
};
template<typename Ty>
struct ArgsTupleToTuple<volatile Ty>
{
	using type = typename std::add_volatile<ArgsTupleToTuple<Ty>>::type;
};
template<typename Ty>
struct ArgsTupleToTuple<Ty &>
{
	using type = typename std::add_lvalue_reference<ArgsTupleToTuple<Ty>>::type;
};
template<typename Ty>
struct ArgsTupleToTuple<Ty &&>
{
	using type = typename std::add_rvalue_reference<ArgsTupleToTuple<Ty>>::type;
};


//检查是否为ArgsTuple类
template<typename Ty>
struct TypeIsArgsTuple:
	std::false_type
{
};
template<typename ...Ty_S>
struct TypeIsArgsTuple<ArgsTuple<Ty_S...>>:
	std::true_type
{
};


//求ArgsTuple大小的类
template<typename Ty>
struct ArgsTupleSize:
	std::integral_constant<size_t, std::tuple_size<
	typename ArgsTupleToTuple<Ty>::type>::value>
{
};

//求ArgsTuple元素类别的类
template<size_t c_idx, typename Ty>
struct ArgsTupleElement
{
	using type = typename std::tuple_element<c_idx,
		typename ArgsTupleToTuple<Ty>::type>::type;
};


//ArgsTuple类值类型辅助构造类
template<typename ...Ty_S>
inline auto MakeArgsTuple(Ty_S &&...arg_s)
{
	auto ret = std::make_tuple(std::forward<Ty_S>(arg_s)...);
	return TupleCast<typename TupleToArgsTuple<decltype(ret)>::type>(std::move(ret));
}

//ArgsTuple类引用类型辅助构造类
template<typename ...Ty_S>
inline auto TieArgsTuple(Ty_S &&...arg_s)
{
	auto ret = std::tie(std::forward<Ty_S>(arg_s)...);
	return TupleCast<typename TupleToArgsTuple<decltype(ret)>::type>(std::move(ret));
}

//ArgsTuple类转发类型辅助构造类
template<typename ...Ty_S>
inline auto ForwardAsArgsTuple(Ty_S &&...arg_s)
{
	auto ret = std::forward_as_tuple(std::forward<Ty_S>(arg_s)...);
	return TupleCast<typename TupleToArgsTuple<decltype(ret)>::type>(std::move(ret));
}

//连接ArgsTuple类
template<typename ...Ty_S>
inline auto ArgsTupleCat(Ty_S &&...arg_s)
{
	auto ret = std::tuple_cat(std::forward<Ty_S>(arg_s)...);
	return TupleCast<typename TupleToArgsTuple<decltype(ret)>::type>(std::move(ret));
}


//ArgsTuple类型转换重载扩展函数辅助函数
template<typename TyDst, typename TySrc, size_t ...c_idx_s>
inline TyDst _ArgsTupleOverloadCastAssist(TySrc &&src, IndexSequence<c_idx_s...>)
{
	return TyDst(std::get<c_idx_s>(std::forward<TySrc>(src))...);
}
//ArgsTuple类型转换重载扩展函数，完美转发源类型
//将ArgsTuple拆包后转化为指定类型，其他类型直接转化为指定类型
template<typename TyDst, typename TySrc, typename TyBlank= BlankType<0>>
inline typename std::enable_if<!TypeIsArgsTuple<
	typename RemoveCVRef<TySrc>::type>::value, TyDst
>::type _ArgsTupleOverloadCast(TySrc &&src)
{
	return TyDst(std::forward<TySrc>(src));
}
template<typename TyDst, typename TySrc>
inline typename std::enable_if<TypeIsArgsTuple<
	typename RemoveCVRef<TySrc>::type>::value, TyDst
>::type _ArgsTupleOverloadCast(TySrc &&src)
{
	return _ArgsTupleOverloadCastAssist<TyDst>(std::forward<TySrc>(src),
		MakeIndexSequence<ArgsTupleSize<
		typename std::remove_reference<TySrc>::type>::value>());
}

//将ArgsTuple转化为与其对应参数引用类型的tuple，否则转化为包含其引用类型的tuple
//非参数包情况，直接作为tuple的参数
template<typename Ty>
struct _ArgsTupleOverloadToRefTuple
{
	using type = std::tuple<Ty>;
};
//参数包的8种情况特化，将参数包里面的类型附加对应属性后作为tuple的参数
template<typename ...Ty_S>
struct _ArgsTupleOverloadToRefTuple<ArgsTuple<Ty_S...> &>
{
	using type = std::tuple<typename std::add_lvalue_reference<Ty_S>::type...>;
};
template<typename ...Ty_S>
struct _ArgsTupleOverloadToRefTuple<ArgsTuple<Ty_S...> &&>
{
	using type = std::tuple<typename std::add_rvalue_reference<Ty_S>::type...>;
};
template<typename ...Ty_S>
struct _ArgsTupleOverloadToRefTuple<const ArgsTuple<Ty_S...> &>
{
	using type = std::tuple<typename std::add_lvalue_reference<
		typename std::add_const<Ty_S>::type>::type...>;
};
template<typename ...Ty_S>
struct _ArgsTupleOverloadToRefTuple<const ArgsTuple<Ty_S...> &&>
{
	using type = std::tuple<typename std::add_rvalue_reference<
		typename std::add_const<Ty_S>::type>::type...>;
};
template<typename ...Ty_S>
struct _ArgsTupleOverloadToRefTuple<volatile ArgsTuple<Ty_S...> &>
{
	using type = std::tuple<typename std::add_lvalue_reference<
		typename std::add_volatile<Ty_S>::type>::type...>;
};
template<typename ...Ty_S>
struct _ArgsTupleOverloadToRefTuple<volatile ArgsTuple<Ty_S...> &&>
{
	using type = std::tuple<typename std::add_rvalue_reference<
		typename std::add_volatile<Ty_S>::type>::type...>;
};
template<typename ...Ty_S>
struct _ArgsTupleOverloadToRefTuple<const volatile ArgsTuple<Ty_S...> &>
{
	using type = std::tuple<typename std::add_lvalue_reference<
		typename std::add_cv<Ty_S>::type>::type...>;
};
template<typename ...Ty_S>
struct _ArgsTupleOverloadToRefTuple<const volatile ArgsTuple<Ty_S...> &&>
{
	using type = std::tuple<typename std::add_rvalue_reference<
		typename std::add_cv<Ty_S>::type>::type...>;
};


//ArgsTuple引用连接操作，变成连接后的tuple类型，计算返回类型辅助类
template<typename ...Ty_S>
struct _ArgsTupleRefCatToTupleReturnType
{
	using type = decltype(std::tuple_cat(std::declval<
		typename _ArgsTupleOverloadToRefTuple<Ty_S>::type>()...));
};

//ArgsTuple引用连接操作，计算返回类型辅助类
template<typename ...Ty_S>
struct ArgsTupleRefCatReturnType
{
	using type =  typename TupleToArgsTuple<
		typename _ArgsTupleRefCatToTupleReturnType<Ty_S...>::type>::type;
};


//ArgsTuple引用连接到tuple，返回连接后的tuple类型
template<typename ...Ty_S>
inline typename _ArgsTupleRefCatToTupleReturnType<Ty_S &&...
>::type _ArgsTupleRefCatToTuple(Ty_S &&...arg_s)
{
	return std::tuple_cat(_ArgsTupleOverloadCast<
		typename _ArgsTupleOverloadToRefTuple<Ty_S &&>::type>(std::forward<Ty_S>(arg_s))...);
}

//ArgsTuple引用连接操作，支持与普通类型混合连接，连接后均为引用类型
template<typename ...Ty_S>
inline typename ArgsTupleRefCatReturnType<Ty_S &&...
>::type ArgsTupleRefCat(Ty_S &&...arg_s)
{
	return TupleCast<typename ArgsTupleRefCatReturnType<Ty_S &&...>::type>(
		_ArgsTupleRefCatToTuple(std::forward<Ty_S>(arg_s)...));
}


//ArgsTuple递归引用合并到tuple类返回值类型计算辅助类辅助类，要求最前面有一个值类型tuple类
template<typename ...Ty_S>
struct _ArgsTupleRecurRefCatToTupleReturnTypeAssist
{
};
//解包完成特化
//注意ready的tuple是值类型
template<typename ...TyReady_S>
struct _ArgsTupleRecurRefCatToTupleReturnTypeAssist<std::tuple<TyReady_S...>>
{
	using type = std::tuple<TyReady_S...>;
};
//当前直接传参特化
template<typename Ty, typename ...TyReady_S, typename ...TyRest_S>
struct _ArgsTupleRecurRefCatToTupleReturnTypeAssist<std::tuple<TyReady_S...>,
	Ty, TyRest_S...>
{
	using type = typename _ArgsTupleRecurRefCatToTupleReturnTypeAssist<
		std::tuple<TyReady_S..., Ty>, TyRest_S...>::type;
};
//当前解包8种情况特化
template<typename ...TyReady_S, typename ...TyArgs_S, typename ...TyRest_S>
struct _ArgsTupleRecurRefCatToTupleReturnTypeAssist<std::tuple<TyReady_S...>,
	ArgsTuple<TyArgs_S...> &, TyRest_S...>
{
	using type = typename _ArgsTupleRecurRefCatToTupleReturnTypeAssist<
		std::tuple<TyReady_S...>,
		typename std::add_lvalue_reference<TyArgs_S>::type...,
		TyRest_S...>::type;
};
template<typename ...TyReady_S, typename ...TyArgs_S, typename ...TyRest_S>
struct _ArgsTupleRecurRefCatToTupleReturnTypeAssist<std::tuple<TyReady_S...>,
	ArgsTuple<TyArgs_S...> &&, TyRest_S...>
{
	using type = typename _ArgsTupleRecurRefCatToTupleReturnTypeAssist<
		std::tuple<TyReady_S...>,
		typename std::add_rvalue_reference<TyArgs_S>::type...,
		TyRest_S...>::type;
};
template<typename ...TyReady_S, typename ...TyArgs_S, typename ...TyRest_S>
struct _ArgsTupleRecurRefCatToTupleReturnTypeAssist<std::tuple<TyReady_S...>,
	const ArgsTuple<TyArgs_S...> &, TyRest_S...>
{
	using type = typename _ArgsTupleRecurRefCatToTupleReturnTypeAssist<
		std::tuple<TyReady_S...>,
		typename std::add_lvalue_reference<typename std::add_const<TyArgs_S>::type>::type...,
		TyRest_S...>::type;
};
template<typename ...TyReady_S, typename ...TyArgs_S, typename ...TyRest_S>
struct _ArgsTupleRecurRefCatToTupleReturnTypeAssist<std::tuple<TyReady_S...>,
	const ArgsTuple<TyArgs_S...> &&, TyRest_S...>
{
	using type = typename _ArgsTupleRecurRefCatToTupleReturnTypeAssist<
		std::tuple<TyReady_S...>,
		typename std::add_rvalue_reference<typename std::add_const<TyArgs_S>::type>::type...,
		TyRest_S...>::type;
};
template<typename ...TyReady_S, typename ...TyArgs_S, typename ...TyRest_S>
struct _ArgsTupleRecurRefCatToTupleReturnTypeAssist<std::tuple<TyReady_S...>,
	volatile ArgsTuple<TyArgs_S...> &, TyRest_S...>
{
	using type = typename _ArgsTupleRecurRefCatToTupleReturnTypeAssist<
		std::tuple<TyReady_S...>,
		typename std::add_lvalue_reference<typename std::add_volatile<TyArgs_S>::type>::type...,
		TyRest_S...>::type;
};
template<typename ...TyReady_S, typename ...TyArgs_S, typename ...TyRest_S>
struct _ArgsTupleRecurRefCatToTupleReturnTypeAssist<std::tuple<TyReady_S...>,
	volatile ArgsTuple<TyArgs_S...> &&, TyRest_S...>
{
	using type = typename _ArgsTupleRecurRefCatToTupleReturnTypeAssist<
		std::tuple<TyReady_S...>,
		typename std::add_rvalue_reference<typename std::add_volatile<TyArgs_S>::type>::type...,
		TyRest_S...>::type;
};
template<typename ...TyReady_S, typename ...TyArgs_S, typename ...TyRest_S>
struct _ArgsTupleRecurRefCatToTupleReturnTypeAssist<std::tuple<TyReady_S...>,
	const volatile ArgsTuple<TyArgs_S...> &, TyRest_S...>
{
	using type = typename _ArgsTupleRecurRefCatToTupleReturnTypeAssist<
		std::tuple<TyReady_S...>,
		typename std::add_lvalue_reference<typename std::add_cv<TyArgs_S>::type>::type...,
		TyRest_S...>::type;
};
template<typename ...TyReady_S, typename ...TyArgs_S, typename ...TyRest_S>
struct _ArgsTupleRecurRefCatToTupleReturnTypeAssist<std::tuple<TyReady_S...>,
	const volatile ArgsTuple<TyArgs_S...> &&, TyRest_S...>
{
	using type = typename _ArgsTupleRecurRefCatToTupleReturnTypeAssist<
		std::tuple<TyReady_S...>,
		typename std::add_rvalue_reference<typename std::add_cv<TyArgs_S>::type>::type...,
		TyRest_S...>::type;
};

//ArgsTuple递归引用合并返回值类型计算辅助类
template<typename ...TyRest_S>
struct ArgsTupleRecurRefCatReturnType
{
	using type = typename TupleToArgsTuple<
		typename _ArgsTupleRecurRefCatToTupleReturnTypeAssist<
		std::tuple<>, TyRest_S...>::type>::type;
};


//使用ArgsTuple类递归引用合并为tuple的前置声明
template<typename ...TyReady_S>
inline typename _ArgsTupleRecurRefCatToTupleReturnTypeAssist<std::tuple<TyReady_S...>
>::type _ArgsTupleRecurRefCatToTuple(std::tuple<TyReady_S...> &&ready
);
template<typename TyArgsTup, typename ...TyReady_S, typename ...TyRest_S,
	typename TyBlank= BlankType<0>
> inline typename std::enable_if<TypeIsArgsTuple<typename RemoveCVRef<TyArgsTup>::type>::value,
	typename _ArgsTupleRecurRefCatToTupleReturnTypeAssist<std::tuple<TyReady_S...>,
	TyArgsTup &&, TyRest_S &&...>::type
>::type _ArgsTupleRecurRefCatToTuple(std::tuple<TyReady_S...> &&ready,
	TyArgsTup &&argsTup, TyRest_S &&...rest_s
);
template<typename Ty, typename ...TyReady_S, typename ...TyRest_S>
inline typename std::enable_if<!TypeIsArgsTuple<typename RemoveCVRef<Ty>::type>::value,
	typename _ArgsTupleRecurRefCatToTupleReturnTypeAssist<std::tuple<TyReady_S...>,
	Ty &&, TyRest_S &&...>::type
>::type _ArgsTupleRecurRefCatToTuple(std::tuple<TyReady_S...> &&ready,
	Ty &&arg, TyRest_S &&...rest_s
);
//使用ArgsTuple类递归引用合并为tuple的辅助函数，将参数包拆开后添加到剩余项中
template<typename ...TyReady_S, typename TyArgsTup, size_t ...c_idx_s,
	typename ...TyRest_S
> inline typename _ArgsTupleRecurRefCatToTupleReturnTypeAssist<std::tuple<TyReady_S...>,
	TyArgsTup &&, TyRest_S &&...
>::type _ArgsTupleRecurRefCatToTupleAssist(std::tuple<TyReady_S...> &&ready,
	TyArgsTup &&argsTup, IndexSequence<c_idx_s...>, TyRest_S &&...rest_s)
{
	return _ArgsTupleRecurRefCatToTuple(
		std::move(ready), std::get<c_idx_s>(std::forward<TyArgsTup>(argsTup))...,
		std::forward<TyRest_S>(rest_s)...);
}

//使用ArgsTuple类递归引用合并为tuple，递归将当前存入准备好的tuple
//递归结束重载
template<typename ...TyReady_S>
inline typename _ArgsTupleRecurRefCatToTupleReturnTypeAssist<std::tuple<TyReady_S...>
>::type _ArgsTupleRecurRefCatToTuple(std::tuple<TyReady_S...> &&ready)
{
	return std::move(ready);
}
//递归到参数包重载
template<typename TyArgsTup, typename ...TyReady_S, typename ...TyRest_S,
	typename TyBlank/*= BlankType<0>*/
> inline typename std::enable_if<TypeIsArgsTuple<typename RemoveCVRef<TyArgsTup>::type>::value,
	typename _ArgsTupleRecurRefCatToTupleReturnTypeAssist<std::tuple<TyReady_S...>,
	TyArgsTup &&, TyRest_S &&...>::type
>::type _ArgsTupleRecurRefCatToTuple(std::tuple<TyReady_S...> &&ready,
	TyArgsTup &&argsTup, TyRest_S &&...rest_s)
{
	return _ArgsTupleRecurRefCatToTupleAssist(
		std::move(ready), std::forward<TyArgsTup>(argsTup),
		MakeIndexSequence<ArgsTupleSize<typename std::remove_reference<
		TyArgsTup>::type>::value>(),std::forward<TyRest_S>(rest_s)...);
}
//递归到非参数包重载
template<typename Ty, typename ...TyReady_S, typename ...TyRest_S>
inline typename std::enable_if<!TypeIsArgsTuple<typename RemoveCVRef<Ty>::type>::value,
	typename _ArgsTupleRecurRefCatToTupleReturnTypeAssist<std::tuple<TyReady_S...>,
	Ty &&, TyRest_S &&...>::type
>::type _ArgsTupleRecurRefCatToTuple(std::tuple<TyReady_S...> &&ready,
	Ty &&arg, TyRest_S &&...rest_s)
{
	return _ArgsTupleRecurRefCatToTuple(
		std::tuple_cat(std::move(ready), std::tuple<Ty &&>(std::forward<Ty>(arg))),
		std::forward<TyRest_S>(rest_s)...);
}

//使用ArgsTuple类递归引用合并，支持多个ArgsTuple与普通变量递归混合使用
template<typename ...Ty_S>
inline typename ArgsTupleRecurRefCatReturnType<Ty_S &&...
>::type ArgsTupleRecurRefCat(Ty_S &&...arg_s)
{
	return TupleCast<typename ArgsTupleRecurRefCatReturnType<Ty_S &&...>::type>(
		_ArgsTupleRecurRefCatToTuple(std::tuple<>(), std::forward<Ty_S>(arg_s)...));
}


//计算使用值类型tuple调用函数的返回类型辅助类
template<typename ...Tys>
struct _ValueTupleSimpleInvokeReturnType
{
};
template<typename TyFunc, typename ...Ty_S>
struct _ValueTupleSimpleInvokeReturnType<TyFunc, std::tuple<Ty_S...>>
{
	using type = decltype(std::declval<TyFunc>()(std::declval<Ty_S>()...));
};

//ArgsTuple调用，计算返回类型辅助类
template<typename TyFunc, typename ...Ty_S>
struct ArgsTupleInvokeReturnType
{
	using type = typename _ValueTupleSimpleInvokeReturnType<TyFunc,
		typename _ArgsTupleRefCatToTupleReturnType<Ty_S...>::type>::type;
};

//使用单个值类型tuple类调用函数辅助函数，使用整数序列模板展开调用
template<typename TyFunc, typename TyTup, size_t ...c_idx_s>
inline typename _ValueTupleSimpleInvokeReturnType<TyFunc &&, TyTup
>::type _ValueTupleSimpleInvoke(TyFunc &&func, TyTup &&argsTup,
	IndexSequence<c_idx_s...>)
{
	return std::forward<TyFunc>(func)(std::get<c_idx_s>(std::forward<TyTup>(argsTup))...);
}
//使用单个ArgsTuple类调用函数，只能使用一个参数包类型
template<typename TyFunc, typename TyArgsTup>
inline typename std::enable_if<TypeIsArgsTuple<typename RemoveCVRef<TyArgsTup>::type>::value,
	typename ArgsTupleInvokeReturnType<TyFunc &&, TyArgsTup &&>::type
>::type ArgsTupleSimpleInvoke(TyFunc &&func, TyArgsTup &&argsTup)
{
	return _ValueTupleSimpleInvoke(std::forward<TyFunc>(func),
		_ArgsTupleOverloadCast<typename _ArgsTupleOverloadToRefTuple<
		TyArgsTup &&>::type>(std::forward<TyArgsTup>(argsTup)),
		MakeIndexSequence<ArgsTupleSize<
		typename std::remove_reference<TyArgsTup>::type>::value>());
}


//使用ArgsTuple类调用函数，支持多个参数包与普通变量混合使用
template<typename TyFunc, typename ...Ty_S>
inline typename ArgsTupleInvokeReturnType<TyFunc &&, Ty_S &&...
>::type ArgsTupleInvoke(TyFunc &&func, Ty_S &&...arg_s)
{
	return _ValueTupleSimpleInvoke(std::forward<TyFunc>(func),
		_ArgsTupleRefCatToTuple(std::forward<Ty_S>(arg_s)...),
		MakeIndexSequence<std::tuple_size<
		typename _ArgsTupleRefCatToTupleReturnType<Ty_S &&...>::type>::value>());
}


//ArgsTuple递归调用，计算返回类型辅助类
template<typename TyFunc, typename ...Ty_S>
struct ArgsTupleRecurInvokeReturnType
{
	using type = typename _ValueTupleSimpleInvokeReturnType<TyFunc,
		typename _ArgsTupleRecurRefCatToTupleReturnTypeAssist<
		std::tuple<>, Ty_S...>::type>::type;
};

//使用ArgsTuple类递归调用函数，支持多个参数包与普通变量递归的混合使用
template<typename TyFunc, typename ...Ty_S>
inline typename ArgsTupleRecurInvokeReturnType<TyFunc &&, Ty_S &&...
>::type ArgsTupleRecurInvoke(TyFunc &&func, Ty_S &&...arg_s)
{
	return _ValueTupleSimpleInvoke(std::forward<TyFunc>(func),
		_ArgsTupleRecurRefCatToTuple(std::tuple<>(), std::forward<Ty_S>(arg_s)...),
		MakeIndexSequence<std::tuple_size<
		typename _ArgsTupleRecurRefCatToTupleReturnTypeAssist<
		std::tuple<>, Ty_S &&...>::type>::value>());
}



//****************
//工具构造转换函数
//****************

//将timeval转化为duration
template<typename Ty= BlankType<0>>
inline std::chrono::microseconds TimevalToDuration(const timeval &tmval)
{
	return std::chrono::microseconds((long long)tmval.tv_sec*1000000+tmval.tv_usec);
}


//由tm转化为格式化字符串
template<typename Ty= BlankType<0>>
inline std::string TimeToStr(const std::tm &tmStu,
	const std::string &fmt="%F %T", int maxLen= 256)
{
	std::string str(maxLen, '\0');
	if(strftime(&str[0], str.size(), fmt.c_str(), &tmStu)) {
		auto it = std::find(str.begin(), str.end(), '\0');
		str.resize(it-str.begin());
	}
	else
		str.clear();
	return str;
}

//由time_t转化为格式化字符串，bThdSafe进行互斥，若pMtx为null则使用局部静态锁
template<typename TyPtr= std::mutex *>
inline std::string TimeToStr(std::time_t tmNum, bool bLocal= true,
	bool bThdSafe= false, const std::string &fmt="%F %T", TyPtr pMtx= nullptr)
{
	static std::mutex s_mtx;
	if(tmNum>=(1LL<<31))
		tmNum = (1LL<<31)-1;
	else if(tmNum<0)
		tmNum = 0;
	//加锁
	if(bThdSafe) {
		if(pMtx==nullptr)
			s_mtx.lock();
		else
			pMtx->lock();
	}
	//转化
	std::tm *pTimeStu;
	if(bLocal)
		pTimeStu = std::localtime(&tmNum);
	else
		pTimeStu = std::gmtime(&tmNum);
	string str = TimeToStr(*pTimeStu, fmt);
	//解锁
	if(bThdSafe) {
		if(pMtx==nullptr)
			s_mtx.unlock();
		else
			pMtx->unlock();
	}
	return str;
}

//由duration转化为格式化字符串
//DecimalBit为负则自动推断小数位数，非负则设定小数位数，格式中添加%@表示小数部分（带小数点）
template<int c_decimalBit, typename Rep, typename Period,
	typename TyPtr= std::mutex *
> inline typename std::enable_if<
	std::is_integral<Rep>::value && std::is_signed<Rep>::value, std::string
>::type TimeToStr(std::chrono::duration<Rep, Period> tmdu, bool bLocal= true,
	bool bThdSafe= false, const std::string &fmt ="%F %T%@", TyPtr pMtx= nullptr)
{
	if(tmdu<std::chrono::seconds(0))
		tmdu = std::chrono::seconds(0);
	auto tmduInt = std::chrono::duration_cast<std::chrono::seconds>(tmdu);
	//自动推断位数
	constexpr int c_realDecimalBit = c_decimalBit<0 ?
		_TypeExtendRatioPrecision<Period, std::ratio<10>>() : c_decimalBit;
	constexpr long long c_decimalNum = _TypeExtendIntPower(10, c_realDecimalBit);
	//计算小数部分
	auto tmduDecimal = std::chrono::duration_cast<
		std::chrono::duration<long long, std::ratio<1, c_decimalNum>>>(tmdu);
	tmduDecimal -= tmduInt;
	//变更格式字符串
	std::ostringstream oss;
	oss <<std::setfill('0') <<std::right;
	for(int i=0; i!=fmt.size(); ++i) {
		//百分号则处理双字母
		if(fmt[i]=='%' && i+1!=fmt.size()) {
			if(fmt[i+1]!='@')
				oss <<fmt[i] <<fmt[i+1];
			else if(c_realDecimalBit>0)
				oss <<'.' <<std::setw(c_realDecimalBit) <<tmduDecimal.count();
			++ i;
		}
		//否则直接传入
		else
			oss <<fmt[i];
	}
	string newfmt = oss.str();
	//使用新格式计算
	return TimeToStr(tmduInt.count(), bLocal, bThdSafe, newfmt, pMtx);
}

//由system_clock::time_point转化为格式化字符串
template<typename TyPtr= std::mutex *>
inline std::string TimeToStr(
	const std::chrono::system_clock::time_point &tmpt,
	bool bLocal= true, bool bThdSafe= false,
	const std::string &fmt ="%F %T", TyPtr pMtx= nullptr)
{
	return TimeToStr(std::chrono::system_clock::to_time_t(tmpt),
		bLocal, bThdSafe, fmt, pMtx);
}

//高精度由system_clock::time_point转化为格式化字符串，由实现保证正确性
//DecimalBit为负则自动推断小数位数，非负则设定小数位数，格式中添加%@表示小数部分（带小数点）
template<int c_decimalBit, typename TyPtr= std::mutex *>
inline std::string TimeToStr(
	const std::chrono::time_point<std::chrono::system_clock> &tmpt,
	bool bLocal= true, bool bThdSafe= false,
	const std::string &fmt ="%F %T%@", TyPtr pMtx= nullptr)
{
	return TimeToStr<c_decimalBit>(tmpt.time_since_epoch(),
		bLocal, bThdSafe, fmt, pMtx);
}

//由timeval转化为格式化字符串，格式中添加%@表示小数部分（带小数点）
template<int c_decimalBit, typename TyPtr= std::mutex *>
inline std::string TimeToStr(const timeval &tmval, bool bLocal= true,
	bool bThdSafe= false, const std::string &fmt ="%F %T%@", TyPtr pMtx= nullptr)
{
	return TimeToStr<c_decimalBit>(TimevalToDuration(tmval), bLocal, bThdSafe, fmt, pMtx);
}


//将当前时间转化为字符串
template<typename TyPtr= std::mutex *>
inline std::string NowTimeToStr(bool bLocal= true, bool bThdSafe= false,
	const std::string &fmt ="%F %T", TyPtr pMtx= nullptr)
{
	return TimeToStr(std::chrono::system_clock::now(),
		bLocal, bThdSafe, fmt, pMtx);
}

//高精度将当前时间转化为字符串，由实现保证正确性
//DecimalBit为负则自动推断小数位数，非负则设定小数位数，格式中添加%@表示小数部分（带小数点）
template<int c_decimalBit, typename TyPtr= std::mutex *>
inline std::string NowTimeToStr(bool bLocal= true, bool bThdSafe= false,
	const std::string &fmt ="%F %T%@", TyPtr pMtx= nullptr)
{
	return TimeToStr<c_decimalBit>(std::chrono::system_clock::now(),
		bLocal, bThdSafe, fmt, pMtx);
}



//字符串转化数字，各类型重载
inline void FromString(const std::string &str, int &num)
{
	num = std::stoi(str);
}
inline void FromString(const std::string &str, long &num)
{
	num = std::stol(str);
}
inline void FromString(const std::string &str, long long &num)
{
	num = std::stoll(str);
}
inline void FromString(const std::string &str, unsigned &num)
{
	num = std::stoul(str);
}
inline void FromString(const std::string &str, unsigned long &num)
{
	num = std::stoul(str);
}
inline void FromString(const std::string &str, unsigned long long &num)
{
	num = std::stoull(str);
}
inline void FromString(const std::string &str, float &num)
{
	num = std::stof(str);
}
inline void FromString(const std::string &str, double &num)
{
	num = std::stod(str);
}
inline void FromString(const std::string &str, long double &num)
{
	num = std::stold(str);
}
inline bool FromString(const std::string &str, std::string &str2)
{
	str2 = str;
	return true;
}
inline bool FromString(std::string &&str, std::string &str2)
{
	str2 = std::move(str);
	return true;
}
template<typename Ty, typename TyIter>
inline bool FromString(TyIter itSt, TyIter itEd, Ty &num)
{
	return FromString(std::string(itSt, itEd), num);
}

//字符串转化数字，无异常版本，返回是否成功
template<typename Ty>
inline bool FromStringNoError(const std::string &str, Ty &num)
{
	bool ret = true;
	try {
		FromString(str, num);
	}
	catch(std::invalid_argument &) {
		ret = false;
		num = 0;
	}
	catch(std::out_of_range &) {
		ret = false;
		num = 0;
	}
	return ret;
}
template<typename Ty, typename TyIter>
inline bool FromStringNoError(TyIter itSt, TyIter itEd, Ty &num)
{
	return FromStringNoError(std::string(itSt, itEd), num);
}



//整形数字转化字符串，带有千位分隔符
template<typename Ty>
inline typename std::enable_if<std::is_integral<Ty>::value, std::string
>::type GapToString(Ty num, int gap= 3, char ch= '\'')
{
	std::string str;
	//取正
	if(IntLT(num, 0))
		num = -num;
	//反算字符串
	for(int i=0; ; ++i) {
		if(i!=0 && i%gap==0)
			str += ch;
		str += num%10+'0';
		num /= 10;
		if(num==0)
			break;
	}
	//加入负号
	if(IntLT(num, 0))
		str += '-';
	//反转
	std::reverse(str.begin(), str.end());
	return str;
}



//字符串转化宽字符串
inline std::mutex &_StrAndWstrTransStaticVariable()
{
	static std::mutex mtx;
	return mtx;
}
template<typename TyPtr= std::mutex *>
inline bool StrToWstr(std::wstring &wstr, const std::string &str,
	const std::string local ="chs", bool bThdSafe= false, TyPtr pMtx= nullptr)
{
	std::mutex &s_mtx = _StrAndWstrTransStaticVariable();
	//宽字符开空间
	wstr.assign(str.size(), '\0');
	//加锁
	if(bThdSafe) {
		if(pMtx==nullptr)
			s_mtx.lock();
		else
			pMtx->lock();
	}
	//设置本地环境
	std::string strOri = std::setlocale(LC_CTYPE, nullptr);
	std::setlocale(LC_CTYPE, local.c_str());
	//进行转换
	size_t res = mbstowcs(&wstr[0], &str[0], wstr.size());
	std::setlocale(LC_CTYPE, strOri.c_str());
	//解锁
	if(bThdSafe) {
		if(pMtx==nullptr)
			s_mtx.unlock();
		else
			pMtx->unlock();
	}
	//若成功
	if(res!=(size_t)(-1)) {
		wstr.resize(res);
		return true;
	}
	//若失败
	else {
		wstr.empty();
		return false;
	}
}

//宽字符串转化字符串
template<typename TyPtr= std::mutex *>
inline bool WstrToStr(std::string &str, const std::wstring &wstr,
	const std::string local ="chs", bool bThdSafe= false, TyPtr pMtx= nullptr)
{
	std::mutex &s_mtx = _StrAndWstrTransStaticVariable();
	//宽字符开空间
	str.assign(wstr.size()*4, '\0');
	//加锁
	if(bThdSafe) {
		if(pMtx==nullptr)
			s_mtx.lock();
		else
			pMtx->lock();
	}
	//设置本地环境
	std::string strOri = std::setlocale(LC_CTYPE, nullptr);
	std::setlocale(LC_CTYPE, local.c_str());
	//进行转换
	size_t res = wcstombs(&str[0], &wstr[0], str.size());
	std::setlocale(LC_CTYPE, strOri.c_str());
	//解锁
	if(bThdSafe) {
		if(pMtx==nullptr)
			s_mtx.unlock();
		else
			pMtx->unlock();
	}
	//若成功
	if(res!=(size_t)(-1)) {
		str.resize(res);
		return true;
	}
	//若失败
	else {
		str.empty();
		return false;
	}
}



//二进制字符串文本显示
inline std::string StrBinToText(const std::string &str)
{
	std::string strRet;
	//对每个字符判断
	for(unsigned ch : str) {
		//ascii
		if(ch>=0x00 && ch<=0x7F) {
			//转义字符
			if(ch=='\r')
				strRet += "\\r";
			else if(ch=='\n')
				strRet += "\\n";
			else if(ch=='\t')
				strRet += "\\t";
			else if(ch=='\a')
				strRet += "\\a";
			else if(ch=='\b')
				strRet += "\\b";
			else if(ch=='\f')
				strRet += "\\f";
			else if(ch=='\v')
				strRet += "\\v";
			else if(ch=='\'')
				strRet += "\\\'";
			else if(ch=='\"')
				strRet += "\\\"";
			else if(ch=='\\')
				strRet += "\\\\";
			//可打印字符
			else if(ch>=0x20 && ch<=0x7E)
				strRet += ch;
			//不可打印字符
			else {
				strRet <<"\\x" <<HexToUppChar(ch/16) <<HexToUppChar(ch%16);
			}
		}
		//非ascii
		else {
			strRet += ch;
		}
	}
	return strRet;
}

//转义字符串二进制显示
inline std::string StrTextToBin(const std::string &str)
{
	std::string strRet;
	//对每个字符判断
	for(unsigned i=0; i<str.size(); ++i) {
		//ascii
		if(str[i]>=0x00 && str[i]<=0x7F) {
			//转义字符
			if(str[i]=='\\') {
				++ i;
				//单符号转义
				if(i<str.size()) {
					if(str[i]=='r')
						strRet += "\r";
					else if(str[i]=='n')
						strRet += "\n";
					else if(str[i]=='t')
						strRet += "\t";
					else if(str[i]=='a')
						strRet += "\a";
					else if(str[i]=='b')
						strRet += "\b";
					else if(str[i]=='f')
						strRet += "\f";
					else if(str[i]=='v')
						strRet += "\v";
					else if(str[i]=='\'')
						strRet += "\'";
					else if(str[i]=='\"')
						strRet += "\"";
					else if(str[i]=='\\')
						strRet += "\\";
					//16进制数字转义
					else if(str[i]=='x') {
						if(i+2<str.size() && CharIsHex(str[i+1]) && CharIsHex(str[i+2]))
							strRet += CharToHex(str[i+1])*16+CharToHex(str[i+2]);
						i+=2;
					}
					//无法识别转义
					else
						strRet += str[i];
				}
				else
					break;
			}
			//非转义
			else
				strRet += str[i];
		}
		//非ascii
		else {
			strRet += str[i];
		}
	}
	return strRet;
}
