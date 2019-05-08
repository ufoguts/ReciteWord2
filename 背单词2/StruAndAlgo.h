#pragma once

//数据结构和算法扩充
//若使用ArgsTuple时需要先将TypeExtend引入进来

#include <cmath>
#include <cassert>

#include <type_traits>
#include <limits>
#include <algorithm>
#include <initializer_list>
#include <utility>
#include <tuple>
#include <functional>
#include <iterator>

#include <string>
#include <vector>
#include <list>
#include <map>

#include <stdexcept>
#include <random>
#include <ratio>



//测试参数是否合法类
template<typename ...>
struct _StruAndAlgoParamValidTester
{
	typedef void type;
};

//先去除引用再去除常数易变属性
template<typename Ty>
struct _StruAndAlgoRemoveCVRef
{
	using type = typename std::remove_cv<typename std::remove_reference<Ty>::type>::type;
};

//后者去除引用常数易变后是否为前者
template<typename TyDst, typename TySrc>
struct _StruAndAlgoIsRemoveCVRefSame:
	std::is_same<TyDst, typename _StruAndAlgoRemoveCVRef<TySrc>::type>
{
};

//后者参数包是否要不没有参数，要不只有一个并且为前者
template<typename TyDst, typename ...TySrc_S>
struct _StruAndAlgoIsNoneOrSame:
	std::integral_constant<bool, sizeof...(TySrc_S)==0>
{
};
template<typename TyDst, typename TySrc>
struct _StruAndAlgoIsNoneOrSame<TyDst, TySrc>:
	std::is_same<TyDst, TySrc>
{
};

//后者参数包是否要不没有参数，要不只有一个并且去除引用常数易变后为前者
template<typename TyDst, typename ...TySrc_S>
struct _StruAndAlgoIsNoneOrRemoveCVRefSame:
	std::integral_constant<bool, sizeof...(TySrc_S)==0>
{
};
template<typename TyDst, typename TySrc>
struct _StruAndAlgoIsNoneOrRemoveCVRefSame<TyDst, TySrc>:
	_StruAndAlgoIsRemoveCVRefSame<TyDst, TySrc>
{
};

//测试是否为迭代器
template<class Ty, class= void>
struct _StruAndAlgoIsIterator:
	std::false_type
{
};
template<class Ty>
struct _StruAndAlgoIsIterator<Ty, typename _StruAndAlgoParamValidTester<
	typename std::iterator_traits<Ty>::iterator_category>::type
>: std::true_type
{
};

//生成迭代器解引用的类型
template<typename TyIt>
struct _StruAndAlgoIteratorDerefType
{
	using type = decltype(*std::declval<TyIt>());
};

//存储某值类型，针对void进行特化
template<typename Ty>
struct _StruAndAlgoValueHold
{
	Ty hold;
public:
	_StruAndAlgoValueHold()
	{
	}
	//模板构造
	template<typename ...Ty_S>
	explicit _StruAndAlgoValueHold(Ty_S &&...args):
		hold(std::forward<Ty_S>(args)...)
	{
	}
};
template<>
struct _StruAndAlgoValueHold<void>
{
};

//调用变量的析构函数
template<typename Ty>
inline void _StruAndAlgoCallDestruct(Ty &value)
{
	value.~Ty();
}

//浮点数比较辅助宏
#define _STRUANDALGO_FLOAT_DEFAULT_ULP 10
#define _STRUANDALGO_IS_FLOAT_TYPE(Ty1, Ty2) \
	(std::is_floating_point<Ty1>::value && std::is_floating_point<Ty2>::value)
#define _STRUANDALGO_FLOAT_COMMON_EPSILON(Ty1, Ty2) \
	(std::max(std::numeric_limits<Ty1>::epsilon(), std::numeric_limits<Ty2>::epsilon()))
#define _STRUANDALGO_FLOAT_COMMON_MIN(Ty1, Ty2) \
	(std::max(std::numeric_limits<Ty1>::min(), std::numeric_limits<Ty2>::min()))
#define _STRUANDALGO_FLOAT_ABS(num) ((num)>=0? (num):-(num))
//浮点数比较相等，保证逻辑关系
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_STRUANDALGO_IS_FLOAT_TYPE(Ty1, Ty2), bool
>::type _StruAndAlgoFloatEQ(Ty1 num1, Ty2 num2, bool bAbs= false, int ulp= _STRUANDALGO_FLOAT_DEFAULT_ULP)
{
	return bAbs ?
		(_STRUANDALGO_FLOAT_ABS(num1-num2)<_STRUANDALGO_FLOAT_COMMON_EPSILON(Ty1, Ty2)*ulp)
		:
		(_STRUANDALGO_FLOAT_ABS(num1-num2)<_STRUANDALGO_FLOAT_COMMON_EPSILON(Ty1, Ty2)
		*(_STRUANDALGO_FLOAT_ABS(num1)+_STRUANDALGO_FLOAT_ABS(num2))*ulp
		|| _STRUANDALGO_FLOAT_ABS(num1-num2)<_STRUANDALGO_FLOAT_COMMON_MIN(Ty1, Ty2)*ulp);
}
//浮点数比较小于，保证逻辑关系
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_STRUANDALGO_IS_FLOAT_TYPE(Ty1, Ty2), bool
>::type _StruAndAlgoFloatLT(Ty1 num1, Ty2 num2, bool bAbs= false, int ulp= _STRUANDALGO_FLOAT_DEFAULT_ULP)
{
	return num1<num2 && !_StruAndAlgoFloatEQ(num1, num2, bAbs, ulp);
}
//浮点数比较小于等于，保证逻辑关系
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_STRUANDALGO_IS_FLOAT_TYPE(Ty1, Ty2), bool
>::type _StruAndAlgoFloatLTE(Ty1 num1, Ty2 num2, bool bAbs= false, int ulp= _STRUANDALGO_FLOAT_DEFAULT_ULP)
{
	return _StruAndAlgoFloatLT(num1, num2, bAbs, ulp) || _StruAndAlgoFloatEQ(num1, num2, bAbs, ulp);
}
//浮点数比较大于，保证逻辑关系
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_STRUANDALGO_IS_FLOAT_TYPE(Ty1, Ty2), bool
>::type _StruAndAlgoFloatGT(Ty1 num1, Ty2 num2, bool bAbs= false, int ulp= _STRUANDALGO_FLOAT_DEFAULT_ULP)
{
	return !(_StruAndAlgoFloatLT(num1, num2, bAbs, ulp) || _StruAndAlgoFloatEQ(num1, num2, bAbs, ulp));
}
//浮点数比较大于等于，保证逻辑关系
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_STRUANDALGO_IS_FLOAT_TYPE(Ty1, Ty2), bool
>::type _StruAndAlgoFloatGTE(Ty1 num1, Ty2 num2, bool bAbs= false, int ulp= _STRUANDALGO_FLOAT_DEFAULT_ULP)
{
	return !_StruAndAlgoFloatLT(num1, num2, bAbs, ulp);
}
//浮点数比较不等于，保证逻辑关系
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_STRUANDALGO_IS_FLOAT_TYPE(Ty1, Ty2), bool
>::type _StruAndAlgoFloatNEQ(Ty1 num1, Ty2 num2, bool bAbs= false, int ulp= _STRUANDALGO_FLOAT_DEFAULT_ULP)
{
	return !_StruAndAlgoFloatEQ(num1, num2, bAbs, ulp);
}
#undef _STRUANDALGO_FLOAT_DEFAULT_ULP
#undef _STRUANDALGO_IS_FLOAT_TYPE
#undef _STRUANDALGO_FLOAT_COMMON_EPSILON
#undef _STRUANDALGO_FLOAT_COMMON_MIN
#undef _STRUANDALGO_FLOAT_ABS

//广义的比较函数类
template<typename Ty1, typename Ty2>
struct _StruAndAlgoGeneralLess
{
	typedef bool result_type;
	typedef Ty1 first_argument_type;
	typedef Ty2 second_argument_type;
	result_type operator()(const first_argument_type &arg1,
		const second_argument_type &arg2) const
	{
		return arg1<arg2;
	}
};
template<typename Ty1, typename Ty2>
struct _StruAndAlgoGeneralEqualTo
{
	typedef bool result_type;
	typedef Ty1 first_argument_type;
	typedef Ty2 second_argument_type;
	result_type operator()(const first_argument_type &arg1,
		const second_argument_type &arg2) const
	{
		return arg1==arg2;
	}
};

//范围内字典序比较操作，使用迭代器，小于等于大于分别返回-1,0,1
template<typename TyIt1, typename TyIt2,
	typename TyLess= _StruAndAlgoGeneralLess<typename IteratorDerefType<TyIt1>::type,
	typename IteratorDerefType<TyIt2>::type>,
	typename TyEqual= _StruAndAlgoGeneralEqualTo<typename IteratorDerefType<TyIt1>::type,
	typename IteratorDerefType<TyIt2>::type>>
inline int _StruAndAlgoSequenceCompare(TyIt1 st1, TyIt1 ed1, TyIt2 st2, TyIt2 ed2,
	TyLess funcLess= TyLess(), TyEqual funcEqual= TyEqual())
{
	for(; ; ++st1, ++st2) {
		//判断结束条件
		if(st1==ed1) {
			if(st2==ed2)
				return 0;
			else
				return -1;
		}
		else if(st2==ed2)
			return 1;
		//判断大小
		if(funcLess(*st1, *st2))
			return -1;
		else if(!(funcEqual(*st1, *st2)))
			return 1;
	}
}

//模板定长整形
template<bool c_bSigned, int c_size>
struct _StruAndAlgoTemplateInt
{
};
template<>
struct _StruAndAlgoTemplateInt<true, 1>
{
	using type = int8_t;
};
template<>
struct _StruAndAlgoTemplateInt<false, 1>
{
	using type = uint8_t;
};
template<>
struct _StruAndAlgoTemplateInt<true, 2>
{
	using type = int16_t;
};
template<>
struct _StruAndAlgoTemplateInt<false, 2>
{
	using type = uint16_t;
};
template<>
struct _StruAndAlgoTemplateInt<true, 4>
{
	using type = int32_t;
};
template<>
struct _StruAndAlgoTemplateInt<false, 4>
{
	using type = uint32_t;
};
template<>
struct _StruAndAlgoTemplateInt<true, 8>
{
	using type = int64_t;
};
template<>
struct _StruAndAlgoTemplateInt<false, 8>
{
	using type = uint64_t;
};

//与size_t大小相同的距离类型，有符号类型
using _StruAndAlgoDifferenceType = typename _StruAndAlgoTemplateInt<true, sizeof(size_t)>::type;

//各类迭代器比较函数，尾后迭代器解引用不安全
template<typename Ty>
inline bool _StruAndAlgoIteratorLT(const Ty &it1, const Ty &it2)
{
	return &*it1<&*it2;
}
template<typename Ty>
inline bool _StruAndAlgoIteratorEQ(const Ty &it1, const Ty &it2)
{
	return &*it1==&*it2;
}


//其他库文件声明

//参数包类声明
template<typename ...Ty_S>
class ArgsTuple;

//ArgsTuple调用，计算返回类型辅助类
template<typename TyFunc, typename ...Ty_S>
struct ArgsTupleInvokeReturnType;

//使用ArgsTuple类调用函数，支持多个参数包与普通变量混合使用
template<typename TyFunc, typename ...Ty_S>
inline typename ArgsTupleInvokeReturnType<TyFunc &&, Ty_S &&...
>::type ArgsTupleInvoke(TyFunc &&func, Ty_S &&...arg_s);

//写类型二进制文件类
//不同环境的内置类型大小不一定相同，要保证使用大小无关变量
class BinWriteFile;

//读类型二进制文件类
//不同环境的内置类型大小不一定相同，要保证使用大小无关变量
class BinReadFile;




//************
//数据结构部分
//************

//支持下标快速删除的容器适配器，需提供支持下标访问的容器，如string,vector和deque
//实现上为删除元素后将最后的元素移动到前面
//所以快速删除时最后的迭代器会失效，被删除的迭代器指向原本最后的迭代器数据
template<typename TyContainer>
class FastEraseWrap:
	public TyContainer
{
public://基本函数
	//默认构造函数
	FastEraseWrap()
		= default;
	//继承构造函数
	using TyContainer::TyContainer;
	//省略合成5拷贝

public://成员函数
	//迭代器快速删除，返回移动的迭代器数量
	size_t FastErase(typename TyContainer::iterator itPos)
	{
		size_t ret = 0;
		auto itBack = --end();
		//若不为最后一个元素则移动填空
		if(itPos!=itBack) {
			*itPos = std::move(*itBack);
			ret = 1;
		}
		pop_back();
		return ret;
	}
	//迭代器范围快速删除，返回移动的迭代器数量
	size_t FastErase(typename TyContainer::iterator itSt,
		typename TyContainer::iterator itEd)
	{
		size_t ret = 0;
		//若不为最后一组元素则移动填空
		if(itEd!=end()) {
			ret = (size_t)std::min(itEd-itSt, end()-itEd);
			for(auto itSrc=end()-ret, itDst=itSt; itSrc!=end(); ++itSrc, ++itDst)
				*itDst = std::move(*itSrc);
		}
		resize(end()-(itEd-itSt)-begin());
		return ret;
	}
	//下标快速删除，返回移动的迭代器数量
	size_t IndexFastErase(size_t idx)
	{
		return FastErase(begin()+idx);
	}
	//下标范围快速删除，返回移动的迭代器数量
	size_t IndexFastErase(size_t idxSt, size_t idxEd)
	{
		return FastErase(begin()+idxSt, begin()+idxEd);
	}
};



template<typename IterStruMy, typename TyDataMy, typename TyContainer>
class _SequenceSkipListTemplateIter;
//顺序跳表结构，支持自定义顺序访问，使用跳表实现
//删除对应元素会使对应的两种迭代器失效
//插入删除和涉及下标操作的访问为对数复杂度
//随机生长的概率，指的是1/2的幂次数，即Prb = (1/2)^prbPow
//建议使用2-4，越大速度越慢，但空间占用约低
//理论上指数操作平均的时间复杂度为O(2^prbPow/prbPow*lg(n))
//理论上的额外空间复杂度为O(n/(2^prbPow-1))
/*实测时间如下，格式：概率值-打分，打分为与最快时间比值倒数
数据类型size_t(uint64_t)
构造1000000个: 4-100, 3-93, 2-77, 1-51
1000000个中随机顺序全部索引: 3-100, 2-93, 4-89, 1-50
1000000个中插入1000000个: 4-100, 3-85, 2-67, 1-39
2000000中删除1000000个: 4-100, 3-92, 2-62, 1-35
*/
template<typename TyData, typename TyRan= std::mt19937>
class SequenceSkipList
{
	/*
	lisLis是索引链表组成的链表，里面是lisIdx，lisVal是值链表
	d指数据，O指辅助节点，X指尾后节点，
	lisIndex中辅助接点存储个数指的是所有指向其的子节点，注意尾后节点也算
	数据链表和索引链表至少有一个头部辅助节点，且至少有一个索引链表
	 /^\
	  X
	  |
	  O-lisIdx-O---------------X>
	  |        |-------\-------\
	  O-lisIdx-O-------O-------X>
	  |        |---\   |---\---\
	  O-lisIdx-O---O---O---O---X>
	  |        |-\ |-\ |-\ |-\-\
	  | lisVal-O-d-d-d-d-d-d-d-X>
	lisLis
	*/

public://友元
	//迭代器
	template<typename IterStruMy, typename TyDataMy, typename TyContainer>
	friend class _SequenceSkipListTemplateIter;

private://数据类型定义
	struct IndexListStru;
	struct ValueListStru;
	//外层链表节点数据类型，是索引链表
	using ListListStru = std::list<IndexListStru>;
	//索引链表迭代器
	using IndexListIter = typename std::list<IndexListStru>::iterator;
	//值链表迭代器
	using ValueListIter = typename std::list<ValueListStru>::iterator;
	using ValueListConstIter = typename std::list<ValueListStru>::const_iterator;
	//外层链表迭代器
	using ListListIter = typename std::list<ListListStru>::iterator;
	//无数据构造标志，对于数据链表结构表示不构造数据
	struct NoDataConstruFlag
	{
	};

	//索引链表结构，需自己控制析构
	struct IndexListStru
	{
		IndexListIter itPr;//向上指的迭代器，指向自己表示空
		union {
			IndexListIter itChIdx;//向下指的索引迭代器
			ValueListIter itChVal;//向下指的值迭代器
		};
		size_t size;//索引链表存储元素个数
	public:
		//构造函数
		IndexListStru(IndexListIter o_itChIdx)
		{
			Constru(o_itChIdx);
		}
		IndexListStru(ValueListIter o_itChVal)
		{
			Constru(o_itChVal);
		}
		~IndexListStru()
		{
		}
		//成员析构
		void DestruIndex()
		{
			_StruAndAlgoCallDestruct(itChIdx);
		}
		void DestruValue()
		{
			_StruAndAlgoCallDestruct(itChVal);
		}
		//成员构造
		void Constru(IndexListIter o_itChIdx)
		{
			size = 0;
			new(&itChIdx) IndexListIter(o_itChIdx);
		}
		void Constru(ValueListIter o_itChVal)
		{
			size = 0;
			new(&itChVal) ValueListIter(o_itChVal);
		}
	};

	//值链表结构，需自己控制数据析构
	struct ValueListStru
	{
		IndexListIter itPr;//向上指的迭代器
		union {
			TyData data;//存储数据
		};
		//数据节点构造函数
		template<typename ...Ty_S>
		ValueListStru(Ty_S &&...arg_s):
			data(std::forward<Ty_S>(arg_s)...)
		{
		}
		//无数据构造函数
		ValueListStru(NoDataConstruFlag)
		{
		}
		~ValueListStru()
		{
		}
		//数据析构
		void DestruData()
		{
			_StruAndAlgoCallDestruct(data);
		}
	};

	//迭代器结构封装，用在迭代器模板继承
	struct IterStru
	{
		ValueListIter it;
	public:
		IterStru()
			= default;
	protected:
		IterStru(ValueListIter o_it):
			it(o_it)
		{
		}
	};
	struct ConstIterStru
	{
		ValueListConstIter it;
	public:
		ConstIterStru()
			= default;
	protected:
		ConstIterStru(ValueListConstIter o_it):
			it(o_it)
		{
		}
	};

public://辅助类型
	using DataType = TyData;
	using RanType = TyRan;
private:
	using MyClassType = SequenceSkipList<TyData, TyRan>;//自身类型

public://迭代器类型指定
	//迭代器
	using Iter = _SequenceSkipListTemplateIter<IterStru, TyData, MyClassType>;
	//常迭代器
	using ConstIter = _SequenceSkipListTemplateIter<ConstIterStru, const TyData, MyClassType>;
	//反向迭代器
	using RevIter = std::reverse_iterator<Iter>;
	using ConstRevIter = std::reverse_iterator<ConstIter>;

private://成员数据
	TyRan m_ran;//随机数引擎
	unsigned m_prbPow;//随机生长的概率
	bool m_bHasStru;//是否已经有结构，即析构时需要清理
	std::list<ValueListStru> m_lisVal;//数据链表
	std::list<ListListStru> m_lisLis;//索引链表的链表

public://辅助静态类型
	static constexpr int msc_defaultPrbPow = 3;//随机生长的概率默认值

public://基本函数
	//默认构造
	explicit SequenceSkipList(const TyRan &o_ran= TyRan(), unsigned o_prbPow= msc_defaultPrbPow):
		m_ran(o_ran), m_prbPow(o_prbPow), m_bHasStru(true)
	{
		assert(o_prbPow>0);
		InitBasicStruct();
	}
	//析构函数
	~SequenceSkipList()
	{
		//若为有效的结构
		if(m_bHasStru) {
			assert(!m_lisLis.empty() && !m_lisVal.empty());
			//清空索引链表节点
			CollectIndexNode();
			//析构数据节点
			for(auto itVal= ++m_lisVal.begin(); itVal!=m_lisVal.end(); ++itVal)
				itVal->DestruData();
		}
		//无效的结构（被移动构造后）可直接析构
	}
	//拷贝函数
	SequenceSkipList(const SequenceSkipList &other):
		SequenceSkipList(other.m_ran, other.m_prbPow)
	{
		Assign(other.Begin(), other.End());
	}
	SequenceSkipList &operator =(const SequenceSkipList &other)
	{
		if(this!=&other) {
			m_ran = other.m_ran;
			m_prbPow = other.m_prbPow;
			Assign(other.Begin(), other.End());
		}
		return *this;
	}
	//移动函数
	SequenceSkipList(SequenceSkipList &&other) noexcept
		: m_ran(std::move(other.m_ran)), m_prbPow(std::move(other.m_prbPow)),
		m_bHasStru(std::move(other.m_bHasStru)),
		m_lisVal(std::move(other.m_lisVal)), m_lisLis(std::move(other.m_lisLis))
	{
		//标记对方无结构
		other.m_bHasStru = false;
	}
	SequenceSkipList &operator =(SequenceSkipList &&other) noexcept
	{
		if(this!=&other) {
			//移动成员
			m_ran = std::move(other.m_ran);
			m_prbPow = std::move(other.m_prbPow);
			std::swap(m_bHasStru, other.m_bHasStru);//对方是否有结构取决于我
			m_lisVal.swap(other.m_lisVal);
			m_lisLis.swap(other.m_lisLis);
		}
		return *this;
	}
	//赋值构造函数，同时决定是否使用准随机
	SequenceSkipList(std::initializer_list<TyData> init, bool bQuasiRandom= false,
		const TyRan &o_ran= TyRan(), unsigned o_prbPow= msc_defaultPrbPow
	): SequenceSkipList(o_ran, o_prbPow)
	{
		Assign(init, bQuasiRandom);
	}
	explicit SequenceSkipList(size_t size):
		SequenceSkipList()
	{
		Assign(size);
	}
	explicit SequenceSkipList(size_t size, const TyData &value, bool bQuasiRandom= false,
		const TyRan &o_ran= TyRan(), unsigned o_prbPow= msc_defaultPrbPow
	): SequenceSkipList(o_ran, o_prbPow)
	{
		Assign(size, value, bQuasiRandom);
	}
	template<typename TyIt, typename= typename std::enable_if<
		_StruAndAlgoIsIterator<TyIt>::value>::type
	> explicit SequenceSkipList(TyIt itSt, TyIt itEd, bool bQuasiRandom= false,
		const TyRan &o_ran= TyRan(), unsigned o_prbPow= msc_defaultPrbPow
	): SequenceSkipList(o_ran, o_prbPow)
	{
		Assign(itSt, itEd, bQuasiRandom);
	}
public://运算符重载
	//列表赋值
	SequenceSkipList &operator =(std::initializer_list<TyData> init)
	{
		Assign(init);
		return *this;
	}
	//比较操作，按顺序的字典序
	template<typename TyDataOther, typename TyRanOther>
	bool operator <(const SequenceSkipList<TyDataOther, TyRanOther> &other) const
	{
		return Compare(other)<0;
	}
	template<typename TyDataOther, typename TyRanOther>
	bool operator ==(const SequenceSkipList<TyDataOther, TyRanOther> &other) const
	{
		return Compare(other)==0;
	}
	template<typename TyDataOther, typename TyRanOther>
	bool operator !=(const SequenceSkipList<TyDataOther, TyRanOther> &other) const
	{
		return Compare(other)!=0;
	}
	template<typename TyDataOther, typename TyRanOther>
	bool operator <=(const SequenceSkipList<TyDataOther, TyRanOther> &other) const
	{
		return Compare(other)<=0;
	}
	template<typename TyDataOther, typename TyRanOther>
	bool operator >(const SequenceSkipList<TyDataOther, TyRanOther> &other) const
	{
		return Compare(other)>0;
	}
	template<typename TyDataOther, typename TyRanOther>
	bool operator >=(const SequenceSkipList<TyDataOther, TyRanOther> &other) const
	{
		return Compare(other)>=0;
	}
private://私有辅助函数
	//求迭代器跨越的距离
	static _StruAndAlgoDifferenceType ListIterDistance(IndexListIter st, IndexListIter ed)
	{
		_StruAndAlgoDifferenceType ret = 0;
		for(; st!=ed; ++st)
			ret += (_StruAndAlgoDifferenceType)st->size;
		return ret;
	}
	static _StruAndAlgoDifferenceType ListIterDistance(ValueListConstIter st, ValueListConstIter ed)
	{
		_StruAndAlgoDifferenceType ret = 0;
		for(; st!=ed; ++st)
			++ ret;
		return ret;
	}
	//下标访问辅助函数，stk表示当前链表层号，数据链表为0层
	static ValueListIter DownFindIndexIter(IndexListIter itIdx, size_t idx, size_t stk)
	{
		ValueListIter itVal;
		while(true) {
			//向后走
			for(; idx>=itIdx->size; ++itIdx)
				idx -= itIdx->size;
			-- stk;
			//若没到底向下走
			if(stk!=0)
				itIdx = itIdx->itChIdx;
			//若到底则赋值并跳出
			else {
				itVal = itIdx->itChVal;
				break;
			}
		}
		//值链表继续向后走找数据
		for(; idx!=0; --idx, ++itVal)
			;
		return itVal;
	}
	//进行一次随机决定
	bool RandomDecide()
	{
		auto num = m_ran();
		return (((1<<m_prbPow)-1)&num) == 0;
	}
	//重组辅助函数，根据是否准随机化和序号决定是否生成节点
	bool RestructureAssist(bool bQuasiRandom, unsigned idx)
	{
		if(idx==0)
			return true;
		if(bQuasiRandom)
			return (((1<<m_prbPow)-1)&idx) == 0;
		else
			return RandomDecide();
	}
	//添加新的一层索引链表，并添加一个头部索引，返回新添加的索引链表迭代器
	ListListIter AddIndexList()
	{
		//添加新索引链表
		m_lisLis.emplace_back();
		IndexListIter itIdx;//新索引链表的头部索引节点迭代器
		//若原先只有一层值链表
		if(m_lisLis.size()==1) {
			//新索引链表添加头部节点
			m_lisLis.back().emplace_back(m_lisVal.begin());
			itIdx = m_lisLis.back().begin();
			//将原本最上层链表父节点指向新索引链表头部节点，计算新索引链表头部节点大小
			for(auto it=m_lisVal.begin(); it!=m_lisVal.end(); ++it) {
				it->itPr = itIdx;
				++ itIdx->size;
			}
			//加上值链表尾后迭代器
			++ itIdx->size;
		}
		//若当前最上层是索引链表
		else {
			auto itLisLisPrv = std::prev(m_lisLis.end(), 2);//原本的最上层链表
			//新索引链表添加头部节点
			m_lisLis.back().emplace_back(itLisLisPrv->begin());
			itIdx = m_lisLis.back().begin();
			//将原本最上层链表父节点指向新索引链表头部节点，计算新索引链表头部节点大小
			for(auto it=itLisLisPrv->begin(); it!=itLisLisPrv->end(); ++it) {
				it->itPr = itIdx;
				itIdx->size += it->size;
			}
		}
		//新索引链表头部节点父节点指向自己
		itIdx->itPr = itIdx;
		return --m_lisLis.end();
	}
	//构造基本结构，用于在构造函数中调用
	void InitBasicStruct()
	{
		//添加数据链表和首元素
		m_lisVal.emplace_back(NoDataConstruFlag());
		//添加索引链表和首元素
		AddIndexList();
	}
	//将所有索引链表挪到一个空闲链表当中，但外部链表中的索引链表本体没有清除
	//空闲链表当中的节点已析构，数据链表的父节点没有进行维护
	//需保证至少有一层索引和一个索引节点，无须保证索引之间和值之间互联
	std::list<IndexListStru> CollectIndexNode()
	{
		std::list<IndexListStru> lisRet;//返回链表节点
		//先处理第一层索引链表
		auto itLisLis = m_lisLis.begin();
		//移动到空闲链表
		lisRet.splice(lisRet.end(), *itLisLis);
		//析构其中的元素
		for(auto it= lisRet.begin(); it!=lisRet.end(); ++it)
			it->DestruValue();
		//处理剩余层索引
		for(++itLisLis; itLisLis!=m_lisLis.end(); ++itLisLis) {
			auto itSt = itLisLis->begin(),
				itEd = itLisLis->end();
			//析构索引节点
			for(auto it= itSt; it!=itEd; ++it)
				it->DestruIndex();
			//挪到空闲链表中
			lisRet.splice(lisRet.end(), *itLisLis);
		}
		return lisRet;
	}
public://成员函数
	//赋值函数，一同决定是否使用准随机化
	SequenceSkipList &Assign(std::initializer_list<TyData> init, bool bQuasiRandom= false)
	{
		Assign(init.begin(), init.end(), bQuasiRandom);
		return *this;
	}
	SequenceSkipList &Assign(size_t size)
	{
		Clear();
		for(int i=0; i!=size; ++i)
			m_lisVal.emplace_back();
		Restructure(false);
		return *this;
	}
	SequenceSkipList &Assign(size_t size, const TyData &value, bool bQuasiRandom= false)
	{
		Clear();
		for(int i=0; i!=size; ++i)
			m_lisVal.emplace_back(value);
		Restructure(bQuasiRandom);
		return *this;
	}
	template<typename TyIt>
	typename std::enable_if<_StruAndAlgoIsIterator<TyIt>::value, SequenceSkipList &
	>::type Assign(TyIt itSt, TyIt itEd, bool bQuasiRandom= false)
	{
		Clear();
		m_lisVal.insert(m_lisVal.end(), itSt, itEd);
		Restructure(bQuasiRandom);
		return *this;
	}
	//比较操作，小于等于大于分别返回-1,0,1
	template<typename TyDataOther, typename TyRanOther,
		typename TyFuncLess= _StruAndAlgoGeneralLess<TyData, TyDataOther>,
		typename TyFuncEqual= _StruAndAlgoGeneralEqualTo<TyData, TyDataOther>
	> int Compare(const SequenceSkipList<TyDataOther, TyRanOther> &other,
		TyFuncLess funcLess= TyFuncLess(), TyFuncEqual funcEqual= TyFuncEqual()) const
	{
		return _StruAndAlgoSequenceCompare(Begin(), End(), other.Begin(), other.End(),
			funcLess, funcEqual);
	}
	//顺序产生迭代器
	Iter Begin()
	{
		return Iter(++m_lisVal.begin());
	}
	ConstIter Begin() const
	{
		return ConstIter(++m_lisVal.begin());
	}
	Iter begin()
	{
		return Begin();
	}
	ConstIter begin() const
	{
		return Begin();
	}
	ConstIter ConstBegin() const
	{
		return Begin();
	}
	Iter End()
	{
		return Iter(m_lisVal.end());
	}
	ConstIter End() const
	{
		return ConstIter(m_lisVal.end());
	}
	Iter end()
	{
		return End();
	}
	ConstIter end() const
	{
		return End();
	}
	ConstIter ConstEnd() const
	{
		return End();
	}
	//顺序产生反向迭代器
	RevIter RevBegin()
	{
		return RevIter(End());
	}
	ConstRevIter RevBegin() const
	{
		return ConstRevIter(End());
	}
	ConstRevIter ConstRevBegin() const
	{
		return RevIter();
	}
	RevIter RevEnd()
	{
		return RevIter(Begin());
	}
	ConstRevIter RevEnd() const
	{
		return ConstRevIter(Begin());
	}
	ConstRevIter ConstRevEnd() const
	{
		return RevEnd();
	}
	//产生下标迭代器
	Iter GetIter(size_t idx)
	{
		++ idx;
		//直接向下查找
		return Iter(DownFindIndexIter(m_lisLis.back().begin(), idx, m_lisLis.size()));
	}
	ConstIter GetIter(size_t idx) const
	{
		return ConstIter(((SequenceSkipList *)this)->GetIter(idx));
	}
	ConstIter GetConstIter(size_t idx) const
	{
		return GetIter(idx);
	}
	//获取迭代器下标
	size_t GetIndex(ConstIter it) const
	{
		return it-Begin();
	}
	//访问顺序首尾元素
	TyData &Front()
	{
		return *Begin();
	}
	const TyData &Front() const
	{
		return *Begin();
	}
	TyData &Back()
	{
		return *(--End());
	}
	const TyData &Back() const
	{
		return *(--End());
	}
	//下标访问元素
	TyData &operator[](size_t idx)
	{
		return *GetIter(idx);
	}
	const TyData &operator[](size_t idx) const
	{
		return *GetIter(idx);
	}
	TyData &At(size_t idx)
	{
		if(idx>=Size())
			throw std::out_of_range("");
		return operator[](idx);
	}
	const TyData &At(size_t idx) const
	{
		if(idx>=Size())
			throw std::out_of_range("");
		return operator[](idx);
	}
	//查看容量
	bool Empty() const
	{
		return Size()==0;
	}
	size_t Size() const
	{
		return m_lisVal.size()-1;
	}
	//清空
	void Clear()
	{
		//析构数据节点
		for(auto itVal= ++m_lisVal.begin(); itVal!=m_lisVal.end(); ++itVal)
			itVal->DestruData();
		//删除数据节点
		m_lisVal.erase(++m_lisVal.begin(), m_lisVal.end());
		//结构重组
		Restructure(true);
	}
	//改变容量
	template<typename ...Ty_S>
	typename std::enable_if<_StruAndAlgoIsNoneOrSame<TyData, Ty_S...>::value, void
	>::type Resize(size_t size, const Ty_S &...arg_s)
	{
		while(Size()>size)
			Erase(--End());
		while(Size()<size)
			Emplace(End(), arg_s...);
	}
	//两端插入
	template<typename Ty>
	typename std::enable_if<_StruAndAlgoIsRemoveCVRefSame<TyData, Ty>::value, void
	>::type PushBack(Ty &&value)
	{
		Emplace(End(), std::forward<Ty>(value));
	}
	template<typename ...Ty_S>
	void EmplaceBack(Ty_S &&...arg_s)
	{
		Emplace(End(), std::forward<Ty_S>(arg_s)...);
	}
	template<typename Ty>
	typename std::enable_if<_StruAndAlgoIsRemoveCVRefSame<TyData, Ty>::value, void
	>::type PushFront(Ty &&value)
	{
		Emplace(Begin(), std::forward<Ty>(value));
	}
	template<typename ...Ty_S>
	void EmplaceFront(Ty_S &&...arg_s)
	{
		Emplace(Begin(), std::forward<Ty_S>(arg_s)...);
	}
	//在指定位置前插入，返回插入的第一个元素的迭代器
	template<typename Ty>
	typename std::enable_if<_StruAndAlgoIsRemoveCVRefSame<TyData, Ty>::value, Iter
	>::type Insert(Iter itPos, Ty &&value)
	{
		return Emplace(itPos, std::forward<Ty>(value));
	}
	Iter Insert(Iter itPos, std::initializer_list<TyData> init)
	{
		return Insert(itPos, init.begin(), init.end());
	}
	template<typename TyIt>
	Iter Insert(Iter itPos, TyIt itSt, TyIt itEd)
	{
		auto ret = itPos;
		bool bIns = false;
		for(; itSt!=itEd; ++itSt) {
			if(!bIns)
				ret = Emplace(itPos, *itSt);
			else
				Emplace(itPos, *itSt);
			bIns = true;
		}
		return ret;
	}
	template<typename ...Ty_S>
	Iter Emplace(Iter itPos, Ty_S &&...arg_s)
	{
		auto itVal = SimpleEmplace(itPos, std::forward<Ty_S>(arg_s)...).it;
		IndexListIter itIdx;
		//开始向上插入节点
		bool bHasAdd = false;//是否已插入节点
		auto itLisLis = m_lisLis.begin();
		for(; RandomDecide(); bHasAdd=true) {
			IndexListIter itIdxPrv;//待插入索引节点的前一个迭代器
			size_t dis;//上一个插入节点前面的节点数
			//若添加第一个节点
			if(!bHasAdd) {
				itIdxPrv = itVal->itPr;
				dis = ListIterDistance(itIdxPrv->itChVal, itVal);
				//指向新插入的索引节点
				itIdx = itLisLis->emplace(std::next(itIdxPrv), itVal);
				//改变应指向新插入索引节点的父节点迭代器
				for(auto it=itVal; it!=m_lisVal.end() && it->itPr==itIdxPrv; ++it)
					it->itPr = itIdx;
			}
			//若非第一个节点
			else {
				itIdxPrv = itIdx->itPr;
				dis = ListIterDistance(itIdxPrv->itChIdx, itIdx);
				auto itLisLisPrv = std::prev(itLisLis);//上一个插入节点所在外层链表
				auto itIdxOri = itIdx;//上一个插入节点
				//指向新插入的索引节点
				itIdx = itLisLis->emplace(std::next(itIdxPrv), itIdxOri);
				//改变应指向新插入索引节点的父节点迭代器
				for(auto it=itIdxOri; it!=itLisLisPrv->end() && it->itPr==itIdxPrv; ++it)
					it->itPr = itIdx;
			}
			//改变新插入索引节点和其前面索引接点的大小
			itIdx->size = itIdxPrv->size-dis;
			itIdxPrv->size = dis;
			//递增外层链表迭代器
			++ itLisLis;
			//若为最上层索引则建立新索引
			if(itLisLis==m_lisLis.end())
				itLisLis = AddIndexList();
			//否则父节点指向前者的父节点
			else
				itIdx->itPr = itIdxPrv->itPr;
		}
		return Iter(itVal);
	}
	//使用下标插入，即插入的首个元素下标为指定下标，返回插入首个位置的迭代器
	template<typename ...Ty_S>
	Iter IndexInsert(size_t idx, Ty_S &&...arg_s)
	{
		return Insert(GetIter(idx), std::forward<Ty_S>(arg_s));
	}
	template<typename ...Ty_S>
	Iter IndexEmplace(size_t idx, Ty_S &&...arg_s)
	{
		return Emplace(GetIter(idx), std::forward<Ty_S>(arg_s));
	}
	//简单插入接口，注意这种情况下不会创建索引节点，一般需自己重组索引结构
	template<typename ...Ty_S>
	Iter SimpleEmplace(Iter itPos, Ty_S &&...arg_s)
	{
		assert(itPos.it!=m_lisVal.begin());
		//插入数据
		auto itVal = m_lisVal.emplace(itPos.it, std::forward<Ty_S>(arg_s)...);
		IndexListIter itIdx = std::prev(itVal)->itPr;
		itVal->itPr = itIdx;
		//增加父节点计数
		while(true) {
			++ itIdx->size;
			if(_StruAndAlgoIteratorEQ(itIdx->itPr, itIdx))
				break;
			itIdx = itIdx->itPr;
		}
		return Iter(itVal);
	}
	//两端顺序删除
	void PopBack()
	{
		Erase(--End());
	}
	void PopFront()
	{
		Erase(Begin());
	}
	//指定位置顺序删除，返回删除后一个位置
	Iter Erase(Iter itPos)
	{
		assert(itPos.it!=m_lisVal.begin());
		//删除直连的索引节点
		auto itVal = itPos.it;
		auto itIdx = itPos.it->itPr;
		bool bHasIdxCnct = itIdx->itChVal==itVal;//是否有直连的索引节点
		auto itLisLis = m_lisLis.begin();//记录直连索引接点顶端外层链表
		IndexListIter itIdxCnct = itIdx;//记录直连索引接点顶端
		bool bNowIdxCnct = bHasIdxCnct;//是否当前节点处于直连状态
		//更改索引节点大小循环，顺便记录最上层直连节点
		while(true) {
			-- itIdx->size;
			//无父节点则退出
			if(_StruAndAlgoIteratorEQ(itIdx->itPr, itIdx))
				break;
			//若处于直连状态则更新节点
			if(bNowIdxCnct) {
				if(itIdx->itPr->itChIdx==itIdx) {
					++ itLisLis;
					itIdxCnct = itIdx->itPr;
				}
				else
					bNowIdxCnct = false;
			}
			itIdx = itIdx->itPr;
		}
		//从最顶层直连节点开始向下删除
		while(bHasIdxCnct) {
			//改变前面节点的数量
			auto itIdxPrv = std::prev(itIdxCnct);
			itIdxPrv->size += itIdxCnct->size;
			//改变其子节点的父节点
			if(itLisLis!=m_lisLis.begin()) {
				auto itLisLisPrv = std::prev(itLisLis);
				for(auto it=std::next(itIdxCnct->itChIdx);
					it!=itLisLisPrv->end() && it->itPr==itIdxCnct; ++it
					)
					it->itPr = itIdxPrv;
			}
			else {
				for(auto it=std::next(itIdxCnct->itChVal);
					it!=m_lisVal.end() && it->itPr==itIdxCnct; ++it
					)
					it->itPr = itIdxPrv;
			}
			//若当前层节点数消除后等于1则将其上层消除
			if(itLisLis->size()==2) {
				m_lisLis.back().front().DestruIndex();
				m_lisLis.pop_back();
				itLisLis->front().itPr = itLisLis->begin();
			}
			//若未到底，移除节点并向下
			if(itLisLis!=m_lisLis.begin()) {
				auto itLastIdx = itIdxCnct;
				itIdxCnct = itIdxCnct->itChIdx;
				itLastIdx->DestruIndex();
				itLisLis->erase(itLastIdx);
				-- itLisLis;
			}
			//若到底，移除节点并跳出
			else {
				itIdxCnct->DestruValue();
				itLisLis->erase(itIdxCnct);
				break;
			}
		}
		//删除值链表节点
		itVal->DestruData();
		return Iter(m_lisVal.erase(itVal));
	}
	Iter Erase(Iter itSt, Iter itEd)
	{
		while(itSt!=itEd)
			itSt = Erase(itSt);
		return itEd;
	}
	//使用下标删除，返回删除后一个位置的迭代器
	Iter IndexErase(size_t idx)
	{
		return Erase(GetIter(idx));
	}
	Iter IndexErase(size_t idxSt, size_t idxEd)
	{
		assert(idxSt<=idxEd);
		auto it=GetIter(idxSt);
		for(; idxSt<idxEd; ++idxSt)
			it = Erase(it);
		return it;
	}
	//交换元素，交换后迭代器指向元素值不变，位置交换
	void SwapElement(Iter it1, Iter it2)
	{
		assert(it1.it!=m_lisVal.begin() && it2.it!=m_lisVal.begin());
		if(it1==it2)
			return;
		//交换位置
		auto itTmp = std::next(it2.it);
		m_lisVal.splice(it1.it, m_lisVal, it2.it);
		m_lisVal.splice(itTmp, m_lisVal, it1.it);
		//重组结构
		if(it1.it->itPr->itChVal==it1.it)
			it1.it->itPr->itChVal = it2.it;
		if(it2.it->itPr->itChVal==it2.it)
			it2.it->itPr->itChVal = it1.it;
		std::swap(it1.it->itPr, it2.it->itPr);
	}
	void SwapElement(Iter st1, Iter ed1, Iter st2)
	{
		if(st1==st2)
			return;
		for(; st1!=ed1; ++st1, ++st2)
			SwapElement(st1, st2);
	}
	//序列反转，一同进行结构重组
	void Reverse(bool bQuasiRandom= false)
	{
		//拿出首元素
		std::list<ValueListStru> lisTmp;
		lisTmp.splice(lisTmp.end(), m_lisVal, m_lisVal.begin());
		//反转
		m_lisVal.reverse();
		//放回首元素
		m_lisVal.splice(m_lisVal.begin(), lisTmp);
		//结构重组
		Restructure(bQuasiRandom);
	}
	//排序操作，默认不使用模板提供的比较类，一同进行结构重组
	template<typename TyFunc= std::less<TyData>>
	void Sort(bool bQuasiRandom= false, const TyFunc &func= std::less<TyData>())
	{
		auto lbdCom = [&](const ValueListStru &arg1, const ValueListStru &arg2) {
			return func(arg1.data, arg2.data);
		};
		//拿出首元素
		std::list<ValueListStru> lisTmp;
		lisTmp.splice(lisTmp.end(), m_lisVal, m_lisVal.begin());
		//排序
		m_lisVal.sort(lbdCom);
		//放回首元素
		m_lisVal.splice(m_lisVal.begin(), lisTmp);
		//重组结构
		Restructure(bQuasiRandom);
	}
	//结构重组，线性复杂度，准随机化将结构变为平衡二叉树，否则则随机生成
	//需保证至少有一层索引和一个索引节点，无须保证索引之间和值之间互联
	void Restructure(bool bQuasiRandom)
	{
		//去除所有索引节点
		auto lisFree = CollectIndexNode();
		//添加第一层
		auto itLisLis = m_lisLis.begin();
		auto itVal = m_lisVal.begin();
		for(unsigned i=0; itVal!=m_lisVal.end(); ++itVal, ++i) {
			//若对应节点应该向上添加节点
			bool bJdg = bQuasiRandom? ((((1<<m_prbPow)-1)&i)==0) : RandomDecide();
			if(RestructureAssist(bQuasiRandom, i)) {
				//添加节点
				if(!lisFree.empty()) {
					itLisLis->splice(itLisLis->end(), lisFree, lisFree.begin());
					itLisLis->back().Constru(itVal);
				}
				else
					itLisLis->emplace_back(itVal);
				//整理节点
				auto itIdxPr = --itLisLis->end();
				itIdxPr->itPr = itIdxPr;
				itIdxPr->size = 1;
				itVal->itPr = itIdxPr;
			}
			//否则改变其父节点并增加计数
			else {
				itVal->itPr = --itLisLis->end();
				++ itLisLis->back().size;
			}
		}
		//考虑尾后节点的增加索引计数
		++ itLisLis->back().size;
		//添加若当前层多余一个节点，则添加后续层
		while(itLisLis->size()!=1) {
			auto itLisLisPrv = itLisLis;
			++ itLisLis;
			if(itLisLis==m_lisLis.end()) {
				m_lisLis.emplace_back();
				itLisLis = --m_lisLis.end();
			}
			//添加索引节点
			auto itIdx = itLisLisPrv->begin();
			for(unsigned i=0; itIdx!=itLisLisPrv->end(); ++itIdx, ++i) {
				//若对应节点应该向上添加节点
				if(RestructureAssist(bQuasiRandom, i)) {
					//添加节点
					if(!lisFree.empty()) {
						itLisLis->splice(itLisLis->end(), lisFree, lisFree.begin());
						itLisLis->back().Constru(itIdx);
					}
					else
						itLisLis->emplace_back(itIdx);
					//整理节点
					auto itIdxPr = --itLisLis->end();
					itIdxPr->itPr = itIdxPr;
					itIdxPr->size = itIdx->size;
					itIdx->itPr = itIdxPr;
				}
				//否则改变其父节点并增加计数
				else {
					itIdx->itPr = --itLisLis->end();
					itLisLis->back().size += itIdx->size;
				}
			}	
		}
		//清除多余的外层链表节点
		++ itLisLis;
		m_lisLis.erase(itLisLis, m_lisLis.end());
	}
	//随机器接口
	const TyRan &GetRandomEngin() const
	{
		return m_ran;
	}
	TyRan &GetRandomEngin()
	{
		return m_ran;
	}
	//debug
	void Debug() const
	{
		//检查基本结构
		assert(m_bHasStru);//检查具有结构
		assert(m_lisLis.size()>=1);//至少1个外层索引链表
		assert(m_lisVal.size()>=1);//值链表节点数量大于等于1
		assert(m_lisLis.back().size()==1);//最上层索引链表数量等于1
		assert(m_lisLis.back().front().itPr==m_lisLis.back().begin());//最上层索引链表数量父节点为自身
		//检查值链表
		auto itLisLisNxt = m_lisLis.begin();
		auto itIdxPr = itLisLisNxt->begin();
		for(auto it=m_lisVal.begin(); it!=m_lisVal.end(); ++itIdxPr) {
			assert(it->itPr==itIdxPr);//父节点与子节点互联
			assert(itIdxPr->itChVal==it);//父节点与子节点互联
			size_t size = 0;
			for(; it!=m_lisVal.end() && it->itPr==itIdxPr; ++it)
				++ size;
			if(it==m_lisVal.end())
				++ size;
			assert(itIdxPr->size==size);//父节点大小正确
		}
		assert(itIdxPr==itLisLisNxt->end());//父节点无剩余
		//检查索引链表，除最上层外
		auto itLisLis = itLisLisNxt;
		++ itLisLisNxt;
		for(; itLisLisNxt!=m_lisLis.end(); ++itLisLis, ++itLisLisNxt) {
			assert(itLisLis->size()>=2);//索引链表节点数量应大于等于2
			itIdxPr = itLisLisNxt->begin();
			for(auto it=itLisLis->begin(); it!=itLisLis->end(); ++itIdxPr) {
				assert(it->itPr==itIdxPr);//父节点与子节点互联
				assert(itIdxPr->itChIdx==it);//父节点与子节点互联
				size_t size = 0;
				for(; it!=itLisLis->end() && it->itPr==itIdxPr; ++it)
					size += it->size;
				assert(itIdxPr->size==size);//父节点大小正确
			}
			assert(itIdxPr==itLisLisNxt->end());//父节点无剩余
		}
	}
	//debug and print，和类开头注释一个格式
	void DebugAndPrint() const
	{
		Debug();
		std::vector<std::string> vecStr;
		std::vector<size_t> vecIdx;//上一层数据的字符串下标
		vecStr.push_back("lisLis");
		//打印值链表
		auto itLisLisNxt = m_lisLis.begin();
		auto itIdxPr = itLisLisNxt->begin();
		std::string strData = "  | lisVal";//数据行
		std::string strLink = "  |       ";//连接行
		for(auto it=m_lisVal.begin(); it!=m_lisVal.end(); ++itIdxPr) {
			for(; it!=m_lisVal.end() && it->itPr==itIdxPr; ++it) {
				//数据行处理
				if(it==m_lisVal.begin())
					strData += "-O";
				else
					strData += "-d";
				//连接行处理
				if(itIdxPr->itChVal==it) {
					strLink += " |";
					vecIdx.push_back(strLink.size()-1);
				}
				else
					strLink += "-\\";
			}
		}
		//尾后节点处理
		strData += "-X>";
		strLink += "-\\";
		vecIdx.push_back(strLink.size()-1);
		vecStr.push_back(strData);
		vecStr.push_back(strLink);
		//打印索引链表
		std::vector<size_t> vecLastIdx;
		auto itLisLis = itLisLisNxt;
		++ itLisLisNxt;
		for(; itLisLisNxt!=m_lisLis.end(); ++itLisLis, ++itLisLisNxt) {
			vecIdx.swap(vecLastIdx);
			vecIdx.clear();
			strData = "  O-lisIdx";//数据行
			strLink = "  |       ";//连接行
			itIdxPr = itLisLisNxt->begin();
			auto itVecIdx = vecLastIdx.begin();
			for(auto it=itLisLis->begin(); it!=itLisLis->end(); ++itIdxPr) {
				for(; it!=itLisLis->end() && it->itPr==itIdxPr; ++it) {
					//数据行处理
					strData.resize(*itVecIdx, '-');
					strData += "O";
					//连接行处理
					if(itIdxPr->itChIdx==it) {
						strLink.resize(*itVecIdx, ' ');
						strLink += "|";
						vecIdx.push_back(strLink.size()-1);
					}
					else {
						strLink.resize(*itVecIdx, '-');
						strLink += "\\";
					}
					//下一个索引
					++ itVecIdx;
				}
			}
			//尾后节点处理
			strData.resize(*itVecIdx, '-');
			strData += "X>";
			strLink.resize(*itVecIdx, '-');
			strLink += "\\";
			vecIdx.push_back(strLink.size()-1);
			vecStr.push_back(strData);
			vecStr.push_back(strLink);
		}
		//打印最顶部链表
		vecStr.push_back("  O-lisIdx-O>");
		vecStr.back().resize(vecIdx.back(), '-');
		vecStr.back() += "X";
		vecStr.push_back(" /^\\");
		//打印
		for(auto rit=vecStr.rbegin(); rit!=vecStr.rend(); ++rit)
			std::cout <<*rit <<"\n";
		std::cout <<std::flush;
	}
};

//顺序跳表结构，迭代器模板
//是随机访问迭代器，但是随机访问、随机增减、小于比较操作与距离成对数复杂度
template<typename IterStruMy, typename TyDataMy, typename TyContainer>
class _SequenceSkipListTemplateIter:
	private IterStruMy,
	public std::iterator<std::random_access_iterator_tag, TyDataMy,
	_StruAndAlgoDifferenceType, TyDataMy *, TyDataMy &>
{
public://友元
	template<typename TyData, typename TyRan>
	friend class SequenceSkipList;
	template<typename IterStruOther, typename TyDataOther, typename TyContainerOther>
	friend class _SequenceSkipListTemplateIter;

private://数据
	//继承自IterStruMy，it为迭代器

public://基本函数
	//默认构造
	_SequenceSkipListTemplateIter()
		= default;
	//继承基类数据构造
	using IterStruMy::IterStruMy;
	//由非常类构造常类
	_SequenceSkipListTemplateIter(const typename TyContainer::Iter &other)
	{
		operator =(other);
	}
	_SequenceSkipListTemplateIter &operator =(const typename TyContainer::Iter &other)
	{
		if((void *)this!=(void *)&other) {
			it = other.it;
		}
		return *this;
	}
public://运算符重载
	//解引用操作
	TyDataMy &operator *() const
	{
		return it->data;
	}
	TyDataMy *operator ->() const
	{
		return &operator *();
	}
	TyDataMy &operator [](difference_type offset) const
	{
		return *operator +(offset);
	}
	//递增递减操作
	_SequenceSkipListTemplateIter &operator ++()
	{
		++ it;
		return *this;
	}
	_SequenceSkipListTemplateIter operator ++(int)
	{
		TemplateIter ret(*this);
		operator ++();
		return ret;
	}
	_SequenceSkipListTemplateIter &operator --()
	{
		-- it;
		return *this;
	}
	_SequenceSkipListTemplateIter operator --(int)
	{
		_SequenceSkipListTemplateIter ret(*this);
		operator --();
		return ret;
	}
	//随机增减操作
	_SequenceSkipListTemplateIter &operator +=(difference_type offset)
	{
		//当数字小于等于2^defaultPrPow时直接递增
		if(std::abs(offset)<=(1U<<TyContainer::msc_defaultPrbPow))
			return ForIncrease(offset);
		size_t stk = 0;//记录向上的层数
		//初始向前向上走
		-- it;
		++ offset;
		auto itIdx = it->itPr;//索引迭代器
		offset += TyContainer::ListIterDistance(itIdx->itChVal, it);
		//向上走循环
		while(true) {
			++ stk;
			//若可以则向前走一步
			if(offset<0) {
				-- itIdx;
				offset += (difference_type)itIdx->size;
			}
			//若可以则向后走一步
			else if(offset>=(difference_type)itIdx->size) {
				offset -= (difference_type)itIdx->size;
				++ itIdx;
			}
			//若没有上层索引了，或者当前索引包含了目标，则退出
			if(_StruAndAlgoIteratorEQ(itIdx->itPr, itIdx)
				|| (0<=offset && offset<(difference_type)itIdx->size))
				break;
			//向上走
			offset += TyContainer::ListIterDistance(itIdx->itPr->itChIdx, itIdx);
			itIdx = itIdx->itPr;
		}
		//向下查找
		it = TyContainer::DownFindIndexIter(itIdx, offset, stk);
		return *this;
	}
	_SequenceSkipListTemplateIter operator +(difference_type offset) const
	{
		_SequenceSkipListTemplateIter ret(*this);
		ret.operator +=(offset);
		return ret;
	}
	_SequenceSkipListTemplateIter &operator -=(difference_type offset)
	{
		return operator +=(-offset);
	}
	_SequenceSkipListTemplateIter operator -(difference_type offset) const
	{
		_SequenceSkipListTemplateIter ret(*this);
		ret.operator -=(offset);
		return ret;
	}
	//相减操作
	template<typename IterStruOther, typename TyDataOther>
	difference_type operator -(const _SequenceSkipListTemplateIter<
		IterStruOther, TyDataOther, TyContainer> &other) const
	{
		if(it==other.it)
			return 0;
		//初始向左向上走
		auto itVal = it;
		auto itValOth = other.it;
		-- itVal, -- itValOth;
		auto itIdx= itVal->itPr, itIdxOth= itValOth->itPr;
		auto off= TyContainer::ListIterDistance(itIdx->itChVal, it),
			offOth= TyContainer::ListIterDistance(itIdxOth->itChVal, other.it);//分别记录偏移量
		//向上走循环
		while(itIdx!=itIdxOth) {
			off += TyContainer::ListIterDistance(itIdx->itPr->itChIdx, itIdx);
			offOth += TyContainer::ListIterDistance(itIdxOth->itPr->itChIdx, itIdxOth);
			itIdx = itIdx->itPr;
			itIdxOth = itIdxOth->itPr;
		}
		return off-offOth;
	}
	//比较操作
	template<typename IterStruOther, typename TyDataOther>
	bool operator ==(const _SequenceSkipListTemplateIter<
		IterStruOther, TyDataOther, TyContainer> &other) const
	{
		return it==other.it;
	}
	template<typename IterStruOther, typename TyDataOther>
	bool operator <(const _SequenceSkipListTemplateIter<
		IterStruOther, TyDataOther, TyContainer> &other) const
	{
		return operator -(other)<0;
	}
	template<typename IterStruOther, typename TyDataOther>
	bool operator !=(const _SequenceSkipListTemplateIter<
		IterStruOther, TyDataOther, TyContainer> &other) const
	{
		return !operator ==(other);
	}
	template<typename IterStruOther, typename TyDataOther>
	bool operator <=(const _SequenceSkipListTemplateIter<
		IterStruOther, TyDataOther, TyContainer> &other) const
	{
		return operator ==(other) || operator <(other);
	}
	template<typename IterStruOther, typename TyDataOther>
	bool operator >(const _SequenceSkipListTemplateIter<
		IterStruOther, TyDataOther, TyContainer> &other) const
	{
		return !(operator ==(other) || operator <(other));
	}
	template<typename IterStruOther, typename TyDataOther>
	bool operator >=(const _SequenceSkipListTemplateIter<
		IterStruOther, TyDataOther, TyContainer> &other) const
	{
		return !operator <(other);
	}
public://成员函数
	//循环递增减操作，线性复杂度，操作数小时比较快
	_SequenceSkipListTemplateIter &ForIncrease(difference_type offset)
	{
		for(; offset>0; --offset)
			operator ++();
		for(; offset<0; ++offset)
			operator --();
		return *this;
	}
};
//与数字相加操作
template<typename IterStruMy, typename TyDataMy, typename TyContainer>
inline _SequenceSkipListTemplateIter<IterStruMy, TyDataMy, TyContainer> operator +(
	_StruAndAlgoDifferenceType offset,
	const _SequenceSkipListTemplateIter<IterStruMy, TyDataMy, TyContainer> &it)
{
	return it.operator +(offset);
}

//二进制文件读写顺序跳表操作
template<typename TyData, typename TyRan>
BinWriteFile &operator <<(BinWriteFile &bwf, const SequenceSkipList<TyData, TyRan> &skl)
{
	//写尺寸
	int64_t size = skl.Size();
	bwf <<size;
	//写数据
	for(auto &r : skl)
		bwf <<r;
	return bwf;
}
template<typename TyData, typename TyRan>
BinReadFile &operator >>(BinReadFile &brf, SequenceSkipList<TyData, TyRan> &skl)
{
	//读尺寸
	int64_t size = -1;
	brf >>size;
	if(brf.IsFail())
		return brf;
	skl.Clear();
	//读数据
	for(int64_t i=0; i<size; ++i) {
		if(brf.IsFail())
			return brf;
		skl.SimpleEmplace(skl.End());
		brf >>skl.Back();
	}
	skl.Restructure(false);
	return brf;
}



template<typename IterStruMy, typename ValueIterStruMy,
	typename TyDataMy, typename TyContainer
> class _SequenceMultiSetTemplateIter;
template<typename IterStruMy, typename ValueIterStruMy,
	typename TyDataMy, typename TyContainer
> class _SequenceMultiSetTemplateValueIter;
//有序的多值集合，支持顺序访问和值序访问
//几乎所有操作都是对数复杂度，对数复杂度并不稳定，根据跳表而定
//删除对应元素会使对应的两种迭代器失效，更改元素的值会使对应值序迭代器失效
template<typename TyData, typename TyRan= std::mt19937, typename TyComp= std::less<TyData>>
class SequenceMultiSet
{
	/*
	bst <-> skl
	bst与skl双相连接
	bst为二叉树，key是数据，value是skl迭代器
	skl是顺序跳表，装有bst迭代器
	在各种操作时都要维护互相索引的结构
	*/

private://数据类型定义
	struct ListStru;
	struct MultimapStru;
	//SkipList迭代器
	using ListIter = typename SequenceSkipList<ListStru, TyRan>::Iter;
	using ListConstIter = typename SequenceSkipList<ListStru, TyRan>::ConstIter;
	//multimap迭代器
	using MultimapIter = typename std::multimap<TyData, MultimapStru, TyComp>::iterator;
	using MultimapConstIter = typename std::multimap<TyData, MultimapStru, TyComp>::const_iterator;
	//SkipList存放数据结构体
	struct ListStru
	{
		MultimapIter itBst;//树的迭代器
	};
	//multimap存放数据结构体
	struct MultimapStru
	{
		ListIter itSkl;//跳表的迭代器
		MultimapStru()
			= default;
		MultimapStru(ListIter o_itSkl):
			itSkl(o_itSkl)
		{
		}
	};

	//顺序迭代器结构封装，用在迭代器模板继承
	struct IterStru
	{
		ListIter it;
	public:
		IterStru()
			= default;
	protected:
		IterStru(ListIter o_it):
			it(o_it)
		{
		}
	};
	struct ConstIterStru
	{
		ListConstIter it;
	public:
		ConstIterStru()
			= default;
	protected:
		ConstIterStru(ListConstIter o_it):
			it(o_it)
		{
		}
	};

	//值序迭代器结构封装，用在迭代器模板继承
	struct ValueIterStru
	{
		MultimapIter it;
	public:
		ValueIterStru()
			= default;
	protected:
		ValueIterStru(MultimapIter o_it):
			it(o_it)
		{
		}
	};
	struct ValueConstIterStru
	{
		MultimapConstIter it;
	public:
		ValueConstIterStru()
			= default;
	protected:
		ValueConstIterStru(MultimapConstIter o_it):
			it(o_it)
		{
		}
	};

public://辅助类型
	using DataType = TyData;
	using RanType = TyRan;
	using CompType = TyComp;
private:
	using MyClassType = SequenceMultiSet<TyData, TyRan, TyComp>;//自身类型

public://迭代器类型指定
	//顺序迭代器
	using Iter = _SequenceMultiSetTemplateIter<
		IterStru, ValueIterStru, const TyData, MyClassType>;
	//顺序常迭代器
	using ConstIter = _SequenceMultiSetTemplateIter<
		ConstIterStru, ValueConstIterStru, const TyData, MyClassType>;
	//值序访问的迭代器
	using ValueIter = _SequenceMultiSetTemplateValueIter<
		IterStru, ValueIterStru, const TyData, MyClassType>;
	//值序访问的迭代器
	using ValueConstIter = _SequenceMultiSetTemplateValueIter<
		ConstIterStru, ValueConstIterStru, const TyData, MyClassType>;
	//顺序反向迭代器
	using RevIter = std::reverse_iterator<Iter>;
	using ConstRevIter = std::reverse_iterator<ConstIter>;
	//值序反向迭代器
	using ValueRevIter = std::reverse_iterator<ValueIter>;
	using ValueConstRevIter = std::reverse_iterator<ValueConstIter>;

public://友元
	template<typename IterStruMy, typename ValueIterStruMy,
		typename TyDataMy, typename TyContainer
	> friend class _SequenceMultiSetTemplateIter;
	template<typename IterStruMy, typename ValueIterStruMy,
		typename TyDataMy, typename TyContainer
	> friend class _SequenceMultiSetTemplateValueIter;

private://成员数据
	SequenceSkipList<ListStru, TyRan> m_skl;//跳表结构，存放树结构迭代器
	std::multimap<TyData, MultimapStru, TyComp> m_bst;//树结构，存放值和跳表迭代器

public://辅助静态类型
	static constexpr int msc_defaultPrbPow =
		SequenceSkipList<ListStru, TyRan>::msc_defaultPrbPow;//跳表随机生长的概率默认值

public://基本函数
	//默认构造
	explicit SequenceMultiSet(const TyRan &o_ran= TyRan(), unsigned o_prbPow= msc_defaultPrbPow,
		const TyComp &func= TyComp()
	): m_skl(o_ran, o_prbPow), m_bst(func)
	{
	}
	//拷贝函数
	SequenceMultiSet(const SequenceMultiSet &other)
	{
		operator =(other);
	}
	SequenceMultiSet &operator =(const SequenceMultiSet &other)
	{
		if(this!=&other) {
			_StruAndAlgoCallDestruct(m_bst);
			new(&m_bst) std::multimap<TyData, MultimapStru, TyComp>(other.m_bst.key_comp());
			Assign(other.Begin(), other.End());
		}
		return *this;
	}
	//移动函数
	SequenceMultiSet(SequenceMultiSet &&other) noexcept
		= default;
	SequenceMultiSet &operator =(SequenceMultiSet &&other) noexcept
		= default;
	//赋值构造函数，同时决定是否使用准随机
	SequenceMultiSet(std::initializer_list<TyData> init, bool bQuasiRandom= false,
		const TyRan &o_ran= TyRan(), unsigned o_prbPow= msc_defaultPrbPow,
		const TyComp &func= TyComp()
	): SequenceMultiSet(o_ran, o_prbPow, func)
	{
		Assign(init, bQuasiRandom);
	}
	explicit SequenceMultiSet(size_t size):
		SequenceMultiSet()
	{
		Assign(size);
	}
	explicit SequenceMultiSet(size_t size, const TyData &value, bool bQuasiRandom= false,
		const TyRan &o_ran= TyRan(), unsigned o_prbPow= msc_defaultPrbPow,
		const TyComp &func= TyComp()
	): SequenceMultiSet(o_ran, o_prbPow, func)
	{
		Assign(size, value, bQuasiRandom);
	}
	template<typename TyIt, typename= typename std::enable_if<
		_StruAndAlgoIsIterator<TyIt>::value>::type>
	explicit SequenceMultiSet(TyIt itSt, TyIt itEd, bool bQuasiRandom= false,
		const TyRan &o_ran= TyRan(), unsigned o_prbPow= msc_defaultPrbPow,
		const TyComp &func= TyComp()
	): SequenceMultiSet(o_ran, o_prbPow, func)
	{
		Assign(itSt, itEd, bQuasiRandom);
	}
public://运算符重载
	//列表赋值
	SequenceMultiSet &operator =(std::initializer_list<TyData> init)
	{
		Assign(init);
		return *this;
	}
	//比较操作，按顺序的字典序
	template<typename TyDataOther, typename TyRanOther, typename TyCompOther>
	bool operator <(const SequenceMultiSet<TyDataOther, TyRanOther, TyCompOther> &other) const
	{
		return Compare(other)<0;
	}
	template<typename TyDataOther, typename TyRanOther, typename TyCompOther>
	bool operator ==(const SequenceMultiSet<TyDataOther, TyRanOther, TyCompOther> &other) const
	{
		return Compare(other)==0;
	}
	template<typename TyDataOther, typename TyRanOther, typename TyCompOther>
	bool operator !=(const SequenceMultiSet<TyDataOther, TyRanOther, TyCompOther> &other) const
	{
		return Compare(other)!=0;
	}
	template<typename TyDataOther, typename TyRanOther, typename TyCompOther>
	bool operator <=(const SequenceMultiSet<TyDataOther, TyRanOther, TyCompOther> &other) const
	{
		return Compare(other)<=0;
	}
	template<typename TyDataOther, typename TyRanOther, typename TyCompOther>
	bool operator >(const SequenceMultiSet<TyDataOther, TyRanOther, TyCompOther> &other) const
	{
		return Compare(other)>0;
	}
	template<typename TyDataOther, typename TyRanOther, typename TyCompOther>
	bool operator >=(const SequenceMultiSet<TyDataOther, TyRanOther, TyCompOther> &other) const
	{
		return Compare(other)>=0;
	}
public://成员函数
	//赋值函数，一同决定是否使用准随机化
	SequenceMultiSet &Assign(std::initializer_list<TyData> init, bool bQuasiRandom= false)
	{
		Assign(init.begin(), init.end(), bQuasiRandom);
		return *this;
	}
	SequenceMultiSet &Assign(size_t size)
	{
		Clear();
		for(int i=0; i!=size; ++i)
			SimpleEmplace(End());
		Restructure(false);
		return *this;
	}
	SequenceMultiSet &Assign(size_t size, const TyData &value, bool bQuasiRandom= false)
	{
		Clear();
		for(int i=0; i!=size; ++i)
			SimpleEmplace(End(), value);
		Restructure(bQuasiRandom);
		return *this;
	}
	template<typename TyIt>
	typename std::enable_if<_StruAndAlgoIsIterator<TyIt>::value, SequenceMultiSet &
	>::type Assign(TyIt itSt, TyIt itEd, bool bQuasiRandom= false)
	{
		Clear();
		for(; itSt!=itEd; ++itSt)
			SimpleEmplace(End(), *itSt);
		Restructure(bQuasiRandom);
		return *this;
	}
	//比较操作，小于等于大于分别返回-1,0,1
	template<typename TyDataOther, typename TyRanOther, typename TyCompOther,
		typename TyFuncLess= _StruAndAlgoGeneralLess<TyData, TyDataOther>,
		typename TyFuncEqual= _StruAndAlgoGeneralEqualTo<TyData, TyDataOther>
	> int Compare(const SequenceMultiSet<TyDataOther, TyRanOther, TyCompOther> &other,
		TyFuncLess funcLess= TyFuncLess(), TyFuncEqual funcEqual= TyFuncEqual()) const
	{
		return _StruAndAlgoSequenceCompare(Begin(), End(),
			other.Begin(), other.End(), funcLess, funcEqual);
	}
	//顺序产生迭代器
	Iter Begin()
	{
		return Iter(m_skl.Begin());
	}
	ConstIter Begin() const
	{
		return ConstIter(m_skl.Begin());
	}
	Iter begin()
	{
		return Begin();
	}
	ConstIter begin() const
	{
		return Begin();
	}
	ConstIter ConstBegin() const
	{
		return Begin();
	}
	Iter End()
	{
		return Iter(m_skl.End());
	}
	ConstIter End() const
	{
		return ConstIter(m_skl.End());
	}
	Iter end()
	{
		return End();
	}
	ConstIter end() const
	{
		return End();
	}
	ConstIter ConstEnd() const
	{
		return End();
	}
	//顺序产生反向迭代器
	RevIter RevBegin()
	{
		return RevIter(End());
	}
	ConstRevIter RevBegin() const
	{
		return ConstRevIter(End());
	}
	ConstRevIter ConstRevBegin() const
	{
		return RevBegin();
	}
	RevIter RevEnd()
	{
		return RevIter(Begin());
	}
	ConstRevIter RevEnd() const
	{
		return ConstRevIter(Begin());
	}
	ConstRevIter ConstRevEnd() const
	{
		return RevEnd();
	}
	//值序产生迭代器
	ValueIter ValueBegin()
	{
		return ValueIter(m_bst.begin());
	}
	ValueConstIter ValueBegin() const
	{
		return ValueConstIter(m_bst.begin());
	}
	ValueConstIter ValueConstBegin() const
	{
		return ValueBegin();
	}
	ValueIter ValueEnd()
	{
		return ValueIter(m_bst.end());
	}
	ValueConstIter ValueEnd() const
	{
		return ValueConstIter(m_bst.end());
	}
	ValueConstIter ValueConstEnd() const
	{
		return ValueEnd();
	}
	//值序产生反向迭代器
	ValueRevIter ValueRevBegin()
	{
		return ValueRevIter(ValueEnd());
	}
	ValueConstRevIter ValueRevBegin() const
	{
		return ValueConstRevIter(ValueEnd());
	}
	ValueConstRevIter ValueConstRevBegin() const
	{
		return ValueRevBegin();
	}
	ValueRevIter ValueRevEnd()
	{
		return ValueRevIter(ValueBegin());
	}
	ValueConstRevIter ValueRevEnd() const
	{
		return ValueConstRevIter(ValueBegin());
	}
	ValueConstRevIter ValueConstRevEnd() const
	{
		return ValueRevEnd();
	}
	//产生下标迭代器
	Iter GetIter(size_t idx)
	{
		return Iter(m_skl.GetIter(idx));
	}
	ConstIter GetIter(size_t idx) const
	{
		return ConstIter(m_skl.GetIter(idx));
	}
	ConstIter GetConstIter(size_t idx) const
	{
		return GetIter(idx);
	}
	//获取迭代器下标
	size_t GetIndex(ConstIter it) const
	{
		return it-Begin();
	}
	//访问顺序首尾元素
	const TyData &Front() const
	{
		return *Begin();
	}
	const TyData &Back() const
	{
		return *(--End());
	}
	//访问值序首尾元素
	const TyData &ValueFront() const
	{
		return *ValueBegin();
	}
	const TyData &ValueBack() const
	{
		return *(--ValueEnd());
	}
	//下标访问元素
	const TyData &operator[](size_t idx) const
	{
		return *GetIter(idx);
	}
	const TyData &At(size_t idx) const
	{
		if(idx>=Size())
			throw std::out_of_range("");
		return operator[](idx);
	}
	//查看容量
	bool Empty() const
	{
		return m_skl.Empty();
	}
	size_t Size() const
	{
		return m_skl.Size();
	}
	//清空
	void Clear()
	{
		m_skl.Clear();
		m_bst.clear();
	}
	//改变容量
	template<typename ...Ty_S>
	typename std::enable_if<_StruAndAlgoIsNoneOrSame<TyData, Ty_S...>::value, void
	>::type Resize(size_t size, const Ty_S &...arg_s)
	{
		while(Size()>size)
			Erase(--End());
		while(Size()<size)
			Emplace(End(), arg_s...);
	}
	//两端插入
	template<typename Ty>
	typename std::enable_if<_StruAndAlgoIsRemoveCVRefSame<TyData, Ty>::value, void
	>::type PushBack(Ty &&value)
	{
		Emplace(End(), std::forward<Ty>(value));
	}
	template<typename ...Ty_S>
	void EmplaceBack(Ty_S &&...arg_s)
	{
		Emplace(End(), std::forward<Ty_S>(arg_s)...);
	}
	template<typename Ty>
	typename std::enable_if<_StruAndAlgoIsRemoveCVRefSame<TyData, Ty>::value, void
	>::type PushFront(Ty &&value)
	{
		Emplace(Begin(), std::forward<Ty>(value));
	}
	template<typename ...Ty_S>
	void EmplaceFront(Ty_S &&...arg_s)
	{
		Emplace(Begin(), std::forward<Ty_S>(arg_s)...);
	}
	//在指定位置前插入，返回第一个元素的迭代器
	template<typename Ty>
	typename std::enable_if<_StruAndAlgoIsRemoveCVRefSame<TyData, Ty>::value, Iter
	>::type Insert(Iter itPos, Ty &&value)
	{
		return Emplace(itPos, std::forward<Ty>(value));
	}
	Iter Insert(Iter itPos, std::initializer_list<TyData> init)
	{
		return Insert(itPos, init.begin(), init.end());
	}
	template<typename TyIt>
	Iter Insert(Iter itPos, TyIt itSt, TyIt itEd)
	{
		auto ret = itPos;
		bool bIns = false;
		for(; itSt!=itEd; ++itSt) {
			if(!bIns)
				ret = Emplace(itPos, *itSt);
			else
				Emplace(itPos, *itSt);
			bIns = true;
		}
		return ret;
	}
	template<typename ...Ty_S>
	Iter Emplace(Iter itPos, Ty_S &&...arg_s)
	{
		auto itSkl = m_skl.Emplace(itPos.it);
		auto itBst = m_bst.emplace(std::piecewise_construct,
			std::forward_as_tuple(std::forward<Ty_S>(arg_s)...), std::make_tuple(itSkl));
		itSkl->itBst = itBst;
		return Iter(itSkl);
	}
	//使用下标插入，即插入的首个元素下标为指定下标，返回插入首个位置的迭代器
	template<typename ...Ty_S>
	Iter IndexInsert(size_t idx, Ty_S &&...arg_s)
	{
		return Insert(GetIter(idx), std::forward<Ty_S>(arg_s));
	}
	template<typename ...Ty_S>
	Iter IndexEmplace(size_t idx, Ty_S &&...arg_s)
	{
		return Emplace(GetIter(idx), std::forward<Ty_S>(arg_s));
	}
	//跳表简单插入接口，不创建索引节点
	template<typename ...Ty_S>
	Iter SimpleEmplace(Iter itPos, Ty_S &&...arg_s)
	{
		auto itSkl = m_skl.SimpleEmplace(itPos.it);
		auto itBst = m_bst.emplace(std::piecewise_construct,
			std::forward_as_tuple(std::forward<Ty_S>(arg_s)...), std::make_tuple(itSkl));
		itSkl->itBst = itBst;
		return Iter(itSkl);
	}
	//带提示的插入元素
	template<typename ...Ty_S>
	Iter EmplaceHint(Iter itPos, ValueIter itHint, Ty_S &&...arg_s)
	{
		auto itSkl = m_skl.Emplace(itPos.it);
		auto itBst = m_bst.emplace_hint(itHint.it, std::piecewise_construct,
			std::forward_as_tuple(std::forward<Ty_S>(arg_s)...), std::make_tuple(itSkl));
		itSkl->itBst = itBst;
		return Iter(itSkl);
	}
	template<typename ...Ty_S>
	Iter SimpleEmplaceHint(Iter itPos, ValueIter itHint, Ty_S &&...arg_s)
	{
		auto itSkl = m_skl.SimpleEmplace(itPos.it);
		auto itBst = m_bst.emplace_hint(itHint.it, std::piecewise_construct,
			std::forward_as_tuple(std::forward<Ty_S>(arg_s)...), std::make_tuple(itSkl));
		itSkl->itBst = itBst;
		return Iter(itSkl);
	}
	//两端顺序删除
	void PopBack()
	{
		Erase(--End());
	}
	void PopFront()
	{
		Erase(Begin());
	}
	//指定位置顺序删除，返回删除后一个位置
	Iter Erase(Iter itPos)
	{
		//删除skl和bst
		m_bst.erase(itPos.it->itBst);
		return Iter(m_skl.Erase(itPos.it));
	}
	Iter Erase(Iter itSt, Iter itEd)
	{
		while(itSt!=itEd)
			itSt = Erase(itSt);
		return itEd;
	}
	//使用下标顺序删除，返回删除后一个位置的迭代器
	Iter IndexErase(size_t idx)
	{
		return Erase(GetIter(idx));
	}
	Iter IndexErase(size_t idxSt, size_t idxEd)
	{
		assert(idxSt<=idxEd);
		auto it = GetIter(idxSt);
		for(; idxSt<idxEd; ++idxSt)
			it = Erase(it);
		return it;
	}
	//指定位置值序删除，返回删除后一个位置
	ValueIter ValueErase(ValueIter itPos)
	{
		return ValueErase(itPos, itPos.ForIncrease(1));
	}
	ValueIter ValueErase(ValueIter itSt, ValueIter itEd)
	{
		//删除对应skl
		for(auto it=itSt.it; it!=itEd.it; ++it) {
			m_skl.Erase(it->second.itSkl);
		}
		//删除bst
		return ValueIter(m_bst.erase(itSt.it, itEd.it));
	}
	//查找删除，返回删除个数
	size_t FindErase(const TyData &value)
	{
		auto prRes = m_bst.equal_range(value);
		auto ret = std::distance(prRes.first, prRes.second);
		ValueErase(ValueIter(prRes.first), ValueIter(prRes.second));
		return ret;
	}
	//查找
	ValueIter Find(const TyData &value)
	{
		return ValueIter(m_bst.find(value));
	}
	ValueConstIter Find(const TyData &value) const
	{
		return ValueConstIter(m_bst.find(value));
	}
	size_t Count(const TyData &value) const
	{
		return m_bst.count(value);
	}
	//范围查找
	std::pair<ValueIter, ValueIter> EqualRange(const TyData &value)
	{
		return std::pair<ValueIter, ValueIter>(m_bst.equal_range(value));
	}
	std::pair<ValueConstIter, ValueConstIter> EqualRange(const TyData &value) const
	{
		return std::pair<ValueConstIter, ValueConstIter>(m_bst.equal_range(value));
	}
	ValueIter LowerBound(const TyData &value)
	{
		return ValueIter(m_bst.lower_bound(value));
	}
	ValueConstIter LowerBound(const TyData &value) const
	{
		return ValueConstIter(m_bst.lower_bound(value));
	}
	ValueIter UpperBound(const TyData &value)
	{
		return ValueIter(m_bst.upper_bound(value));
	}
	ValueConstIter UpperBound(const TyData &value) const
	{
		return ValueConstIter(m_bst.upper_bound(value));
	}
	//修改值，返回修改位置的值迭代器
	template<typename Ty>
	typename std::enable_if<_StruAndAlgoIsRemoveCVRefSame<TyData, Ty>::value, ValueIter
	>::type Modify(Iter itPos, Ty &&value)
	{
		return EmplaceModify(itPos, std::forward<Ty>(value));
	}
	template<typename ...Ty_S>
	ValueIter EmplaceModify(Iter itPos, Ty_S &&...arg_s)
	{
		m_bst.erase(itPos.it->itBst);
		itPos.it->itBst = m_bst.emplace(std::piecewise_construct,
			std::forward_as_tuple(std::forward<Ty_S>(arg_s)...), std::make_tuple(itPos.it));
		return ValueIter(itPos.it->itBst);
	}
	//使用下标修改值
	template<typename ...Ty_S>
	ValueIter IndexModify(size_t idx, Ty_S &&...arg_s)
	{
		return Modify(GetIter(idx), std::forward<Ty_S>(arg_s)...);
	}
	template<typename ...Ty_S>
	ValueIter IndexEmplaceModify(size_t idx, Ty_S &&...arg_s)
	{
		return EmplaceModify(GetIter(idx), std::forward<Ty_S>(arg_s)...);
	}
	//查找修改值
	template<typename Ty>
	typename std::enable_if<_StruAndAlgoIsRemoveCVRefSame<TyData, Ty>::value, size_t
	>::type FindModify(const TyData &valueSrc, Ty &&valueDst)
	{
		auto prRes = m_bst.equal_range(valueSrc);
		auto size = std::distance(prRes.first, prRes.second);
		if(size>0) {
			//记录查找结果
			std::vector<ListIter> vecIt(size);
			int i = 0;
			for(auto it=prRes.first; it!=prRes.second; ++it, ++i)
				vecIt[i] = it->second.itSkl;
			//删除bst
			m_bst.erase(prRes.first, prRes.second);
			//重建bst
			for(i=0; i+1!=size; ++i)
				vecIt[i]->itBst = m_bst.emplace((const Ty &)(valueDst), vecIt[i]);
			vecIt[i]->itBst = m_bst.emplace(std::forward<Ty>(valueDst), vecIt[i]);
		}
		return size;
	}
	//交换元素，交换后迭代器指向元素值不变，位置交换
	void SwapElement(Iter it1, Iter it2)
	{
		m_skl(it1.it, it2.it);
	}
	void SwapElement(Iter st1, Iter ed1, Iter st2)
	{
		m_skl(st1.it, ed1.it, st2.it);
	}
	//序列反转
	void Reverse(bool bQuasiRandom= false)
	{
		m_skl.Reverse(bQuasiRandom);
	}
	//排序操作，默认不使用模板提供的比较类
	template<typename TyFunc= std::less<TyData>>
	void Sort(bool bQuasiRandom= false, const TyFunc &func= std::less<TyData>())
	{
		auto lbdCom = [&](const ListStru &arg1, const ListStru &arg2) {
			return func(arg1.itBst->first, arg2.itBst->first);
		};
		m_skl.Sort(bQuasiRandom, lbdCom);
	}
	//结构重组，线性复杂度，准随机化将结构变为平衡二叉树，否则则随机生成
	//需保证至少有一层索引和一个索引节点，无须保证索引之间和值之间互联
	void Restructure(bool bQuasiRandom)
	{
		m_skl.Restructure(bQuasiRandom);
	}
	//随机器接口
	const TyRan &GetRandomEngin() const
	{
		return m_skl.GetRandomEngin();
	}
	TyRan &GetRandomEngin()
	{
		return m_skl.GetRandomEngin();
	}
	//获得比较函数
	TyComp GetCompare() const
	{
		return m_bst.key_comp();
	}
	//Debug
	void Debug() const
	{
		m_skl.Debug();//测试跳表
		assert(m_skl.Size()==m_bst.size());//大小相同
		for(auto it=m_skl.Begin(); it!=m_skl.End(); ++it) {
			assert(it->itBst->second.itSkl==it);//测试互联情况
		}
	}
};

//带额外顺序的集合，的顺序迭代器
//遍历时根据顺序进行遍历
//是随机访问迭代器，同跳表迭代器
//尾后迭代器失效不能解引用，不能转化值序迭代器
template<typename IterStruMy, typename ValueIterStruMy,
	typename TyDataMy, typename TyContainer
> class _SequenceMultiSetTemplateIter:
	private IterStruMy,
	public std::iterator<std::random_access_iterator_tag, TyDataMy,
	_StruAndAlgoDifferenceType, TyDataMy *, TyDataMy &>
{
public://友元
	template<typename TyData, typename TyRan, typename TyComp>
	friend class SequenceMultiSet;
	template<typename IterStruOther, typename ValueIterStruOther,
		typename TyDataOther, typename TyContainerOther
	> friend class _SequenceMultiSetTemplateIter;
	template<typename IterStruOther, typename ValueIterStruOther,
		typename TyDataOther, typename TyContainerOther
	> friend class _SequenceMultiSetTemplateValueIter;

private://辅助类
	//对应的值序迭代器
	using ValueIterMy = _SequenceMultiSetTemplateValueIter<
		IterStruMy, ValueIterStruMy, TyDataMy, TyContainer>;

private://数据
	//继承自IterStruMy，it为迭代器

public://基本函数
	//默认构造
	_SequenceMultiSetTemplateIter()
		= default;
	//继承基类数据构造
	using IterStruMy::IterStruMy;
	//由非常类构造常类
	_SequenceMultiSetTemplateIter(const typename TyContainer::Iter &other)
	{
		operator =(other);
	}
	_SequenceMultiSetTemplateIter &operator =(const typename TyContainer::Iter &other)
	{
		if((void *)this!=(void *)&other) {
			it = other.it;
		}
		return *this;
	}
	//从值序迭代器构造，不能为尾后迭代器
	explicit _SequenceMultiSetTemplateIter(const ValueIterMy &other)
	{
		it = other.it->second.itSkl;
	}
public://运算符重载
	//解引用操作
	TyDataMy &operator *() const
	{
		return it->itBst->first;
	}
	TyDataMy *operator ->() const
	{
		return &operator *();
	}
	TyDataMy &operator [](difference_type offset) const
	{
		return *operator +(offset);
	}
	//递增递减操作
	_SequenceMultiSetTemplateIter &operator ++()
	{
		++ it;
		return *this;
	}
	_SequenceMultiSetTemplateIter operator ++(int)
	{
		_SequenceMultiSetTemplateIter ret(*this);
		operator ++();
		return ret;
	}
	_SequenceMultiSetTemplateIter &operator --()
	{
		-- it;
		return *this;
	}
	_SequenceMultiSetTemplateIter operator --(int)
	{
		_SequenceMultiSetTemplateIter ret(*this);
		operator --();
		return ret;
	}
	//随机增减操作
	_SequenceMultiSetTemplateIter &operator +=(difference_type offset)
	{
		it += offset;
		return *this;
	}
	_SequenceMultiSetTemplateIter operator +(difference_type offset) const
	{
		_SequenceMultiSetTemplateIter ret(*this);
		ret.operator +=(offset);
		return ret;
	}
	_SequenceMultiSetTemplateIter &operator -=(difference_type offset)
	{
		it -= offset;
		return *this;
	}
	_SequenceMultiSetTemplateIter operator -(difference_type offset) const
	{
		_SequenceMultiSetTemplateIter ret(*this);
		ret.operator -=(offset);
		return ret;
	}
	//相减操作
	template<typename IterStruOther, typename ValueIterStruOther, typename TyDataOther>
	difference_type operator -(const _SequenceMultiSetTemplateIter<
		IterStruOther, ValueIterStruOther, TyDataOther, TyContainer> &other) const
	{
		return it-other.it;
	}
	//比较操作
	template<typename IterStruOther, typename ValueIterStruOther, typename TyDataOther>
	bool operator ==(const _SequenceMultiSetTemplateIter<
		IterStruOther, ValueIterStruOther, TyDataOther, TyContainer> &other) const
	{
		return it==other.it;
	}
	template<typename IterStruOther, typename ValueIterStruOther, typename TyDataOther>
	bool operator <(const _SequenceMultiSetTemplateIter<
		IterStruOther, ValueIterStruOther, TyDataOther, TyContainer> &other) const
	{
		return it<other.it;
	}
	template<typename IterStruOther, typename ValueIterStruOther, typename TyDataOther>
	bool operator !=(const _SequenceMultiSetTemplateIter<
		IterStruOther, ValueIterStruOther, TyDataOther, TyContainer> &other) const
	{
		return !operator ==(other);
	}
	template<typename IterStruOther, typename ValueIterStruOther, typename TyDataOther>
	bool operator <=(const _SequenceMultiSetTemplateIter<
		IterStruOther, ValueIterStruOther, TyDataOther, TyContainer> &other) const
	{
		return operator ==(other) || operator <(other);
	}
	template<typename IterStruOther, typename ValueIterStruOther, typename TyDataOther>
	bool operator >(const _SequenceMultiSetTemplateIter<
		IterStruOther, ValueIterStruOther, TyDataOther, TyContainer> &other) const
	{
		return !(operator ==(other) || operator <(other));
	}
	template<typename IterStruOther, typename ValueIterStruOther, typename TyDataOther>
	bool operator >=(const _SequenceMultiSetTemplateIter<
		IterStruOther, ValueIterStruOther, TyDataOther, TyContainer> &other) const
	{
		return !operator <(other);
	}
public://成员函数
	//获取对应值序迭代器，不能在用尾后迭代器调用
	ValueIterMy GetValueIter() const
	{
		return ValueIterMy(it->itBst);
	}
	//循环递增减操作，线性复杂度，操作数小时比较快
	_SequenceMultiSetTemplateIter &ForIncrease(difference_type offset)
	{
		it.ForIncrease(offset);
		return *this;
	}
};
//与数字相加操作
template<typename IterStruMy, typename ValueIterStruMy,
	typename TyDataMy, typename TyContainer
> inline _SequenceMultiSetTemplateIter<
	IterStruMy, ValueIterStruMy, TyDataMy, TyContainer
> operator +(_StruAndAlgoDifferenceType offset,
	const _SequenceMultiSetTemplateIter<
	IterStruMy, ValueIterStruMy, TyDataMy, TyContainer> &it)
{
	return it.operator +(offset);
}

//带额外顺序的集合，的值序迭代器
//遍历时根据值序进行遍历
//双向迭代器，同二叉树迭代器
//尾后迭代器失效不能解引用，不能转化顺序迭代器
template<typename IterStruMy, typename ValueIterStruMy,
	typename TyDataMy, typename TyContainer
> class _SequenceMultiSetTemplateValueIter:
	private ValueIterStruMy,
	public std::iterator<std::bidirectional_iterator_tag, TyDataMy,
	_StruAndAlgoDifferenceType, TyDataMy *, TyDataMy &>
{
public://友元
	template<typename TyData, typename TyRan, typename TyComp>
	friend class SequenceMultiSet;
	template<typename IterStruOther, typename ValueIterStruOther,
		typename TyDataOther, typename TyContainerOther
	> friend class _SequenceMultiSetTemplateIter;
	template<typename IterStruOther, typename ValueIterStruOther,
		typename TyDataOther, typename TyContainerOther
	> friend class _SequenceMultiSetTemplateValueIter;

private://辅助类型
	//对应的顺序迭代器
	using IterMy = _SequenceMultiSetTemplateIter<
		IterStruMy, ValueIterStruMy, TyDataMy, TyContainer>;

private://数据
	//继承自ValueIterStruMy，it为迭代器

public://基本函数
	//默认构造
	_SequenceMultiSetTemplateValueIter()
		= default;
	//继承基类数据构造
	using ValueIterStruMy::ValueIterStruMy;
	//由非常类构造常类
	_SequenceMultiSetTemplateValueIter(const typename TyContainer::ValueIter &other)
	{
		operator =(other);
	}
	_SequenceMultiSetTemplateValueIter &operator =(const typename TyContainer::ValueIter &other)
	{
		if((void *)this!=(void *)&other) {
			it = other.it;
		}
		return *this;
	}
	//从顺序迭代器构造，不能为尾后迭代器
	explicit _SequenceMultiSetTemplateValueIter(const IterMy &other)
	{
		it = other.it->itBst;
	}
public://运算符重载
	//解引用操作
	TyDataMy &operator *() const
	{
		return it->first;
	}
	TyDataMy *operator ->() const
	{
		return &operator *();
	}
	//递增递减操作
	_SequenceMultiSetTemplateValueIter &operator ++()
	{
		++ it;
		return *this;
	}
	_SequenceMultiSetTemplateValueIter operator ++(int)
	{
		_SequenceMultiSetTemplateValueIter ret(*this);
		operator ++();
		return ret;
	}
	_SequenceMultiSetTemplateValueIter &operator --()
	{
		-- it;
		return *this;
	}
	_SequenceMultiSetTemplateValueIter operator --(int)
	{
		_SequenceMultiSetTemplateValueIter ret(*this);
		operator --();
		return ret;
	}
	//比较操作
	template<typename IterStruOther, typename ValueIterStruOther, typename TyDataOther>
	bool operator ==(const _SequenceMultiSetTemplateValueIter<
		IterStruOther, ValueIterStruOther, TyDataOther, TyContainer> &other) const
	{
		return it==other.it;
	}
	template<typename IterStruOther, typename ValueIterStruOther, typename TyDataOther>
	bool operator !=(const _SequenceMultiSetTemplateValueIter<
		IterStruOther, ValueIterStruOther, TyDataOther, TyContainer> &other) const
	{
		return !operator ==(other);
	}
public://成员函数
	//获取对应顺序迭代器，不能在用尾后迭代器调用
	IterMy GetIter() const
	{
		return IterMy(it->second.itSkl);
	}
};

//二进制文件读写有序多值集合操作操作
template<typename TyData, typename TyRan, typename TyComp>
BinWriteFile &operator <<(BinWriteFile &bwf,
	const SequenceMultiSet<TyData, TyRan, TyComp> &seq)
{
	//写尺寸
	int64_t size = seq.Size();
	bwf <<size;
	//写数据
	for(auto &r : seq)
		bwf <<r;
	return bwf;
}
template<typename TyData, typename TyRan, typename TyComp>
BinReadFile &operator >>(BinReadFile &brf, SequenceMultiSet<TyData, TyRan, TyComp> &seq)
{
	//读尺寸
	int64_t size = -1;
	brf >>size;
	if(brf.IsFail())
		return brf;
	seq.Clear();
	//读数据
	for(int64_t i=0; i<size; ++i) {
		if(brf.IsFail())
			return brf;
		TyData data;
		brf >>data;
		seq.SimpleEmplace(seq.End(), std::move(data));
	}
	seq.Restructure(false);
	return brf;
}



//只能添加元素的图，支持void数据类型
//内部为二维数组实现
//节点迭代器允许使用整形索引构造
//Clear和Assign会导致迭代器失效
//容器复制、移动、交换后迭代器均指向旧容器，允许迭代器更新容器指针
//TODO: 想结构设计，尝试将节点与边对应到整形索引上
template<typename TyNodeData, typename TyEdgeData>
class AddonlyGraph
{
private:
	//边结构体
	struct EdgeStruct:
		public _StruAndAlgoValueHold<TyEdgeData>
	{
		size_t index;//边编号
		//接收数据类型模板参数的构造
		template<typename ...Tys>
		EdgeStruct(size_t o_index, Tys &&...args):
			_StruAndAlgoValueHold<TyEdgeData>(std::forward<Tys>(args)...), index(o_index)
		{
		}
	};
	//节点结构体
	struct NodeStruct:
		public _StruAndAlgoValueHold<TyNodeData>
	{
		std::vector<EdgeStruct> vecEdge;//出边数组
		//接收左值或右值的数据构造
		template<typename ...Tys>
		NodeStruct(Tys &&...args):
			_StruAndAlgoValueHold<TyNodeData>(std::forward<Tys>(args)...)
		{
		}
	};

private://辅助类型定义
	using GraphType = AddonlyGraph<TyNodeData, TyEdgeData>;//图类型
public:
	using NodeDataType = TyNodeData;//节点数据类型
	using EdgeDataType = TyEdgeData;//边数据类型

private://迭代器模板定义
	//节点迭代器模板辅助类
	template<typename GraphTypeMy, typename TyNodeDataMy, typename TyNodeDataPure>
	class TemplateNodeIterAssist;
	//节点迭代器模板
	template<typename GraphTypeMy, typename TyNodeDataMy, typename TyEdgeDataMy>
	class TemplateNodeIter;
	//边迭代器模板辅助类
	template<typename GraphTypeMy, typename TyNodeDataMy, typename TyNodeDataPure>
	class TemplateEdgeIterAssist;
	//边迭代器模板
	template<typename GraphTypeMy, typename TyNodeDataMy, typename TyEdgeDataMy>
	class TemplateEdgeIter;

private://迭代器模板指定类型定义
	//节点迭代器辅助类指定
	using NodeIterAssist = TemplateNodeIter<GraphType, TyNodeData, TyNodeData>;
	//节点常迭代器辅助类指定
	using NodeConstIterAssist = TemplateNodeIter<
		const GraphType, const TyNodeData, TyNodeData>;
	//边迭代器辅助类指定
	using EdgeIterAssist = TemplateEdgeIter<GraphType, TyEdgeData, TyEdgeData>;
	//边常迭代器辅助类指定
	using EdgeConstIterAssist = TemplateEdgeIter<
		const GraphType, const TyEdgeData, TyEdgeData>;
public:
	//节点迭代器类指定
	using NodeIter = TemplateNodeIter<GraphType, TyNodeData, TyEdgeData>;
	//节点常迭代器类指定
	using NodeConstIter = TemplateNodeIter<
		const GraphType, const TyNodeData, const TyEdgeData>;
	//边迭代器类指定
	using EdgeIter = TemplateEdgeIter<GraphType, TyNodeData, TyEdgeData>;
	//边常迭代器类指定
	using EdgeConstIter = TemplateEdgeIter<
		const GraphType, const TyNodeData, const TyEdgeData>;

public://友元声明
	friend class NodeIter;
	friend class NodeIterAssist;
	friend class NodeConstIter;
	friend class NodeConstIterAssist;
	friend class EdgeIter;
	friend class EdgeIterAssist;
	friend class EdgeConstIter;
	friend class EdgeConstIterAssist;

private://成员数据
	std::vector<NodeStruct> vecNode;//节点数组

public://基本函数
	//默认构造
	AddonlyGraph()
		= default;
	//多个数据构造
	template<typename ...Ty_S>
	explicit AddonlyGraph(size_t size, const Ty_S &...arg_s)
	{
		Assign(size, arg_s...);
	}
public://成员函数
	//赋值函数
	template<typename ...Ty_S>
	AddonlyGraph &Assign(size_t size, const Ty_S &...arg_s)
	{
		vecNode.assign(size, NodeStruct(arg_s...));
		return *this;
	}
	//添加节点
	template<typename TyNodeDataMy>
	typename std::enable_if<std::is_convertible<TyNodeDataMy &&, TyNodeData>::value, NodeIter
	>::type AddNode(TyNodeDataMy &&data)
	{
		return EmplaceNode(std::forward<TyNodeDataMy>(data));
	}
	template<typename ...Tys>
	NodeIter EmplaceNode(Tys &&...args)
	{
		vecNode.emplace_back(std::forward<Tys>(args)...);
		return NodeIter(this, vecNode.size()-1);
	}
	//添加边
	template<typename TyEdgeDataMy>
	typename std::enable_if<std::is_convertible<TyEdgeDataMy &&, TyEdgeData>::value, EdgeIter
	>::type AddEdge(NodeIter fromNode, NodeIter toNode, TyEdgeDataMy &&data)
	{
		return EmplaceEdge(fromNode, toNode, std::forward<TyEdgeDataMy>(data));
	}
	//构造边
	template<typename ...Tys>
	EdgeIter EmplaceEdge(NodeIter fromNode, NodeIter toNode, Tys &&...args)
	{
		vecNode[fromNode.idxNode].vecEdge.emplace_back(
			toNode.idxNode, std::forward<Tys>(args)...);
		return EdgeIter(this, fromNode.idxNode, vecNode[fromNode.idxNode].vecEdge.size()-1);
	}
	//添加双向边
	template<typename TyEdgeDataMy>
	typename std::enable_if<std::is_convertible<TyEdgeDataMy &&, TyEdgeData>::value,
		std::pair<EdgeIter, EdgeIter>
	>::type AddDoublyEdge(NodeIter fromNode, NodeIter toNode,
		TyEdgeDataMy &&dataGo, TyEdgeDataMy &&dataRev)
	{
		auto it = AddAdge(fromNode, toNode, std::forward<TyEdgeDataMy>(dataGo));
		return std::make_pair(it,
			AddAdge(toNode, fromNode, std::forward<TyEdgeDataMy>(dataRev)));
	}
private:
	//构造双向边辅助函数
	template<typename ...TyArgsGo_S, typename ...TyArgsRev_S>
	std::pair<EdgeIter, EdgeIter> EmplaceDoublyEdgeAssist(NodeIter fromNode,
		NodeIter toNode, ArgsTuple<TyArgsGo_S...> &&argsTupGo,
		ArgsTuple<TyArgsRev_S...> &&argsTupRev)
	{
		static auto funcGo = std::mem_fn(&AddonlyGraph::EmplaceEdge<TyArgsGo_S...>);
		static auto funcRev = std::mem_fn(&AddonlyGraph::EmplaceEdge<TyArgsRev_S...>);
		auto it = ArgsTupleInvoke(funcGo, this, fromNode, toNode, std::move(argsTupGo));
		return std::make_pair(it,
			ArgsTupleInvoke(funcRev, this, toNode, fromNode, std::move(argsTupRev)));
	}
public:
	//构造双向边
	template<typename TyArgsGo, typename TyArgsRev>
	std::pair<EdgeIter, EdgeIter> EmplaceDoublyEdge(NodeIter fromNode,
		NodeIter toNode, TyArgsGo &&argsTupGo, TyArgsRev &&argsTupRev)
	{
		return EmplaceDoublyEdgeAssist(fromNode, toNode,
			ArgsTupleRefCat(std::forward<TyArgsGo>(argsTupGo)),
			ArgsTupleRefCat(std::forward<TyArgsRev>(argsTupRev)));
	}
	//清除图
	void Clear()
	{
		vecNode.clear();
	}
	//迭代节点
	NodeIter NodeBegin()
	{
		return NodeIter(this, 0);
	}
	NodeConstIter NodeBegin() const
	{
		return NodeConstIter(this, 0);
	}
	NodeIter begin()
	{
		return NodeBegin();
	}
	NodeConstIter begin() const
	{
		return NodeBegin();
	}
	NodeConstIter NodeConstBegin()
	{
		return NodeBegin();
	}
	NodeIter NodeEnd()
	{
		return NodeIter(this, vecNode.size());
	}
	NodeConstIter NodeEnd() const
	{
		return NodeConstIter(this, vecNode.size());
	}
	NodeIter end()
	{
		return NodeEnd();
	}
	NodeConstIter end() const
	{
		return NodeEnd();
	}
	NodeConstIter NodeConstEnd()
	{
		return NodeEnd();
	}
	//节点数
	size_t NodeSize() const
	{
		return vecNode.size();
	}
};

//点迭代器模板辅助类
template<typename TyNodeData, typename TyEdgeData>
template<typename GraphTypeMy, typename TyNodeDataMy, typename TyNodeDataPure>
class AddonlyGraph<TyNodeData, TyEdgeData>::TemplateNodeIterAssist:
	public std::iterator<std::bidirectional_iterator_tag, TyNodeDataMy,
	_StruAndAlgoDifferenceType, TyNodeDataMy *, TyNodeDataMy &>
{
protected://成员数据
	GraphTypeMy *graph;//图指针
	size_t idxNode;//节点

public://基本函数
	//默认构造
	TemplateNodeIterAssist():
		graph(nullptr), idxNode(0)
	{
	}
	//构造函数，允许外部进行构造
	explicit TemplateNodeIterAssist(GraphTypeMy *o_graph, size_t o_idxNode):
		graph(o_graph), idxNode(o_idxNode)
	{
	}
public://运算符重载
	//解引用操作
	TyNodeDataMy &operator *() const
	{
		return graph->vecNode[idxNode].hold;
	}
	TyNodeDataMy *operator ->() const
	{
		return &operator *();
	}
};
//点迭代器模板辅助类特化
template<typename TyNodeData, typename TyEdgeData>
template<typename GraphTypeMy, typename TyNodeDataMy>
class AddonlyGraph<TyNodeData, TyEdgeData>::TemplateNodeIterAssist<
	GraphTypeMy, TyNodeDataMy, void
>: public std::iterator<std::bidirectional_iterator_tag, TyNodeDataMy,
	_StruAndAlgoDifferenceType, void, void>
{
protected://成员数据
	GraphTypeMy *graph;//图指针
	size_t idxNode;//节点

public://基本函数
	//默认构造
	TemplateNodeIterAssist():
		graph(nullptr), idxNode(0)
	{
	}
	//构造函数，允许外部进行构造
	explicit TemplateNodeIterAssist(GraphTypeMy *o_graph, size_t o_idxNode):
		graph(o_graph), idxNode(o_idxNode)
	{
	}
};

//节点迭代器模板类
template<typename TyNodeData, typename TyEdgeData>
template<typename GraphTypeMy, typename TyNodeDataMy, typename TyEdgeDataMy>
class AddonlyGraph<TyNodeData, TyEdgeData>::TemplateNodeIter:
	public TemplateNodeIterAssist<GraphTypeMy, TyNodeDataMy, TyNodeData>
{
public://友元
	friend class AddonlyGraph<TyNodeData, TyEdgeData>;
	friend class NodeIter;
	friend class NodeConstIter;
	friend class EdgeIter;
	friend class EdgeConstIter;
private://辅助类型
	//对应的边迭代器
	using EdgeIterMy = TemplateEdgeIter<GraphTypeMy, TyNodeDataMy, TyEdgeDataMy>;

public://基本函数
	//默认构造
	TemplateNodeIter()
		= default;
	//继承辅助类的构造函数
	using TemplateNodeIterAssist::TemplateNodeIterAssist;
	//由非常类构造常类
	TemplateNodeIter(const NodeIter &other)
	{
		operator =(other);
	}
	TemplateNodeIter &operator =(const NodeIter &other)
	{
		if((void *)this!=(void *)&other) {
			graph = other.graph;
			idxNode = other.idxNode;
		}
		return *this;
	}
public://运算符重载
	//递增递减操作
	TemplateNodeIter &operator ++()
	{
		++ idxNode;
		return *this;
	}
	TemplateNodeIter operator ++(int)
	{
		TemplateNodeIter ret(*this);
		operator ++();
		return ret;
	}
	TemplateNodeIter &operator --()
	{
		-- idxNode;
		return *this;
	}
	TemplateNodeIter operator --(int)
	{
		TemplateNodeIter ret(*this);
		operator --();
		return ret;
	}
	//比较操作
	template<typename GraphTypeOther, typename TyNodeDataOther, typename TyEdgeDataOther>
	bool operator ==(const TemplateNodeIter<
		GraphTypeOther, TyNodeDataOther, TyEdgeDataOther> &other) const
	{
		return idxNode==other.idxNode;
	}
	template<typename GraphTypeOther, typename TyNodeDataOther, typename TyEdgeDataOther>
	bool operator !=(const TemplateNodeIter<
		GraphTypeOther, TyNodeDataOther, TyEdgeDataOther> &other) const
	{
		return !operator ==(other);
	}
public://成员函数
	//出边迭代操作
	EdgeIterMy EdgeBegin() const
	{
		return EdgeIterMy(graph, idxNode, 0);
	}
	EdgeIterMy begin() const
	{
		return EdgeBegin();
	}
	EdgeConstIter EdgeConstBegin() const
	{
		return EdgeBegin();
	}
	EdgeIterMy EdgeEnd() const
	{
		return EdgeIterMy(graph, idxNode, graph->vecNode[idxNode].vecEdge.size());
	}
	EdgeIterMy end() const
	{
		return EdgeEnd();
	}
	EdgeConstIter EdgeConstEnd() const
	{
		return EdgeEnd();
	}
	//处边数
	size_t EdgeSize() const
	{
		return graph->vecNode[idxNode].size();
	}
	//改变指向操作
	void ChangePointer(GraphTypeMy *o_graph)
	{
		graph = o_graph;
	}
};

//边代器模板辅助类
template<typename TyNodeData, typename TyEdgeData>
template<typename GraphTypeMy, typename TyEdgeDataMy, typename TyEdgeDataPure>
class AddonlyGraph<TyNodeData, TyEdgeData>::TemplateEdgeIterAssist:
	public std::iterator<std::bidirectional_iterator_tag, TyEdgeDataMy,
	_StruAndAlgoDifferenceType, TyEdgeDataMy *, TyEdgeDataMy &>
{
protected://成员数据
	GraphTypeMy *graph;//图指针
	size_t idxNode;//节点序号
	size_t idxEdge;//边序号

public://基本函数
	//默认构造
	TemplateEdgeIterAssist():
		graph(nullptr), idxNode(0), idxEdge(0)
	{
	}
protected:
	//构造函数，不允许外部进行构造
	explicit TemplateEdgeIterAssist(GraphTypeMy *o_graph, size_t o_idxNode, size_t o_idxEdge):
		graph(o_graph), idxNode(o_idxNode), idxEdge(o_idxEdge)
	{
	}
public://运算符重载
	//解引用操作
	TyEdgeDataMy &operator *() const
	{
		return graph->vecNode[idxNode].vecEdge[idxEdge].hold;
	}
	TyEdgeDataMy *operator ->() const
	{
		return &operator *();
	}
};
//边迭代器模板辅助类特化
template<typename TyNodeData, typename TyEdgeData>
template<typename GraphTypeMy, typename TyEdgeDataMy>
class AddonlyGraph<TyNodeData, TyEdgeData>::TemplateEdgeIterAssist<
	GraphTypeMy, TyEdgeDataMy, void
>: public std::iterator<std::bidirectional_iterator_tag, TyEdgeDataMy,
	_StruAndAlgoDifferenceType, void, void>
{
protected://成员数据
	GraphTypeMy *graph;//图指针
	size_t idxNode;//节点序号
	size_t idxEdge;//边序号

public://基本函数
	//默认构造
	TemplateEdgeIterAssist():
		graph(nullptr), idxNode(0), idxEdge(0)
	{
	}
protected:
	//构造函数，不允许外部进行构造
	explicit TemplateEdgeIterAssist(GraphTypeMy *o_graph, size_t o_idxNode, size_t o_idxEdge):
		graph(o_graph), idxNode(o_idxNode), idxEdge(o_idxEdge)
	{
	}
};

//边迭代器模板类
template<typename TyNodeData, typename TyEdgeData>
template<typename GraphTypeMy, typename TyNodeDataMy, typename TyEdgeDataMy>
class AddonlyGraph<TyNodeData, TyEdgeData>::TemplateEdgeIter:
	public TemplateEdgeIterAssist<GraphTypeMy, TyEdgeDataMy, TyEdgeData>
{
public://友元
	friend class AddonlyGraph<TyNodeData, TyEdgeData>;
	friend class NodeIter;
	friend class NodeConstIter;
	friend class EdgeIter;
	friend class EdgeConstIter;
private://辅助类型
	//对应的节点迭代器
	using NodeIterMy = TemplateNodeIter<GraphTypeMy, TyNodeDataMy, TyEdgeDataMy>;

public://基本函数
	//继承辅助类的构造函数
	using TemplateEdgeIterAssist::TemplateEdgeIterAssist;
	//由非常类构造常类
	TemplateEdgeIter(const EdgeIter &other)
	{
		operator =(other);
	}
	TemplateEdgeIter &operator =(const EdgeIter &other)
	{
		if((void *)this!=(void *)&other) {
			graph = other.graph;
			idxNode = other.idxNode;
			idxEdge = other.idxEdge;
		}
		return *this;
	}
public://运算符重载
	//递增递减造作
	TemplateEdgeIter &operator ++()
	{
		++ idxEdge;
		return *this;
	}
	TemplateEdgeIter operator ++(int)
	{
		TemplateEdgeIter ret(*this);
		operator ++();
		return ret;
	}
	TemplateEdgeIter &operator --()
	{
		-- idxEdge;
		return *this;
	}
	TemplateEdgeIter operator --(int)
	{
		TemplateEdgeIter ret(*this);
		operator --();
		return ret;
	}
	//比较操作
	template<typename GraphTypeOther, typename TyNodeDataOther, typename TyEdgeDataOther>
	bool operator ==(const TemplateEdgeIter<
		GraphTypeOther, TyNodeDataOther, TyEdgeDataOther> &other) const
	{
		return idxNode==other.idxNode && idxEdge==other.idxEdge;
	}
	template<typename GraphTypeOther, typename TyNodeDataOther, typename TyEdgeDataOther>
	bool operator !=(const TemplateEdgeIter<
		GraphTypeOther, TyNodeDataOther, TyEdgeDataOther> &other) const
	{
		return !operator ==(other);
	}
public://成员函数
	//获取连接节点
	NodeIterMy NodeFrom() const
	{
		return NodeIterMy(graph, idxNode);
	}
	NodeConstIter NodeConstFrom() const
	{
		return NodeFrom();
	}
	NodeIterMy NodeTo() const
	{
		return NodeIterMy(graph, graph->vecNode[idxNode].vecEdge[idxEdge].index);
	}
	NodeIterMy NodeConstTo() const
	{
		return NodeTo();
	}
};


/*图分类设想

1，只能添加的图：
AddNode
AddEdge
Clear
NodeBegin
NodeEnd
NodeIter::EdgeBegin
NodeIter::EdgeEnd
NodeIter::operator ++
NodeIter::operator --
NodeIter::operator *
EdgeIter::NodeFrom
EdgeIter::NodeTo
EdgeIter::operator ++
NodeIter::operator --
EdgeIter::operator *

2，只能添加的图，可以查看来源边：
全部1函数
NodeIter::EdgeFromBegin
NodeIter::EdgeFromEnd

3，只能添加的图，可以根据两个顶点找来源
全部1函数
GetEdge

4，只能添加的图，带有两者属性
全部2，3函数

5，可以修改的图
全部1函数
全部2函数
DelNode
DelEdge

6，在5基础上增加
3功能，还没想好

*/




//********
//算法部分
//********

//快速计算幂次，注意返回类型和基类型相同
template<typename Ty>
inline constexpr Ty FastPower(Ty base, int power)
{
	return power<0 ? 0
		: power==0 ? 1
		: (power&1)==0 ? FastPower(base*base, power>>1)
		: base*FastPower(base*base, power>>1);
}



//推算比例表示精度，即比例扩大底数的几次幂可以大于等于基准比例
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



//生成后一个组合数，返回是否到头
//要求双向迭代器，可递增、相等比较值类型
template<typename TyIt, typename Ty>
bool NextCombination(TyIt st, TyIt ed, Ty rangeSt, Ty rangeEd)
{
	auto it= ed, it1= it;
	//找数值空隙
	for(; it!=st; --it) {
		it1 = it;
		-- it1;
		if(it==ed) {
			if(!(*it1==rangeEd)) {
				++ *it1;
				break;
			}
		}
		else {
			auto value = *it1;
			++ value;
			if(!(value==*it)) {
				*it1 = value;
				break;
			}
		}
	}
	//后续组合数递增
	bool res = it!=st;
	if(it!=ed) {
		if(!res)
			*it = rangeSt;
		else {
			it1 = it;
			-- it1;
			*it = *it1;
			++ *it;
		}
		for(it1=it, ++it; it!=ed; it1=it, ++it) {
			*it = *it1;
			++ *it;
		}
	}
	return res;
}

//生成后一个排列组合数，返回是否到头
//要求双向迭代器，可递增、相等比较值类型
template<typename TyIt, typename Ty>
bool NextCombinationPermutation(TyIt st, TyIt ed, Ty rangeSt, Ty rangeEd)
{
	if(!std::next_permutation(st, ed)) {
		if(!NextCombination(st, ed, rangeSt, rangeEd))
			return false;
	}
	return true;
}



//单纯形结果状态
enum class SimplexStatus
{
	result, infeasible, unbounded,//有结果，无解，无界
};

//单纯形辅助旋转
template<typename Ty>
inline void _SimplexAssistPivot(std::vector<std::vector<Ty>> &aMatrix, std::vector<Ty> &bVector,
	std::vector<Ty> &cVector, Ty &vValue, std::vector<int> &bSet, std::vector<int> &nSet,
	int lIndex, int eIndex)
{
	int mNum= (int)bVector.size(), nNum= (int)cVector.size();//约束数，变量数
	//转换换出变量所在行一行
	aMatrix[lIndex][eIndex] = 1/aMatrix[lIndex][eIndex];
	bVector[lIndex] *= aMatrix[lIndex][eIndex];
	for(int j=0; j<nNum; ++j) {
		if(j==eIndex)
			continue;
		aMatrix[lIndex][j] *= aMatrix[lIndex][eIndex];
	}
	//转换矩阵其他行
	for(int i=0; i<mNum; ++i) {
		if(i==lIndex)
			continue;
		bVector[i] -= aMatrix[i][eIndex]*bVector[lIndex];
		for(int j=0; j<nNum; ++j) {
			if(j==eIndex)
				continue;
			aMatrix[i][j] -= aMatrix[i][eIndex]*aMatrix[lIndex][j];
		}
		aMatrix[i][eIndex] *= -aMatrix[lIndex][eIndex];
	}
	//转换目标函数
	vValue += cVector[eIndex]*bVector[lIndex];
	for(int j=0; j<nNum; ++j) {
		if(j==eIndex)
			continue;
		cVector[j] -= cVector[eIndex]*aMatrix[lIndex][j];
	}
	cVector[eIndex] *= -aMatrix[lIndex][eIndex];
	//转换索引
	std::swap(bSet[lIndex], nSet[eIndex]);
}

//单纯形辅助算法循环主体
template<typename Ty>
inline SimplexStatus _SimplexAssistLoop(std::vector<std::vector<Ty>> &aMatrix, std::vector<Ty> &bVector,
	std::vector<Ty> &cVector, Ty &vValue, std::vector<int> &bSet, std::vector<int> &nSet, int ulp)
{
	int mNum= (int)bVector.size(), nNum= (int)cVector.size();//约束数，变量数
	//单纯形循环
	while(true) {
		//找换入换出变量循环
		bool bFirst = true;
		int lIndex, eIndex;
		while(true) {
			//找换入变量
			eIndex = -1;
			Ty maxC = -std::numeric_limits<Ty>::infinity();
			for(int j=0; j<nNum; ++j) {
				if(bFirst) {
					if(cVector[j]>maxC) {
						eIndex = j;
						maxC = cVector[j];
					}
				}
				else {
					//退化时选标号最小
					if(_StruAndAlgoFloatGT(cVector[j], 0.0, true, ulp) && (eIndex==-1 || nSet[j]<nSet[eIndex])) {
						eIndex = j;
						maxC = cVector[j];
					}
				}
			}
			if(_StruAndAlgoFloatLTE(maxC, 0.0, true, ulp))
				return SimplexStatus::result;
			//找换出变量
			lIndex = -1;
			Ty minB = std::numeric_limits<Ty>::infinity();
			for(int i=0; i<mNum; ++i) {
				if(_StruAndAlgoFloatGT(aMatrix[i][eIndex], 0.0, true, ulp)) {
					Ty tmp = bVector[i]/aMatrix[i][eIndex];
					if(tmp<minB) {
						minB = tmp;
						lIndex = i;
					}
				}
			}
			//判断无界
			if(lIndex==-1) {
				vValue = std::numeric_limits<Ty>::infinity();
				return SimplexStatus::unbounded;
			}
			//退化则继续循环
			if(bFirst && _StruAndAlgoFloatEQ(minB, 0.0, true, ulp))
				bFirst = false;
			else
				break;
		}
		//旋转
		_SimplexAssistPivot(aMatrix, bVector, cVector, vValue, bSet, nSet, lIndex, eIndex);
	}
}

//单纯形辅助初始化
template<typename Ty>
inline SimplexStatus _SimplexAssistInit(std::vector<std::vector<Ty>> &aMatrix, std::vector<Ty> &bVector,
	std::vector<Ty> &cVector, Ty &vValue, std::vector<int> &bSet, std::vector<int> &nSet, int ulp)
{
	int mNum= (int)bVector.size(), nNum= (int)cVector.size();//约束数，变量数
	//判断基本解可行性
	bool bPoss = true;
	bSet.reserve(mNum);
	nSet.reserve(nNum+1);
	for(int j=0; j<nNum; ++j)
		nSet.push_back(j);
	Ty minB = std::numeric_limits<Ty>::infinity();
	int lIndex = -1;
	for(int i=0; i<mNum; ++i) {
		bSet.push_back(nNum+i);
		if(bVector[i]<minB) {
			minB = bVector[i];
			lIndex = i;
		}
	}
	if(_StruAndAlgoFloatGTE(minB, 0.0, true, ulp))
		return SimplexStatus::result;
	//构造辅助规划
	Ty vValue2 = 0;
	std::vector<Ty> cVector2(nNum+1, 0);
	cVector2[nNum] = -1;
	for(int i=0; i<mNum; ++i)
		aMatrix[i].push_back(-1);
	nSet.push_back(-1);
	//初始旋转并求解
	_SimplexAssistPivot(aMatrix, bVector, cVector2, vValue2, bSet, nSet, lIndex, nNum);
	SimplexStatus status = _SimplexAssistLoop(aMatrix, bVector, cVector2, vValue2, bSet, nSet, ulp);
	assert(status==SimplexStatus::result);
	//判断可行性
	status = _StruAndAlgoFloatEQ(vValue2, 0.0, true, ulp) ?
		SimplexStatus::result : SimplexStatus::infeasible;
	//反旋转辅助变量
	lIndex = (int)(std::find(bSet.begin(), bSet.end(), -1)-bSet.begin());
	int eIndex;
	if(lIndex!=mNum) {
		eIndex = -1;
		for(int j=0; j<nNum+1; ++j) {
			if(_StruAndAlgoFloatNEQ(aMatrix[lIndex][j], 0.0, true, ulp)) {
				eIndex = j;
				break;
			}
		}
		assert(eIndex!=-1);
		_SimplexAssistPivot(aMatrix, bVector, cVector2, vValue2, bSet, nSet, lIndex, eIndex);
	}
	else {
		eIndex = (int)(std::find(nSet.begin(), nSet.end(), -1)-nSet.begin());
		assert(eIndex!=nNum+1);
	}
	//交换辅助变量
	if(eIndex!=nNum) {
		for(int i=0; i<mNum; ++i)
			std::swap(aMatrix[i][eIndex], aMatrix[i][nNum]);
		std::swap(nSet[eIndex], nSet[nNum]);
	}
	//去除辅助变量
	for(int i=0; i<mNum; ++i)
		aMatrix[i].pop_back();
	nSet.pop_back();
	cVector2.assign(nNum, 0);
	for(int j=0; j<nNum; ++j) {
		if(nSet[j]<nNum)
			cVector2[j] = cVector[nSet[j]];
	}
	for(int i=0; i<mNum; ++i) {
		if(bSet[i]<nNum) {
			vValue += cVector[bSet[i]]*bVector[i];
			for(int j=0; j<nNum; ++j) {
				cVector2[j] -= cVector[bSet[i]]*aMatrix[i][j];
			}
		}
	}
	std::swap(cVector, cVector2);
	return status;
}

//单纯形法，模板为浮点数
//参数为标准型即：系数矩阵、常数向量、目标系数向量、目标常数值，浮点绝对误差界倍率
//返回值为状态、解向量、基本变量序号、非基本变量序号
//aMatrix、bVector、cVector中均为为最终松弛型的结构，vValue是目标值
//标准型的样例：
//MAX: cVector^T * x + vValue
//s.t.：aMatrix * x <= bVector
template<typename Ty>
inline typename std::enable_if<std::is_floating_point<Ty>::value,
	std::tuple<SimplexStatus, std::vector<Ty>, std::vector<int>, std::vector<int>>
>::type SimplexAlgo(std::vector<std::vector<Ty>> &aMatrix, std::vector<Ty> &bVector,
	std::vector<Ty> &cVector, Ty &vValue, int ulp= 2000)
{
	int mNum= (int)bVector.size(), nNum= (int)cVector.size();//约束数，变量数
	std::vector<int> bSet, nSet;//基本、非基本变量索引下标
	std::vector<Ty> resVector;//解系数向量
	//找初始解
	SimplexStatus status = _SimplexAssistInit(aMatrix, bVector, cVector, vValue, bSet, nSet, ulp);
	if(status==SimplexStatus::infeasible)
		return make_tuple(status, resVector, bSet, nSet);
	//单纯形循环
	status = _SimplexAssistLoop(aMatrix, bVector, cVector, vValue, bSet, nSet, ulp);
	if(status==SimplexStatus::unbounded)
		return make_tuple(status, resVector, bSet, nSet);
	//生成解向量
	resVector.resize(nNum, 0);
	for(int i=0; i<mNum; ++i) {
		if(bSet[i]<nNum)
			resVector[bSet[i]] = bVector[i];
	}
	return make_tuple(status, std::move(resVector), std::move(bSet), std::move(nSet));
}

