#pragma once

//���ݽṹ���㷨����
//��ʹ��ArgsTupleʱ��Ҫ�Ƚ�TypeExtend�������

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



//���Բ����Ƿ�Ϸ���
template<typename ...>
struct _StruAndAlgoParamValidTester
{
	typedef void type;
};

//��ȥ��������ȥ�������ױ�����
template<typename Ty>
struct _StruAndAlgoRemoveCVRef
{
	using type = typename std::remove_cv<typename std::remove_reference<Ty>::type>::type;
};

//ǰ��ȥ�����ó����ױ���Ƿ�Ϊ����
template<typename TySrc, typename TyDst>
struct _StruAndAlgoIsRemoveCVRefSame:
	std::is_same<typename _StruAndAlgoRemoveCVRef<TySrc>::type, TyDst>
{
};

//�����Ƿ�Ϊ������
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

//�洢ĳֵ���ͣ����void�����ػ�
template<typename Ty>
struct _StruAndAlgoValueHold
{
	Ty hold;
public:
	_StruAndAlgoValueHold()
	{
	}
	//ģ�幹��
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

//������������
template<typename ...Ty_S>
class ArgsTuple;

//�������Ƚϸ�����
#define _STRUANDALGO_FLOAT_DEFAULT_ULP 10
#define _STRUANDALGO_IS_FLOAT_TYPE(Ty1, Ty2) \
	(std::is_floating_point<Ty1>::value && std::is_floating_point<Ty2>::value)
#define _STRUANDALGO_FLOAT_COMMON_EPSILON(Ty1, Ty2) \
	(std::max(std::numeric_limits<Ty1>::epsilon(), std::numeric_limits<Ty2>::epsilon()))
#define _STRUANDALGO_FLOAT_COMMON_MIN(Ty1, Ty2) \
	(std::max(std::numeric_limits<Ty1>::min(), std::numeric_limits<Ty2>::min()))
#define _STRUANDALGO_FLOAT_ABS(num) ((num)>=0? (num):-(num))
//�������Ƚ���ȣ���֤�߼���ϵ
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
//�������Ƚ�С�ڣ���֤�߼���ϵ
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_STRUANDALGO_IS_FLOAT_TYPE(Ty1, Ty2), bool
>::type _StruAndAlgoFloatLT(Ty1 num1, Ty2 num2, bool bAbs= false, int ulp= _STRUANDALGO_FLOAT_DEFAULT_ULP)
{
	return num1<num2 && !_StruAndAlgoFloatEQ(num1, num2, bAbs, ulp);
}
//�������Ƚ�С�ڵ��ڣ���֤�߼���ϵ
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_STRUANDALGO_IS_FLOAT_TYPE(Ty1, Ty2), bool
>::type _StruAndAlgoFloatLTE(Ty1 num1, Ty2 num2, bool bAbs= false, int ulp= _STRUANDALGO_FLOAT_DEFAULT_ULP)
{
	return _StruAndAlgoFloatLT(num1, num2, bAbs, ulp) || _StruAndAlgoFloatEQ(num1, num2, bAbs, ulp);
}
//�������Ƚϴ��ڣ���֤�߼���ϵ
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_STRUANDALGO_IS_FLOAT_TYPE(Ty1, Ty2), bool
>::type _StruAndAlgoFloatGT(Ty1 num1, Ty2 num2, bool bAbs= false, int ulp= _STRUANDALGO_FLOAT_DEFAULT_ULP)
{
	return !(_StruAndAlgoFloatLT(num1, num2, bAbs, ulp) || _StruAndAlgoFloatEQ(num1, num2, bAbs, ulp));
}
//�������Ƚϴ��ڵ��ڣ���֤�߼���ϵ
template<typename Ty1, typename Ty2>
inline constexpr typename std::enable_if<_STRUANDALGO_IS_FLOAT_TYPE(Ty1, Ty2), bool
>::type _StruAndAlgoFloatGTE(Ty1 num1, Ty2 num2, bool bAbs= false, int ulp= _STRUANDALGO_FLOAT_DEFAULT_ULP)
{
	return !_StruAndAlgoFloatLT(num1, num2, bAbs, ulp);
}
//�������Ƚϲ����ڣ���֤�߼���ϵ
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
//���ݽṹ����
//************

//֧���±����ɾ�������������������ṩ֧���±���ʵ���������string,vector��deque
//ʵ����Ϊɾ��Ԫ�غ�����Ԫ���ƶ���ǰ��
//���Կ���ɾ��ʱ���ĵ�������ʧЧ����ɾ���ĵ�����ָ��ԭ�����ĵ���������
template<typename TyContainer>
class FastEraseVector:
	public TyContainer
{
public://��������
	//Ĭ�Ϲ��캯��
	FastEraseVector()
		= default;
	//�̳й��캯��
	using TyContainer::TyContainer;
	//ʡ�Ժϳ�5����

public://��Ա����
	//����������ɾ���������ƶ��ĵ���������
	size_t IterFastErase(typename TyContainer::iterator itPos)
	{
		size_t ret = 0;
		auto itBack = --end();
		//����Ϊ���һ��Ԫ�����ƶ����
		if(itPos!=itBack) {
			*itPos = std::move(*itBack);
			ret = 1;
		}
		pop_back();
		return ret;
	}
	//��������Χ����ɾ���������ƶ��ĵ���������
	size_t IterFastErase(typename TyContainer::iterator itSt,
		typename TyContainer::iterator itEd)
	{
		size_t ret = 0;
		//����Ϊ���һ��Ԫ�����ƶ����
		if(itEd!=end()) {
			ret = (size_t)std::min(itEd-itSt, end()-itEd);
			for(auto itSrc=end()-ret, itDst=itSt; itSrc!=end(); ++itSrc, ++itDst)
				*itDst = std::move(*itSrc);
		}
		resize(end()-(itEd-itSt)-begin());
		return ret;
	}
	//�±����ɾ���������ƶ��ĵ���������
	size_t IndexFastErase(size_t idx)
	{
		return IterFastErase(begin()+idx);
	}
	//�±귶Χ����ɾ���������ƶ��ĵ���������
	size_t IndexFastErase(size_t idxSt, size_t idxEd)
	{
		return IterFastErase(begin()+idxSt, begin()+idxEd);
	}
};




//������˳��ļ��ϣ�֧��˳����ʺ�ֵ����ʡ���չ����������ʹ���
//ɾ����ӦԪ�ػ�ʹ��Ӧ�����ֵ�����ʧЧ������Ԫ�ص�ֵ��ʹ��Ӧֵ�������ʧЧ
//�Է��������
template<typename TyData, typename TyCompare= std::less<TyData>>
class SequenceSet
{
private://�������Ͷ���
	struct ListStru;
	//list������
	using ListIter = typename std::list<ListStru>::iterator;
	using ListConstIter = typename std::list<ListStru>::const_iterator;
	//multimap������
	using MultimapIter = typename std::multimap<TyData, ListIter, TyCompare>::iterator;
	using MultimapConstIter = typename std::multimap<TyData, ListIter, TyCompare>::const_iterator;
	//list������ݽṹ��
	struct ListStru
	{
		size_t idxVec;//���е�����
		MultimapIter itBst;//����ĵ�����
	public:
		ListStru()
			= default;
		ListStru(size_t o_idxVec):
			idxVec(o_idxVec)
		{
		}
	};

private://������ģ�嶨��
	//˳�������ģ��
	template<typename ListIterMy, typename MultimapIterMy>
	class TemplateSeqIter;
	//ֵ�������ģ��
	template<typename ListIterMy, typename MultimapIterMy>
	class TemplateValueIter;

private://����������ָ��
	//˳�������
	using SeqIter = TemplateSeqIter<ListIter, MultimapIter>;
	//˳�򳣵�����
	using SeqConstIter = TemplateSeqIter<ListConstIter, MultimapConstIter>;
	//ֵ����ʵĵ�����
	using ValueIter = TemplateValueIter<ListIter, MultimapIter>;
	//ֵ����ʵĵ�����
	using ValueConstIter = TemplateValueIter<ListConstIter, MultimapConstIter>;

public://��Ԫ
	friend class SeqIter;
	friend class SeqConstIter;
	friend class ValueIter;
	friend class ValueConstIter;

public://��������
	using DataType = TyData;

private://��Ա����
	std::list<ListStru> lis;//����ṹ��������ṹ�����������������
	std::multimap<TyData, ListIter, TyCompare> bst;//���ṹ�����ֵ�����������
	std::vector<ListIter> vec;//������������������

public://��������
	//Ĭ�Ϲ���
	explicit SequenceSet(const TyCompare &func= TyCompare()):
		bst(func)
	{
	}
	//��ֵ���캯��
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
public://���������
	//�б�ֵ
	SequenceSet &operator =(std::initializer_list<TyData> init)
	{
		Assign(init);
		return *this;
	}
	//�Ƚϲ�������˳����ֵ���
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
public://��Ա����
	//��ֵ����
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
	//�Ƚϲ�����С�ڵ��ڴ��ڷֱ𷵻�-1,0,1
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
			//�жϽ�������
			if(itMe==this->end()) {
				if(itOther==other.end())
					return 0;
				else
					return -1;
			}
			else
				return 1;
			//�жϴ�С
			if(funcLess(*itMe, *itOther))
				return -1;
			else if(!(funcEqual(*itMe, *itOther)))
				return 1;
		}
	}
	//˳�����������
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
	//ֵ�����������
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
	//�������������
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
	//����˳����βԪ��
	const TyData &SeqFront() const
	{
		return lis.front().itBst->first;
	}
	const TyData &SeqBack() const
	{
		return lis.back().itBst->first;
	}
	//����ֵ����βԪ��
	const TyData &ValueFront() const
	{
		return bst.begin()->first;
	}
	const TyData &ValueBack() const
	{
		return (--bst.end())->first;
	}
	//�������Ԫ��
	const TyData &RandomElement(size_t idx) const
	{
		return vec[idx]->itBst->first;
	}
	//�鿴����
	bool Empty() const
	{
		return lis.empty();
	}
	size_t Size() const
	{
		return lis.size();
	}
	//���
	void Clear()
	{
		lis.clear();
		bst.clear();
		vec.clear();
	}
	//�ı�����
	void Resize(size_t size, const TyData &value)
	{
		while(Size()>size)
			PopBack();
		while(Size()<size)
			PushBack(value);
	}
private:
	//���븨������
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
	//���˲���
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
	//��ָ��λ��ǰ���룬���ص�һ��Ԫ�صĵ�����
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
	//˳��ɾ����������
	ListIter SeqEraseAssist(ListIter itPos)
	{
		//�����vecһ��Ԫ���Ƶ�ɾ����λ�ã��ı�vecԭ�����һ��Ԫ�ض�Ӧ��lis�е�����
		auto posVecIdx = itPos->idxVec;
		if(posVecIdx!=vec.size()-1) {
			vec[posVecIdx] = vec.back();
			vec[posVecIdx]->idxVec = posVecIdx;
		}
		vec.pop_back();
		//ɾ��lis��bst
		bst.erase(itPos->itBst);
		return lis.erase(itPos);
	}
	//ֵ��ɾ����������
	MultimapIter ValueEraseAssist(MultimapIter itSt, MultimapIter itEd)
	{
		for(auto it=itSt; it!=itEd; ++it) {
			//�����vecһ��Ԫ���Ƶ�ɾ����λ�ã��ı�vecԭ�����һ��Ԫ�ض�Ӧ��lis�е�����
			auto posVecIdx = it->second->idxVec;
			if(posVecIdx!=vec.size()-1) {
				vec[posVecIdx] = vec.back();
				vec[posVecIdx]->idxVec = posVecIdx;
			}
			vec.pop_back();
			//ɾ��lis
			lis.erase(it->second);
		}
		//ɾ��bst
		return bst.erase(itSt, itEd);
	}
public:
	//����˳��ɾ��
	void PopBack()
	{
		SeqEraseAssist(--lis.end());
	}
	void PopFront()
	{
		SeqEraseAssist(lis.begin());
	}
	//ָ��λ��˳��ɾ��
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
	//ָ��λ��ֵ��ɾ��
	ValueIter ValueErase(ValueIter itPos)
	{
		return ValueIter(ValueEraseAssist(itPos.it, std::next(itPos.it)));
	}
	ValueIter ValueErase(ValueIter itSt, ValueIter itEd)
	{
		return ValueIter(ValueEraseAssist(itSt.it, itSt.it));
	}
	//����ɾ��
	size_t FindErase(const TyData &value)
	{
		auto prRes = bst.equal_range(value);
		auto ret = std::distance(prRes.first, prRes.second);
		ValueEraseAssist(prRes.first, prRes.second);
		return ret;
	}
	//����
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
	//��Χ����
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
	//�޸�ֵ
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
	//�����޸�ֵ
	template<typename Ty>
	typename std::enable_if<_StruAndAlgoIsRemoveCVRefSame<Ty, TyData>::value, size_t
	>::type FindModify(const TyData &valueSrc, Ty &&valueDst)
	{
		auto prRes = bst.equal_range(valueSrc);
		auto size = std::distance(prRes.first, prRes.second);
		//��¼���ҽ��
		std::vector<ListIter> vecIt(size);
		int i = 0;
		for(auto it=prRes.first; it!=prRes.second; ++it, ++i)
			vecIt[i] = it->second;
		//ɾ��bst
		bst.erase(prRes.first, prRes.second);
		//�ؽ�bst
		for(i=0; i!=size; ++i) {
			if(i!=size-1)
				vecIt[i]->itBst = bst.emplace((const Ty &)(valueDst), vecIt[i]);
			else
				vecIt[i]->itBst = bst.emplace(std::forward<Ty>(valueDst), vecIt[i]);
		}
		return size;
	}
	//����������Ƭ����
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
	//���з�ת
	void SeqReverse()
	{
		lis.reverse();
	}
	//���������Ĭ�ϲ�ʹ��ģ���ṩ�ıȽ���
	template<typename TyFunc= std::function<bool(const TyData &, const TyData &)>>
	void SeqSort(const TyFunc &func= std::less<TyData>())
	{
		auto lbdCom = [&](const ListStru &arg1, const ListStru &arg2) {
			return func(arg1.itBst->first, arg2.itBst->first);
		};
		lis.sort(lbdCom);
	}
	//�鿴����Ԥ���ռ�
	size_t VectorCapacity() const
	{
		return vec.capacity();
	}
	//��������Ԥ���ռ�
	void VectorReserve(size_t size)
	{
		vec.reserve(size);
	}
	//���������ռ�
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

//������˳��ļ��ϵ�˳�������
template<typename TyData, typename TyCompare>
template<typename ListIterMy, typename MultimapIterMy>
class SequenceSet<TyData, TyCompare>::TemplateSeqIter:
	public std::iterator<std::bidirectional_iterator_tag, const TyData,
	int, const TyData *, const TyData &>
{
public://��Ԫ
	friend class SequenceSet<TyData, TyCompare>;
	friend class SeqIter;
	friend class SeqConstIter;
	friend class ValueIter;
	friend class ValueConstIter;
private://������
	//��Ӧ��ֵ�������
	using ValueIterMy = TemplateValueIter<ListIterMy, MultimapIterMy>;

private://����
	ListIterMy it;//���������

public://��������
	//Ĭ�Ϲ���
	TemplateSeqIter()
		= default;
	//�ɷǳ��๹�쳣��
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
	//��ֵ����������죬����Ϊβ�������
	TemplateSeqIter(const ValueIterMy &other)
	{
		it = other.it->second;
	}
private:
	//˽�����ݹ���
	explicit TemplateSeqIter(ListIterMy o_it):
		it(o_it)
	{
	}
public://���������
	//�����ò���
	const TyData &operator *() const
	{
		return it->itBst->first;
	}
	const TyData *operator ->() const
	{
		return &it->itBst->first;
	}
	//�����ݼ�����
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
	//�Ƚϲ���
	bool operator ==(const TemplateSeqIter &other) const
	{
		return it==other.it;
	}
	bool operator !=(const TemplateSeqIter &other) const
	{
		return !operator ==(other);
	}
public://��Ա����
	//��ȡ��Ӧֵ�����������������β�����������
	ListIterMy GetValueIter() const
	{
		return ListIterMy(it->itBst);
	}
};

//������˳��ļ��ϵ�ֵ�������
template<typename TyData, typename TyCompare>
template<typename ListIterMy, typename MultimapIterMy>
class SequenceSet<TyData, TyCompare>::TemplateValueIter:
	public std::iterator<std::bidirectional_iterator_tag, const TyData,
	int, const TyData *, const TyData &>
{
public://��Ԫ
	friend class SequenceSet<TyData, TyCompare>;
	friend class SeqIter;
	friend class SeqConstIter;
	friend class ValueIter;
	friend class ValueConstIter;
private://��������
	//��Ӧ��˳�������
	using SeqIterMy = TemplateSeqIter<ListIterMy, MultimapIterMy>;

private://����
	MultimapIterMy it;//���������

public://��������
	//Ĭ�Ϲ���
	TemplateValueIter()
		= default;
	//�ɷǳ��๹�쳣��
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
	//��˳����������죬����Ϊβ�������
	TemplateValueIter(const SeqIterMy &other)
	{
		it = other.it->itBst;
	}
private:
	//˽�����ݹ���
	explicit TemplateValueIter(MultimapIterMy o_it):
		it(o_it)
	{
	}
public://���������
	//�����ò���
	const TyData &operator *() const
	{
		return it->first;
	}
	const TyData *operator ->() const
	{
		return &it->first;
	}
	//�����ݼ�����
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
	//�Ƚϲ���
	bool operator ==(const TemplateValueIter &other) const
	{
		return it==other.it;
	}
	bool operator !=(const TemplateValueIter &other) const
	{
		return !operator ==(other);
	}
public://��Ա����
	//��ȡ��Ӧ˳�����������������β�����������
	SeqIterMy GetSeqIter() const
	{
		return SeqIterMy(it->second);
	}
};



//ֻ�����Ԫ�ص�ͼ��֧��void��������
//�ڲ�Ϊ��ά����ʵ��
//�ڵ����������ʹ��������������
//Clear��Assign�ᵼ�µ�����ʧЧ
//�������ơ��ƶ����������������ָ��������������������������ָ��
//��ṹ���
template<typename TyNodeData, typename TyEdgeData>
class AddonlyGraph
{
private:
	//�߽ṹ��
	struct EdgeStruct:
		public _StruAndAlgoValueHold<TyEdgeData>
	{
		size_t index;//�߱��
		//������������ģ������Ĺ���
		template<typename ...Tys>
		EdgeStruct(size_t o_index, Tys &&...args):
			_StruAndAlgoValueHold<TyEdgeData>(std::forward<Tys>(args)...), index(o_index)
		{
		}
	};
	//�ڵ�ṹ��
	struct NodeStruct:
		public _StruAndAlgoValueHold<TyNodeData>
	{
		std::vector<EdgeStruct> vecEdge;//��������
		//������ֵ����ֵ�����ݹ���
		template<typename ...Tys>
		NodeStruct(Tys &&...args):
			_StruAndAlgoValueHold<TyNodeData>(std::forward<Tys>(args)...)
		{
		}
	};

private://�������Ͷ���
	using GraphType = AddonlyGraph<TyNodeData, TyEdgeData>;//ͼ����
public:
	using NodeDataType = TyNodeData;//�ڵ���������
	using EdgeDataType = TyEdgeData;//����������

private://������ģ�嶨��
	//�ڵ������ģ�帨����
	template<typename GraphTypeMy, typename TyNodeDataMy, typename TyNodeDataPure>
	class TemplateNodeIterAssist;
	//�ڵ������ģ��
	template<typename GraphTypeMy, typename TyNodeDataMy, typename TyEdgeDataMy>
	class TemplateNodeIter;
	//�ߵ�����ģ�帨����
	template<typename GraphTypeMy, typename TyNodeDataMy, typename TyNodeDataPure>
	class TemplateEdgeIterAssist;
	//�ߵ�����ģ��
	template<typename GraphTypeMy, typename TyNodeDataMy, typename TyEdgeDataMy>
	class TemplateEdgeIter;

private://������ģ��ָ�����Ͷ���
	//�ڵ������������ָ��
	using NodeIterAssist = TemplateNodeIter<GraphType, TyNodeData, TyNodeData>;
	//�ڵ㳣������������ָ��
	using NodeConstIterAssist = TemplateNodeIter<
		const GraphType, const TyNodeData, TyNodeData>;
	//�ߵ�����������ָ��
	using EdgeIterAssist = TemplateEdgeIter<GraphType, TyEdgeData, TyEdgeData>;
	//�߳�������������ָ��
	using EdgeConstIterAssist = TemplateEdgeIter<
		const GraphType, const TyEdgeData, TyEdgeData>;
public:
	//�ڵ��������ָ��
	using NodeIter = TemplateNodeIter<GraphType, TyNodeData, TyEdgeData>;
	//�ڵ㳣��������ָ��
	using NodeConstIter = TemplateNodeIter<
		const GraphType, const TyNodeData, const TyEdgeData>;
	//�ߵ�������ָ��
	using EdgeIter = TemplateEdgeIter<GraphType, TyNodeData, TyEdgeData>;
	//�߳���������ָ��
	using EdgeConstIter = TemplateEdgeIter<
		const GraphType, const TyNodeData, const TyEdgeData>;

public://��Ԫ����
	friend class NodeIter;
	friend class NodeIterAssist;
	friend class NodeConstIter;
	friend class NodeConstIterAssist;
	friend class EdgeIter;
	friend class EdgeIterAssist;
	friend class EdgeConstIter;
	friend class EdgeConstIterAssist;

private://��Ա����
	std::vector<NodeStruct> vecNode;//�ڵ�����

public://��������
	//Ĭ�Ϲ���
	AddonlyGraph()
		= default;
	//������ݹ���
	template<typename ...Ty_S>
	explicit AddonlyGraph(size_t size, const Ty_S &...arg_s)
	{
		Assign(size, arg_s...);
	}

public://��Ա����
	//��ֵ����
	template<typename ...Ty_S>
	AddonlyGraph &Assign(size_t size, const Ty_S &...arg_s)
	{
		vecNode.assign(size, NodeStruct(arg_s...));
		return *this;
	}
	//��ӽڵ�
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
	//��ӱ�
	template<typename TyEdgeDataMy>
	typename std::enable_if<std::is_convertible<TyEdgeDataMy &&, TyEdgeData>::value, EdgeIter
	>::type AddEdge(NodeIter fromNode, NodeIter toNode, TyEdgeDataMy &&data)
	{
		return EmplaceEdge(fromNode, toNode, std::forward<TyEdgeDataMy>(data));
	}
	//�����
	template<typename ...Tys>
	EdgeIter EmplaceEdge(NodeIter fromNode, NodeIter toNode, Tys &&...args)
	{
		vecNode[fromNode.idxNode].vecEdge.emplace_back(
			toNode.idxNode, std::forward<Tys>(args)...);
		return EdgeIter(this, fromNode.idxNode, vecNode[fromNode.idxNode].vecEdge.size()-1);
	}
	//���˫���
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
	//����˫��߸�������
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
	//����˫���
	template<typename TyArgsGo, typename TyArgsRev>
	std::pair<EdgeIter, EdgeIter> EmplaceDoublyEdge(NodeIter fromNode,
		NodeIter toNode, TyArgsGo &&argsTupGo, TyArgsRev &&argsTupRev)
	{
		return EmplaceDoublyEdgeAssist(fromNode, toNode,
			ArgsTupleRefCat(std::forward<TyArgsGo>(argsTupGo)),
			ArgsTupleRefCat(std::forward<TyArgsRev>(argsTupRev)));
	}
	//���ͼ
	void Clear()
	{
		vecNode.clear();
	}
	//�����ڵ�
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
	//�ڵ���
	size_t NodeSize() const
	{
		return vecNode.size();
	}
};

//�������ģ�帨����
template<typename TyNodeData, typename TyEdgeData>
template<typename GraphTypeMy, typename TyNodeDataMy, typename TyNodeDataPure>
class AddonlyGraph<TyNodeData, TyEdgeData>::TemplateNodeIterAssist:
	public std::iterator<std::bidirectional_iterator_tag, TyNodeDataMy,
	int, TyNodeDataMy *, TyNodeDataMy &>
{
protected://��Ա����
	GraphTypeMy *graph;//ͼָ��
	size_t idxNode;//�ڵ�

public://��������
	//Ĭ�Ϲ���
	TemplateNodeIterAssist():
		graph(nullptr), idxNode(0)
	{
	}
	//���캯���������ⲿ���й���
	explicit TemplateNodeIterAssist(GraphTypeMy *o_graph, size_t o_idxNode):
		graph(o_graph), idxNode(o_idxNode)
	{
	}
public://���������
	//�����ò���
	TyNodeDataMy &operator *() const
	{
		return graph->vecNode[idxNode].hold;
	}
	TyNodeDataMy *operator ->() const
	{
		return &graph->vecNode[idxNode].hold;
	}
};
//�������ģ�帨�����ػ�
template<typename TyNodeData, typename TyEdgeData>
template<typename GraphTypeMy, typename TyNodeDataMy>
class AddonlyGraph<TyNodeData, TyEdgeData>::TemplateNodeIterAssist<
	GraphTypeMy, TyNodeDataMy, void
>: public std::iterator<std::bidirectional_iterator_tag, TyNodeDataMy,
	int, void, void>
{
protected://��Ա����
	GraphTypeMy *graph;//ͼָ��
	size_t idxNode;//�ڵ�

public://��������
	//Ĭ�Ϲ���
	TemplateNodeIterAssist():
		graph(nullptr), idxNode(0)
	{
	}
	//���캯���������ⲿ���й���
	explicit TemplateNodeIterAssist(GraphTypeMy *o_graph, size_t o_idxNode):
		graph(o_graph), idxNode(o_idxNode)
	{
	}
};

//�ڵ������ģ����
template<typename TyNodeData, typename TyEdgeData>
template<typename GraphTypeMy, typename TyNodeDataMy, typename TyEdgeDataMy>
class AddonlyGraph<TyNodeData, TyEdgeData>::TemplateNodeIter:
	public TemplateNodeIterAssist<GraphTypeMy, TyNodeDataMy, TyNodeData>
{
public://��Ԫ
	friend class AddonlyGraph<TyNodeData, TyEdgeData>;
	friend class NodeIter;
	friend class NodeConstIter;
	friend class EdgeIter;
	friend class EdgeConstIter;
private://��������
	//��Ӧ�ıߵ�����
	using EdgeIterMy = TemplateEdgeIter<GraphTypeMy, TyNodeDataMy, TyEdgeDataMy>;

public://��������
	//Ĭ�Ϲ���
	TemplateNodeIter()
		= default;
	//�̳и�����Ĺ��캯��
	using TemplateNodeIterAssist::TemplateNodeIterAssist;
	//�ɷǳ��๹�쳣��
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
public://���������
	//�����ݼ�����
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
	//�Ƚϲ���
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
public://��Ա����
	//���ߵ�������
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
	//������
	size_t EdgeSize() const
	{
		return graph->vecNode[idxNode].size();
	}
	//�ı�ָ�����
	void ChangePointer(GraphTypeMy *o_graph)
	{
		graph = o_graph;
	}
};

//�ߴ���ģ�帨����
template<typename TyNodeData, typename TyEdgeData>
template<typename GraphTypeMy, typename TyEdgeDataMy, typename TyEdgeDataPure>
class AddonlyGraph<TyNodeData, TyEdgeData>::TemplateEdgeIterAssist:
	public std::iterator<std::bidirectional_iterator_tag, TyEdgeDataMy,
	int, TyEdgeDataMy *, TyEdgeDataMy &>
{
protected://��Ա����
	GraphTypeMy *graph;//ͼָ��
	size_t idxNode;//�ڵ����
	size_t idxEdge;//�����

public://��������
	//Ĭ�Ϲ���
	TemplateEdgeIterAssist():
		graph(nullptr), idxNode(0), idxEdge(0)
	{
	}
protected:
	//���캯�����������ⲿ���й���
	explicit TemplateEdgeIterAssist(GraphTypeMy *o_graph, size_t o_idxNode, size_t o_idxEdge):
		graph(o_graph), idxNode(o_idxNode), idxEdge(o_idxEdge)
	{
	}
public://���������
	//�����ò���
	TyEdgeDataMy &operator *() const
	{
		return graph->vecNode[idxNode].vecEdge[idxEdge].hold;
	}
	TyEdgeDataMy *operator ->() const
	{
		return &graph->vecNode[idxNode].vecEdge[idxEdge].hold;
	}
};
//�ߵ�����ģ�帨�����ػ�
template<typename TyNodeData, typename TyEdgeData>
template<typename GraphTypeMy, typename TyEdgeDataMy>
class AddonlyGraph<TyNodeData, TyEdgeData>::TemplateEdgeIterAssist<
	GraphTypeMy, TyEdgeDataMy, void
>: public std::iterator<std::bidirectional_iterator_tag, TyEdgeDataMy,
	int, void, void>
{
protected://��Ա����
	GraphTypeMy *graph;//ͼָ��
	size_t idxNode;//�ڵ����
	size_t idxEdge;//�����

public://��������
	//Ĭ�Ϲ���
	TemplateEdgeIterAssist():
		graph(nullptr), idxNode(0), idxEdge(0)
	{
	}
protected:
	//���캯�����������ⲿ���й���
	explicit TemplateEdgeIterAssist(GraphTypeMy *o_graph, size_t o_idxNode, size_t o_idxEdge):
		graph(o_graph), idxNode(o_idxNode), idxEdge(o_idxEdge)
	{
	}
};

//�ߵ�����ģ����
template<typename TyNodeData, typename TyEdgeData>
template<typename GraphTypeMy, typename TyNodeDataMy, typename TyEdgeDataMy>
class AddonlyGraph<TyNodeData, TyEdgeData>::TemplateEdgeIter:
	public TemplateEdgeIterAssist<GraphTypeMy, TyEdgeDataMy, TyEdgeData>
{
public://��Ԫ
	friend class AddonlyGraph<TyNodeData, TyEdgeData>;
	friend class NodeIter;
	friend class NodeConstIter;
	friend class EdgeIter;
	friend class EdgeConstIter;
private://��������
	//��Ӧ�Ľڵ������
	using NodeIterMy = TemplateNodeIter<GraphTypeMy, TyNodeDataMy, TyEdgeDataMy>;

public://��������
	//�̳и�����Ĺ��캯��
	using TemplateEdgeIterAssist::TemplateEdgeIterAssist;
	//�ɷǳ��๹�쳣��
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
public://���������
	//�����ݼ�����
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
	//�Ƚϲ���
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
public://��Ա����
	//��ȡ���ӽڵ�
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


/*ͼ��������

1��ֻ����ӵ�ͼ��
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

2��ֻ����ӵ�ͼ�����Բ鿴��Դ�ߣ�
ȫ��1����
NodeIter::EdgeFromBegin
NodeIter::EdgeFromEnd

3��ֻ����ӵ�ͼ�����Ը���������������Դ
ȫ��1����
GetEdge

4��ֻ����ӵ�ͼ��������������
ȫ��2��3����

5�������޸ĵ�ͼ
ȫ��1����
ȫ��2����
DelNode
DelEdge

6����5����������
3���ܣ���û���

*/




//********
//�㷨����
//********

//���ټ����ݴΣ�ע�ⷵ�����ͺͻ�������ͬ
template<typename Ty>
inline constexpr Ty FastPower(Ty base, int power)
{
	return power<0 ? 0
		: power==0 ? 1
		: (power&1)==0 ? FastPower(base*base, power>>1)
		: base*FastPower(base*base, power>>1);
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



//���ɺ�һ��������������Ƿ�ͷ
//Ҫ��˫����������ɵ�������ȱȽ�ֵ����
template<typename TyIt, typename Ty>
bool NextCombination(TyIt st, TyIt ed, Ty rangeSt, Ty rangeEd)
{
	auto it= ed, it1= it;
	//����ֵ��϶
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
	//�������������
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

//���ɺ�һ������������������Ƿ�ͷ
//Ҫ��˫����������ɵ�������ȱȽ�ֵ����
template<typename TyIt, typename Ty>
bool NextCombinationPermutation(TyIt st, TyIt ed, Ty rangeSt, Ty rangeEd)
{
	if(!std::next_permutation(st, ed)) {
		if(!NextCombination(st, ed, rangeSt, rangeEd))
			return false;
	}
	return true;
}



//�����ν��״̬
enum class SimplexStatus
{
	result, infeasible, unbounded,//�н�����޽⣬�޽�
};

//�����θ�����ת
template<typename Ty>
inline void _SimplexAssistPivot(std::vector<std::vector<Ty>> &aMatrix, std::vector<Ty> &bVector,
	std::vector<Ty> &cVector, Ty &vValue, std::vector<int> &bSet, std::vector<int> &nSet,
	int lIndex, int eIndex)
{
	int mNum= (int)bVector.size(), nNum= (int)cVector.size();//Լ������������
	//ת����������������һ��
	aMatrix[lIndex][eIndex] = 1/aMatrix[lIndex][eIndex];
	bVector[lIndex] *= aMatrix[lIndex][eIndex];
	for(int j=0; j<nNum; ++j) {
		if(j==eIndex)
			continue;
		aMatrix[lIndex][j] *= aMatrix[lIndex][eIndex];
	}
	//ת������������
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
	//ת��Ŀ�꺯��
	vValue += cVector[eIndex]*bVector[lIndex];
	for(int j=0; j<nNum; ++j) {
		if(j==eIndex)
			continue;
		cVector[j] -= cVector[eIndex]*aMatrix[lIndex][j];
	}
	cVector[eIndex] *= -aMatrix[lIndex][eIndex];
	//ת������
	std::swap(bSet[lIndex], nSet[eIndex]);
}

//�����θ����㷨ѭ������
template<typename Ty>
inline SimplexStatus _SimplexAssistLoop(std::vector<std::vector<Ty>> &aMatrix, std::vector<Ty> &bVector,
	std::vector<Ty> &cVector, Ty &vValue, std::vector<int> &bSet, std::vector<int> &nSet, int ulp)
{
	int mNum= (int)bVector.size(), nNum= (int)cVector.size();//Լ������������
	//������ѭ��
	while(true) {
		//�һ��뻻������ѭ��
		bool bFirst = true;
		int lIndex, eIndex;
		while(true) {
			//�һ������
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
					//�˻�ʱѡ�����С
					if(_StruAndAlgoFloatGT(cVector[j], 0.0, true, ulp) && (eIndex==-1 || nSet[j]<nSet[eIndex])) {
						eIndex = j;
						maxC = cVector[j];
					}
				}
			}
			if(_StruAndAlgoFloatLTE(maxC, 0.0, true, ulp))
				return SimplexStatus::result;
			//�һ�������
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
			//�ж��޽�
			if(lIndex==-1) {
				vValue = std::numeric_limits<Ty>::infinity();
				return SimplexStatus::unbounded;
			}
			//�˻������ѭ��
			if(bFirst && _StruAndAlgoFloatEQ(minB, 0.0, true, ulp))
				bFirst = false;
			else
				break;
		}
		//��ת
		_SimplexAssistPivot(aMatrix, bVector, cVector, vValue, bSet, nSet, lIndex, eIndex);
	}
}

//�����θ�����ʼ��
template<typename Ty>
inline SimplexStatus _SimplexAssistInit(std::vector<std::vector<Ty>> &aMatrix, std::vector<Ty> &bVector,
	std::vector<Ty> &cVector, Ty &vValue, std::vector<int> &bSet, std::vector<int> &nSet, int ulp)
{
	int mNum= (int)bVector.size(), nNum= (int)cVector.size();//Լ������������
	//�жϻ����������
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
	//���츨���滮
	Ty vValue2 = 0;
	std::vector<Ty> cVector2(nNum+1, 0);
	cVector2[nNum] = -1;
	for(int i=0; i<mNum; ++i)
		aMatrix[i].push_back(-1);
	nSet.push_back(-1);
	//��ʼ��ת�����
	_SimplexAssistPivot(aMatrix, bVector, cVector2, vValue2, bSet, nSet, lIndex, nNum);
	SimplexStatus status = _SimplexAssistLoop(aMatrix, bVector, cVector2, vValue2, bSet, nSet, ulp);
	assert(status==SimplexStatus::result);
	//�жϿ�����
	status = _StruAndAlgoFloatEQ(vValue2, 0.0, true, ulp) ?
		SimplexStatus::result : SimplexStatus::infeasible;
	//����ת��������
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
	//������������
	if(eIndex!=nNum) {
		for(int i=0; i<mNum; ++i)
			std::swap(aMatrix[i][eIndex], aMatrix[i][nNum]);
		std::swap(nSet[eIndex], nSet[nNum]);
	}
	//ȥ����������
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

//�����η���ģ��Ϊ������
//����Ϊ��׼�ͼ���ϵ�����󡢳���������Ŀ��ϵ��������Ŀ�곣��ֵ������������籶��
//����ֵΪ״̬��������������������š��ǻ����������
//aMatrix��bVector��cVector�о�ΪΪ�����ɳ��͵Ľṹ��vValue��Ŀ��ֵ
//��׼�͵�������
//MAX: cVector^T * x + vValue
//s.t.��aMatrix * x <= bVector
template<typename Ty>
inline typename std::enable_if<std::is_floating_point<Ty>::value,
	std::tuple<SimplexStatus, std::vector<Ty>, std::vector<int>, std::vector<int>>
>::type SimplexAlgo(std::vector<std::vector<Ty>> &aMatrix, std::vector<Ty> &bVector,
	std::vector<Ty> &cVector, Ty &vValue, int ulp= 2000)
{
	int mNum= (int)bVector.size(), nNum= (int)cVector.size();//Լ������������
	std::vector<int> bSet, nSet;//�������ǻ������������±�
	std::vector<Ty> resVector;//��ϵ������
	//�ҳ�ʼ��
	SimplexStatus status = _SimplexAssistInit(aMatrix, bVector, cVector, vValue, bSet, nSet, ulp);
	if(status==SimplexStatus::infeasible)
		return make_tuple(status, resVector, bSet, nSet);
	//������ѭ��
	status = _SimplexAssistLoop(aMatrix, bVector, cVector, vValue, bSet, nSet, ulp);
	if(status==SimplexStatus::unbounded)
		return make_tuple(status, resVector, bSet, nSet);
	//���ɽ�����
	resVector.resize(nNum, 0);
	for(int i=0; i<mNum; ++i) {
		if(bSet[i]<nNum)
			resVector[bSet[i]] = bVector[i];
	}
	return make_tuple(status, std::move(resVector), std::move(bSet), std::move(nSet));
}

