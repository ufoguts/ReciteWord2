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

#include <string>
#include <vector>
#include <list>
#include <map>

#include <stdexcept>
#include <random>
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

//����ȥ�����ó����ױ���Ƿ�Ϊǰ��
template<typename TyDst, typename TySrc>
struct _StruAndAlgoIsRemoveCVRefSame:
	std::is_same<TyDst, typename _StruAndAlgoRemoveCVRef<TySrc>::type>
{
};

//���߲������Ƿ�Ҫ��û�в�����Ҫ��ֻ��һ������Ϊǰ��
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

//���߲������Ƿ�Ҫ��û�в�����Ҫ��ֻ��һ������ȥ�����ó����ױ��Ϊǰ��
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

//���ɵ����������õ�����
template<typename TyIt>
struct _StruAndAlgoIteratorDerefType
{
	using type = decltype(*std::declval<TyIt>());
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

//���ñ�������������
template<typename Ty>
inline void _StruAndAlgoCallDestruct(Ty &value)
{
	value.~Ty();
}

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

//����ıȽϺ�����
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

//��Χ���ֵ���Ƚϲ�����ʹ�õ�������С�ڵ��ڴ��ڷֱ𷵻�-1,0,1
template<typename TyIt1, typename TyIt2,
	typename TyLess= _StruAndAlgoGeneralLess<typename IteratorDerefType<TyIt1>::type,
	typename IteratorDerefType<TyIt2>::type>,
	typename TyEqual= _StruAndAlgoGeneralEqualTo<typename IteratorDerefType<TyIt1>::type,
	typename IteratorDerefType<TyIt2>::type>>
inline int _StruAndAlgoSequenceCompare(TyIt1 st1, TyIt1 ed1, TyIt2 st2, TyIt2 ed2,
	TyLess funcLess= TyLess(), TyEqual funcEqual= TyEqual())
{
	for(; ; ++st1, ++st2) {
		//�жϽ�������
		if(st1==ed1) {
			if(st2==ed2)
				return 0;
			else
				return -1;
		}
		else if(st2==ed2)
			return 1;
		//�жϴ�С
		if(funcLess(*st1, *st2))
			return -1;
		else if(!(funcEqual(*st1, *st2)))
			return 1;
	}
}

//ģ�嶨������
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

//��size_t��С��ͬ�ľ������ͣ��з�������
using _StruAndAlgoDifferenceType = typename _StruAndAlgoTemplateInt<true, sizeof(size_t)>::type;

//����������ȽϺ�����β������������ò���ȫ
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


//�������ļ�����

//������������
template<typename ...Ty_S>
class ArgsTuple;

//ArgsTuple���ã����㷵�����͸�����
template<typename TyFunc, typename ...Ty_S>
struct ArgsTupleInvokeReturnType;

//ʹ��ArgsTuple����ú�����֧�ֶ������������ͨ�������ʹ��
template<typename TyFunc, typename ...Ty_S>
inline typename ArgsTupleInvokeReturnType<TyFunc &&, Ty_S &&...
>::type ArgsTupleInvoke(TyFunc &&func, Ty_S &&...arg_s);

//д���Ͷ������ļ���
//��ͬ�������������ʹ�С��һ����ͬ��Ҫ��֤ʹ�ô�С�޹ر���
class BinWriteFile;

//�����Ͷ������ļ���
//��ͬ�������������ʹ�С��һ����ͬ��Ҫ��֤ʹ�ô�С�޹ر���
class BinReadFile;




//************
//���ݽṹ����
//************

//֧���±����ɾ�������������������ṩ֧���±���ʵ���������string,vector��deque
//ʵ����Ϊɾ��Ԫ�غ�����Ԫ���ƶ���ǰ��
//���Կ���ɾ��ʱ���ĵ�������ʧЧ����ɾ���ĵ�����ָ��ԭ�����ĵ���������
template<typename TyContainer>
class FastEraseWrap:
	public TyContainer
{
public://��������
	//Ĭ�Ϲ��캯��
	FastEraseWrap()
		= default;
	//�̳й��캯��
	using TyContainer::TyContainer;
	//ʡ�Ժϳ�5����

public://��Ա����
	//����������ɾ���������ƶ��ĵ���������
	size_t FastErase(typename TyContainer::iterator itPos)
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
	size_t FastErase(typename TyContainer::iterator itSt,
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
		return FastErase(begin()+idx);
	}
	//�±귶Χ����ɾ���������ƶ��ĵ���������
	size_t IndexFastErase(size_t idxSt, size_t idxEd)
	{
		return FastErase(begin()+idxSt, begin()+idxEd);
	}
};



template<typename IterStruMy, typename TyDataMy, typename TyContainer>
class _SequenceSkipListTemplateIter;
//˳������ṹ��֧���Զ���˳����ʣ�ʹ������ʵ��
//ɾ����ӦԪ�ػ�ʹ��Ӧ�����ֵ�����ʧЧ
//����ɾ�����漰�±�����ķ���Ϊ�������Ӷ�
//��������ĸ��ʣ�ָ����1/2���ݴ�������Prb = (1/2)^prbPow
//����ʹ��2-4��Խ���ٶ�Խ�������ռ�ռ��Լ��
//������ָ������ƽ����ʱ�临�Ӷ�ΪO(2^prbPow/prbPow*lg(n))
//�����ϵĶ���ռ临�Ӷ�ΪO(n/(2^prbPow-1))
/*ʵ��ʱ�����£���ʽ������ֵ-��֣����Ϊ�����ʱ���ֵ����
��������size_t(uint64_t)
����1000000��: 4-100, 3-93, 2-77, 1-51
1000000�������˳��ȫ������: 3-100, 2-93, 4-89, 1-50
1000000���в���1000000��: 4-100, 3-85, 2-67, 1-39
2000000��ɾ��1000000��: 4-100, 3-92, 2-62, 1-35
*/
template<typename TyData, typename TyRan= std::mt19937>
class SequenceSkipList
{
	/*
	lisLis������������ɵ�����������lisIdx��lisVal��ֵ����
	dָ���ݣ�Oָ�����ڵ㣬Xָβ��ڵ㣬
	lisIndex�и����ӵ�洢����ָ��������ָ������ӽڵ㣬ע��β��ڵ�Ҳ��
	�����������������������һ��ͷ�������ڵ㣬��������һ����������
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

public://��Ԫ
	//������
	template<typename IterStruMy, typename TyDataMy, typename TyContainer>
	friend class _SequenceSkipListTemplateIter;

private://�������Ͷ���
	struct IndexListStru;
	struct ValueListStru;
	//�������ڵ��������ͣ�����������
	using ListListStru = std::list<IndexListStru>;
	//�������������
	using IndexListIter = typename std::list<IndexListStru>::iterator;
	//ֵ���������
	using ValueListIter = typename std::list<ValueListStru>::iterator;
	using ValueListConstIter = typename std::list<ValueListStru>::const_iterator;
	//������������
	using ListListIter = typename std::list<ListListStru>::iterator;
	//�����ݹ����־��������������ṹ��ʾ����������
	struct NoDataConstruFlag
	{
	};

	//��������ṹ�����Լ���������
	struct IndexListStru
	{
		IndexListIter itPr;//����ָ�ĵ�������ָ���Լ���ʾ��
		union {
			IndexListIter itChIdx;//����ָ������������
			ValueListIter itChVal;//����ָ��ֵ������
		};
		size_t size;//��������洢Ԫ�ظ���
	public:
		//���캯��
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
		//��Ա����
		void DestruIndex()
		{
			_StruAndAlgoCallDestruct(itChIdx);
		}
		void DestruValue()
		{
			_StruAndAlgoCallDestruct(itChVal);
		}
		//��Ա����
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

	//ֵ����ṹ�����Լ�������������
	struct ValueListStru
	{
		IndexListIter itPr;//����ָ�ĵ�����
		union {
			TyData data;//�洢����
		};
		//���ݽڵ㹹�캯��
		template<typename ...Ty_S>
		ValueListStru(Ty_S &&...arg_s):
			data(std::forward<Ty_S>(arg_s)...)
		{
		}
		//�����ݹ��캯��
		ValueListStru(NoDataConstruFlag)
		{
		}
		~ValueListStru()
		{
		}
		//��������
		void DestruData()
		{
			_StruAndAlgoCallDestruct(data);
		}
	};

	//�������ṹ��װ�����ڵ�����ģ��̳�
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

public://��������
	using DataType = TyData;
	using RanType = TyRan;
private:
	using MyClassType = SequenceSkipList<TyData, TyRan>;//��������

public://����������ָ��
	//������
	using Iter = _SequenceSkipListTemplateIter<IterStru, TyData, MyClassType>;
	//��������
	using ConstIter = _SequenceSkipListTemplateIter<ConstIterStru, const TyData, MyClassType>;
	//���������
	using RevIter = std::reverse_iterator<Iter>;
	using ConstRevIter = std::reverse_iterator<ConstIter>;

private://��Ա����
	TyRan m_ran;//���������
	unsigned m_prbPow;//��������ĸ���
	bool m_bHasStru;//�Ƿ��Ѿ��нṹ��������ʱ��Ҫ����
	std::list<ValueListStru> m_lisVal;//��������
	std::list<ListListStru> m_lisLis;//�������������

public://������̬����
	static constexpr int msc_defaultPrbPow = 3;//��������ĸ���Ĭ��ֵ

public://��������
	//Ĭ�Ϲ���
	explicit SequenceSkipList(const TyRan &o_ran= TyRan(), unsigned o_prbPow= msc_defaultPrbPow):
		m_ran(o_ran), m_prbPow(o_prbPow), m_bHasStru(true)
	{
		assert(o_prbPow>0);
		InitBasicStruct();
	}
	//��������
	~SequenceSkipList()
	{
		//��Ϊ��Ч�Ľṹ
		if(m_bHasStru) {
			assert(!m_lisLis.empty() && !m_lisVal.empty());
			//�����������ڵ�
			CollectIndexNode();
			//�������ݽڵ�
			for(auto itVal= ++m_lisVal.begin(); itVal!=m_lisVal.end(); ++itVal)
				itVal->DestruData();
		}
		//��Ч�Ľṹ�����ƶ�����󣩿�ֱ������
	}
	//��������
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
	//�ƶ�����
	SequenceSkipList(SequenceSkipList &&other) noexcept
		: m_ran(std::move(other.m_ran)), m_prbPow(std::move(other.m_prbPow)),
		m_bHasStru(std::move(other.m_bHasStru)),
		m_lisVal(std::move(other.m_lisVal)), m_lisLis(std::move(other.m_lisLis))
	{
		//��ǶԷ��޽ṹ
		other.m_bHasStru = false;
	}
	SequenceSkipList &operator =(SequenceSkipList &&other) noexcept
	{
		if(this!=&other) {
			//�ƶ���Ա
			m_ran = std::move(other.m_ran);
			m_prbPow = std::move(other.m_prbPow);
			std::swap(m_bHasStru, other.m_bHasStru);//�Է��Ƿ��нṹȡ������
			m_lisVal.swap(other.m_lisVal);
			m_lisLis.swap(other.m_lisLis);
		}
		return *this;
	}
	//��ֵ���캯����ͬʱ�����Ƿ�ʹ��׼���
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
public://���������
	//�б�ֵ
	SequenceSkipList &operator =(std::initializer_list<TyData> init)
	{
		Assign(init);
		return *this;
	}
	//�Ƚϲ�������˳����ֵ���
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
private://˽�и�������
	//���������Խ�ľ���
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
	//�±���ʸ���������stk��ʾ��ǰ�����ţ���������Ϊ0��
	static ValueListIter DownFindIndexIter(IndexListIter itIdx, size_t idx, size_t stk)
	{
		ValueListIter itVal;
		while(true) {
			//�����
			for(; idx>=itIdx->size; ++itIdx)
				idx -= itIdx->size;
			-- stk;
			//��û����������
			if(stk!=0)
				itIdx = itIdx->itChIdx;
			//��������ֵ������
			else {
				itVal = itIdx->itChVal;
				break;
			}
		}
		//ֵ������������������
		for(; idx!=0; --idx, ++itVal)
			;
		return itVal;
	}
	//����һ���������
	bool RandomDecide()
	{
		auto num = m_ran();
		return (((1<<m_prbPow)-1)&num) == 0;
	}
	//���鸨�������������Ƿ�׼���������ž����Ƿ����ɽڵ�
	bool RestructureAssist(bool bQuasiRandom, unsigned idx)
	{
		if(idx==0)
			return true;
		if(bQuasiRandom)
			return (((1<<m_prbPow)-1)&idx) == 0;
		else
			return RandomDecide();
	}
	//����µ�һ���������������һ��ͷ����������������ӵ��������������
	ListListIter AddIndexList()
	{
		//�������������
		m_lisLis.emplace_back();
		IndexListIter itIdx;//�����������ͷ�������ڵ������
		//��ԭ��ֻ��һ��ֵ����
		if(m_lisLis.size()==1) {
			//�������������ͷ���ڵ�
			m_lisLis.back().emplace_back(m_lisVal.begin());
			itIdx = m_lisLis.back().begin();
			//��ԭ�����ϲ������ڵ�ָ������������ͷ���ڵ㣬��������������ͷ���ڵ��С
			for(auto it=m_lisVal.begin(); it!=m_lisVal.end(); ++it) {
				it->itPr = itIdx;
				++ itIdx->size;
			}
			//����ֵ����β�������
			++ itIdx->size;
		}
		//����ǰ���ϲ�����������
		else {
			auto itLisLisPrv = std::prev(m_lisLis.end(), 2);//ԭ�������ϲ�����
			//�������������ͷ���ڵ�
			m_lisLis.back().emplace_back(itLisLisPrv->begin());
			itIdx = m_lisLis.back().begin();
			//��ԭ�����ϲ������ڵ�ָ������������ͷ���ڵ㣬��������������ͷ���ڵ��С
			for(auto it=itLisLisPrv->begin(); it!=itLisLisPrv->end(); ++it) {
				it->itPr = itIdx;
				itIdx->size += it->size;
			}
		}
		//����������ͷ���ڵ㸸�ڵ�ָ���Լ�
		itIdx->itPr = itIdx;
		return --m_lisLis.end();
	}
	//��������ṹ�������ڹ��캯���е���
	void InitBasicStruct()
	{
		//��������������Ԫ��
		m_lisVal.emplace_back(NoDataConstruFlag());
		//��������������Ԫ��
		AddIndexList();
	}
	//��������������Ų��һ�����������У����ⲿ�����е�����������û�����
	//���������еĽڵ�����������������ĸ��ڵ�û�н���ά��
	//�豣֤������һ��������һ�������ڵ㣬���뱣֤����֮���ֵ֮�以��
	std::list<IndexListStru> CollectIndexNode()
	{
		std::list<IndexListStru> lisRet;//��������ڵ�
		//�ȴ����һ����������
		auto itLisLis = m_lisLis.begin();
		//�ƶ�����������
		lisRet.splice(lisRet.end(), *itLisLis);
		//�������е�Ԫ��
		for(auto it= lisRet.begin(); it!=lisRet.end(); ++it)
			it->DestruValue();
		//����ʣ�������
		for(++itLisLis; itLisLis!=m_lisLis.end(); ++itLisLis) {
			auto itSt = itLisLis->begin(),
				itEd = itLisLis->end();
			//���������ڵ�
			for(auto it= itSt; it!=itEd; ++it)
				it->DestruIndex();
			//Ų������������
			lisRet.splice(lisRet.end(), *itLisLis);
		}
		return lisRet;
	}
public://��Ա����
	//��ֵ������һͬ�����Ƿ�ʹ��׼�����
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
	//�Ƚϲ�����С�ڵ��ڴ��ڷֱ𷵻�-1,0,1
	template<typename TyDataOther, typename TyRanOther,
		typename TyFuncLess= _StruAndAlgoGeneralLess<TyData, TyDataOther>,
		typename TyFuncEqual= _StruAndAlgoGeneralEqualTo<TyData, TyDataOther>
	> int Compare(const SequenceSkipList<TyDataOther, TyRanOther> &other,
		TyFuncLess funcLess= TyFuncLess(), TyFuncEqual funcEqual= TyFuncEqual()) const
	{
		return _StruAndAlgoSequenceCompare(Begin(), End(), other.Begin(), other.End(),
			funcLess, funcEqual);
	}
	//˳�����������
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
	//˳��������������
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
	//�����±������
	Iter GetIter(size_t idx)
	{
		++ idx;
		//ֱ�����²���
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
	//��ȡ�������±�
	size_t GetIndex(ConstIter it) const
	{
		return it-Begin();
	}
	//����˳����βԪ��
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
	//�±����Ԫ��
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
	//�鿴����
	bool Empty() const
	{
		return Size()==0;
	}
	size_t Size() const
	{
		return m_lisVal.size()-1;
	}
	//���
	void Clear()
	{
		//�������ݽڵ�
		for(auto itVal= ++m_lisVal.begin(); itVal!=m_lisVal.end(); ++itVal)
			itVal->DestruData();
		//ɾ�����ݽڵ�
		m_lisVal.erase(++m_lisVal.begin(), m_lisVal.end());
		//�ṹ����
		Restructure(true);
	}
	//�ı�����
	template<typename ...Ty_S>
	typename std::enable_if<_StruAndAlgoIsNoneOrSame<TyData, Ty_S...>::value, void
	>::type Resize(size_t size, const Ty_S &...arg_s)
	{
		while(Size()>size)
			Erase(--End());
		while(Size()<size)
			Emplace(End(), arg_s...);
	}
	//���˲���
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
	//��ָ��λ��ǰ���룬���ز���ĵ�һ��Ԫ�صĵ�����
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
		//��ʼ���ϲ���ڵ�
		bool bHasAdd = false;//�Ƿ��Ѳ���ڵ�
		auto itLisLis = m_lisLis.begin();
		for(; RandomDecide(); bHasAdd=true) {
			IndexListIter itIdxPrv;//�����������ڵ��ǰһ��������
			size_t dis;//��һ������ڵ�ǰ��Ľڵ���
			//����ӵ�һ���ڵ�
			if(!bHasAdd) {
				itIdxPrv = itVal->itPr;
				dis = ListIterDistance(itIdxPrv->itChVal, itVal);
				//ָ���²���������ڵ�
				itIdx = itLisLis->emplace(std::next(itIdxPrv), itVal);
				//�ı�Ӧָ���²��������ڵ�ĸ��ڵ������
				for(auto it=itVal; it!=m_lisVal.end() && it->itPr==itIdxPrv; ++it)
					it->itPr = itIdx;
			}
			//���ǵ�һ���ڵ�
			else {
				itIdxPrv = itIdx->itPr;
				dis = ListIterDistance(itIdxPrv->itChIdx, itIdx);
				auto itLisLisPrv = std::prev(itLisLis);//��һ������ڵ������������
				auto itIdxOri = itIdx;//��һ������ڵ�
				//ָ���²���������ڵ�
				itIdx = itLisLis->emplace(std::next(itIdxPrv), itIdxOri);
				//�ı�Ӧָ���²��������ڵ�ĸ��ڵ������
				for(auto it=itIdxOri; it!=itLisLisPrv->end() && it->itPr==itIdxPrv; ++it)
					it->itPr = itIdx;
			}
			//�ı��²��������ڵ����ǰ�������ӵ�Ĵ�С
			itIdx->size = itIdxPrv->size-dis;
			itIdxPrv->size = dis;
			//����������������
			++ itLisLis;
			//��Ϊ���ϲ���������������
			if(itLisLis==m_lisLis.end())
				itLisLis = AddIndexList();
			//���򸸽ڵ�ָ��ǰ�ߵĸ��ڵ�
			else
				itIdx->itPr = itIdxPrv->itPr;
		}
		return Iter(itVal);
	}
	//ʹ���±���룬��������׸�Ԫ���±�Ϊָ���±꣬���ز����׸�λ�õĵ�����
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
	//�򵥲���ӿڣ�ע����������²��ᴴ�������ڵ㣬һ�����Լ����������ṹ
	template<typename ...Ty_S>
	Iter SimpleEmplace(Iter itPos, Ty_S &&...arg_s)
	{
		assert(itPos.it!=m_lisVal.begin());
		//��������
		auto itVal = m_lisVal.emplace(itPos.it, std::forward<Ty_S>(arg_s)...);
		IndexListIter itIdx = std::prev(itVal)->itPr;
		itVal->itPr = itIdx;
		//���Ӹ��ڵ����
		while(true) {
			++ itIdx->size;
			if(_StruAndAlgoIteratorEQ(itIdx->itPr, itIdx))
				break;
			itIdx = itIdx->itPr;
		}
		return Iter(itVal);
	}
	//����˳��ɾ��
	void PopBack()
	{
		Erase(--End());
	}
	void PopFront()
	{
		Erase(Begin());
	}
	//ָ��λ��˳��ɾ��������ɾ����һ��λ��
	Iter Erase(Iter itPos)
	{
		assert(itPos.it!=m_lisVal.begin());
		//ɾ��ֱ���������ڵ�
		auto itVal = itPos.it;
		auto itIdx = itPos.it->itPr;
		bool bHasIdxCnct = itIdx->itChVal==itVal;//�Ƿ���ֱ���������ڵ�
		auto itLisLis = m_lisLis.begin();//��¼ֱ�������ӵ㶥���������
		IndexListIter itIdxCnct = itIdx;//��¼ֱ�������ӵ㶥��
		bool bNowIdxCnct = bHasIdxCnct;//�Ƿ�ǰ�ڵ㴦��ֱ��״̬
		//���������ڵ��Сѭ����˳���¼���ϲ�ֱ���ڵ�
		while(true) {
			-- itIdx->size;
			//�޸��ڵ����˳�
			if(_StruAndAlgoIteratorEQ(itIdx->itPr, itIdx))
				break;
			//������ֱ��״̬����½ڵ�
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
		//�����ֱ���ڵ㿪ʼ����ɾ��
		while(bHasIdxCnct) {
			//�ı�ǰ��ڵ������
			auto itIdxPrv = std::prev(itIdxCnct);
			itIdxPrv->size += itIdxCnct->size;
			//�ı����ӽڵ�ĸ��ڵ�
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
			//����ǰ��ڵ������������1�����ϲ�����
			if(itLisLis->size()==2) {
				m_lisLis.back().front().DestruIndex();
				m_lisLis.pop_back();
				itLisLis->front().itPr = itLisLis->begin();
			}
			//��δ���ף��Ƴ��ڵ㲢����
			if(itLisLis!=m_lisLis.begin()) {
				auto itLastIdx = itIdxCnct;
				itIdxCnct = itIdxCnct->itChIdx;
				itLastIdx->DestruIndex();
				itLisLis->erase(itLastIdx);
				-- itLisLis;
			}
			//�����ף��Ƴ��ڵ㲢����
			else {
				itIdxCnct->DestruValue();
				itLisLis->erase(itIdxCnct);
				break;
			}
		}
		//ɾ��ֵ����ڵ�
		itVal->DestruData();
		return Iter(m_lisVal.erase(itVal));
	}
	Iter Erase(Iter itSt, Iter itEd)
	{
		while(itSt!=itEd)
			itSt = Erase(itSt);
		return itEd;
	}
	//ʹ���±�ɾ��������ɾ����һ��λ�õĵ�����
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
	//����Ԫ�أ������������ָ��Ԫ��ֵ���䣬λ�ý���
	void SwapElement(Iter it1, Iter it2)
	{
		assert(it1.it!=m_lisVal.begin() && it2.it!=m_lisVal.begin());
		if(it1==it2)
			return;
		//����λ��
		auto itTmp = std::next(it2.it);
		m_lisVal.splice(it1.it, m_lisVal, it2.it);
		m_lisVal.splice(itTmp, m_lisVal, it1.it);
		//����ṹ
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
	//���з�ת��һͬ���нṹ����
	void Reverse(bool bQuasiRandom= false)
	{
		//�ó���Ԫ��
		std::list<ValueListStru> lisTmp;
		lisTmp.splice(lisTmp.end(), m_lisVal, m_lisVal.begin());
		//��ת
		m_lisVal.reverse();
		//�Ż���Ԫ��
		m_lisVal.splice(m_lisVal.begin(), lisTmp);
		//�ṹ����
		Restructure(bQuasiRandom);
	}
	//���������Ĭ�ϲ�ʹ��ģ���ṩ�ıȽ��࣬һͬ���нṹ����
	template<typename TyFunc= std::less<TyData>>
	void Sort(bool bQuasiRandom= false, const TyFunc &func= std::less<TyData>())
	{
		auto lbdCom = [&](const ValueListStru &arg1, const ValueListStru &arg2) {
			return func(arg1.data, arg2.data);
		};
		//�ó���Ԫ��
		std::list<ValueListStru> lisTmp;
		lisTmp.splice(lisTmp.end(), m_lisVal, m_lisVal.begin());
		//����
		m_lisVal.sort(lbdCom);
		//�Ż���Ԫ��
		m_lisVal.splice(m_lisVal.begin(), lisTmp);
		//����ṹ
		Restructure(bQuasiRandom);
	}
	//�ṹ���飬���Ը��Ӷȣ�׼��������ṹ��Ϊƽ����������������������
	//�豣֤������һ��������һ�������ڵ㣬���뱣֤����֮���ֵ֮�以��
	void Restructure(bool bQuasiRandom)
	{
		//ȥ�����������ڵ�
		auto lisFree = CollectIndexNode();
		//��ӵ�һ��
		auto itLisLis = m_lisLis.begin();
		auto itVal = m_lisVal.begin();
		for(unsigned i=0; itVal!=m_lisVal.end(); ++itVal, ++i) {
			//����Ӧ�ڵ�Ӧ��������ӽڵ�
			bool bJdg = bQuasiRandom? ((((1<<m_prbPow)-1)&i)==0) : RandomDecide();
			if(RestructureAssist(bQuasiRandom, i)) {
				//��ӽڵ�
				if(!lisFree.empty()) {
					itLisLis->splice(itLisLis->end(), lisFree, lisFree.begin());
					itLisLis->back().Constru(itVal);
				}
				else
					itLisLis->emplace_back(itVal);
				//����ڵ�
				auto itIdxPr = --itLisLis->end();
				itIdxPr->itPr = itIdxPr;
				itIdxPr->size = 1;
				itVal->itPr = itIdxPr;
			}
			//����ı��丸�ڵ㲢���Ӽ���
			else {
				itVal->itPr = --itLisLis->end();
				++ itLisLis->back().size;
			}
		}
		//����β��ڵ��������������
		++ itLisLis->back().size;
		//�������ǰ�����һ���ڵ㣬����Ӻ�����
		while(itLisLis->size()!=1) {
			auto itLisLisPrv = itLisLis;
			++ itLisLis;
			if(itLisLis==m_lisLis.end()) {
				m_lisLis.emplace_back();
				itLisLis = --m_lisLis.end();
			}
			//��������ڵ�
			auto itIdx = itLisLisPrv->begin();
			for(unsigned i=0; itIdx!=itLisLisPrv->end(); ++itIdx, ++i) {
				//����Ӧ�ڵ�Ӧ��������ӽڵ�
				if(RestructureAssist(bQuasiRandom, i)) {
					//��ӽڵ�
					if(!lisFree.empty()) {
						itLisLis->splice(itLisLis->end(), lisFree, lisFree.begin());
						itLisLis->back().Constru(itIdx);
					}
					else
						itLisLis->emplace_back(itIdx);
					//����ڵ�
					auto itIdxPr = --itLisLis->end();
					itIdxPr->itPr = itIdxPr;
					itIdxPr->size = itIdx->size;
					itIdx->itPr = itIdxPr;
				}
				//����ı��丸�ڵ㲢���Ӽ���
				else {
					itIdx->itPr = --itLisLis->end();
					itLisLis->back().size += itIdx->size;
				}
			}	
		}
		//���������������ڵ�
		++ itLisLis;
		m_lisLis.erase(itLisLis, m_lisLis.end());
	}
	//������ӿ�
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
		//�������ṹ
		assert(m_bHasStru);//�����нṹ
		assert(m_lisLis.size()>=1);//����1�������������
		assert(m_lisVal.size()>=1);//ֵ����ڵ��������ڵ���1
		assert(m_lisLis.back().size()==1);//���ϲ�����������������1
		assert(m_lisLis.back().front().itPr==m_lisLis.back().begin());//���ϲ����������������ڵ�Ϊ����
		//���ֵ����
		auto itLisLisNxt = m_lisLis.begin();
		auto itIdxPr = itLisLisNxt->begin();
		for(auto it=m_lisVal.begin(); it!=m_lisVal.end(); ++itIdxPr) {
			assert(it->itPr==itIdxPr);//���ڵ����ӽڵ㻥��
			assert(itIdxPr->itChVal==it);//���ڵ����ӽڵ㻥��
			size_t size = 0;
			for(; it!=m_lisVal.end() && it->itPr==itIdxPr; ++it)
				++ size;
			if(it==m_lisVal.end())
				++ size;
			assert(itIdxPr->size==size);//���ڵ��С��ȷ
		}
		assert(itIdxPr==itLisLisNxt->end());//���ڵ���ʣ��
		//����������������ϲ���
		auto itLisLis = itLisLisNxt;
		++ itLisLisNxt;
		for(; itLisLisNxt!=m_lisLis.end(); ++itLisLis, ++itLisLisNxt) {
			assert(itLisLis->size()>=2);//��������ڵ�����Ӧ���ڵ���2
			itIdxPr = itLisLisNxt->begin();
			for(auto it=itLisLis->begin(); it!=itLisLis->end(); ++itIdxPr) {
				assert(it->itPr==itIdxPr);//���ڵ����ӽڵ㻥��
				assert(itIdxPr->itChIdx==it);//���ڵ����ӽڵ㻥��
				size_t size = 0;
				for(; it!=itLisLis->end() && it->itPr==itIdxPr; ++it)
					size += it->size;
				assert(itIdxPr->size==size);//���ڵ��С��ȷ
			}
			assert(itIdxPr==itLisLisNxt->end());//���ڵ���ʣ��
		}
	}
	//debug and print�����࿪ͷע��һ����ʽ
	void DebugAndPrint() const
	{
		Debug();
		std::vector<std::string> vecStr;
		std::vector<size_t> vecIdx;//��һ�����ݵ��ַ����±�
		vecStr.push_back("lisLis");
		//��ӡֵ����
		auto itLisLisNxt = m_lisLis.begin();
		auto itIdxPr = itLisLisNxt->begin();
		std::string strData = "  | lisVal";//������
		std::string strLink = "  |       ";//������
		for(auto it=m_lisVal.begin(); it!=m_lisVal.end(); ++itIdxPr) {
			for(; it!=m_lisVal.end() && it->itPr==itIdxPr; ++it) {
				//�����д���
				if(it==m_lisVal.begin())
					strData += "-O";
				else
					strData += "-d";
				//�����д���
				if(itIdxPr->itChVal==it) {
					strLink += " |";
					vecIdx.push_back(strLink.size()-1);
				}
				else
					strLink += "-\\";
			}
		}
		//β��ڵ㴦��
		strData += "-X>";
		strLink += "-\\";
		vecIdx.push_back(strLink.size()-1);
		vecStr.push_back(strData);
		vecStr.push_back(strLink);
		//��ӡ��������
		std::vector<size_t> vecLastIdx;
		auto itLisLis = itLisLisNxt;
		++ itLisLisNxt;
		for(; itLisLisNxt!=m_lisLis.end(); ++itLisLis, ++itLisLisNxt) {
			vecIdx.swap(vecLastIdx);
			vecIdx.clear();
			strData = "  O-lisIdx";//������
			strLink = "  |       ";//������
			itIdxPr = itLisLisNxt->begin();
			auto itVecIdx = vecLastIdx.begin();
			for(auto it=itLisLis->begin(); it!=itLisLis->end(); ++itIdxPr) {
				for(; it!=itLisLis->end() && it->itPr==itIdxPr; ++it) {
					//�����д���
					strData.resize(*itVecIdx, '-');
					strData += "O";
					//�����д���
					if(itIdxPr->itChIdx==it) {
						strLink.resize(*itVecIdx, ' ');
						strLink += "|";
						vecIdx.push_back(strLink.size()-1);
					}
					else {
						strLink.resize(*itVecIdx, '-');
						strLink += "\\";
					}
					//��һ������
					++ itVecIdx;
				}
			}
			//β��ڵ㴦��
			strData.resize(*itVecIdx, '-');
			strData += "X>";
			strLink.resize(*itVecIdx, '-');
			strLink += "\\";
			vecIdx.push_back(strLink.size()-1);
			vecStr.push_back(strData);
			vecStr.push_back(strLink);
		}
		//��ӡ�������
		vecStr.push_back("  O-lisIdx-O>");
		vecStr.back().resize(vecIdx.back(), '-');
		vecStr.back() += "X";
		vecStr.push_back(" /^\\");
		//��ӡ
		for(auto rit=vecStr.rbegin(); rit!=vecStr.rend(); ++rit)
			std::cout <<*rit <<"\n";
		std::cout <<std::flush;
	}
};

//˳������ṹ��������ģ��
//��������ʵ�����������������ʡ����������С�ڱȽϲ��������ɶ������Ӷ�
template<typename IterStruMy, typename TyDataMy, typename TyContainer>
class _SequenceSkipListTemplateIter:
	private IterStruMy,
	public std::iterator<std::random_access_iterator_tag, TyDataMy,
	_StruAndAlgoDifferenceType, TyDataMy *, TyDataMy &>
{
public://��Ԫ
	template<typename TyData, typename TyRan>
	friend class SequenceSkipList;
	template<typename IterStruOther, typename TyDataOther, typename TyContainerOther>
	friend class _SequenceSkipListTemplateIter;

private://����
	//�̳���IterStruMy��itΪ������

public://��������
	//Ĭ�Ϲ���
	_SequenceSkipListTemplateIter()
		= default;
	//�̳л������ݹ���
	using IterStruMy::IterStruMy;
	//�ɷǳ��๹�쳣��
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
public://���������
	//�����ò���
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
	//�����ݼ�����
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
	//�����������
	_SequenceSkipListTemplateIter &operator +=(difference_type offset)
	{
		//������С�ڵ���2^defaultPrPowʱֱ�ӵ���
		if(std::abs(offset)<=(1U<<TyContainer::msc_defaultPrbPow))
			return ForIncrease(offset);
		size_t stk = 0;//��¼���ϵĲ���
		//��ʼ��ǰ������
		-- it;
		++ offset;
		auto itIdx = it->itPr;//����������
		offset += TyContainer::ListIterDistance(itIdx->itChVal, it);
		//������ѭ��
		while(true) {
			++ stk;
			//����������ǰ��һ��
			if(offset<0) {
				-- itIdx;
				offset += (difference_type)itIdx->size;
			}
			//�������������һ��
			else if(offset>=(difference_type)itIdx->size) {
				offset -= (difference_type)itIdx->size;
				++ itIdx;
			}
			//��û���ϲ������ˣ����ߵ�ǰ����������Ŀ�꣬���˳�
			if(_StruAndAlgoIteratorEQ(itIdx->itPr, itIdx)
				|| (0<=offset && offset<(difference_type)itIdx->size))
				break;
			//������
			offset += TyContainer::ListIterDistance(itIdx->itPr->itChIdx, itIdx);
			itIdx = itIdx->itPr;
		}
		//���²���
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
	//�������
	template<typename IterStruOther, typename TyDataOther>
	difference_type operator -(const _SequenceSkipListTemplateIter<
		IterStruOther, TyDataOther, TyContainer> &other) const
	{
		if(it==other.it)
			return 0;
		//��ʼ����������
		auto itVal = it;
		auto itValOth = other.it;
		-- itVal, -- itValOth;
		auto itIdx= itVal->itPr, itIdxOth= itValOth->itPr;
		auto off= TyContainer::ListIterDistance(itIdx->itChVal, it),
			offOth= TyContainer::ListIterDistance(itIdxOth->itChVal, other.it);//�ֱ��¼ƫ����
		//������ѭ��
		while(itIdx!=itIdxOth) {
			off += TyContainer::ListIterDistance(itIdx->itPr->itChIdx, itIdx);
			offOth += TyContainer::ListIterDistance(itIdxOth->itPr->itChIdx, itIdxOth);
			itIdx = itIdx->itPr;
			itIdxOth = itIdxOth->itPr;
		}
		return off-offOth;
	}
	//�Ƚϲ���
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
public://��Ա����
	//ѭ�����������������Ը��Ӷȣ�������Сʱ�ȽϿ�
	_SequenceSkipListTemplateIter &ForIncrease(difference_type offset)
	{
		for(; offset>0; --offset)
			operator ++();
		for(; offset<0; ++offset)
			operator --();
		return *this;
	}
};
//��������Ӳ���
template<typename IterStruMy, typename TyDataMy, typename TyContainer>
inline _SequenceSkipListTemplateIter<IterStruMy, TyDataMy, TyContainer> operator +(
	_StruAndAlgoDifferenceType offset,
	const _SequenceSkipListTemplateIter<IterStruMy, TyDataMy, TyContainer> &it)
{
	return it.operator +(offset);
}

//�������ļ���д˳���������
template<typename TyData, typename TyRan>
BinWriteFile &operator <<(BinWriteFile &bwf, const SequenceSkipList<TyData, TyRan> &skl)
{
	//д�ߴ�
	int64_t size = skl.Size();
	bwf <<size;
	//д����
	for(auto &r : skl)
		bwf <<r;
	return bwf;
}
template<typename TyData, typename TyRan>
BinReadFile &operator >>(BinReadFile &brf, SequenceSkipList<TyData, TyRan> &skl)
{
	//���ߴ�
	int64_t size = -1;
	brf >>size;
	if(brf.IsFail())
		return brf;
	skl.Clear();
	//������
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
//����Ķ�ֵ���ϣ�֧��˳����ʺ�ֵ�����
//�������в������Ƕ������Ӷȣ��������ӶȲ����ȶ��������������
//ɾ����ӦԪ�ػ�ʹ��Ӧ�����ֵ�����ʧЧ������Ԫ�ص�ֵ��ʹ��Ӧֵ�������ʧЧ
template<typename TyData, typename TyRan= std::mt19937, typename TyComp= std::less<TyData>>
class SequenceMultiSet
{
	/*
	bst <-> skl
	bst��skl˫������
	bstΪ��������key�����ݣ�value��skl������
	skl��˳������װ��bst������
	�ڸ��ֲ���ʱ��Ҫά�����������Ľṹ
	*/

private://�������Ͷ���
	struct ListStru;
	struct MultimapStru;
	//SkipList������
	using ListIter = typename SequenceSkipList<ListStru, TyRan>::Iter;
	using ListConstIter = typename SequenceSkipList<ListStru, TyRan>::ConstIter;
	//multimap������
	using MultimapIter = typename std::multimap<TyData, MultimapStru, TyComp>::iterator;
	using MultimapConstIter = typename std::multimap<TyData, MultimapStru, TyComp>::const_iterator;
	//SkipList������ݽṹ��
	struct ListStru
	{
		MultimapIter itBst;//���ĵ�����
	};
	//multimap������ݽṹ��
	struct MultimapStru
	{
		ListIter itSkl;//����ĵ�����
		MultimapStru()
			= default;
		MultimapStru(ListIter o_itSkl):
			itSkl(o_itSkl)
		{
		}
	};

	//˳��������ṹ��װ�����ڵ�����ģ��̳�
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

	//ֵ��������ṹ��װ�����ڵ�����ģ��̳�
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

public://��������
	using DataType = TyData;
	using RanType = TyRan;
	using CompType = TyComp;
private:
	using MyClassType = SequenceMultiSet<TyData, TyRan, TyComp>;//��������

public://����������ָ��
	//˳�������
	using Iter = _SequenceMultiSetTemplateIter<
		IterStru, ValueIterStru, const TyData, MyClassType>;
	//˳�򳣵�����
	using ConstIter = _SequenceMultiSetTemplateIter<
		ConstIterStru, ValueConstIterStru, const TyData, MyClassType>;
	//ֵ����ʵĵ�����
	using ValueIter = _SequenceMultiSetTemplateValueIter<
		IterStru, ValueIterStru, const TyData, MyClassType>;
	//ֵ����ʵĵ�����
	using ValueConstIter = _SequenceMultiSetTemplateValueIter<
		ConstIterStru, ValueConstIterStru, const TyData, MyClassType>;
	//˳���������
	using RevIter = std::reverse_iterator<Iter>;
	using ConstRevIter = std::reverse_iterator<ConstIter>;
	//ֵ���������
	using ValueRevIter = std::reverse_iterator<ValueIter>;
	using ValueConstRevIter = std::reverse_iterator<ValueConstIter>;

public://��Ԫ
	template<typename IterStruMy, typename ValueIterStruMy,
		typename TyDataMy, typename TyContainer
	> friend class _SequenceMultiSetTemplateIter;
	template<typename IterStruMy, typename ValueIterStruMy,
		typename TyDataMy, typename TyContainer
	> friend class _SequenceMultiSetTemplateValueIter;

private://��Ա����
	SequenceSkipList<ListStru, TyRan> m_skl;//����ṹ��������ṹ������
	std::multimap<TyData, MultimapStru, TyComp> m_bst;//���ṹ�����ֵ�����������

public://������̬����
	static constexpr int msc_defaultPrbPow =
		SequenceSkipList<ListStru, TyRan>::msc_defaultPrbPow;//������������ĸ���Ĭ��ֵ

public://��������
	//Ĭ�Ϲ���
	explicit SequenceMultiSet(const TyRan &o_ran= TyRan(), unsigned o_prbPow= msc_defaultPrbPow,
		const TyComp &func= TyComp()
	): m_skl(o_ran, o_prbPow), m_bst(func)
	{
	}
	//��������
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
	//�ƶ�����
	SequenceMultiSet(SequenceMultiSet &&other) noexcept
		= default;
	SequenceMultiSet &operator =(SequenceMultiSet &&other) noexcept
		= default;
	//��ֵ���캯����ͬʱ�����Ƿ�ʹ��׼���
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
public://���������
	//�б�ֵ
	SequenceMultiSet &operator =(std::initializer_list<TyData> init)
	{
		Assign(init);
		return *this;
	}
	//�Ƚϲ�������˳����ֵ���
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
public://��Ա����
	//��ֵ������һͬ�����Ƿ�ʹ��׼�����
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
	//�Ƚϲ�����С�ڵ��ڴ��ڷֱ𷵻�-1,0,1
	template<typename TyDataOther, typename TyRanOther, typename TyCompOther,
		typename TyFuncLess= _StruAndAlgoGeneralLess<TyData, TyDataOther>,
		typename TyFuncEqual= _StruAndAlgoGeneralEqualTo<TyData, TyDataOther>
	> int Compare(const SequenceMultiSet<TyDataOther, TyRanOther, TyCompOther> &other,
		TyFuncLess funcLess= TyFuncLess(), TyFuncEqual funcEqual= TyFuncEqual()) const
	{
		return _StruAndAlgoSequenceCompare(Begin(), End(),
			other.Begin(), other.End(), funcLess, funcEqual);
	}
	//˳�����������
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
	//˳��������������
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
	//ֵ�����������
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
	//ֵ��������������
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
	//�����±������
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
	//��ȡ�������±�
	size_t GetIndex(ConstIter it) const
	{
		return it-Begin();
	}
	//����˳����βԪ��
	const TyData &Front() const
	{
		return *Begin();
	}
	const TyData &Back() const
	{
		return *(--End());
	}
	//����ֵ����βԪ��
	const TyData &ValueFront() const
	{
		return *ValueBegin();
	}
	const TyData &ValueBack() const
	{
		return *(--ValueEnd());
	}
	//�±����Ԫ��
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
	//�鿴����
	bool Empty() const
	{
		return m_skl.Empty();
	}
	size_t Size() const
	{
		return m_skl.Size();
	}
	//���
	void Clear()
	{
		m_skl.Clear();
		m_bst.clear();
	}
	//�ı�����
	template<typename ...Ty_S>
	typename std::enable_if<_StruAndAlgoIsNoneOrSame<TyData, Ty_S...>::value, void
	>::type Resize(size_t size, const Ty_S &...arg_s)
	{
		while(Size()>size)
			Erase(--End());
		while(Size()<size)
			Emplace(End(), arg_s...);
	}
	//���˲���
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
	//��ָ��λ��ǰ���룬���ص�һ��Ԫ�صĵ�����
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
	//ʹ���±���룬��������׸�Ԫ���±�Ϊָ���±꣬���ز����׸�λ�õĵ�����
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
	//����򵥲���ӿڣ������������ڵ�
	template<typename ...Ty_S>
	Iter SimpleEmplace(Iter itPos, Ty_S &&...arg_s)
	{
		auto itSkl = m_skl.SimpleEmplace(itPos.it);
		auto itBst = m_bst.emplace(std::piecewise_construct,
			std::forward_as_tuple(std::forward<Ty_S>(arg_s)...), std::make_tuple(itSkl));
		itSkl->itBst = itBst;
		return Iter(itSkl);
	}
	//����ʾ�Ĳ���Ԫ��
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
	//����˳��ɾ��
	void PopBack()
	{
		Erase(--End());
	}
	void PopFront()
	{
		Erase(Begin());
	}
	//ָ��λ��˳��ɾ��������ɾ����һ��λ��
	Iter Erase(Iter itPos)
	{
		//ɾ��skl��bst
		m_bst.erase(itPos.it->itBst);
		return Iter(m_skl.Erase(itPos.it));
	}
	Iter Erase(Iter itSt, Iter itEd)
	{
		while(itSt!=itEd)
			itSt = Erase(itSt);
		return itEd;
	}
	//ʹ���±�˳��ɾ��������ɾ����һ��λ�õĵ�����
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
	//ָ��λ��ֵ��ɾ��������ɾ����һ��λ��
	ValueIter ValueErase(ValueIter itPos)
	{
		return ValueErase(itPos, itPos.ForIncrease(1));
	}
	ValueIter ValueErase(ValueIter itSt, ValueIter itEd)
	{
		//ɾ����Ӧskl
		for(auto it=itSt.it; it!=itEd.it; ++it) {
			m_skl.Erase(it->second.itSkl);
		}
		//ɾ��bst
		return ValueIter(m_bst.erase(itSt.it, itEd.it));
	}
	//����ɾ��������ɾ������
	size_t FindErase(const TyData &value)
	{
		auto prRes = m_bst.equal_range(value);
		auto ret = std::distance(prRes.first, prRes.second);
		ValueErase(ValueIter(prRes.first), ValueIter(prRes.second));
		return ret;
	}
	//����
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
	//��Χ����
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
	//�޸�ֵ�������޸�λ�õ�ֵ������
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
	//ʹ���±��޸�ֵ
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
	//�����޸�ֵ
	template<typename Ty>
	typename std::enable_if<_StruAndAlgoIsRemoveCVRefSame<TyData, Ty>::value, size_t
	>::type FindModify(const TyData &valueSrc, Ty &&valueDst)
	{
		auto prRes = m_bst.equal_range(valueSrc);
		auto size = std::distance(prRes.first, prRes.second);
		if(size>0) {
			//��¼���ҽ��
			std::vector<ListIter> vecIt(size);
			int i = 0;
			for(auto it=prRes.first; it!=prRes.second; ++it, ++i)
				vecIt[i] = it->second.itSkl;
			//ɾ��bst
			m_bst.erase(prRes.first, prRes.second);
			//�ؽ�bst
			for(i=0; i+1!=size; ++i)
				vecIt[i]->itBst = m_bst.emplace((const Ty &)(valueDst), vecIt[i]);
			vecIt[i]->itBst = m_bst.emplace(std::forward<Ty>(valueDst), vecIt[i]);
		}
		return size;
	}
	//����Ԫ�أ������������ָ��Ԫ��ֵ���䣬λ�ý���
	void SwapElement(Iter it1, Iter it2)
	{
		m_skl(it1.it, it2.it);
	}
	void SwapElement(Iter st1, Iter ed1, Iter st2)
	{
		m_skl(st1.it, ed1.it, st2.it);
	}
	//���з�ת
	void Reverse(bool bQuasiRandom= false)
	{
		m_skl.Reverse(bQuasiRandom);
	}
	//���������Ĭ�ϲ�ʹ��ģ���ṩ�ıȽ���
	template<typename TyFunc= std::less<TyData>>
	void Sort(bool bQuasiRandom= false, const TyFunc &func= std::less<TyData>())
	{
		auto lbdCom = [&](const ListStru &arg1, const ListStru &arg2) {
			return func(arg1.itBst->first, arg2.itBst->first);
		};
		m_skl.Sort(bQuasiRandom, lbdCom);
	}
	//�ṹ���飬���Ը��Ӷȣ�׼��������ṹ��Ϊƽ����������������������
	//�豣֤������һ��������һ�������ڵ㣬���뱣֤����֮���ֵ֮�以��
	void Restructure(bool bQuasiRandom)
	{
		m_skl.Restructure(bQuasiRandom);
	}
	//������ӿ�
	const TyRan &GetRandomEngin() const
	{
		return m_skl.GetRandomEngin();
	}
	TyRan &GetRandomEngin()
	{
		return m_skl.GetRandomEngin();
	}
	//��ñȽϺ���
	TyComp GetCompare() const
	{
		return m_bst.key_comp();
	}
	//Debug
	void Debug() const
	{
		m_skl.Debug();//��������
		assert(m_skl.Size()==m_bst.size());//��С��ͬ
		for(auto it=m_skl.Begin(); it!=m_skl.End(); ++it) {
			assert(it->itBst->second.itSkl==it);//���Ի������
		}
	}
};

//������˳��ļ��ϣ���˳�������
//����ʱ����˳����б���
//��������ʵ�������ͬ���������
//β�������ʧЧ���ܽ����ã�����ת��ֵ�������
template<typename IterStruMy, typename ValueIterStruMy,
	typename TyDataMy, typename TyContainer
> class _SequenceMultiSetTemplateIter:
	private IterStruMy,
	public std::iterator<std::random_access_iterator_tag, TyDataMy,
	_StruAndAlgoDifferenceType, TyDataMy *, TyDataMy &>
{
public://��Ԫ
	template<typename TyData, typename TyRan, typename TyComp>
	friend class SequenceMultiSet;
	template<typename IterStruOther, typename ValueIterStruOther,
		typename TyDataOther, typename TyContainerOther
	> friend class _SequenceMultiSetTemplateIter;
	template<typename IterStruOther, typename ValueIterStruOther,
		typename TyDataOther, typename TyContainerOther
	> friend class _SequenceMultiSetTemplateValueIter;

private://������
	//��Ӧ��ֵ�������
	using ValueIterMy = _SequenceMultiSetTemplateValueIter<
		IterStruMy, ValueIterStruMy, TyDataMy, TyContainer>;

private://����
	//�̳���IterStruMy��itΪ������

public://��������
	//Ĭ�Ϲ���
	_SequenceMultiSetTemplateIter()
		= default;
	//�̳л������ݹ���
	using IterStruMy::IterStruMy;
	//�ɷǳ��๹�쳣��
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
	//��ֵ����������죬����Ϊβ�������
	explicit _SequenceMultiSetTemplateIter(const ValueIterMy &other)
	{
		it = other.it->second.itSkl;
	}
public://���������
	//�����ò���
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
	//�����ݼ�����
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
	//�����������
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
	//�������
	template<typename IterStruOther, typename ValueIterStruOther, typename TyDataOther>
	difference_type operator -(const _SequenceMultiSetTemplateIter<
		IterStruOther, ValueIterStruOther, TyDataOther, TyContainer> &other) const
	{
		return it-other.it;
	}
	//�Ƚϲ���
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
public://��Ա����
	//��ȡ��Ӧֵ�����������������β�����������
	ValueIterMy GetValueIter() const
	{
		return ValueIterMy(it->itBst);
	}
	//ѭ�����������������Ը��Ӷȣ�������Сʱ�ȽϿ�
	_SequenceMultiSetTemplateIter &ForIncrease(difference_type offset)
	{
		it.ForIncrease(offset);
		return *this;
	}
};
//��������Ӳ���
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

//������˳��ļ��ϣ���ֵ�������
//����ʱ����ֵ����б���
//˫���������ͬ������������
//β�������ʧЧ���ܽ����ã�����ת��˳�������
template<typename IterStruMy, typename ValueIterStruMy,
	typename TyDataMy, typename TyContainer
> class _SequenceMultiSetTemplateValueIter:
	private ValueIterStruMy,
	public std::iterator<std::bidirectional_iterator_tag, TyDataMy,
	_StruAndAlgoDifferenceType, TyDataMy *, TyDataMy &>
{
public://��Ԫ
	template<typename TyData, typename TyRan, typename TyComp>
	friend class SequenceMultiSet;
	template<typename IterStruOther, typename ValueIterStruOther,
		typename TyDataOther, typename TyContainerOther
	> friend class _SequenceMultiSetTemplateIter;
	template<typename IterStruOther, typename ValueIterStruOther,
		typename TyDataOther, typename TyContainerOther
	> friend class _SequenceMultiSetTemplateValueIter;

private://��������
	//��Ӧ��˳�������
	using IterMy = _SequenceMultiSetTemplateIter<
		IterStruMy, ValueIterStruMy, TyDataMy, TyContainer>;

private://����
	//�̳���ValueIterStruMy��itΪ������

public://��������
	//Ĭ�Ϲ���
	_SequenceMultiSetTemplateValueIter()
		= default;
	//�̳л������ݹ���
	using ValueIterStruMy::ValueIterStruMy;
	//�ɷǳ��๹�쳣��
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
	//��˳����������죬����Ϊβ�������
	explicit _SequenceMultiSetTemplateValueIter(const IterMy &other)
	{
		it = other.it->itBst;
	}
public://���������
	//�����ò���
	TyDataMy &operator *() const
	{
		return it->first;
	}
	TyDataMy *operator ->() const
	{
		return &operator *();
	}
	//�����ݼ�����
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
	//�Ƚϲ���
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
public://��Ա����
	//��ȡ��Ӧ˳�����������������β�����������
	IterMy GetIter() const
	{
		return IterMy(it->second.itSkl);
	}
};

//�������ļ���д�����ֵ���ϲ�������
template<typename TyData, typename TyRan, typename TyComp>
BinWriteFile &operator <<(BinWriteFile &bwf,
	const SequenceMultiSet<TyData, TyRan, TyComp> &seq)
{
	//д�ߴ�
	int64_t size = seq.Size();
	bwf <<size;
	//д����
	for(auto &r : seq)
		bwf <<r;
	return bwf;
}
template<typename TyData, typename TyRan, typename TyComp>
BinReadFile &operator >>(BinReadFile &brf, SequenceMultiSet<TyData, TyRan, TyComp> &seq)
{
	//���ߴ�
	int64_t size = -1;
	brf >>size;
	if(brf.IsFail())
		return brf;
	seq.Clear();
	//������
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



//ֻ�����Ԫ�ص�ͼ��֧��void��������
//�ڲ�Ϊ��ά����ʵ��
//�ڵ����������ʹ��������������
//Clear��Assign�ᵼ�µ�����ʧЧ
//�������ơ��ƶ����������������ָ��������������������������ָ��
//TODO: ��ṹ��ƣ����Խ��ڵ���߶�Ӧ������������
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
	_StruAndAlgoDifferenceType, TyNodeDataMy *, TyNodeDataMy &>
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
		return &operator *();
	}
};
//�������ģ�帨�����ػ�
template<typename TyNodeData, typename TyEdgeData>
template<typename GraphTypeMy, typename TyNodeDataMy>
class AddonlyGraph<TyNodeData, TyEdgeData>::TemplateNodeIterAssist<
	GraphTypeMy, TyNodeDataMy, void
>: public std::iterator<std::bidirectional_iterator_tag, TyNodeDataMy,
	_StruAndAlgoDifferenceType, void, void>
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
	_StruAndAlgoDifferenceType, TyEdgeDataMy *, TyEdgeDataMy &>
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
		return &operator *();
	}
};
//�ߵ�����ģ�帨�����ػ�
template<typename TyNodeData, typename TyEdgeData>
template<typename GraphTypeMy, typename TyEdgeDataMy>
class AddonlyGraph<TyNodeData, TyEdgeData>::TemplateEdgeIterAssist<
	GraphTypeMy, TyEdgeDataMy, void
>: public std::iterator<std::bidirectional_iterator_tag, TyEdgeDataMy,
	_StruAndAlgoDifferenceType, void, void>
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

