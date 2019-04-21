#pragma once

//��׼��������չ����

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




//���ο��ټ����ݴ�
inline constexpr long long _TypeExtendIntPower(long long base, int power)
{
	return power<0 ? 0
		: power==0 ? 1
		: (power&1)==0 ? _TypeExtendIntPower(base*base, power>>1)
		: base*_TypeExtendIntPower(base*base, power>>1);
}

//���������ʾ���ȣ���������������ļ����ݿ��Դ��ڵ��ڻ�׼����
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



//���ͱ�����չ

struct timeval;


//duration��չ
namespace std
{
namespace chrono
{
typedef duration<long, ratio<3600*24>> days;
typedef duration<long, ratio<3600*24*7>> weeks;
}
}



//************
//����������չ
//************

//����ռλ����
template<int N>
class BlankType
{
};



//���Բ����Ƿ�Ϸ���
template<typename ...>
struct ParamValidTester
{
	typedef void type;
};


//��ȥ��������ȥ�������ױ�����
template<typename Ty>
struct RemoveCVRef
{
	using type = typename std::remove_cv<typename std::remove_reference<Ty>::type>::type;
};

//ǰ��ȥ�����ó����ױ���Ƿ�Ϊ����
template<typename TySrc, typename TyDst>
struct IsRemoveCVRefSame:
	std::is_same<typename RemoveCVRef<TySrc>::type, TyDst>
{
};


//�����Ƿ�Ϊ������
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



//��ȡ�����ĳ�����
template<typename Ty>
const Ty &ConstRef(const Ty &value)
{
	return value;
}



//�����ṩ��Դ���͵����á��������ױ����ԣ���Ŀ�����͸���ͬ��������
template<typename TySrc, typename TyDst>
struct TypeAttachAttribute
{
	using type = TyDst;
};
//�������ػ�
template<typename TySrc, typename TyDst>
struct TypeAttachAttribute<const TySrc, TyDst>
{
	using type = typename std::add_const<
		typename TypeAttachAttribute<TySrc, TyDst>>::type;
};
//�ױ������ػ�
template<typename TySrc, typename TyDst>
struct TypeAttachAttribute<volatile TySrc, TyDst>
{
	using type = typename std::add_volatile<
		typename TypeAttachAttribute<TySrc, TyDst>>::type;
};
//��ֵ���������ػ�
template<typename TySrc, typename TyDst>
struct TypeAttachAttribute<TySrc &, TyDst>
{
	using type = typename std::add_lvalue_reference<
		typename TypeAttachAttribute<TySrc, TyDst>>::type;
};
//��ֵ���������ػ�
template<typename TySrc, typename TyDst>
struct TypeAttachAttribute<TySrc &&, TyDst>
{
	using type = typename std::add_rvalue_reference<
		typename TypeAttachAttribute<TySrc, TyDst>>::type;
};



//��������ģ��
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

//����ģ�����Ԫ�غ���
template<size_t c_new, size_t ...c_idx_s>
inline IndexSequence<c_idx_s..., c_new> AddIndexSqeuence(IndexSequence<c_idx_s...>)
{
	return IndexSequence<c_rest_s..., c_idx>();
}

//����˳���������ģ�帨����
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
//����˳���������ģ����
template<size_t c_size>
using MakeIndexSequence = typename _MakeIndexSequenceAssist<c_size>::type;



//ģ�嶨������
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



//�洢ĳ���ͣ�ֵ����
template<typename Ty>
struct TypeHold
{
	Ty hold;
public:
	//����
	TypeHold(const Ty &o_hold):
		hold(o_hold)
	{
	}
	TypeHold(Ty &&o_hold):
		hold(std::move(o_hold))
	{
	}
};
//�洢ĳ���ͣ���ֵ�����ػ�
template<typename Ty>
struct TypeHold<Ty &>
{
	Ty &hold;
public:
	//����
	TypeHold(Ty &o_hold):
		hold(o_hold)
	{
	}
};
//�洢ĳ���ͣ���ֵ�����ػ�
template<typename Ty>
struct TypeHold<Ty &&>
{
	Ty &&hold;
public:
	//����
	TypeHold(Ty &&o_hold):
		hold(std::move(o_hold))
	{
	}
};



//�洢ĳֵ���ͣ�֧��void
template<typename Ty>
struct ValueHold
{
	Ty hold;
public:
	ValueHold()
	{
	}
	//ģ�幹��
	template<typename ...Ty_S>
	explicit ValueHold(Ty_S &&...args):
		hold(std::forward<Ty_S>(args)...)
	{
	}
};
//void�ػ�
template<>
struct ValueHold<void>
{
};



//**********************
//���ͳ�Ա����������չ
//**********************

//c����ַ���ת���ַ�����������������
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



//���αȽϸ�����
#define _IS_INT_TYPE(Ty1, Ty2) \
 (std::is_integral<Ty1>::value && std::is_integral<Ty2>::value)
#define _INT_SIGN_UNSIGN(Ty1, Ty2) \
	((std::is_integral<Ty1>::value && std::is_signed<Ty1>::value)\
	&& std::is_unsigned<Ty2>::value)

//�������Ͱ�ȫ�ж�С��
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

//�������Ͱ�ȫ�жϵ���
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

//�������Ͱ�ȫ�ж�С�ڵ���
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_IS_INT_TYPE(Ty1, Ty2), bool
>::type IntLTE(Ty1 num1, Ty2 num2)
{
	return IntLT(num1, num2) || IntEQ(num1, num2);
}

//�������Ͱ�ȫ�жϴ���
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_IS_INT_TYPE(Ty1, Ty2), bool
>::type IntGT(Ty1 num1, Ty2 num2)
{
	return !(IntLT(num1, num2) || IntEQ(num1, num2));
}

//�������Ͱ�ȫ�жϴ��ڵ���
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_IS_INT_TYPE(Ty1, Ty2), bool
>::type IntGTE(Ty1 num1, Ty2 num2)
{
	return !IntLT(num1, num2);
}

//�������Ͱ�ȫ�жϲ�����
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_IS_INT_TYPE(Ty1, Ty2), bool
>::type IntNEQ(Ty1 num1, Ty2 num2)
{
	return !IntEQ(num1, num2);
}
#undef _IS_INT_TYPE
#undef _INT_SIGN_UNSIGN



//�������Ƚϸ�����
#define _FLOAT_DEFAULT_ULP 10
#define _IS_FLOAT_TYPE(Ty1, Ty2) \
	(std::is_floating_point<Ty1>::value && std::is_floating_point<Ty2>::value)
#define _FLOAT_COMMON_EPSILON(Ty1, Ty2) \
	(std::max(std::numeric_limits<Ty1>::epsilon(), std::numeric_limits<Ty2>::epsilon()))
#define _FLOAT_COMMON_MIN(Ty1, Ty2) \
	(std::max(std::numeric_limits<Ty1>::min(), std::numeric_limits<Ty2>::min()))
#define _FLOAT_ABS(num) ((num)>=0? (num):-(num))

//�������Ƚ���ȣ���֤�߼���ϵ
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

//�������Ƚ�С�ڣ���֤�߼���ϵ
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_IS_FLOAT_TYPE(Ty1, Ty2), bool
>::type FloatLT(Ty1 num1, Ty2 num2, bool bAbs= false, int ulp= _FLOAT_DEFAULT_ULP)
{
	return num1<num2 && !FloatEQ(num1, num2, bAbs, ulp);
}

//�������Ƚ�С�ڵ��ڣ���֤�߼���ϵ
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_IS_FLOAT_TYPE(Ty1, Ty2), bool
>::type FloatLTE(Ty1 num1, Ty2 num2, bool bAbs= false, int ulp= _FLOAT_DEFAULT_ULP)
{
	return FloatLT(num1, num2, bAbs, ulp) || FloatEQ(num1, num2, bAbs, ulp);
}

//�������Ƚϴ��ڣ���֤�߼���ϵ
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_IS_FLOAT_TYPE(Ty1, Ty2), bool
>::type FloatGT(Ty1 num1, Ty2 num2, bool bAbs= false, int ulp= _FLOAT_DEFAULT_ULP)
{
	return !(FloatLT(num1, num2, bAbs, ulp) || FloatEQ(num1, num2, bAbs, ulp));
}

//�������Ƚϴ��ڵ��ڣ���֤�߼���ϵ
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_IS_FLOAT_TYPE(Ty1, Ty2), bool
>::type FloatGTE(Ty1 num1, Ty2 num2, bool bAbs= false, int ulp= _FLOAT_DEFAULT_ULP)
{
	return !FloatLT(num1, num2, bAbs, ulp);
}

//�������Ƚϲ����ڣ���֤�߼���ϵ
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_IS_FLOAT_TYPE(Ty1, Ty2), bool
>::type FloatNEQ(Ty1 num1, Ty2 num2, bool bAbs= false, int ulp= _FLOAT_DEFAULT_ULP)
{
	return !FloatEQ(num1, num2, bAbs, ulp);
}


//��������ת������
template<typename Ty>
inline typename std::enable_if<std::is_floating_point<Ty>::value, Ty
>::type FloatFloor(Ty num, bool bAbs= false, int ulp= _FLOAT_DEFAULT_ULP)
{
	Ty res = std::round(num);
	if(FloatEQ(num, res, bAbs, ulp))
		return res;
	return std::floor(num);
}

//��������ת������
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



//string���ۼӲ�����ʹ�����������
template<typename Ele, typename Ty>
inline std::string &operator <<(std::basic_string<Ele> &str, Ty &&arg)
{
	return str += std::forward<Ty>(arg);
}



//����Ƿ�Ϊtuple��
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


//tuple����ת����չ��������
template<typename TyDst, typename TySrc, size_t ...c_idx_s>
inline TyDst _TupleCastAssist(TySrc &&src, IndexSequence<c_idx_s...>)
{
	return TyDst(std::get<c_idx_s>(std::forward<TySrc>(src))...);
}
//tuple����ת����չ��ֱ������ת����֧�����е��������
//Դ������Ҫ�ܱ�get������⣬Ŀ��������Ҫ�ܽ���Դ���Ͳ���Ĺ��캯��
template<typename TyDst, typename TySrc>
inline TyDst TupleCast(TySrc &&src)
{
	return _TupleCastAssist<TyDst>(std::forward<TySrc>(src), MakeIndexSequence<
		std::tuple_size<typename std::remove_reference<TySrc>::type>::value>());
}



//hash��������
namespace std
{
//��pair��������û�н���������
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

//��tuple��������û�н���������
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


//���������hash�࣬β������������ò���ȫ
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



//����������ȽϺ�����β������������ò���ȫ
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


//����������Ƚ��࣬β������������ò���ȫ
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



//�ַ������ж�
inline constexpr bool CharIsNum(char ch)
{
	return ch>='0' && ch<='9';
}
inline constexpr bool CharIsNotNum(char ch)
{
	return !CharIsNum(ch);
}
//�ַ���ĸ�ж�
inline constexpr bool CharIsLet(char ch)
{
	return (ch>='a' && ch<='z') || (ch>='A' && ch<='Z');
}
inline constexpr bool CharIsNotLet(char ch)
{
	return !CharIsLet(ch);
}
//�ַ�������ĸ�ж�
inline constexpr bool CharIsNumLet(char ch)
{
	return (ch>='0' && ch<='9')
		|| (ch>='a' && ch<='z') || (ch>='A' && ch<='Z');
}
inline constexpr bool CharIsNotNumLet(char ch)
{
	return !CharIsNumLet(ch);
}
//�ַ���ʶ���ж�
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
//�ַ�ʮ�������ж�
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
//�ַ�ascii�ж�
inline constexpr bool CharIsAscii(char ch)
{
	return ch>=0 && ch<=127;
}
inline constexpr bool CharIsNotAscii(char ch)
{
	return !CharIsAscii(ch);
}
//�ַ������ʶ���ж�
inline constexpr bool CharIsBroadId(char ch)
{
	return CharIsId(ch) || CharIsNotAscii(ch);
}
inline constexpr bool CharIsNotBroadId(char ch)
{
	return !CharIsBroadId(ch);
}
//�ַ��հ׷��ж�
inline constexpr bool CharIsBlank(char ch)
{
	return ch==' ' || ch=='\t' || ch=='\r' || ch=='\n'
		|| ch=='\v' || ch=='\f';
}
inline constexpr bool CharIsNotBlank(char ch)
{
	return !CharIsBlank(ch);
}


//�����ַ�ת������
inline int CharToNum(char ch)
{
	if(ch>='0' && ch<='9')
		return ch-'0';
	else
		return -1;
}
//��ĸ�ַ�ת������
inline int CharToLet(char ch)
{
	if(ch>='a' && ch<='z')
		return ch-'a';
	else if(ch>='A' && ch<='Z')
		return ch-'A';
	else
		return -1;
}
//ʮ�������ַ�ת������
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


//����ת�������ַ�
inline char NumToChar(int i)
{
	if(i>=0 && i<=9)
		return i+'0';
	else
		return 0;
}
//����ת����д��ĸ�ַ�
inline char LetToUppChar(int i)
{
	if(i>=0 && i<=25)
		return i+'A';
	else
		return 0;
}
//����ת��Сд��ĸ�ַ�
inline char LetToLowChar(int i)
{
	if(i>=0 && i<=25)
		return i+'a';
	else
		return 0;
}
//����ת����дʮ�������ַ�
inline char HexToUppChar(int i)
{
	if(i>=0 && i<=9)
		return i+'0';
	else if(i>=10 && i<=15)
		return i+'A';
	else
		return 0;
}
//����ת��Сдʮ�������ַ�
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
//ʵ��������չ
//************

//�����쳣��
class CatchError:
	public std::runtime_error
{
	//�̳л��๹�캯������Ĭ�Ͽ������죩
	using std::runtime_error::runtime_error;
public:
	//Ĭ�Ϲ���
	CatchError():
		std::runtime_error("")
	{
	}
	//�̳л��ำֵ
	using std::runtime_error::operator =;
	//ʡ�Ժϳ�5����
	//ʡ�Լ̳�what
};

//�����쳣����չ
template<int c_n>
class CatchErrorEx:
	virtual public CatchError
{
	//�̳л��๹�캯������Ĭ�Ͽ������죩
	using CatchError::CatchError;
public:
	//Ĭ�Ϲ���
	CatchErrorEx()
		= default;
	//����
	CatchErrorEx(const CatchErrorEx &other)
		= default;
	CatchErrorEx &operator =(const CatchErrorEx &other)
		= default;
	//�ƶ�
	CatchErrorEx(CatchErrorEx &&other) noexcept
		= default;
	CatchErrorEx &operator =(CatchErrorEx &&other) noexcept
		= default;
	//�̳л��ำֵ
	using CatchError::operator =;
	//ʡ�Ժϳ�����
	//ʡ�Լ̳�what
};

//���������쳣��
template<typename Ty>
class TypeError:
	virtual public CatchError
{
public:
	Ty value;//�洢ֵ
public:
	//���������죬�����������
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
	//����
	TypeError(const TypeError &other)
		= default;
	TypeError &operator =(const TypeError &other)
		= default;
	//�ƶ�
	TypeError(TypeError &&other) noexcept
		= default;
	TypeError &operator =(TypeError &&other) noexcept
		= default;
	//ֵ���͸�ֵ���������
	TypeError &operator =(const Ty &o_value)
	{
		value = o_value;
		return *this;
	}
	//ֵ����ת��
	operator Ty &()
	{
		return value;
	}
	operator const Ty &() const
	{
		return value;
	}
	//�̳л��ำֵ
	using CatchError::operator =;
	//ʡ�Ժϳ�����
	//ʡ�Լ̳�what
};

//���������쳣����չ
template<typename Ty, int c_n>
class TypeErrorEx:
	public TypeError<Ty>, public CatchErrorEx<c_n>
{
	//�̳�TypeError���๹�캯������Ĭ�Ͽ������죩
	using TypeError::TypeError;
public:
	//����
	TypeErrorEx(const TypeErrorEx &other)
		= default;
	TypeErrorEx &operator =(const TypeErrorEx &other)
		= default;
	//�ƶ�
	TypeErrorEx(TypeErrorEx &&other) noexcept
		= default;
	TypeErrorEx &operator =(TypeErrorEx &&other) noexcept
		= default;
	//�̳����л��ำֵ
	using TypeError<Ty>::operator =;
	using CatchErrorEx<c_n>::operator =;
	//ʡ�Ժϳ�����
	//ʡ�Լ̳�what��value������ת��
};



//��ʱ���࣬Rep��ʱ�������ͣ�Period��ʱ��λ
template<typename Rep= double, typename Period= std::ratio<1>>
class Timer
{
private:
	std::chrono::steady_clock::time_point tmptStart;//��¼��ʼʱ���
	Rep recordCount;//�洢��¼ʱ���

public:
	//���캯����bStart�Ƿ�ʼ��ʱ
	explicit Timer(bool bStart= false)
	{
		if(bStart)
			Start();
	}
	//��ʼ��ʱ
	void Start()
	{
		tmptStart = std::chrono::steady_clock::now();
	}
	//��¼ʱ�䲢����
	Rep Record()
	{
		recordCount = std::chrono::duration_cast<typename std::chrono::duration<Rep, Period>>
			(std::chrono::steady_clock::now()-tmptStart).count();
		return recordCount;
	}
	//��ȡ�ϴ�ʱ��
	Rep Get()
	{
		return tmduRecord;
	}
};



//�����������
template<typename ...Ty_S>
class ArgsTuple:
	public std::tuple<Ty_S...>
{
	//�̳л��๹�캯������Ĭ�Ͽ������죩
	using std::tuple<Ty_S...>::tuple;
public:
	//Ĭ�Ϲ���
	ArgsTuple()
		= default;
	//�̳л��ำֵ
	using std::tuple<Ty_S...>::operator =;
	//ʡ�Ժϳ�5����
};
//�̳зǳ�Ա����
//tuple_cat, std::get(std::tuple), std::swap(std::tuple)
//operator==, operator!=, operator<, operator<=, operator>, operator>=


//tupleת����Ӧ��ArgsTuple�ศ������
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

//ArgsTupleת����Ӧ��tuple�ศ������
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


//����Ƿ�ΪArgsTuple��
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


//��ArgsTuple��С����
template<typename Ty>
struct ArgsTupleSize:
	std::integral_constant<size_t, std::tuple_size<
	typename ArgsTupleToTuple<Ty>::type>::value>
{
};

//��ArgsTupleԪ��������
template<size_t c_idx, typename Ty>
struct ArgsTupleElement
{
	using type = typename std::tuple_element<c_idx,
		typename ArgsTupleToTuple<Ty>::type>::type;
};


//ArgsTuple��ֵ���͸���������
template<typename ...Ty_S>
inline auto MakeArgsTuple(Ty_S &&...arg_s)
{
	auto ret = std::make_tuple(std::forward<Ty_S>(arg_s)...);
	return TupleCast<typename TupleToArgsTuple<decltype(ret)>::type>(std::move(ret));
}

//ArgsTuple���������͸���������
template<typename ...Ty_S>
inline auto TieArgsTuple(Ty_S &&...arg_s)
{
	auto ret = std::tie(std::forward<Ty_S>(arg_s)...);
	return TupleCast<typename TupleToArgsTuple<decltype(ret)>::type>(std::move(ret));
}

//ArgsTuple��ת�����͸���������
template<typename ...Ty_S>
inline auto ForwardAsArgsTuple(Ty_S &&...arg_s)
{
	auto ret = std::forward_as_tuple(std::forward<Ty_S>(arg_s)...);
	return TupleCast<typename TupleToArgsTuple<decltype(ret)>::type>(std::move(ret));
}

//����ArgsTuple��
template<typename ...Ty_S>
inline auto ArgsTupleCat(Ty_S &&...arg_s)
{
	auto ret = std::tuple_cat(std::forward<Ty_S>(arg_s)...);
	return TupleCast<typename TupleToArgsTuple<decltype(ret)>::type>(std::move(ret));
}


//ArgsTuple����ת��������չ������������
template<typename TyDst, typename TySrc, size_t ...c_idx_s>
inline TyDst _ArgsTupleOverloadCastAssist(TySrc &&src, IndexSequence<c_idx_s...>)
{
	return TyDst(std::get<c_idx_s>(std::forward<TySrc>(src))...);
}
//ArgsTuple����ת��������չ����������ת��Դ����
//��ArgsTuple�����ת��Ϊָ�����ͣ���������ֱ��ת��Ϊָ������
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

//��ArgsTupleת��Ϊ�����Ӧ�����������͵�tuple������ת��Ϊ�������������͵�tuple
//�ǲ����������ֱ����Ϊtuple�Ĳ���
template<typename Ty>
struct _ArgsTupleOverloadToRefTuple
{
	using type = std::tuple<Ty>;
};
//��������8������ػ�������������������͸��Ӷ�Ӧ���Ժ���Ϊtuple�Ĳ���
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


//ArgsTuple�������Ӳ�����������Ӻ��tuple���ͣ����㷵�����͸�����
template<typename ...Ty_S>
struct _ArgsTupleRefCatToTupleReturnType
{
	using type = decltype(std::tuple_cat(std::declval<
		typename _ArgsTupleOverloadToRefTuple<Ty_S>::type>()...));
};

//ArgsTuple�������Ӳ��������㷵�����͸�����
template<typename ...Ty_S>
struct ArgsTupleRefCatReturnType
{
	using type =  typename TupleToArgsTuple<
		typename _ArgsTupleRefCatToTupleReturnType<Ty_S...>::type>::type;
};


//ArgsTuple�������ӵ�tuple���������Ӻ��tuple����
template<typename ...Ty_S>
inline typename _ArgsTupleRefCatToTupleReturnType<Ty_S &&...
>::type _ArgsTupleRefCatToTuple(Ty_S &&...arg_s)
{
	return std::tuple_cat(_ArgsTupleOverloadCast<
		typename _ArgsTupleOverloadToRefTuple<Ty_S &&>::type>(std::forward<Ty_S>(arg_s))...);
}

//ArgsTuple�������Ӳ�����֧������ͨ���ͻ�����ӣ����Ӻ��Ϊ��������
template<typename ...Ty_S>
inline typename ArgsTupleRefCatReturnType<Ty_S &&...
>::type ArgsTupleRefCat(Ty_S &&...arg_s)
{
	return TupleCast<typename ArgsTupleRefCatReturnType<Ty_S &&...>::type>(
		_ArgsTupleRefCatToTuple(std::forward<Ty_S>(arg_s)...));
}


//ArgsTuple�ݹ����úϲ���tuple�෵��ֵ���ͼ��㸨���ศ���࣬Ҫ����ǰ����һ��ֵ����tuple��
template<typename ...Ty_S>
struct _ArgsTupleRecurRefCatToTupleReturnTypeAssist
{
};
//�������ػ�
//ע��ready��tuple��ֵ����
template<typename ...TyReady_S>
struct _ArgsTupleRecurRefCatToTupleReturnTypeAssist<std::tuple<TyReady_S...>>
{
	using type = std::tuple<TyReady_S...>;
};
//��ǰֱ�Ӵ����ػ�
template<typename Ty, typename ...TyReady_S, typename ...TyRest_S>
struct _ArgsTupleRecurRefCatToTupleReturnTypeAssist<std::tuple<TyReady_S...>,
	Ty, TyRest_S...>
{
	using type = typename _ArgsTupleRecurRefCatToTupleReturnTypeAssist<
		std::tuple<TyReady_S..., Ty>, TyRest_S...>::type;
};
//��ǰ���8������ػ�
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

//ArgsTuple�ݹ����úϲ�����ֵ���ͼ��㸨����
template<typename ...TyRest_S>
struct ArgsTupleRecurRefCatReturnType
{
	using type = typename TupleToArgsTuple<
		typename _ArgsTupleRecurRefCatToTupleReturnTypeAssist<
		std::tuple<>, TyRest_S...>::type>::type;
};


//ʹ��ArgsTuple��ݹ����úϲ�Ϊtuple��ǰ������
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
//ʹ��ArgsTuple��ݹ����úϲ�Ϊtuple�ĸ������������������𿪺���ӵ�ʣ������
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

//ʹ��ArgsTuple��ݹ����úϲ�Ϊtuple���ݹ齫��ǰ����׼���õ�tuple
//�ݹ��������
template<typename ...TyReady_S>
inline typename _ArgsTupleRecurRefCatToTupleReturnTypeAssist<std::tuple<TyReady_S...>
>::type _ArgsTupleRecurRefCatToTuple(std::tuple<TyReady_S...> &&ready)
{
	return std::move(ready);
}
//�ݹ鵽����������
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
//�ݹ鵽�ǲ���������
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

//ʹ��ArgsTuple��ݹ����úϲ���֧�ֶ��ArgsTuple����ͨ�����ݹ���ʹ��
template<typename ...Ty_S>
inline typename ArgsTupleRecurRefCatReturnType<Ty_S &&...
>::type ArgsTupleRecurRefCat(Ty_S &&...arg_s)
{
	return TupleCast<typename ArgsTupleRecurRefCatReturnType<Ty_S &&...>::type>(
		_ArgsTupleRecurRefCatToTuple(std::tuple<>(), std::forward<Ty_S>(arg_s)...));
}


//����ʹ��ֵ����tuple���ú����ķ������͸�����
template<typename ...Tys>
struct _ValueTupleSimpleInvokeReturnType
{
};
template<typename TyFunc, typename ...Ty_S>
struct _ValueTupleSimpleInvokeReturnType<TyFunc, std::tuple<Ty_S...>>
{
	using type = decltype(std::declval<TyFunc>()(std::declval<Ty_S>()...));
};

//ArgsTuple���ã����㷵�����͸�����
template<typename TyFunc, typename ...Ty_S>
struct ArgsTupleInvokeReturnType
{
	using type = typename _ValueTupleSimpleInvokeReturnType<TyFunc,
		typename _ArgsTupleRefCatToTupleReturnType<Ty_S...>::type>::type;
};

//ʹ�õ���ֵ����tuple����ú�������������ʹ����������ģ��չ������
template<typename TyFunc, typename TyTup, size_t ...c_idx_s>
inline typename _ValueTupleSimpleInvokeReturnType<TyFunc &&, TyTup
>::type _ValueTupleSimpleInvoke(TyFunc &&func, TyTup &&argsTup,
	IndexSequence<c_idx_s...>)
{
	return std::forward<TyFunc>(func)(std::get<c_idx_s>(std::forward<TyTup>(argsTup))...);
}
//ʹ�õ���ArgsTuple����ú�����ֻ��ʹ��һ������������
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


//ʹ��ArgsTuple����ú�����֧�ֶ������������ͨ�������ʹ��
template<typename TyFunc, typename ...Ty_S>
inline typename ArgsTupleInvokeReturnType<TyFunc &&, Ty_S &&...
>::type ArgsTupleInvoke(TyFunc &&func, Ty_S &&...arg_s)
{
	return _ValueTupleSimpleInvoke(std::forward<TyFunc>(func),
		_ArgsTupleRefCatToTuple(std::forward<Ty_S>(arg_s)...),
		MakeIndexSequence<std::tuple_size<
		typename _ArgsTupleRefCatToTupleReturnType<Ty_S &&...>::type>::value>());
}


//ArgsTuple�ݹ���ã����㷵�����͸�����
template<typename TyFunc, typename ...Ty_S>
struct ArgsTupleRecurInvokeReturnType
{
	using type = typename _ValueTupleSimpleInvokeReturnType<TyFunc,
		typename _ArgsTupleRecurRefCatToTupleReturnTypeAssist<
		std::tuple<>, Ty_S...>::type>::type;
};

//ʹ��ArgsTuple��ݹ���ú�����֧�ֶ������������ͨ�����ݹ�Ļ��ʹ��
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
//���߹���ת������
//****************

//��timevalת��Ϊduration
template<typename Ty= BlankType<0>>
inline std::chrono::microseconds TimevalToDuration(const timeval &tmval)
{
	return std::chrono::microseconds((long long)tmval.tv_sec*1000000+tmval.tv_usec);
}


//��tmת��Ϊ��ʽ���ַ���
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

//��time_tת��Ϊ��ʽ���ַ�����bThdSafe���л��⣬��pMtxΪnull��ʹ�þֲ���̬��
template<typename TyPtr= std::mutex *>
inline std::string TimeToStr(std::time_t tmNum, bool bLocal= true,
	bool bThdSafe= false, const std::string &fmt="%F %T", TyPtr pMtx= nullptr)
{
	static std::mutex s_mtx;
	if(tmNum>=(1LL<<31))
		tmNum = (1LL<<31)-1;
	else if(tmNum<0)
		tmNum = 0;
	//����
	if(bThdSafe) {
		if(pMtx==nullptr)
			s_mtx.lock();
		else
			pMtx->lock();
	}
	//ת��
	std::tm *pTimeStu;
	if(bLocal)
		pTimeStu = std::localtime(&tmNum);
	else
		pTimeStu = std::gmtime(&tmNum);
	string str = TimeToStr(*pTimeStu, fmt);
	//����
	if(bThdSafe) {
		if(pMtx==nullptr)
			s_mtx.unlock();
		else
			pMtx->unlock();
	}
	return str;
}

//��durationת��Ϊ��ʽ���ַ���
//DecimalBitΪ�����Զ��ƶ�С��λ�����Ǹ����趨С��λ������ʽ�����%@��ʾС�����֣���С���㣩
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
	//�Զ��ƶ�λ��
	constexpr int c_realDecimalBit = c_decimalBit<0 ?
		_TypeExtendRatioPrecision<Period, std::ratio<10>>() : c_decimalBit;
	constexpr long long c_decimalNum = _TypeExtendIntPower(10, c_realDecimalBit);
	//����С������
	auto tmduDecimal = std::chrono::duration_cast<
		std::chrono::duration<long long, std::ratio<1, c_decimalNum>>>(tmdu);
	tmduDecimal -= tmduInt;
	//�����ʽ�ַ���
	std::ostringstream oss;
	oss <<std::setfill('0') <<std::right;
	for(int i=0; i!=fmt.size(); ++i) {
		//�ٷֺ�����˫��ĸ
		if(fmt[i]=='%' && i+1!=fmt.size()) {
			if(fmt[i+1]!='@')
				oss <<fmt[i] <<fmt[i+1];
			else if(c_realDecimalBit>0)
				oss <<'.' <<std::setw(c_realDecimalBit) <<tmduDecimal.count();
			++ i;
		}
		//����ֱ�Ӵ���
		else
			oss <<fmt[i];
	}
	string newfmt = oss.str();
	//ʹ���¸�ʽ����
	return TimeToStr(tmduInt.count(), bLocal, bThdSafe, newfmt, pMtx);
}

//��system_clock::time_pointת��Ϊ��ʽ���ַ���
template<typename TyPtr= std::mutex *>
inline std::string TimeToStr(
	const std::chrono::system_clock::time_point &tmpt,
	bool bLocal= true, bool bThdSafe= false,
	const std::string &fmt ="%F %T", TyPtr pMtx= nullptr)
{
	return TimeToStr(std::chrono::system_clock::to_time_t(tmpt),
		bLocal, bThdSafe, fmt, pMtx);
}

//�߾�����system_clock::time_pointת��Ϊ��ʽ���ַ�������ʵ�ֱ�֤��ȷ��
//DecimalBitΪ�����Զ��ƶ�С��λ�����Ǹ����趨С��λ������ʽ�����%@��ʾС�����֣���С���㣩
template<int c_decimalBit, typename TyPtr= std::mutex *>
inline std::string TimeToStr(
	const std::chrono::time_point<std::chrono::system_clock> &tmpt,
	bool bLocal= true, bool bThdSafe= false,
	const std::string &fmt ="%F %T%@", TyPtr pMtx= nullptr)
{
	return TimeToStr<c_decimalBit>(tmpt.time_since_epoch(),
		bLocal, bThdSafe, fmt, pMtx);
}

//��timevalת��Ϊ��ʽ���ַ�������ʽ�����%@��ʾС�����֣���С���㣩
template<int c_decimalBit, typename TyPtr= std::mutex *>
inline std::string TimeToStr(const timeval &tmval, bool bLocal= true,
	bool bThdSafe= false, const std::string &fmt ="%F %T%@", TyPtr pMtx= nullptr)
{
	return TimeToStr<c_decimalBit>(TimevalToDuration(tmval), bLocal, bThdSafe, fmt, pMtx);
}


//����ǰʱ��ת��Ϊ�ַ���
template<typename TyPtr= std::mutex *>
inline std::string NowTimeToStr(bool bLocal= true, bool bThdSafe= false,
	const std::string &fmt ="%F %T", TyPtr pMtx= nullptr)
{
	return TimeToStr(std::chrono::system_clock::now(),
		bLocal, bThdSafe, fmt, pMtx);
}

//�߾��Ƚ���ǰʱ��ת��Ϊ�ַ�������ʵ�ֱ�֤��ȷ��
//DecimalBitΪ�����Զ��ƶ�С��λ�����Ǹ����趨С��λ������ʽ�����%@��ʾС�����֣���С���㣩
template<int c_decimalBit, typename TyPtr= std::mutex *>
inline std::string NowTimeToStr(bool bLocal= true, bool bThdSafe= false,
	const std::string &fmt ="%F %T%@", TyPtr pMtx= nullptr)
{
	return TimeToStr<c_decimalBit>(std::chrono::system_clock::now(),
		bLocal, bThdSafe, fmt, pMtx);
}



//�ַ���ת�����֣�����������
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

//�ַ���ת�����֣����쳣�汾�������Ƿ�ɹ�
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



//��������ת���ַ���������ǧλ�ָ���
template<typename Ty>
inline typename std::enable_if<std::is_integral<Ty>::value, std::string
>::type GapToString(Ty num, int gap= 3, char ch= '\'')
{
	std::string str;
	//ȡ��
	if(IntLT(num, 0))
		num = -num;
	//�����ַ���
	for(int i=0; ; ++i) {
		if(i!=0 && i%gap==0)
			str += ch;
		str += num%10+'0';
		num /= 10;
		if(num==0)
			break;
	}
	//���븺��
	if(IntLT(num, 0))
		str += '-';
	//��ת
	std::reverse(str.begin(), str.end());
	return str;
}



//�ַ���ת�����ַ���
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
	//���ַ����ռ�
	wstr.assign(str.size(), '\0');
	//����
	if(bThdSafe) {
		if(pMtx==nullptr)
			s_mtx.lock();
		else
			pMtx->lock();
	}
	//���ñ��ػ���
	std::string strOri = std::setlocale(LC_CTYPE, nullptr);
	std::setlocale(LC_CTYPE, local.c_str());
	//����ת��
	size_t res = mbstowcs(&wstr[0], &str[0], wstr.size());
	std::setlocale(LC_CTYPE, strOri.c_str());
	//����
	if(bThdSafe) {
		if(pMtx==nullptr)
			s_mtx.unlock();
		else
			pMtx->unlock();
	}
	//���ɹ�
	if(res!=(size_t)(-1)) {
		wstr.resize(res);
		return true;
	}
	//��ʧ��
	else {
		wstr.empty();
		return false;
	}
}

//���ַ���ת���ַ���
template<typename TyPtr= std::mutex *>
inline bool WstrToStr(std::string &str, const std::wstring &wstr,
	const std::string local ="chs", bool bThdSafe= false, TyPtr pMtx= nullptr)
{
	std::mutex &s_mtx = _StrAndWstrTransStaticVariable();
	//���ַ����ռ�
	str.assign(wstr.size()*4, '\0');
	//����
	if(bThdSafe) {
		if(pMtx==nullptr)
			s_mtx.lock();
		else
			pMtx->lock();
	}
	//���ñ��ػ���
	std::string strOri = std::setlocale(LC_CTYPE, nullptr);
	std::setlocale(LC_CTYPE, local.c_str());
	//����ת��
	size_t res = wcstombs(&str[0], &wstr[0], str.size());
	std::setlocale(LC_CTYPE, strOri.c_str());
	//����
	if(bThdSafe) {
		if(pMtx==nullptr)
			s_mtx.unlock();
		else
			pMtx->unlock();
	}
	//���ɹ�
	if(res!=(size_t)(-1)) {
		str.resize(res);
		return true;
	}
	//��ʧ��
	else {
		str.empty();
		return false;
	}
}



//�������ַ����ı���ʾ
inline std::string StrBinToText(const std::string &str)
{
	std::string strRet;
	//��ÿ���ַ��ж�
	for(unsigned ch : str) {
		//ascii
		if(ch>=0x00 && ch<=0x7F) {
			//ת���ַ�
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
			//�ɴ�ӡ�ַ�
			else if(ch>=0x20 && ch<=0x7E)
				strRet += ch;
			//���ɴ�ӡ�ַ�
			else {
				strRet <<"\\x" <<HexToUppChar(ch/16) <<HexToUppChar(ch%16);
			}
		}
		//��ascii
		else {
			strRet += ch;
		}
	}
	return strRet;
}

//ת���ַ�����������ʾ
inline std::string StrTextToBin(const std::string &str)
{
	std::string strRet;
	//��ÿ���ַ��ж�
	for(unsigned i=0; i<str.size(); ++i) {
		//ascii
		if(str[i]>=0x00 && str[i]<=0x7F) {
			//ת���ַ�
			if(str[i]=='\\') {
				++ i;
				//������ת��
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
					//16��������ת��
					else if(str[i]=='x') {
						if(i+2<str.size() && CharIsHex(str[i+1]) && CharIsHex(str[i+2]))
							strRet += CharToHex(str[i+1])*16+CharToHex(str[i+2]);
						i+=2;
					}
					//�޷�ʶ��ת��
					else
						strRet += str[i];
				}
				else
					break;
			}
			//��ת��
			else
				strRet += str[i];
		}
		//��ascii
		else {
			strRet += str[i];
		}
	}
	return strRet;
}
