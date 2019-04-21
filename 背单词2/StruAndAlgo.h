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

#include <vector>
#include <list>
#include <map>

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

//前者去除引用常数易变后是否为后者
template<typename TySrc, typename TyDst>
struct _StruAndAlgoIsRemoveCVRefSame:
	std::is_same<typename _StruAndAlgoRemoveCVRef<TySrc>::type, TyDst>
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

//参数包类声明
template<typename ...Ty_S>
class ArgsTuple;

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



//************
//数据结构部分
//************

//支持下标快速删除的容器适配器，需提供支持下标访问的容器，如string,vector和deque
//实现上为删除元素后将最后的元素移动到前面
//所以快速删除时最后的迭代器会失效，被删除的迭代器指向原本最后的迭代器数据
template<typename TyContainer>
class FastEraseVector:
	public TyContainer
{
public://基本函数
	//默认构造函数
	FastEraseVector()
		= default;
	//继承构造函数
	using TyContainer::TyContainer;
	//省略合成5拷贝

public://成员函数
	//迭代器快速删除，返回移动的迭代器数量
	size_t IterFastErase(typename TyContainer::iterator itPos)
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
	size_t IterFastErase(typename TyContainer::iterator itSt,
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
		return IterFastErase(begin()+idx);
	}
	//下标范围快速删除，返回移动的迭代器数量
	size_t IndexFastErase(size_t idxSt, size_t idxEd)
	{
		return IterFastErase(begin()+idxSt, begin()+idxEd);
	}
};




//带额外顺序的集合，支持顺序访问和值序访问、扩展不定随机访问功能
//删除对应元素会使对应的两种迭代器失效，更改元素的值会使对应值序迭代器失效
//试反向迭代器
template<typename TyData, typename TyCompare= std::less<TyData>>
class SequenceSet
{
private://数据类型定义
	struct ListStru;
	//list迭代器
	using ListIter = typename std::list<ListStru>::iterator;
	using ListConstIter = typename std::list<ListStru>::const_iterator;
	//multimap迭代器
	using MultimapIter = typename std::multimap<TyData, ListIter, TyCompare>::iterator;
	using MultimapConstIter = typename std::multimap<TyData, ListIter, TyCompare>::const_iterator;
	//list存放数据结构体
	struct ListStru
	{
		size_t idxVec;//队列的索引
		MultimapIter itBst;//链表的迭代器
	public:
		ListStru()
			= default;
		ListStru(size_t o_idxVec):
			idxVec(o_idxVec)
		{
		}
	};

private://迭代器模板定义
	//顺序迭代器模板
	template<typename ListIterMy, typename MultimapIterMy>
	class TemplateSeqIter;
	//值序迭代器模板
	template<typename ListIterMy, typename MultimapIterMy>
	class TemplateValueIter;

private://迭代器类型指定
	//顺序迭代器
	using SeqIter = TemplateSeqIter<ListIter, MultimapIter>;
	//顺序常迭代器
	using SeqConstIter = TemplateSeqIter<ListConstIter, MultimapConstIter>;
	//值序访问的迭代器
	using ValueIter = TemplateValueIter<ListIter, MultimapIter>;
	//值序访问的迭代器
	using ValueConstIter = TemplateValueIter<ListConstIter, MultimapConstIter>;

public://友元
	friend class SeqIter;
	friend class SeqConstIter;
	friend class ValueIter;
	friend class ValueConstIter;

public://辅助类型
	using DataType = TyData;

private://成员数据
	std::list<ListStru> lis;//链表结构，存放树结构迭代器和数组的索引
	std::multimap<TyData, ListIter, TyCompare> bst;//树结构，存放值和链表迭代器
	std::vector<ListIter> vec;//向量，存放链表迭代器

public://基本函数
	//默认构造
	explicit SequenceSet(const TyCompare &func= TyCompare()):
		bst(func)
	{
	}
	//赋值构造函数
	explicit SequenceSet(std::initializer_list<TyData> init,
		const TyCompare &func= TyCompare()
	): bst(func)
	{
		Assign(init);
	}
	explicit SequenceSet(size_t size, const TyData &value,
		const TyCompare &func= TyCompare()
	): bst(func)
	{
		Assign(size, value);
	}
	template<typename TyIt, typename= typename std::enable_if<
		_StruAndAlgoIsIterator<TyIt>::value>::type>
	explicit SequenceSet(TyIt itSt, TyIt itEd, const TyCompare &func= TyCompare()):
		bst(func)
	{
		Assign(itSt, itEd);
	}
public://运算符重载
	//列表赋值
	SequenceSet &operator =(std::initializer_list<TyData> init)
	{
		Assign(init);
		return *this;
	}
	//比较操作，按顺序的字典序
	template<typename TyDataOther, typename TyCompareOther>
	bool operator <(const SequenceSet<TyDataOther, TyCompareOther> &other) const
	{
		return Compare(other)<0;
	}
	template<typename TyDataOther, typename TyCompareOther>
	bool operator ==(const SequenceSet<TyDataOther, TyCompareOther> &other) const
	{
		return Compare(other)==0;
	}
	template<typename TyDataOther, typename TyCompareOther>
	bool operator !=(const SequenceSet<TyDataOther, TyCompareOther> &other) const
	{
		return Compare(other)!=0;
	}
	template<typename TyDataOther, typename TyCompareOther>
	bool operator <=(const SequenceSet<TyDataOther, TyCompareOther> &other) const
	{
		return Compare(other)<=0;
	}
	template<typename TyDataOther, typename TyCompareOther>
	bool operator >(const SequenceSet<TyDataOther, TyCompareOther> &other) const
	{
		return Compare(other)>0;
	}
	template<typename TyDataOther, typename TyCompareOther>
	bool operator >=(const SequenceSet<TyDataOther, TyCompareOther> &other) const
	{
		return Compare(other)>=0;
	}
public://成员函数
	//赋值函数
	SequenceSet &Assign(std::initializer_list<TyData> init)
	{
		Assign(init.begin(), init.end());
		return *this;
	}
	SequenceSet &Assign(size_t size, const TyData &value)
	{
		Clear();
		for(int i=0; i!=size; ++i)
			EmplaceAssist(lis.end(), value);
		return *this;
	}
	template<typename TyIt>
	typename std::enable_if<_StruAndAlgoIsIterator<TyIt>::value, SequenceSet &
	>::type Assign(TyIt itSt, TyIt itEd)
	{
		Clear();
		for(; itSt!=itEd; ++itSt) {
			EmplaceAssist(lis.end(), *itSt);
		}
		return *this;
	}
	//比较操作，小于等于大于分别返回-1,0,1
	template<typename TyDataOther, typename TyCompareOther,
		typename TyFuncLess= std::function<bool(const TyData &, const TyDataOther &)>,
		typename TyFuncEqual= std::function<bool(const TyData &, const TyDataOther &)>
	> int Compare(const SequenceSet<TyDataOther, TyCompareOther> &other,
		TyFuncLess funcLess= [](const TyData &arg1, const TyDataOther &arg2) {
		return arg1<arg2;},
		TyFuncEqual funcEqual= [](const TyData &arg1, const TyDataOther &arg2) {
		return arg1==arg2;}) const
	{
		auto itMe = this->begin();
		auto itOther = other.begin();
		for(; ; ++itMe, ++itOther) {
			//判断结束条件
			if(itMe==this->end()) {
				if(itOther==other.end())
					return 0;
				else
					return -1;
			}
			else
				return 1;
			//判断大小
			if(funcLess(*itMe, *itOther))
				return -1;
			else if(!(funcEqual(*itMe, *itOther)))
				return 1;
		}
	}
	//顺序产生迭代器
	SeqIter SeqBegin()
	{
		return SeqIter(lis.begin());
	}
	SeqConstIter SeqBegin() const
	{
		return SeqConstIter(lis.begin());
	}
	SeqIter begin()
	{
		return SeqBegin();
	}
	SeqConstIter begin() const
	{
		return SeqBegin();
	}
	SeqConstIter SeqConstBegin() const
	{
		return SeqBegin();
	}
	SeqIter SeqEnd()
	{
		return SeqIter(lis.end());
	}
	SeqConstIter SeqEnd() const
	{
		return SeqConstIter(lis.end());
	}
	SeqIter end()
	{
		return SeqEnd();
	}
	SeqConstIter end() const
	{
		return SeqEnd();
	}
	SeqConstIter SeqConstEnd() const
	{
		return SeqEnd();
	}
	//值序产生迭代器
	ValueIter ValueBegin()
	{
		return ValueIter(bst.begin());
	}
	ValueConstIter ValueBegin() const
	{
		return ValueConstIter(bst.begin());
	}
	ValueConstIter ValueConstBegin() const
	{
		return ValueBegin();
	}
	ValueIter ValueEnd()
	{
		return ValueIter(bst.end());
	}
	ValueConstIter ValueEnd() const
	{
		return ValueConstIter(bst.end());
	}
	ValueConstIter ValueConstEnd() const
	{
		return ValueEnd();
	}
	//随机产生迭代器
	SeqIter RandomIter(size_t idx)
	{
		return SeqIter(vec[idx]);
	}
	SeqConstIter RandomIter(size_t idx) const
	{
		return SeqConstIter(vec[idx]);
	}
	SeqConstIter RandomConstIter(size_t idx) const
	{
		return RandomIter(idx);
	}
	//访问顺序首尾元素
	const TyData &SeqFront() const
	{
		return lis.front().itBst->first;
	}
	const TyData &SeqBack() const
	{
		return lis.back().itBst->first;
	}
	//访问值序首尾元素
	const TyData &ValueFront() const
	{
		return bst.begin()->first;
	}
	const TyData &ValueBack() const
	{
		return (--bst.end())->first;
	}
	//随机访问元素
	const TyData &RandomElement(size_t idx) const
	{
		return vec[idx]->itBst->first;
	}
	//查看容量
	bool Empty() const
	{
		return lis.empty();
	}
	size_t Size() const
	{
		return lis.size();
	}
	//清空
	void Clear()
	{
		lis.clear();
		bst.clear();
		vec.clear();
	}
	//改变容量
	void Resize(size_t size, const TyData &value)
	{
		while(Size()>size)
			PopBack();
		while(Size()<size)
			PushBack(value);
	}
private:
	//插入辅助函数
	template<typename ...Ty_S>
	ListIter EmplaceAssist(ListIter itPos, Ty_S &&...arg_s)
	{
		auto itLis = lis.emplace(itPos, vec.size());
		auto itBst = bst.emplace(std::piecewise_construct,
			std::forward_as_tuple(std::forward<Ty_S>(arg_s)...), std::make_tuple(itLis));
		itLis->itBst = itBst;
		vec.push_back(itLis);
		return itLis;
	}
public:
	//两端插入
	template<typename Ty>
	typename std::enable_if<_StruAndAlgoIsRemoveCVRefSame<Ty, TyData>::value, void
	>::type PushBack(Ty &&value)
	{
		EmplaceAssist(lis.end(), std::forward<Ty>(value));
	}
	template<typename ...Ty_S>
	void EmplaceBack(Ty_S &&...arg_s)
	{
		EmplaceAssist(lis.end(), std::forward<Ty_S>(arg_s)...);
	}
	template<typename Ty>
	typename std::enable_if<_StruAndAlgoIsRemoveCVRefSame<Ty, TyData>::value, void
	>::type PushFront(Ty &&value)
	{
		EmplaceAssist(lis.begin(), std::forward<Ty>(value));
	}
	template<typename ...Ty_S>
	void EmplaceFront(Ty_S &&...arg_s)
	{
		EmplaceAssist(lis.begin(), std::forward<Ty_S>(arg_s)...);
	}
	//在指定位置前插入，返回第一个元素的迭代器
	template<typename Ty>
	typename std::enable_if<_StruAndAlgoIsRemoveCVRefSame<Ty, TyData>::value, SeqIter
	>::type Insert(SeqIter itPos, Ty &&value)
	{
		return SeqIter(EmplaceAssist(itPos.it, std::forward<Ty>(value)));
	}
	SeqIter Insert(SeqIter itPos, std::initializer_list<TyData> init)
	{
		return Insert(itPos, init.begin(), init.end());
	}
	template<typename TyIt>
	SeqIter Insert(SeqIter itPos, TyIt itSt, TyIt itEd)
	{
		auto ret = itPos.it;
		bool bIns = false;
		for(; itSt!=itEd; ++itSt) {
			if(!bIns)
				ret = EmplaceAssist(itPos.it, *itSt);
			else
				EmplaceAssist(itPos.it, *itSt);
		}
		return SeqIter(ret);
	}
	template<typename ...Ty_S>
	SeqIter Emplace(SeqIter itPos, Ty_S &&...arg_s)
	{
		return SeqIter(EmplaceAssist(itPos.it, std::forward<Ty_S>(arg_s)...));
	}
private:
	//顺序删除函数辅助
	ListIter SeqEraseAssist(ListIter itPos)
	{
		//将最后vec一个元素移到删除的位置，改变vec原本最后一个元素对应的lis中的索引
		auto posVecIdx = itPos->idxVec;
		if(posVecIdx!=vec.size()-1) {
			vec[posVecIdx] = vec.back();
			vec[posVecIdx]->idxVec = posVecIdx;
		}
		vec.pop_back();
		//删除lis和bst
		bst.erase(itPos->itBst);
		return lis.erase(itPos);
	}
	//值序删除函数辅助
	MultimapIter ValueEraseAssist(MultimapIter itSt, MultimapIter itEd)
	{
		for(auto it=itSt; it!=itEd; ++it) {
			//将最后vec一个元素移到删除的位置，改变vec原本最后一个元素对应的lis中的索引
			auto posVecIdx = it->second->idxVec;
			if(posVecIdx!=vec.size()-1) {
				vec[posVecIdx] = vec.back();
				vec[posVecIdx]->idxVec = posVecIdx;
			}
			vec.pop_back();
			//删除lis
			lis.erase(it->second);
		}
		//删除bst
		return bst.erase(itSt, itEd);
	}
public:
	//两端顺序删除
	void PopBack()
	{
		SeqEraseAssist(--lis.end());
	}
	void PopFront()
	{
		SeqEraseAssist(lis.begin());
	}
	//指定位置顺序删除
	SeqIter SeqErase(SeqIter itPos)
	{
		return SeqIter(SeqEraseAssist(itPos.it));
	}
	SeqIter SeqErase(SeqIter itSt, SeqIter itEd)
	{
		for(auto it= itSt.it; it!=itEd.it; ++it)
			SeqEraseAssist(it);
		return SeqIter(itEd);
	}
	//指定位置值序删除
	ValueIter ValueErase(ValueIter itPos)
	{
		return ValueIter(ValueEraseAssist(itPos.it, std::next(itPos.it)));
	}
	ValueIter ValueErase(ValueIter itSt, ValueIter itEd)
	{
		return ValueIter(ValueEraseAssist(itSt.it, itSt.it));
	}
	//查找删除
	size_t FindErase(const TyData &value)
	{
		auto prRes = bst.equal_range(value);
		auto ret = std::distance(prRes.first, prRes.second);
		ValueEraseAssist(prRes.first, prRes.second);
		return ret;
	}
	//查找
	ValueIter Find(const TyData &value)
	{
		return ValueIter(bst.find(value));
	}
	ValueConstIter Find(const TyData &value) const
	{
		return ValueConstIter(bst.find(value));
	}
	size_t Count(const TyData &value) const
	{
		return bst.count(value);
	}
	//范围查找
	std::pair<ValueIter, ValueIter> EqualRange(const TyData &value)
	{
		return std::pair<ValueIter, ValueIter>(bst.equal_range(value));
	}
	std::pair<ValueConstIter, ValueConstIter> EqualRange(const TyData &value) const
	{
		return std::pair<ValueConstIter, ValueConstIter>(bst.equal_range(value));
	}
	ValueIter LowerBound(const TyData &value)
	{
		return ValueIter(bst.lower_bound(value));
	}
	ValueConstIter LowerBound(const TyData &value) const
	{
		return ValueConstIter(bst.lower_bound(value));
	}
	ValueIter UpperBound(const TyData &value)
	{
		return ValueIter(bst.upper_bound(value));
	}
	ValueConstIter UpperBound(const TyData &value) const
	{
		return ValueConstIter(bst.upper_bound(value));
	}
	//修改值
	template<typename Ty>
	typename std::enable_if<_StruAndAlgoIsRemoveCVRefSame<Ty, TyData>::value, ValueIter
	>::type Modify(SeqIter itPos, Ty &&value)
	{
		return EmplaceModify(itPos, std::forward<Ty>(value));
	}
	template<typename ...Ty_S>
	ValueIter EmplaceModify(SeqIter itPos, Ty_S &&...arg_s)
	{
		bst.erase(itPos.it->itBst);
		itPos.it->itBst = itBst.emplace(std::piecewise_construct,
			std::forward_as_tuple(std::forward<Ty_S>(arg_s...)), std::make_tuple(itPos.it));
		return ValueIter(itPos.it->itBst);
	}
	//查找修改值
	template<typename Ty>
	typename std::enable_if<_StruAndAlgoIsRemoveCVRefSame<Ty, TyData>::value, size_t
	>::type FindModify(const TyData &valueSrc, Ty &&valueDst)
	{
		auto prRes = bst.equal_range(valueSrc);
		auto size = std::distance(prRes.first, prRes.second);
		//记录查找结果
		std::vector<ListIter> vecIt(size);
		int i = 0;
		for(auto it=prRes.first; it!=prRes.second; ++it, ++i)
			vecIt[i] = it->second;
		//删除bst
		bst.erase(prRes.first, prRes.second);
		//重建bst
		for(i=0; i!=size; ++i) {
			if(i!=size-1)
				vecIt[i]->itBst = bst.emplace((const Ty &)(valueDst), vecIt[i]);
			else
				vecIt[i]->itBst = bst.emplace(std::forward<Ty>(valueDst), vecIt[i]);
		}
		return size;
	}
	//序列自身切片插入
	template<typename TyIt>
	void SeqSplice(SeqIter itPos, TyIt itSrc)
	{
		lis.splice(lis, itPos.it, itSrc.it);
	}
	template<typename TyIt>
	void SeqSplice(SeqIter itPos, TyIt itSt, TyIt itEd)
	{
		lis.splice(lis, itPos.it, itSt.it, itEd.it);
	}
	//序列反转
	void SeqReverse()
	{
		lis.reverse();
	}
	//排序操作，默认不使用模板提供的比较类
	template<typename TyFunc= std::function<bool(const TyData &, const TyData &)>>
	void SeqSort(const TyFunc &func= std::less<TyData>())
	{
		auto lbdCom = [&](const ListStru &arg1, const ListStru &arg2) {
			return func(arg1.itBst->first, arg2.itBst->first);
		};
		lis.sort(lbdCom);
	}
	//查看向量预留空间
	size_t VectorCapacity() const
	{
		return vec.capacity();
	}
	//增加向量预留空间
	void VectorReserve(size_t size)
	{
		vec.reserve(size);
	}
	//向量收缩空间
	void VectorShrinkToFit()
	{
		vec.shrink_to_fit();
	}
	//Debug
	void Debug() const
	{
		assert(lis.size()==bst.size() && lis.size()==vec.size());
		for(auto it=lis.begin(); it!=lis.end(); ++it) {
			assert(it->itBst->second==it);
			assert(vec[it->idxVec]==it);
		}
	}
};

//带额外顺序的集合的顺序迭代器
template<typename TyData, typename TyCompare>
template<typename ListIterMy, typename MultimapIterMy>
class SequenceSet<TyData, TyCompare>::TemplateSeqIter:
	public std::iterator<std::bidirectional_iterator_tag, const TyData,
	int, const TyData *, const TyData &>
{
public://友元
	friend class SequenceSet<TyData, TyCompare>;
	friend class SeqIter;
	friend class SeqConstIter;
	friend class ValueIter;
	friend class ValueConstIter;
private://辅助类
	//对应的值序迭代器
	using ValueIterMy = TemplateValueIter<ListIterMy, MultimapIterMy>;

private://数据
	ListIterMy it;//链表迭代器

public://基本函数
	//默认构造
	TemplateSeqIter()
		= default;
	//由非常类构造常类
	TemplateSeqIter(const SeqIter &other)
	{
		operator =(other);
	}
	TemplateSeqIter &operator =(const SeqIter &other)
	{
		if((void *)this!=(void *)&other) {
			it = other.it;
		}
		return *this;
	}
	//从值序迭代器构造，不能为尾后迭代器
	TemplateSeqIter(const ValueIterMy &other)
	{
		it = other.it->second;
	}
private:
	//私有数据构造
	explicit TemplateSeqIter(ListIterMy o_it):
		it(o_it)
	{
	}
public://运算符重载
	//解引用操作
	const TyData &operator *() const
	{
		return it->itBst->first;
	}
	const TyData *operator ->() const
	{
		return &it->itBst->first;
	}
	//递增递减操作
	TemplateSeqIter &operator ++()
	{
		++ it;
		return *this;
	}
	TemplateSeqIter operator ++(int)
	{
		Iter ret = *this;
		operator ++();
		return ret;
	}
	TemplateSeqIter &operator --()
	{
		-- it;
		return *this;
	}
	TemplateSeqIter operator --(int)
	{
		Iter ret = *this;
		operator --();
		return ret;
	}
	//比较操作
	bool operator ==(const TemplateSeqIter &other) const
	{
		return it==other.it;
	}
	bool operator !=(const TemplateSeqIter &other) const
	{
		return !operator ==(other);
	}
public://成员函数
	//获取对应值序迭代器，不能在用尾后迭代器调用
	ListIterMy GetValueIter() const
	{
		return ListIterMy(it->itBst);
	}
};

//带额外顺序的集合的值序迭代器
template<typename TyData, typename TyCompare>
template<typename ListIterMy, typename MultimapIterMy>
class SequenceSet<TyData, TyCompare>::TemplateValueIter:
	public std::iterator<std::bidirectional_iterator_tag, const TyData,
	int, const TyData *, const TyData &>
{
public://友元
	friend class SequenceSet<TyData, TyCompare>;
	friend class SeqIter;
	friend class SeqConstIter;
	friend class ValueIter;
	friend class ValueConstIter;
private://辅助类型
	//对应的顺序迭代器
	using SeqIterMy = TemplateSeqIter<ListIterMy, MultimapIterMy>;

private://数据
	MultimapIterMy it;//链表迭代器

public://基本函数
	//默认构造
	TemplateValueIter()
		= default;
	//由非常类构造常类
	TemplateValueIter(const ValueIter &other)
	{
		operator =(other);
	}
	TemplateValueIter &operator =(const ValueIter &other)
	{
		if((void *)this!=(void *)&other) {
			it = other.it;
		}
		return *this;
	}
	//从顺序迭代器构造，不能为尾后迭代器
	TemplateValueIter(const SeqIterMy &other)
	{
		it = other.it->itBst;
	}
private:
	//私有数据构造
	explicit TemplateValueIter(MultimapIterMy o_it):
		it(o_it)
	{
	}
public://运算符重载
	//解引用操作
	const TyData &operator *() const
	{
		return it->first;
	}
	const TyData *operator ->() const
	{
		return &it->first;
	}
	//递增递减操作
	TemplateValueIter &operator ++()
	{
		++ it;
		return *this;
	}
	TemplateValueIter operator ++(int)
	{
		Iter ret = *this;
		operator ++();
		return ret;
	}
	TemplateValueIter &operator --()
	{
		-- it;
		return *this;
	}
	TemplateValueIter operator --(int)
	{
		Iter ret = *this;
		operator --();
		return ret;
	}
	//比较操作
	bool operator ==(const TemplateValueIter &other) const
	{
		return it==other.it;
	}
	bool operator !=(const TemplateValueIter &other) const
	{
		return !operator ==(other);
	}
public://成员函数
	//获取对应顺序迭代器，不能在用尾后迭代器调用
	SeqIterMy GetSeqIter() const
	{
		return SeqIterMy(it->second);
	}
};



//只能添加元素的图，支持void数据类型
//内部为二维数组实现
//节点迭代器允许使用整形索引构造
//Clear和Assign会导致迭代器失效
//容器复制、移动、交换后迭代器均指向旧容器，允许迭代器更新容器指针
//想结构设计
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
	int, TyNodeDataMy *, TyNodeDataMy &>
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
		return &graph->vecNode[idxNode].hold;
	}
};
//点迭代器模板辅助类特化
template<typename TyNodeData, typename TyEdgeData>
template<typename GraphTypeMy, typename TyNodeDataMy>
class AddonlyGraph<TyNodeData, TyEdgeData>::TemplateNodeIterAssist<
	GraphTypeMy, TyNodeDataMy, void
>: public std::iterator<std::bidirectional_iterator_tag, TyNodeDataMy,
	int, void, void>
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
		TemplateNodeIter ret = *this;
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
		TemplateNodeIter ret = *this;
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
	int, TyEdgeDataMy *, TyEdgeDataMy &>
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
		return &graph->vecNode[idxNode].vecEdge[idxEdge].hold;
	}
};
//边迭代器模板辅助类特化
template<typename TyNodeData, typename TyEdgeData>
template<typename GraphTypeMy, typename TyEdgeDataMy>
class AddonlyGraph<TyNodeData, TyEdgeData>::TemplateEdgeIterAssist<
	GraphTypeMy, TyEdgeDataMy, void
>: public std::iterator<std::bidirectional_iterator_tag, TyEdgeDataMy,
	int, void, void>
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
		TemplateEdgeIter ret = *this;
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
		TemplateEdgeIter ret = *this;
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

