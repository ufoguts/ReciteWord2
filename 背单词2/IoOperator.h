#pragma once

//IO������

#include <cstdint>
#include <cassert>

#include <iostream>
#include <fstream>

#include <limits>
#include <type_traits>
#include <algorithm>
#include <initializer_list>
#include <utility>
#include <tuple>
#include <iterator>

#include <string>
#include <vector>
#include <array>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

#include <exception>
#include <stdexcept>

#include <mutex>




//��ȥ��������ȥ�������ױ�����
template<typename Ty>
struct _IoOperatorRemoveCVRef
{
	using type = typename std::remove_cv<typename std::remove_reference<Ty>::type>::type;
};

//����ȥ�����ó����ױ���Ƿ�Ϊǰ��
template<typename TyDst, typename TySrc>
struct _IoOperatorIsRemoveCVRefSame:
	std::is_same<TyDst, typename _IoOperatorRemoveCVRef<TySrc>::type>
{
};

//����ȥ�����ױ��������Ժ��Ƿ�Ϊ�ַ�ָ�룬�ַ������string����
template<class Ty>
struct _IoOperatorIsRemoveCVRefSameSzOrString:
	std::integral_constant<bool, _IoOperatorIsRemoveCVRefSame<char *, Ty>::value
	|| _IoOperatorIsRemoveCVRefSame<const char *, Ty>::value
	|| (std::is_array<typename _IoOperatorRemoveCVRef<Ty>::type>::value
	&& _IoOperatorIsRemoveCVRefSame<char, typename std::remove_extent<
	typename _IoOperatorRemoveCVRef<Ty>::type>::type>::value)
	|| _IoOperatorIsRemoveCVRefSame<std::string, Ty>::value>
{
};

//�洢ĳ���ͣ�ֵ����
template<typename Ty>
struct _IoOperatorTypeHold
{
	Ty hold;
public:
	//����
	_IoOperatorTypeHold(const Ty &o_hold):
		hold(o_hold)
	{
	}
	_IoOperatorTypeHold(Ty &&o_hold):
		hold(std::move(o_hold))
	{
	}
};
//�洢ĳ���ͣ���ֵ�����ػ�
template<typename Ty>
struct _IoOperatorTypeHold<Ty &>
{
	Ty &hold;
public:
	//����
	_IoOperatorTypeHold(Ty &o_hold):
		hold(o_hold)
	{
	}
};
//�洢ĳ���ͣ���ֵ�����ػ�
template<typename Ty>
struct _IoOperatorTypeHold<Ty &&>
{
	Ty &&hold;
public:
	//����
	_IoOperatorTypeHold(Ty &&o_hold):
		hold(std::move(o_hold))
	{
	}
};

//�ַ��հ׷��ж�
inline constexpr bool _IoOperatorCharIsBlank(char ch)
{
	return ch==' ' || ch=='\t' || ch=='\r' || ch=='\n'
		|| ch=='\v' || ch=='\f';
}

//���ñ�������������
template<typename Ty>
inline void _IoOperatorCallDestruct(Ty &value)
{
	value.~Ty();
}



//����ʽ��ȡһ�в������ַ���
inline bool _LineStringToInit(const std::string &strIn, std::string &strField,
	std::string &strValue)
{
	//��"(\t1)abc(\t2)(\t3)def(\t4)(\t5)ghi\r\n"
	//����Ч�ַ�ͷ
	auto itSt = std::find_if_not(strIn.begin(), strIn.end(), _IoOperatorCharIsBlank);//a
	//�жϿջ�ע��'#'';'"//"
	if(itSt==strIn.end() || *itSt=='#' || *itSt==';'
		||(*itSt=='/' && itSt+1!=strIn.end() && itSt[1]=='/'))
		return false;
	//���ֶ�β����Ҫ�����
	auto itGap = std::find_if(itSt+1, strIn.end(), _IoOperatorCharIsBlank);//\t2
	strField.assign(itSt, itGap);
	//�Һ�����
	auto itWordSt = std::find_if_not(itGap, strIn.end(), _IoOperatorCharIsBlank);//d
	auto ritWordEd = std::find_if_not(strIn.rbegin(),
		std::string::const_reverse_iterator(itWordSt), _IoOperatorCharIsBlank);//i
	//�洢ֵ������Ϊ��
	if(itWordSt!=strIn.end() && itWordSt<ritWordEd.base())
		strValue.assign(itWordSt, ritWordEd.base());
	return true;
}



//��������ȡ�����ļ�����Ԥ�����ֶη�Χ����ȡ��Χ�ڵ�ֵ������δ�ҵ��ĵ�һ���ֶ�
//�����ļ���ʽӦΪȥ��"#define"�ĺ궨���ʽ��'#'';'"//"��Ϊע��
//bReplace�Ƿ�ͬһ�ֶκ����滻ǰ�ߣ���Ϊ�ظ�������Ϊ�����ظ����
//bCheckNull�Ƿ�ܾ���ȡֵΪ�յ����
template<typename Ty1, typename Ty2>
inline Ty2 StreamReadPredefInit(std::istream &is, Ty1 &table,
	Ty2 itFieldSt, Ty2 itFieldEd, bool bReplace= true, bool bCheckNull= true)
{
	//���ؼ��ֶ�д��
	Ty2 itField;
	std::set<std::string> bstField;
	for(itField=itFieldSt; itField!=itFieldEd; ++itField) {
		bstField.emplace(*itField);
	}
	//������ȡ�ؼ��ֶ�
	std::string strIn, strField, strValue;
	while(std::getline(is, strIn)) {
		//�����д����Ӧ�ַ���
		if(!_LineStringToInit(strIn, strField, strValue))
			continue;
		//�ж��ڱ���
		if(bstField.find(strField)==bstField.end())
			continue;
		//�ж�ֵΪ��
		if(bCheckNull && strValue.empty())
			continue;
		//�滻ģʽ��
		if(bReplace) {
			auto itRes = table.find(strField);
			if(itRes==table.end())
				table.emplace(std::move(strField), std::move(strValue));
			else
				itRes->second = std::move(strValue);
		}
		//�޶���ģʽ��
		else {
			table.emplace(std::move(strField), std::move(strValue));
		}
	}
	//�ж�������
	for(itField=itFieldSt; itField!=itFieldEd; ++itField) {
		std::string strTmp = *itField;
		auto res = table.find(strTmp);
		//��û�ҵ��ֶλ�������ֶο�
		if(res==table.end())
			break;
	}
	return itField;
}


//��������ȡ�����ļ�����Ԥ�����ֶη�Χ����ȡ�����ļ���ֵ�����ض�ȡ����
//�����ļ���ʽӦΪȥ��"#define"�ĺ궨���ʽ��'#'';'"//"��Ϊע��
//bReplace�Ƿ�ͬһ�ֶκ����滻ǰ�ߣ���Ϊ�ظ�������Ϊ�����ظ����
//bCheckNull�Ƿ�ܾ���ȡֵΪ�յ����
template<typename Ty1>
inline int StreamReadAllInit(std::istream &is, Ty1 &table,
	bool bReplace= true, bool bCheckNull= true)
{
	//������ȡ�ؼ��ֶ�
	std::string strIn, strField, strValue;
	while(std::getline(is, strIn)) {
		//�����д����Ӧ�ַ���
		if(!_LineStringToInit(strIn, strField, strValue))
			continue;
		//�ж�ֵΪ��
		if(bCheckNull && strValue.empty())
			continue;
		//�滻ģʽ��
		if(bReplace) {
			auto itRes = table.find(strField);
			if(itRes==table.end())
				table.emplace(std::move(strField), std::move(strValue));
			else
				itRes->second = std::move(strValue);
		}
		//�޶���ģʽ��
		else {
			table.emplace(std::move(strField), std::move(strValue));
		}
	}
	return (int)table.size();
}



//��������������
//ʹ��flushˢ��һ�У�ʹ��ends���״̬��ʹ��endlͬʱ���ˢ�º����
inline bool operator >>(std::istream &is, 
	std::basic_ostream<char> &(*func)(std::basic_ostream<char> &))
{
	bool res = (bool)is;
	if(func==std::flush<char, std::char_traits<char>>)
		is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	else if(func==std::ends<char, std::char_traits<char>>)
		is.clear();
	else if(func==std::endl<char, std::char_traits<char>>) {
		is.clear();
		is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	else
		assert(false);
	return res;
}



//�����������
//����������󸽼ӿ��Ƹ�ʽ���������ʱ������������ⲿ���������
template<typename Ty>
class OstreamHold
{
	OstreamHold()
		= delete;
};
//�������������ֵ�����ػ�
template<typename Ty>
class OstreamHold<Ty &>
{
private:
	std::ostream &os;//�洢os
	Ty &ref;//�洢����
public:
	//���캯��
	explicit OstreamHold(std::ostream &o_os, Ty &o_ref):
		os(o_os), ref(o_ref)
	{
	}
	//ɾ������
	OstreamHold(const OstreamHold<Ty &> &)
		= delete;
	OstreamHold<Ty &> &operator =(const OstreamHold<Ty &> &)
		= delete;
	//Ĭ���ƶ�
	OstreamHold(OstreamHold<Ty &> &&)
		= default;
	OstreamHold<Ty &> &operator =(OstreamHold<Ty &> &&)
		= default;
	//������
	std::ostream &operator()() const
	{
		return os;
	}
	//���úͳ�Ա����
	Ty &operator *() const
	{
		return ref;
	}
	Ty *operator ->() const
	{
		return &ref;
	}
	//ת�������Ͱ汾
	operator OstreamHold<const Ty &>() const
	{
		return OstreamHold<const Ty &>(os, ref);
	}
};
//�������������ֵ�����ػ�
template<typename Ty>
class OstreamHold<Ty &&>
{
private:
	std::ostream &os;//�洢os
	Ty data;//�洢����
public:
	//���캯��
	explicit OstreamHold(std::ostream &o_os, Ty &&o_data):
		os(o_os), data(std::move(o_data))
	{
	}
	//ɾ������
	OstreamHold(const OstreamHold<Ty &&> &)
		= delete;
	OstreamHold<Ty &&> &operator =(const OstreamHold<Ty &&> &)
		= delete;
	//Ĭ���ƶ�
	OstreamHold(OstreamHold<Ty &&> &&)
		= default;
	OstreamHold<Ty &&> &operator =(OstreamHold<Ty &&> &&)
		= default;
	//ת����ֵ��
	 OstreamHold<const Ty &> operator()() const
	{
		return OstreamHold<const Ty &>(os, data);
	}
	 OstreamHold<Ty &> operator()()
	{
		return OstreamHold<Ty &>(os, data);
	}
	//���úͳ�Ա����
	Ty &operator *() const
	{
		return data;
	}
	Ty *operator ->() const
	{
		return &data;
	}
};



//���������������־����Ϊ�������ʾ����
class OstreamSetNoLock
{
};

//�����������
//���ڶ�������ͬʱ�����֧�ַ������÷���ֵ�Ĵ���������
//���ӻ��������ܽӿڣ�Ҫ������������ͣ�ͬʱ����Ҳ�������
//���������������Զ������Ǵ��ݷ�����־����һ����������ʼ����ʱ������������
template<typename TyMtx= std::mutex>
class OstreamSet
{
public:
	//������ϴ�������
	template<typename TyRes>
	class Hold;

public:
	std::vector<std::ostream *> vecPOs;//����������ķ��̰߳�ȫ
	std::vector<TyMtx *> vecPMtx;//�������ķ��̰߳�ȫ

public:
	//Ĭ�Ϲ���
	OstreamSet()
		= default;
	//ɾ������
	OstreamSet(const OstreamSet &)
		= delete;
	OstreamSet &operator =(const OstreamSet &)
		= delete;
	//Ĭ���ƶ������̰߳�ȫ
	OstreamSet(OstreamSet &&)
		= default;
	OstreamSet &operator =(OstreamSet &&)
		= default;
	//�б���
	explicit OstreamSet(std::initializer_list<std::ostream *> intlOs,
		std::initializer_list<TyMtx *> intlMtx = {}
	): vecPOs(intlOs), vecPMtx(intlMtx)
	{
	}
	//����������
	template<typename TyItOs, typename TyItMtx>
	explicit OstreamSet(TyItOs itOsSt, TyItOs itOsEd, 
		TyItMtx itMtxSt, TyItMtx itMtxEd)
	{
		for(; itOsSt!=itOsEd; ++itOsSt)
			vecPOs.push_back(&*itOsSt);
		for(; itMtxSt!=itMtxEd; ++itMtxSt)
			vecPMtx.push_back(&*itMtxSt);
	}
	template<typename TyItOs>
	explicit OstreamSet(TyItOs itOsSt, TyItOs itOsEd):
		OstreamSet(itOsSt, itOsEd, (TyMtx *)nullptr, (TyMtx *)nullptr)
	{
	}
public:
	//ת�����������̰߳�ȫ����˳����������豣֤������
	template<typename Ty>
	auto operator <<(Ty &&arg) const->
		Hold<decltype(*vecPOs.front()<<std::forward<Ty>(arg))> 
	{
		//�������������
		using TyRes = decltype(*vecPOs.front()<<std::forward<Ty>(arg));
		std::vector<_IoOperatorTypeHold<TyRes>> vecRes;
		vecRes.reserve(vecPOs.size());
		//��������
		Lock();
		//������洢
		for(auto pOs : vecPOs) {
			vecRes.emplace_back(*pOs <<std::forward<Ty>(arg));
		}
		//���ɽ����Hold����
		return Hold<TyRes>(std::move(vecRes), vecPMtx.empty()? nullptr:this);
	}
	//�Բ���������ת��
	template<int c_blank =0>
	auto operator <<(std::ios_base &(*func)(std::ios_base &)) const->
		decltype(this->operator << <decltype((func))>(func))
	{
		return operator << <decltype((func))>(func);
	}
	template<int c_blank =0>
	auto operator <<(std::basic_ios<char> &(*func)(std::basic_ios<char> &)) const->
		decltype(this->operator << <decltype((func))>(func))
	{
		return operator << <decltype((func))>(func);
	}
	template<int c_blank =0>
	auto operator <<(
		std::basic_ostream<char> &(*func)(std::basic_ostream<char> &)) const->
		decltype(this->operator << <decltype((func))>(func))
	{
		return operator << <decltype((func))>(func);
	}
	//�Բ�����־������������������������������ͽ���
	Hold<std::ostream &> operator <<(OstreamSetNoLock) const
	{
		//�������������
		using TyRes = std::ostream &;
		std::vector<_IoOperatorTypeHold<TyRes>> vecRes;
		vecRes.reserve(vecPOs.size());
		//�洢����
		for(auto pOs : vecPOs) {
			vecRes.emplace_back(*pOs);
		}
		//���ɽ����Hold����
		return Hold<TyRes>(std::move(vecRes), nullptr);
	}
	//����ostream�ĳ�Ա�������������������Է���ֵ
	template<typename Ty, typename ...Tys>
	void operator()(const Ty &pmf, Tys &&...args) const
	{
		//��������
		Lock();
		//���ó�Ա����
		for(auto pOs : vecPOs) {
			(pOs->*pmf)(std::forward<Tys>(args)...);
		}
		//���ý���
		Unlock();
	}
	//��������
	void Lock() const
	{
		//˳�������
		for(auto pMtx : vecPMtx)
			pMtx->lock();
	}
	void lock() const
	{
		Lock();
	}
	//���ý���
	void Unlock() const
	{
		//�������
		for(auto rit =vecPMtx.rbegin(); rit!=vecPMtx.rend(); ++rit)
			(*rit)->unlock();
	}
	void unlock() const
	{
		Unlock();
	}
	//��������
	bool TryLock() const
	{
		//˳����ó�����
		unsigned i;
		for(i =0; i<vecPMtx.size(); ++i) {
			if(!vecPMtx[i]->try_lock())
				break;
		}
		//����ʧ�������
		if(i!=vecPMtx.size()) {
			for(--i; i>=0; --i)
				vecPMtx[i]->unlock();
			return false;
		}
		else
			return true;
	}
	bool try_lock() const
	{
		return TryLock();
	}
};

//��������ϴ�������
template<typename TyMtx>
template<typename TyRes>
class OstreamSet<TyMtx>::Hold
{
	friend class OstreamSet<TyMtx>;
private:
	std::vector<_IoOperatorTypeHold<TyRes>> vecRes;//������Ľ������
	const OstreamSet<TyMtx> *pSet;//���������ָ�룬����������־

private:
	//���죬����������ó�ʼ��
	explicit Hold(std::vector<_IoOperatorTypeHold<TyRes>> &&o_vecRes,
		const OstreamSet<TyMtx> *o_pSet
	): vecRes(std::move(o_vecRes)), pSet(o_pSet)
	{
	}
	//ɾ������
	Hold(const Hold &)
		= delete;
	Hold &operator =(const Hold &)
		= delete;
	//�ƶ��������Ƴ����ݲ�������������־
	Hold(Hold &&other) noexcept
	{
		this->operator =(std::move(other));
	}
	Hold &operator =(Hold &&other) noexcept
	{
		if(this!=&other) {
			vecRes = std::move(other.vecRes);
			pSet = std::move(other.pSet);
			other.pSet = nullptr;
		}
		return *this;
	}
public:
	//�������������ݱ�־���н������������
	~Hold()
	{
		if(pSet!=nullptr) {
			//���ý���
			pSet->Unlock();
		}
	}
public:
	//ת����������ת����ȥ��������־
	template<typename Ty>
	auto operator <<(Ty &&arg)->
		Hold<decltype(vecRes.front().hold<<std::forward<Ty>(arg))>
	{
		//�˶�������������
		auto ptr = pSet;
		pSet = nullptr;
		//�µ��������������
		using TyResTmp = decltype(vecRes.front().hold<<std::forward<Ty>(arg));
		std::vector<_IoOperatorTypeHold<TyResTmp>> vecResTmp;
		vecResTmp.reserve(vecRes.size());
		//��������洢
		for(auto &refRes : vecRes) {
			vecResTmp.emplace_back(refRes.hold <<std::forward<Ty>(arg));
		}
		//�����µ�Hold���ͣ�ת������������־
		return Hold<TyResTmp>(std::move(vecResTmp), ptr);
	}
	//�Բ���������ת��
	template<int c_blank =0>
	auto operator <<(std::ios_base &(*func)(std::ios_base &))->
		decltype(this->operator << <decltype((func))>(func))
	{
		return operator << <decltype((func))>(func);
	}
	template<int c_blank =0>
	auto operator <<(std::basic_ios<char> &(*func)(std::basic_ios<char> &))->
		decltype(this->operator << <decltype((func))>(func))
	{
		return operator << <decltype((func))>(func);
	}
	template<int c_blank =0>
	auto operator <<(
		std::basic_ostream<char> &(*func)(std::basic_ostream<char> &))->
		decltype(this->operator << <decltype((func))>(func))
	{
		return operator << <decltype((func))>(func);
	}
};



//���Բ��������ѡ��
enum class AssertOption
{
	thrw_log,//�׳��쳣�������־
	thrw,//�׳��쳣
	asst_log,//���Զ��Բ������־
	asst,//���Զ���
	log,//�����־
	none,//�޲���
};

//���Բ�����
template<typename TyOs>
class AssertOperator
{
private:
	//�ǲ����쳣
	class NoCatchError:
		private std::runtime_error
	{
		//�̳л��๹�캯������Ĭ�Ͽ������죩
		using std::runtime_error::runtime_error;
	public:
		//Ĭ�Ϲ���
		NoCatchError():
			std::runtime_error("")
		{
		}
		//ʹ�ü̳�what
		using std::runtime_error::what;
		//ʡ�Ժϳ�5����
	};

private:
	TyOs &os;//�����
public:
	AssertOption option;//���ѡ��

public:
	//����
	AssertOperator() =
		delete;
	explicit AssertOperator(TyOs &o_os, AssertOption o_option):
		os(o_os), option(o_option)
	{
	}
public:
	//ָ��ѡ����ж���
	bool operator()(AssertOption optionTmp,
		bool bCond, const std::string &str= "") const
	{
		if(bCond)
			return true;
		//��ѡ����в���
		if(optionTmp==AssertOption::thrw_log) {
			os <<str <<flush;
			throw NoCatchError(str);
		}
		else if(optionTmp==AssertOption::thrw) {
			throw NoCatchError(str);
		}
		else if(optionTmp==AssertOption::asst_log) {
			os <<str <<flush;
			assert(false);
		}
		else if(optionTmp==AssertOption::asst) {
			assert(false);
		}
		else if(optionTmp==AssertOption::log) {
			os <<str <<flush;
		}
		else if(optionTmp==AssertOption::none) {
		}
		return false;
	}
	//ʹ��Ĭ��ѡ����ж���
	bool operator()(bool bCond, const std::string &str= "") const
	{
		return operator()(option, bCond, str);
	}
};



//д���Ͷ������ļ���
//��ͬ�������������ʹ�С��һ����ͬ��Ҫ��֤ʹ�ô�С�޹ر���
class BinWriteFile
{
public:
	std::ofstream ofs;//�ļ���

public:
	//����
	BinWriteFile()
		= default;
	explicit BinWriteFile(const std::string &name, bool o_bAfter=false)
	{
		Open(name, o_bAfter);
	}
	//����
	~BinWriteFile()
		= default;
	//ɾ������
	BinWriteFile(const BinWriteFile &)
		= delete;
	BinWriteFile &operator =(BinWriteFile &)
		= delete;
	//�ƶ�����
	BinWriteFile(BinWriteFile &&file)
		= default;
	//�ƶ���ֵ
	BinWriteFile &operator =(BinWriteFile &&file)
		= default;

public:
	//���ļ���o_bAfter�Ƿ�׷��д
	BinWriteFile &Open(const std::string &name, bool o_bAfter=false)
	{
		ofs.close();
		if(o_bAfter)
			ofs.open(name, std::ios::binary|std::ios::ate);
		else
			ofs.open(name, std::ios::binary);
		if(!ofs.is_open())
			throw std::runtime_error("Can't Open Write File");
		return *this;
	}
	//�ر��ļ�
	bool Close()
	{
		ofs.close();
		if(ofs.bad())
			throw std::runtime_error("Can't Close File");
		return true;
	}
	//���ش�״̬
	bool isopen() const
	{
		return ofs.is_open();
	}
	//�ļ������ã�һ������
	bool IsFail() const
	{
		return !ofs.good();
	}
	operator bool() const
	{
		return ofs.good();
	}
	//��������־
	void Clear()
	{
		ofs.clear();
	}
	//ˢ���ļ�������
	BinWriteFile &Fflush()
	{
		ofs.flush();
		return *this;
	}
	//�ض�λλ��
	BinWriteFile &Seekp(std::ofstream::pos_type pos)
	{
		ofs.seekp(pos);
		return *this;
	}
	BinWriteFile &Seekp(std::ofstream::off_type off, std::ios_base::seekdir dir)
	{
		ofs.seekp(off, dir);
		return *this;
	}
	//��ѯλ��
	std::ofstream::pos_type Tellp()
	{
		return ofs.tellp();
	}
	//�Ի����������ļ�д��
	BinWriteFile &WriteBuf(const void *buf, size_t size)
	{
		ofs.write(reinterpret_cast<const char *>(buf), size);
		if(ofs.bad())
			throw std::runtime_error("Can't Write File");
		return *this;
	}
	//����������������ж�ȡ
	template<typename Ty>
	typename std::enable_if<std::is_scalar<Ty>::value, BinWriteFile &
	>::Type WriteArray(const Ty *pt, size_t count)
	{
		WriteBuf(pt, sizeof(Ty)*count);
		return *this;
	}
	//�Է���������������ж�ȡ
	template<typename Ty>
	typename std::enable_if<!std::is_scalar<Ty>::value, BinWriteFile &
	>::Type WriteArray(const Ty *pt, size_t count)
	{
		for(; count!=0; --count, ++pt)
			*this <<*pt;
		return *this;
	}
	//д�ļ����������
	template<typename Ty>
	BinWriteFile &operator <<(const Ty &r)
	{
		TypeWrite(r);
		return *this;
	}
public:
	//���������ͽ����ļ�д�룬����������������
	template<typename Ty>
	typename std::enable_if<std::is_scalar<
		typename std::remove_all_extents<Ty>::type>::value, void
	>::type TypeWrite(const Ty &r)
	{
		WriteBuf(&r, sizeof(r));
	}
	//�Է�����������������ļ�д��
	template<typename Ty, size_t N>
	typename std::enable_if<!std::is_scalar<
		typename std::remove_all_extents<Ty>::type>::value, void
	>::type TypeWrite(const Ty (&arrStr)[N])
	{
		for(int64_t i=0; i<N; ++i)
			*this <<arrStr[i];
	}
	//��pair�����ļ�д��
	template<typename Ty1, typename Ty2>
	void TypeWrite(const std::pair<Ty1, Ty2> &pr)
	{
		*this <<pr.first <<pr.second;
	}
	//��tuple�����ļ�д��
	template<typename ...Tys>
	void TypeWrite(const std::tuple<Tys...> &tup)
	{
		TupleWrite<sizeof...(Tys)-1>(tup);
	}
	template<int index, typename ...Tys>
	typename std::enable_if<(index>=0), void
	>::type TupleWrite(const std::tuple<Tys...> &tup)
	{
		*this <<std::get<index>(tup);
		TupleWrite<index-1>(tup);
	}
	template<int index, typename Ty>
	void TupleWrite(const Ty &tup)
	{
	}
	//��string�����ļ�д��
	template<typename Ty>
	void TypeWrite(const std::basic_string<Ty> &str)
	{
		//д�ߴ�
		int64_t size = str.size();
		TypeWrite(size);
		//д����
		WriteBuf(&str[0], (size_t)(size*sizeof(Ty)));
	}
	//��vector�������ͽ����ļ�д��
	template<typename Ty>
	typename std::enable_if<std::is_scalar<Ty>::value, void
	>::type TypeWrite(const std::vector<Ty> &vec)
	{
		//д�ߴ�
		int64_t size = vec.size();
		TypeWrite(size);
		//д����
		WriteBuf(vec.data(), size*sizeof(Ty));
	}
	//��vector���������ͽ����ļ�д��
	template<typename Ty>
	typename std::enable_if<!std::is_scalar<Ty>::value, void
	>::type TypeWrite(const std::vector<Ty> &vec)
	{
		//д�ߴ�
		int64_t size = vec.size();
		TypeWrite(size);
		//д����
		for(auto &r : vec)
			*this <<r;
	}
	//��array�������ͽ����ļ�д��
	template<typename Ty, size_t c_size>
	typename std::enable_if<std::is_scalar<Ty>::value, void
	>::type TypeWrite(const std::array<Ty, c_size> &arr)
	{
		//д����
		WriteBuf(arr.data(), c_size*sizeof(Ty));
	}
	//��array���������ͽ����ļ�д��
	template<typename Ty, size_t c_size>
	typename std::enable_if<!std::is_scalar<Ty>::value, void
	>::type TypeWrite(const std::array<Ty, c_size> &arr)
	{
		//д����
		for(auto &r : arr)
			*this <<r;
	}
	//��deque�����ļ�д��
	template<typename Ty>
	void TypeWrite(const std::deque<Ty> &deq)
	{
		//д�ߴ�
		int64_t size = deq.size();
		TypeWrite(size);
		//д����
		for(auto &r : deq)
			*this <<r;
	}
	//��list�����ļ�д��
	template<typename Ty>
	void TypeWrite(const std::list<Ty> &lis)
	{
		//д�ߴ�
		int64_t size = lis.size();
		TypeWrite(size);
		//д����
		for(auto &r : lis)
			*this <<r;
	}
	//�Զ����������ļ�д��
	template<typename Ty, typename TyComp>
	void TypeWrite(const std::set<Ty, TyComp> &bst)
	{
		//д�ߴ�
		int64_t size = bst.size();
		TypeWrite(size);
		//д����
		for(auto &r : bst)
			*this <<r;
	}
	//�Զ�ֵ�����������ļ�д��
	template<typename Ty, typename TyComp>
	void TypeWrite(const std::multiset<Ty, TyComp> &bst)
	{
		//д�ߴ�
		int64_t size = bst.size();
		TypeWrite(size);
		//д����
		for(auto &r : bst)
			*this <<r;
	}
	//�Զ������Խ����ļ�д��
	template<typename Ty1, typename Ty2, typename TyComp>
	void TypeWrite(const std::map<Ty1, Ty2, TyComp> &bst)
	{
		//д�ߴ�
		int64_t size = bst.size();
		TypeWrite(size);
		//д����
		for(auto &r : bst)
			*this <<r.first <<r.second;
	}
	//�Զ�ֵ�������Խ����ļ�д��
	template<typename Ty1, typename Ty2, typename TyComp>
	void TypeWrite(const std::multimap<Ty1, Ty2, TyComp> &bst)
	{
		//д�ߴ�
		int64_t size = bst.size();
		TypeWrite(size);
		//д����
		for(auto &r : bst)
			*this <<r.first <<r.second;
	}
	//�Թ�ϣ������ļ�д��
	template<typename Ty, typename TyHash, typename TyComp>
	void TypeWrite(const std::unordered_set<Ty, TyHash, TyComp> &htb)
	{
		//д�ߴ�
		int64_t size = htb.size();
		TypeWrite(size);
		//д����
		for(auto &r : htb)
			*this <<r;
	}
	//�Զ�ֵ��ϣ������ļ�д��
	template<typename Ty, typename TyHash, typename TyComp>
	void TypeWrite(const std::unordered_multiset<Ty, TyHash, TyComp> &htb)
	{
		//д�ߴ�
		int64_t size = htb.size();
		TypeWrite(size);
		//д����
		for(auto &r : htb)
			*this <<r;
	}
	//�Թ�ϣ��Խ����ļ�д��
	template<typename Ty1, typename Ty2, typename TyHash, typename TyComp>
	void TypeWrite(const std::unordered_map<Ty1, Ty2, TyHash, TyComp> &htb)
	{
		//д�ߴ�
		int64_t size = htb.size();
		TypeWrite(size);
		//д����
		for(auto &r : htb)
			*this <<r.first <<r.second;
	}
	//�Զ�ֵ��ϣ��Խ����ļ�д��
	template<typename Ty1, typename Ty2, typename TyHash, typename TyComp>
	void TypeWrite(const std::unordered_multimap<Ty1, Ty2, TyHash, TyComp> &htb)
	{
		//д�ߴ�
		int64_t size = htb.size();
		TypeWrite(size);
		//д����
		for(auto &r : htb)
			*this <<r.first <<r.second;
	}
};

//д���Ͷ������ļ��������
//�����������
template<typename Ty>
class BinWriteFileIter:
	public std::iterator<std::output_iterator_tag, void,
	void, void, void>
{
private://����
	BinWriteFile *pBwf;//�����ļ�

public://��������
	//û��Ĭ�Ϲ���
	BinWriteFileIter()
		= delete;
	//��������
	BinWriteFileIter(BinWriteFile &bwf):
		pBwf(&bwf)
	{
	}
public://���������
	//�����ò�������������
	BinWriteFileIter &operator *() const
	{
		return *this;
	}
	//����������ʲôҲ��������������
	BinWriteFileIter &operator ++()
	{
		return *this;
	}
	BinWriteFileIter &operator ++(int)
	{
		return *this;
	}
	//��ֵ�������������
	BinWriteFileIter &operator =(const Ty &data)
	{
		*pBwf <<data;
		return *this;
	}
};


template<typename Ty>
class BinReadFileIter;
//�����Ͷ������ļ���
//��ͬ�������������ʹ�С��һ����ͬ��Ҫ��֤ʹ�ô�С�޹ر���
class BinReadFile
{
public:
	std::ifstream ifs;//�ļ���
	bool bFast= false;//����ģʽ�����Կ��ܵĻ��������
	static constexpr int64_t bufSize = 4096;//��ȡ�����С

public:
	//����
	explicit BinReadFile(bool o_bFast= false):
		bFast(o_bFast)
	{
	}
	//���ļ�����
	template<typename Ty, typename= typename std::enable_if<
		_IoOperatorIsRemoveCVRefSameSzOrString<Ty>::value>::type
	> explicit BinReadFile(Ty &&name, bool o_bFast= false):
		bFast(o_bFast)
	{
		Open(std::forward<Ty>(name));
	}
	//����
	~BinReadFile()
		= default;
	//ɾ������
	BinReadFile(const BinReadFile &)
		= delete;
	BinReadFile &operator =(BinReadFile &)
		= delete;
	//�ƶ�����
	BinReadFile(BinReadFile &&file)
		= default;
	//�ƶ���ֵ
	BinReadFile &operator =(BinReadFile &&file)
		= default;

public:
	//���ļ�
	BinReadFile &Open(const std::string &name)
	{
		ifs.close();
		ifs.open(name, std::ios::binary);
		if(ifs.bad())
			throw std::runtime_error("Open Read File Bad Error");
		return *this;
	}
	//����Close������������β�򷵻ؼ����
	bool Close(bool bTestEnd = false)
	{
		bool res = !bTestEnd;
		//����β
		if(bTestEnd) {
			res = TestEof();
		}
		ifs.close();
		if(ifs.bad())
			throw std::runtime_error("Can't Close File");
		return res;
	}
	//���ش�״̬
	bool IsOpen() const
	{
		return ifs.is_open();
	}
	//�ļ�������
	bool IsFail() const
	{
		return !ifs.good();
	}
	operator bool() const
	{
		return ifs.good();
	}
	//��������־
	void Clear()
	{
		ifs.clear();
	}
	//����β�������һ�ζ�ȡ����
	bool TestEof()
	{
		ifs.get();
		if(ifs.bad())
			throw std::runtime_error("Read File Bad Error");
		return ifs.eof();
	}
	//�ض�λλ��
	BinReadFile &Seekg(std::ifstream::pos_type pos)
	{
		ifs.seekg(pos);
		return *this;
	}
	BinReadFile &Seekg(std::ifstream::off_type off, std::ios_base::seekdir dir)
	{
		ifs.seekg(off, dir);
		return *this;
	}
	//��ѯλ��
	std::ifstream::pos_type Tellg()
	{
		return ifs.tellg();
	}
	//�Ի����������ļ���ȡ
	BinReadFile &ReadBuf(void *buf, size_t size)
	{
		ifs.read(reinterpret_cast<char *>(buf), size);
		if(ifs.bad())
			throw std::runtime_error("Read File Bad Error");
		return *this;
	}
	//����������������ж�ȡ
	template<typename Ty>
	typename std::enable_if<std::is_scalar<Ty>::value, BinReadFile &
	>::Type ReadArray(Ty *pt, size_t count)
	{
		ReadBuf(pt, sizeof(Ty)*count);
		return *this;
	}
	//�Է���������������ж�ȡ
	template<typename Ty>
	typename std::enable_if<!std::is_scalar<Ty>::value, BinReadFile &
	>::Type ReadArray(Ty *pt, size_t count)
	{
		for(; count!=0; --count, ++pt) {
			if(ifs.fail())
				return;
			*this >>*pt;
		}
		return *this;
	}
	//���ļ����������
	template<typename Ty>
	BinReadFile &operator >>(Ty &r)
	{
		TypeRead(r);
		return *this;
	}
public:
	//���������ͽ����ļ���ȡ������������������
	template<typename Ty>
	typename std::enable_if<std::is_scalar<
		typename std::remove_all_extents<Ty>::type>::value, void
	>::type TypeRead(Ty &r)
	{
		ReadBuf(&r, sizeof(r));
	}
	//�Է�����������������ļ���ȡ
	template<typename Ty, size_t N>
	typename std::enable_if<!std::is_scalar<
		typename std::remove_all_extents<Ty>::type>::value, void
	>::type TypeRead(Ty (&arrStr)[N])
	{
		for(int64_t i=0; i<N; ++i) {
			if(ifs.fail())
				return;
			*this >>arrStr[i];
		}
	}
	//��pair�����ļ���ȡ
	template<typename Ty1, typename Ty2>
	void TypeRead(std::pair<Ty1, Ty2> &pr)
	{
		*this >>pr.first >>pr.second;
	}
	//��tuple�����ļ���ȡ
	template<typename ...Tys>
	void TypeRead(std::tuple<Tys...> &tup)
	{
		TupleRead<sizeof...(Tys)-1>(tup);
	}
	template<int index, typename ...Tys>
	typename std::enable_if<(index>=0), void
	>::type TupleRead(std::tuple<Tys...> &tup)
	{
		*this >>std::get<index>(tup);
		TupleRead<index-1>(tup);
	}
	template<int index, typename Ty>
	void TupleRead(Ty &tup)
	{
	}
	//��string�����ļ���ȡ
	template<typename Ty>
	void TypeRead(std::basic_string<Ty> &str)
	{
		//���ߴ�
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		str.clear();
		//������
		if(bFast) {
			str.resize((std::basic_string<Ty>::size_type)size);
			ReadBuf(&str[0], (size_t)(size*sizeof(Ty)));
		}
		else {
			constexpr int64_t perBufSize = bufSize/sizeof(Ty);
			for(; size>0; size-=perBufSize) {
				if(ifs.fail())
					return;
				int64_t realSize = size<perBufSize? size:perBufSize;
				auto nowSize = str.size();
				str.resize((std::basic_string<Ty>::size_type)(nowSize+realSize));
				ReadBuf(&str[0]+nowSize*sizeof(Ty), (size_t)(realSize*sizeof(Ty)));
			}
		}
	}
	//��vector�������ͽ����ļ���ȡ
	template<typename Ty>
	typename std::enable_if<std::is_scalar<Ty>::value, void
	>::type TypeRead(std::vector<Ty> &vec)
	{
		//���ߴ�
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		constexpr int64_t perBufSize = bufSize/sizeof(Ty);
		vec.clear();
		//������
		if(bFast) {
			vec.resize((std::vector<Ty>::size_type)(size));
			ReadBuf(vec.data(), size*sizeof(Ty));
		}
		else {
			for(; size>0; size-=perBufSize) {
				if(ifs.fail())
					return;
				int64_t realSize = size<perBufSize? size:perBufSize;
				auto nowSize = vec.size();
				vec.resize((std::vector<Ty>::size_type)(nowSize+realSize));
				ReadBuf(vec.data()+nowSize*sizeof(Ty), realSize*sizeof(Ty));
			}
		}
	}
	//��vector���������ͽ����ļ���ȡ����Ĭ�Ϲ��캯��
	template<typename Ty>
	typename std::enable_if<!std::is_scalar<Ty>::value, void
	>::type TypeRead(std::vector<Ty> &vec)
	{
		//���ߴ�
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		//������
		vec.clear();
		if(bFast) {
			vec.resize((std::vector<Ty>::size_type)size);
			for(auto &r : vec) {
				if(ifs.fail())
					return;
				*this >>r;
			}
		}
		else {
			for(int64_t i=0; i<size; ++i) {
				if(ifs.fail())
					return;
				vec.emplace_back();
				*this >>vec.back();
			}
		}
	}
	//��array�������ͽ����ļ���ȡ
	template<typename Ty, size_t c_size>
	typename std::enable_if<std::is_scalar<Ty>::value, void
	>::type TypeRead(std::array<Ty, c_size> &arr)
	{
		//������
		ReadBuf(arr.data(), c_size*sizeof(Ty));
	}
	//��array���������ͽ����ļ���ȡ
	template<typename Ty, size_t c_size>
	typename std::enable_if<!std::is_scalar<Ty>::value, void
	>::type TypeRead(std::array<Ty, c_size> &arr)
	{
		//������
		for(int64_t i=0; i<c_size; ++i) {
			if(ifs.fail())
				return;
			*this >>arr[i];
		}
	}
	//��deque�����ļ���ȡ����Ĭ�Ϲ��캯��
	template<typename Ty>
	void TypeRead(std::deque<Ty> &deq)
	{
		//���ߴ�
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		deq.clear();
		//������
		for(int64_t i=0; i<size; ++i) {
			if(ifs.fail())
				return;
			deq.emplace_back();
			*this >>deq.back();
		}
	}
	//��list�����ļ���ȡ����Ĭ�Ϲ��캯��
	template<typename Ty>
	void TypeRead(std::list<Ty> &lis)
	{
		//���ߴ�
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		lis.clear();
		//������
		for(int64_t i=0; i<size; ++i) {
			if(ifs.fail())
				return;
			lis.emplace_back();
			*this >>lis.back();
		}
	}
	//�Զ����������ļ���ȡ����Ĭ�Ϲ�����ƶ���ֵ
	template<typename Ty, typename TyComp>
	void TypeRead(std::set<Ty, TyComp> &bst)
	{
		//���ߴ�
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		bst.clear();
		//������
		for(int64_t i=0; i<size; ++i) {
			if(ifs.fail())
				return;
			Ty data;
			*this >>data;
			bst.insert(bst.end(), std::move(data));
		}
	}
	//�Զ�ֵ�����������ļ���ȡ����Ĭ�Ϲ�����ƶ���ֵ
	template<typename Ty, typename TyComp>
	void TypeRead(std::multiset<Ty, TyComp> &bst)
	{
		//���ߴ�
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		bst.clear();
		//������
		for(int64_t i=0; i<size; ++i) {
			if(ifs.fail())
				return;
			Ty data;
			*this >>data;
			bst.insert(bst.end(), std::move(data));
		}
	}
	//�Զ������Խ����ļ���ȡ����Ĭ�Ϲ�����ƶ���ֵ
	template<typename Ty1, typename Ty2, typename TyComp>
	void TypeRead(std::map<Ty1, Ty2, TyComp> &bst)
	{
		//���ߴ�
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		bst.clear();
		//������
		for(int64_t i=0; i<size; ++i) {
			if(ifs.fail())
				return;
			Ty1 data;
			*this >>data;
			if(ifs.fail())
				return;
			auto res = bst.emplace_hint(bst.end(), std::piecewise_construct,
				std::forward_as_tuple(std::move(data)), std::make_tuple());
			*this >>res->second;
		}
	}
	//�Զ�ֵ�������Խ����ļ���ȡ����Ĭ�Ϲ�����ƶ���ֵ
	template<typename Ty1, typename Ty2, typename TyComp>
	void TypeRead(std::multimap<Ty1, Ty2, TyComp> &bst)
	{
		//���ߴ�
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		bst.clear();
		//������
		for(int64_t i=0; i<size; ++i) {
			if(ifs.fail())
				return;
			Ty1 data;
			*this >>data;
			if(ifs.fail())
				return;
			auto res = bst.emplace_hint(bst.end(), std::piecewise_construct,
				std::forward_as_tuple(std::move(data)), std::make_tuple());
			*this >>res->second;
		}
	}
	//�Թ�ϣ������ļ���ȡ����Ĭ�Ϲ�����ƶ���ֵ
	template<typename Ty, typename TyHash, typename TyComp>
	void TypeRead(std::unordered_set<Ty, TyHash, TyComp> &htb)
	{
		//���ߴ�
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		htb.clear();
		//������
		for(int64_t i=0; i<size; ++i) {
			if(ifs.fail())
				return;
			Ty data;
			*this >>data;
			htb.insert(std::move(data));
		}
	}
	//�Զ�ֵ��ϣ������ļ���ȡ����Ĭ�Ϲ�����ƶ���ֵ
	template<typename Ty, typename TyHash, typename TyComp>
	void TypeRead(std::unordered_multiset<Ty, TyHash, TyComp> &htb)
	{
		//���ߴ�
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		htb.clear();
		//������
		for(int64_t i=0; i<size; ++i) {
			if(ifs.fail())
				return;
			Ty data;
			*this >>data;
			htb.insert(std::move(data));
		}
	}
	//�Թ�ϣ��Խ����ļ���ȡ����Ĭ�Ϲ�����ƶ���ֵ
	template<typename Ty1, typename Ty2, typename TyHash, typename TyComp>
	void TypeRead(std::unordered_map<Ty1, Ty2, TyHash, TyComp> &htb)
	{
		//���ߴ�
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		htb.clear();
		//������
		for(int64_t i=0; i<size; ++i) {
			if(ifs.fail())
				return;
			Ty1 data;
			*this >>data;
			if(ifs.fail())
				return;
			auto res = htb.emplace(std::piecewise_construct,
				std::forward_as_tuple(std::move(data)), std::make_tuple()).first;
			*this >>res->second;
		}
	}
	//�Զ�ֵ��ϣ��Խ����ļ���ȡ����Ĭ�Ϲ�����ƶ���ֵ
	template<typename Ty1, typename Ty2, typename TyHash, typename TyComp>
	void TypeRead(std::unordered_multimap<Ty1, Ty2, TyHash, TyComp> &htb)
	{
		//���ߴ�
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		htb.clear();
		//������
		for(int64_t i=0; i<size; ++i) {
			if(ifs.fail())
				return;
			Ty1 data;
			*this >>data;
			if(ifs.fail())
				return;
			auto res = htb.emplace(std::piecewise_construct,
				std::forward_as_tuple(std::move(data)), std::make_tuple());
			*this >>res->second;
		}
	}
};

//�����Ͷ������ļ��������
//��������������洢���ͱ����Ĭ�Ϲ���
template<typename Ty>
class BinReadFileIter:
	public std::iterator<std::input_iterator_tag, Ty,
	int64_t, Ty *, Ty &>
{
private://����
	BinReadFile *pBrf;//�����ļ�
	int64_t size;//ʣ��д���С��������ʾ����д��
	Ty data;//�洢����

public://��������
	//Ĭ�Ϲ��죬��Ϊβ�������
	BinReadFileIter():
		pBrf(nullptr), size(0)
	{
	}
	//�������죬��ʼ���ж�ȡ����ѡ��ļ����ȡ��������
	BinReadFileIter(BinReadFile &brf, int64_t o_size= -1):
		pBrf(&brf), size(o_size)
	{
		Read();
	}
public://���������
	//�����ò����������������ã�֧���ƶ�����
	Ty &operator *() const
	{
		return data;
	}
	Ty *operator ->() const
	{
		return &operator *();
	}
	//����������ͬʱ���ж�ȡ
	BinReadFileIter &operator ++()
	{
		Read();
		return *this;
	}
	BinReadFileIter operator ++(int)
	{
		TemplateIter ret(*this);
		operator ++();
		return ret;
	}
	//�Ƚϲ�����ָ����ͬ���ļ�����Ϊ��ͬ
	bool operator ==(const BinReadFileIter &other) const
	{
		return pBrf==other.pBrf;
	}
	bool operator !=(const BinReadFileIter &other) const
	{
		return !operator ==(other);
	}
private:
	//��ȡ���ݣ�ÿ�ζ�ȡ��������Ĭ�Ϲ���һ�����ݣ���֧���ƶ�����
	void Read()
	{
		assert(pBrf!=nullptr);
		//��ʣ��0��תΪβ��
		if(size==0) {
			pBrf = nullptr;
			return;
		}
		//���
		_IoOperatorCallDestruct(data);
		new(&data) Ty();
		//��ȡ
		*pBrf >>data;
		//����ȡʧ����תΪβ��
		if(!*pBrf)
			pBrf = nullptr;
		//����ʣ��
		if(size>0)
			-- size;
	}
};
