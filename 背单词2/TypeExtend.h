#pragma once

//��׼��������չ����
//д�������쳣

#include <cassert>
#include <ctime>
#include <clocale>

#include <sstream>

#include <type_traits>
#include <algorithm>
#include <utility>
#include <tuple>
#include <functional>

#include <string>

#include <exception>
#include <stdexcept>
#include <ratio>

#include <chrono>
#include <mutex>



//�����
#define _TYPE_COPY_ASSIGN(Type, other) {\
	if(this!=&(other)) {\
		this->~Type();\
		new(this) Type(other);\
	}\
	return *this;}//������ֵ

#define _TYPE_MOVE_ASSIGN(Type, other) {\
	if(this!=&(other)) {\
		this->~Type();\
		new(this) Type(std::move(other));\
	}\
	return *this;}//�ƶ���ֵ



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



//����������չ

//����ռλ����
template<int N>
class BlankType
{
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
	{
		_TYPE_COPY_ASSIGN(CatchErrorEx, other);
	}
	//�ƶ�
	CatchErrorEx(CatchErrorEx &&other)
		= default;
	CatchErrorEx &operator =(CatchErrorEx &&other)
	{
		_TYPE_MOVE_ASSIGN(CatchErrorEx, other);
	}
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
	TypeError(const Ty &o_value, Tys &&...args):
		CatchError(std::forward<Tys>(args)...), value(o_value)
	{
	}
	//����
	TypeError(const TypeError &other)
		= default;
	TypeError &operator =(const TypeError &other)
	{
		_TYPE_COPY_ASSIGN(TypeError, other);
	}
	//�ƶ�
	TypeError(TypeError &&other)
		= default;
	TypeError &operator =(TypeError &&other)
	{
		_TYPE_MOVE_ASSIGN(TypeError, other);
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
template<typename Ty, int c_n>
class TypeErrorEx:
	public TypeError<Ty>, public CatchErrorEx<c_n>
{
public:
	//���������죬�����������
	template<typename ...Tys>
	TypeErrorEx(const Ty &o_value, Tys &&...args):
		CatchError(std::forward<Tys>(args)...), TypeError<Ty>(o_value)
	{
	}
	//����
	TypeErrorEx(const TypeErrorEx &other)
		= default;
	TypeErrorEx &operator =(const TypeErrorEx &other)
	{
		_TYPE_COPY_ASSIGN(TypeErrorEx, other);
	}
	//�ƶ�
	TypeErrorEx(TypeErrorEx &&other)
		= default;
	TypeErrorEx &operator =(TypeErrorEx &&other)
	{
		_TYPE_MOVE_ASSIGN(TypeErrorEx, other);
	}
	//�̳����л��ำֵ
	using TypeError<Ty>::operator =;
	using CatchErrorEx<c_n>::operator =;
	//ʡ�Ժϳ�����
	//ʡ�Լ̳�what��value������ת��
};



//��ֵ���㺯��

//����10���ݴ�
inline constexpr long long PowerOperator(long long base, int power)
{
	return power<0 ? 0 :
		power==0 ? 1 : PowerOperator(base, power-1)*base;
}

//���������ʾ���ȣ���������������ļ����ݿ��Դ��ڵ��ڻ�׼����
template<typename Ratio, typename BaseRatio,
	typename CompareRatio =std::ratio<1>
> inline constexpr typename std::enable_if<
	std::ratio_greater_equal<Ratio, CompareRatio>::value, int
>::type RatioPrecision()
{
	return 0;
}
template<typename Ratio, typename BaseRatio,
	typename CompareRatio =std::ratio<1>
> inline constexpr typename std::enable_if<
	!std::ratio_greater_equal<Ratio, CompareRatio>::value, int
>::type RatioPrecision()
{
	return RatioPrecision<std::ratio_multiply<Ratio, BaseRatio>,
		BaseRatio, CompareRatio>()+1;
}



//����ת����������

//c����ַ���ת���ַ���
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

//�޷�������ת���з��ţ������ܱ���ض�
inline constexpr int ToSigned(unsigned char num)
{
	return static_cast<int>(num);
}
inline constexpr int ToSigned(char num)
{
	return static_cast<int>(num);
}
inline constexpr long ToSigned(unsigned short num)
{
	return static_cast<long>(num);
}
inline constexpr long long ToSigned(unsigned num)
{
	return static_cast<long long>(num);
}
inline constexpr long long ToSigned(unsigned long num)
{
	return static_cast<long long>(num);
}
inline constexpr long long ToSigned(unsigned long long num)
{
	return static_cast<long long>(num);
}



//���ͳ�Ա��չ

//�����з����޷����ж�
#define _InterSignUnsign(Ty1, Ty2) \
	((std::is_integral<Ty1>::value && std::is_signed<Ty1>::value)\
	&& std::is_unsigned<Ty2>::value)
//�������Ͱ�ȫ�ж�С��
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_InterSignUnsign(Ty1, Ty2), bool
>::type SafeLT(Ty1 num1, Ty2 num2)
{
	return (num1<0) ? (true)
		: (static_cast<typename std::make_unsigned<Ty1>::type>(num1)<num2);
}
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_InterSignUnsign(Ty2, Ty1), bool
>::type SafeLT(Ty1 num1, Ty2 num2)
{
	return (num2<0) ? (false)
		: (num1<static_cast<typename std::make_unsigned<Ty2>::type>(num2));
}
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<
	std::is_fundamental<Ty1>::value && std::is_fundamental<Ty2>::value
	&& !_InterSignUnsign(Ty1, Ty2) && !_InterSignUnsign(Ty2, Ty1), bool
>::type SafeLT(Ty1 num1, Ty2 num2)
{
	return num1<num2;
}
//�������Ͱ�ȫ�жϵ���
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_InterSignUnsign(Ty1, Ty2), bool
>::type SafeEQ(Ty1 num1, Ty2 num2)
{
	return (num1<0) ? (false)
		: (static_cast<typename std::make_unsigned<Ty1>::type>(num1)==num2);
}
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_InterSignUnsign(Ty2, Ty1), bool
>::type SafeEQ(Ty1 num1, Ty2 num2)
{
	return (num2<0) ? (false)
		: (num1==static_cast<typename std::make_unsigned<Ty2>::type>(num2));
}
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<
	std::is_fundamental<Ty1>::value && std::is_fundamental<Ty2>::value
	&& !_InterSignUnsign(Ty1, Ty2) && !_InterSignUnsign(Ty2, Ty1), bool
>::type SafeEQ(Ty1 num1, Ty2 num2)
{
	return num1==num2;
}//�������Ͱ�ȫ�ж�С�ڵ���
template<typename Ty1, typename Ty2>
inline constexpr bool SafeLTE(Ty1 &&num1, Ty2 &&num2)
{
	return SafeLT(std::forward<Ty1>(num1), std::forward<Ty2>(num2))
		|| SafeEQ(std::forward<Ty1>(num1), std::forward<Ty2>(num2));
}
//�������Ͱ�ȫ�жϴ���
template<typename Ty1, typename Ty2>
inline constexpr bool SafeGT(Ty1 &&num1, Ty2 &&num2)
{
	return !(SafeLT(std::forward<Ty1>(num1), std::forward<Ty2>(num2))
		|| SafeEQ(std::forward<Ty1>(num1), std::forward<Ty2>(num2)));
}
//�������Ͱ�ȫ�жϴ��ڵ���
template<typename Ty1, typename Ty2>
inline constexpr bool SafeGTE(Ty1 &&num1, Ty2 &&num2)
{
	return !SafeLT(std::forward<Ty1>(num1), std::forward<Ty2>(num2));
}
//�������Ͱ�ȫ�жϲ�����
template<typename Ty1, typename Ty2>
inline constexpr bool SafeNEQ(Ty1 &&num1, Ty2 &&num2)
{
	return !SafeEQ(std::forward<Ty1>(num1), std::forward<Ty2>(num2));
}
#undef _InterSignUnsign

//string���ۼӲ�����ʹ�����������
template<typename Ty>
inline std::string &operator <<(std::string &str, Ty &&arg)
{
	return str += std::forward<Ty>(arg);
}
template<typename Ty>
inline std::string &operator <<(std::wstring &str, Ty &&arg)
{
	return str += std::forward<Ty>(arg);
}

//hash��������
namespace std
{
//��pair������
template<typename Ty1, typename Ty2>
struct hash<std::pair<Ty1, Ty2>>
{
	typedef size_t result_type;
	typedef std::pair<Ty1, Ty2> argument_type;
	size_t operator()(const std::pair<Ty1, Ty2> &pr) const
	{
		return std::hash<Ty1>()(pr.first)
			^ std::hash<Ty2>()(pr.second);
	}
};
//��tuple������
template<typename ...Tys>
struct hash<std::tuple<Tys...>>
{
	typedef size_t result_type;
	typedef std::tuple<Tys...> argument_type;
	size_t operator()(const std::tuple<Tys...> &tup) const
	{
		return Hash<sizeof...(Tys)-1>(tup);
	}
	template<int index>
	static typename std::enable_if<(index>=0), size_t
	>::type Hash(const std::tuple<Tys...> &tup)
	{
		return std::hash<typename std::tuple_element<index, std::tuple<Tys...>>::type>
			()(std::get<index>(tup))
			^ Hash<index-1>(tup);
	}
	template<int index, typename Ty>
	static size_t Hash(const Ty &tup)
	{
		return 0;
	}
};
}

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



//���߹���ת������

//��tmת��Ϊ��ʽ���ַ���
template<typename Ty= BlankType<0>>
inline std::string TimeToStr(const std::tm &tmStu,
	const std::string &fmt="%F %T")
{
	char buf[256];
	std::string str;
	if(strftime(buf, sizeof(buf), fmt.c_str(), &tmStu)) {
		str = buf;
	}
	return str;
}
//��time_tת��Ϊ��ʽ���ַ�����bThdSafe���л��⣬��pMtxΪnull��ʹ�þֲ���̬��
template<typename TyPtr =std::mutex *>
inline std::string TimeToStr(std::time_t tmNum, bool bLocal= true,
	bool bThdSafe= false, const std::string &fmt="%F %T", TyPtr pMtx =nullptr)
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
//��timevalת��Ϊ��ʽ���ַ�����bMicro����С��
template<typename TyPtr =std::mutex *>
inline std::string TimeToStr(const timeval &tmval, bool bLocal =true,
	bool bThdSafe =false, const std::string &fmt ="%F %T",
	bool bMicro =true, TyPtr pMtx =nullptr)
{
	std::ostringstream oss;
	oss <<TimeToStr(tmval.tv_sec, bLocal, bThdSafe, fmt, pMtx);
	if(bMicro)
		oss <<'.' <<std::setfill('0') <<std::right <<std::setw(6) <<tmval.tv_usec;
	return oss.str();
}
//��durationת��Ϊ��ʽ���ַ�����DecimalBitΪ�����Զ��ƶ�С��λ�����Ǹ����趨С��λ��
template<int c_decimalBit, typename Rep, typename Period,
	typename TyPtr =std::mutex *
> inline typename std::enable_if<
	std::is_integral<Rep>::value && std::is_signed<Rep>::value, std::string
>::type TimeToStr(std::chrono::duration<Rep, Period> tmdu, bool bLocal =true,
	bool bThdSafe =false, const std::string &fmt ="%F %T", TyPtr pMtx =nullptr)
{
	if(tmdu<std::chrono::seconds(0))
		tmdu = std::chrono::seconds(0);
	auto tmduInt = std::chrono::duration_cast<std::chrono::seconds>(tmdu);
	std::ostringstream oss;
	oss <<TimeToStr(tmduInt.count(), bLocal, bThdSafe, fmt, pMtx);
	//�Զ��ƶ�λ��
	constexpr int c_realDecimalBit = c_decimalBit<0 ?
		RatioPrecision<Period, std::ratio<10>>() : c_decimalBit;
	constexpr long long c_decimalNum = PowerOperator(10, c_realDecimalBit);
	//����С������
	if(c_realDecimalBit>0) {
		auto tmduDecimal = std::chrono::duration_cast<
			std::chrono::duration<long long, std::ratio<1, c_decimalNum>>>(tmdu);
		tmduDecimal -= tmduInt;
		oss <<'.' <<std::setfill('0') <<std::right <<std::setw(c_realDecimalBit)
			<<tmduDecimal.count();
	}
	return oss.str();
}
//��system_clock::time_pointת��Ϊ��ʽ���ַ���
template<typename TyPtr =std::mutex *>
inline std::string TimeToStr(
	const std::chrono::time_point<std::chrono::system_clock> &tmpt,
	bool bLocal =true, bool bThdSafe =false,
	const std::string &fmt ="%F %T", TyPtr pMtx =nullptr)
{
	return TimeToStr(std::chrono::system_clock::to_time_t(tmpt),
		bLocal, bThdSafe, fmt, pMtx);
}
//�߾�����system_clock::time_pointת��Ϊ��ʽ���ַ�������ʵ�ֱ�֤��ȷ��
//DecimalBitΪ�����Զ��ƶ�С��λ�����Ǹ����趨С��λ��
template<int c_decimalBit, typename TyPtr =std::mutex *>
inline std::string TimeToStr(
	const std::chrono::time_point<std::chrono::system_clock> &tmpt,
	bool bLocal =true, bool bThdSafe =false,
	const std::string &fmt ="%F %T", TyPtr pMtx =nullptr)
{
	return TimeToStr<c_decimalBit>(tmpt.time_since_epoch(),
		bLocal, bThdSafe, fmt, pMtx);
}

//����ǰʱ��ת��Ϊ�ַ���
template<typename TyPtr =std::mutex *>
inline std::string NowTimeToStr(bool bLocal =true, bool bThdSafe =false,
	const std::string &fmt ="%F %T", TyPtr pMtx =nullptr)
{
	return TimeToStr(std::chrono::system_clock::now(),
		bLocal, bThdSafe, fmt, pMtx);
}
//�߾��Ƚ���ǰʱ��ת��Ϊ�ַ�������ʵ�ֱ�֤��ȷ��
//DecimalBitΪ�����Զ��ƶ�С��λ�����Ǹ����趨С��λ��
template<int c_decimalBit, typename TyPtr =std::mutex *>
inline std::string NowTimeToStr(bool bLocal =true, bool bThdSafe =false,
	const std::string &fmt ="%F %T", TyPtr pMtx =nullptr)
{
	return TimeToStr<c_decimalBit>(std::chrono::system_clock::now(),
		bLocal, bThdSafe, fmt, pMtx);
}

//��timevalת��Ϊduration
template<typename Ty= BlankType<0>>
inline std::chrono::microseconds TimevalToDuration(const timeval &tmval)
{
	return std::chrono::microseconds((long long)tmval.tv_sec*1000000+tmval.tv_usec);
}


//�ַ���ת�����ָ���
inline void _FromStringAssist(const std::string str, int &num)
{
	num = std::stoi(str);
}
inline void _FromStringAssist(const std::string str, long &num)
{
	num = std::stol(str);
}
inline void _FromStringAssist(const std::string str, long long &num)
{
	num = std::stoll(str);
}
inline void _FromStringAssist(const std::string str, unsigned &num)
{
	num = std::stoul(str);
}
inline void _FromStringAssist(const std::string str, unsigned long &num)
{
	num = std::stoul(str);
}
inline void _FromStringAssist(const std::string str, unsigned long long &num)
{
	num = std::stoull(str);
}
inline void _FromStringAssist(const std::string str, float &num)
{
	num = std::stof(str);
}
inline void _FromStringAssist(const std::string str, double &num)
{
	num = std::stod(str);
}
inline void _FromStringAssist(const std::string str, long double &num)
{
	num = std::stold(str);
}
//�ַ���ת�����֣����쳣
template<typename Ty>
inline bool FromString(const std::string str, Ty &num)
{
	bool ret = true;
	try {
		_FromStringAssist(str, num);
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
inline bool FromString(TyIter itSt, TyIter itEd, Ty &num)
{
	return FromString(std::string(itSt, itEd), num);
}

//��������ת���ַ���������ǧλ�ָ���
template<typename Ty>
inline typename std::enable_if<std::is_integral<Ty>::value, std::string
>::type GapToString(Ty num, int gap= 3, char ch= '\'')
{
	std::string str;
	//ȡ��
	if(SafeLT(num, 0))
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
	if(SafeLT(num, 0))
		str += '-';
	//��ת
	std::reverse(str.begin(), str.end());
	return str;
}

//�ַ����Ϳ��ַ���ת��
template<typename TyPtr =std::mutex *>
inline bool StrToWstr(std::wstring &wstr, const std::string &str,
	const std::string local ="chs", bool bThdSafe =false, TyPtr pMtx =nullptr)
{
	static std::mutex s_mtx;
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
template<typename TyPtr =std::mutex *>
inline bool WstrToStr(std::string &str, const std::wstring &wstr,
	const std::string local ="chs", bool bThdSafe =false, TyPtr pMtx =nullptr)
{
	static std::mutex s_mtx;
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
	std::string strRet = "\"";
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
	strRet += '\"';
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
			//��β����
			if((i==0 || i==str.size()-1) && (str[i]=='\"' || str[i]=='\''))
				continue;
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
