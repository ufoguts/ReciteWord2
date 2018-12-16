#pragma once

//IO������

#include <cstdint>
#include <cassert>

#include <iostream>
#include <fstream>

#include<limits>
#include <type_traits>
#include <algorithm>
#include <initializer_list>
#include <utility>
#include <tuple>

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


//�����뺯���Իس����ķ�װ


#define _IOOPERATOR_COPY_ASSIGN(Type, other) {\
	if(this!=&(other)) {\
		this->~Type();\
		new(this) Type(other);\
	}\
	return *this;}//������ֵ

#define _IOOPERATOR_MOVE_ASSIGN(Type, other) {\
	if(this!=&(other)) {\
		this->~Type();\
		new(this) Type(std::move(other));\
	}\
	return *this;}//�ƶ���ֵ


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



//��������ȡ�����ļ�������δ�ҵ��ĵ�һ���ֶ�
//�ᾡ���ܶ�ȡ��������ļ������ֶΣ���ʹΪ�գ������滻ǰ��
//�����ļ���ʽӦΪȥ��"#define"�ĺ궨���ʽ��'#'';'��Ϊע��
template<typename Ty1, typename Ty2>
inline Ty2 StreamReadInit(std::istream &is, Ty1 &table,
	Ty2 itFieldSt, Ty2 itFieldEd, bool bCheckNull =true)
{
	//���ؼ��ֶ�д��
	Ty2 itField;
	std::set<std::string> bstField;
	for(itField=itFieldSt; itField!=itFieldEd; ++itField) {
		bstField.emplace(*itField);
	}
	auto lbdIsBlank = [](char ch) {
		return ch==' ' || ch=='\t' || ch=='\r' || ch=='\n';
	};
	//��"(\t1)abc(\t2)(\t3)def(\t4)(\t5)ghi\r\n"
	//������ȡ�ؼ��ֶ�
	std::string str;
	while(std::getline(is, str)) {
		std::string strField;
		//����Ч�ַ�ͷ
		auto itSt = std::find_if_not(str.begin(), str.end(), lbdIsBlank);//a
		//�ж�ע��
		if(itSt==str.end() || *itSt=='#' || *itSt==';')
			continue;
		//���ֶ�β����Ҫ�����
		auto itGap = std::find_if(itSt+1, str.end(), lbdIsBlank);//\t2
		strField.assign(itSt, itGap);
		//�ж��ڱ���
		if(bstField.find(strField)==bstField.end())
			continue;
		//�Һ�����
		auto itWordSt = std::find_if_not(itGap, str.end(), lbdIsBlank);//d
		auto ritWordEd = std::find_if_not(str.rbegin(),
			std::string::reverse_iterator(itWordSt), lbdIsBlank);//i
		//�洢ֵ������Ϊ��
		string strValue;
		if(itWordSt!=str.end() && itWordSt<ritWordEd.base())
			strValue.assign(itWordSt, ritWordEd.base());
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
		if(res==table.end() || (bCheckNull && res->second.empty()))
			break;
	}
	return itField;
}


//��������ȡ�����ļ�����Ԥ���ֶη�Χ�����ض�ȡ����
//�滻���޶�����ʹ�ÿ��ظ�����ʱ�޶���Ϊ�ظ����
//�����ļ���ʽӦΪȥ��"#define"�ĺ궨���ʽ��'#'';'��Ϊע��
template<typename Ty1>
inline int StreamReadAllInit(std::istream &is, Ty1 &table,
	bool bReplace =true)
{
	//��"(\t1)abc(\t2)(\t3)def(\t4)(\t5)ghi\r\n"
	//������ȡ�ؼ��ֶ�
	std::string str;
	while(std::getline(is, str)) {
		std::string strField;
		//����Ч�ַ�ͷ
		auto itSt = std::find_if_not(str.begin(), str.end(), lbdIsBlank);//a
		//�ж�ע��
		if(itSt==str.end() || *itSt=='#' || *itSt==';')
			continue;
		//���ֶ�β����Ҫ�����
		auto itGap = std::find_if(itSt+1, str.end(), lbdIsBlank);//\t2
		strField.assign(itSt, itGap);
		//�Һ�����
		auto itWordSt = std::find_if_not(itGap, str.end(), lbdIsBlank);//d
		auto ritWordEd = std::find_if_not(str.rbegin(),
			std::string::reverse_iterator(itWordSt), lbdIsBlank);//i
		//�洢ֵ������Ϊ��
		string strValue;
		if(itWordSt!=str.end() && itWordSt<ritWordEd.base())
			strValue.assign(itWordSt, ritWordEd.base());
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
	return table.size();
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
	//��־���ͣ���Ϊ�������ʾ����
	class NoLock
	{
	};
private:

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
		std::initializer_list<TyMtx *> intlMtx = {})
		:
		vecPOs(intlOs), vecPMtx(intlMtx)
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
	Hold<std::ostream &> operator <<(NoLock) const
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
		const OstreamSet<TyMtx> *o_pSet)
		:
		vecRes(std::move(o_vecRes)), pSet(o_pSet)
	{
	}
	//ɾ������
	Hold(const Hold &)
		= delete;
	Hold &operator =(const Hold &)
		= delete;
	//�ƶ��������Ƴ����ݲ�������������־
	Hold(Hold &&other):
		vecRes(std::move(other.vecRes)), pSet(other.pSet)
	{
		other.pSet = nullptr;
	}
	Hold &operator =(Hold &&other)
	{
		_IOOPERATOR_MOVE_ASSIGN(Hold, other);
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



//���Բ�����
template<typename TyOs =OstreamSet<>>
class AssertOperator
{
public:
	//���ѡ��
	enum class Option
	{
		thrw_log,//�׳��쳣�������־
		thrw,//�׳��쳣
		asst_log,//���Զ��Բ������־
		asst,//���Զ���
		log,//�����־
		none,//�޲���
	};
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
	Option option;//���ѡ��

public:
	//����
	explicit AssertOperator(TyOs &o_os, Option o_option):
		os(o_os), option(o_option)
	{
	}
public:
	//ָ��ѡ����ж���
	bool operator()(Option optionTmp, bool bCond, const std::string &str) const
	{
		if(bCond)
			return true;
		//��ѡ����в���
		if(optionTmp==Option::thrw_log) {
			os <<str <<flush;
			throw NoCatchError(str);
		}
		else if(optionTmp==Option::thrw) {
			throw NoCatchError(str);
		}
		else if(optionTmp==Option::asst_log) {
			os <<str <<flush;
			assert(false);
		}
		else if(optionTmp==Option::asst) {
			assert(false);
		}
		else if(optionTmp==Option::log) {
			os <<str <<flush;
		}
		else if(optionTmp==Option::none) {
		}
		return false;
	}
	//ʹ��Ĭ��ѡ����ж���
	bool operator()(bool bCond, const std::string &str) const
	{
		return operator()(option, bCond, str);
	}
	//ָ��ѡ������޲�������
	bool Omit(Option optionTmp, bool bCond) const
	{
		if(bCond)
			return true;
		//��ѡ����в���
		if(optionTmp==Option::thrw_log) {
			os <<flush;
			throw NoCatchError();
		}
		else if(optionTmp==Option::thrw) {
			throw NoCatchError();
		}
		else if(optionTmp==Option::asst_log) {
			os <<flush;
			assert(false);
		}
		else if(optionTmp==Option::asst) {
			assert(false);
		}
		else if(optionTmp==Option::log) {
			os <<flush;
		}
		else if(optionTmp==Option::none) {
		}
		return false;
	}
	//ʹ��Ĭ��ѡ������޲�������
	bool Omit(bool bCond) const
	{
		return Omit(option, bCond);
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
	BinWriteFile &Write(void *buf, size_t size)
	{
		ofs.write(reinterpret_cast<char *>(buf), size);
		if(ofs.bad())
			throw std::runtime_error("Can't Write File");
		return *this;
	}
	//д�ļ����������
	template<typename Ty>
	BinWriteFile &operator <<(const Ty &r)
	{
		TypeWrite(r);
		return *this;
	}
private:
	//���������ͽ����ļ�д�룬����������������
	template<typename Ty>
	typename std::enable_if<std::is_scalar<
		typename std::remove_all_extents<Ty>::type>::value, void
	>::type TypeWrite(const Ty &r)
	{
		ofs.write(reinterpret_cast<const char *>(&r), sizeof(r));
		if(ofs.bad())
			throw std::runtime_error("Can't Write File");
	}
	//�Է�����������������ļ�д��
	template<typename Ty, size_t N>
	typename std::enable_if<!std::is_scalar<
		typename std::remove_all_extents<Ty>::type>::value, void
	>::type TypeWrite(const Ty (&arrStr)[N])
	{
		for(int i=0; i<N; ++i)
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
		ofs.write(reinterpret_cast<const char *>(&str[0]), size*sizeof(Ty));
		if(ofs.bad())
			throw std::runtime_error("Can't Write File");
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
		ofs.write(reinterpret_cast<const char *>(vec.data()), size*sizeof(Ty));
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
		ofs.write(reinterpret_cast<const char *>(arr.data()), c_size*sizeof(Ty));
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
	template<typename Ty>
	void TypeWrite(const std::set<Ty> &bst)
	{
		//д�ߴ�
		int64_t size = bst.size();
		TypeWrite(size);
		//д����
		for(auto &r : bst)
			*this <<r;
	}
	//�Զ������Խ����ļ�д��
	template<typename Ty1, typename Ty2>
	void TypeWrite(const std::map<Ty1, Ty2> &bst)
	{
		//д�ߴ�
		int64_t size = bst.size();
		TypeWrite(size);
		//д����
		for(auto &r : bst)
			*this <<r.first <<r.second;
	}
	//�Թ�ϣ������ļ�д��
	template<typename Ty>
	void TypeWrite(const std::unordered_set<Ty> &htb)
	{
		//д�ߴ�
		int64_t size = htb.size();
		TypeWrite(size);
		//д����
		for(auto &r : htb)
			*this <<r;
	}
	//�Թ�ϣ��Խ����ļ�д��
	template<typename Ty1, typename Ty2>
	void TypeWrite(const std::unordered_map<Ty1, Ty2> &htb)
	{
		//д�ߴ�
		int64_t size = htb.size();
		TypeWrite(size);
		//д����
		for(auto &r : htb)
			*this <<r.first <<r.second;
	}
};


//�����Ͷ������ļ���
//��ͬ�������������ʹ�С��һ����ͬ��Ҫ��֤ʹ�ô�С�޹ر���
class BinReadFile
{
private:
	std::ifstream ifs;//�ļ���
	static constexpr int64_t bufSize = 1024;//��ȡ�����С

public:
	//����
	BinReadFile()
		= default;
	explicit BinReadFile(const std::string &name)
	{
		Open(name);
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
	BinReadFile &Read(void *buf, size_t size)
	{
		ifs.read(reinterpret_cast<char *>(buf), size);
		if(ifs.bad())
			throw std::runtime_error("Read File Bad Error");
		return *this;
	}
	//���ļ����������
	template<typename Ty>
	BinReadFile &operator >>(Ty &r)
	{
		TypeRead(r);
		return *this;
	}
	//���������ͽ����ļ���ȡ������������������
	template<typename Ty>
	typename std::enable_if<std::is_scalar<
		typename std::remove_all_extents<Ty>::type>::value, void
	>::type TypeRead(Ty &r)
	{
		ifs.read(reinterpret_cast<char *>(&r), sizeof(r));
		if(ifs.bad())
			throw std::runtime_error("Read File Bad Error");
	}
	//�Է�����������������ļ���ȡ
	template<typename Ty, size_t N>
	typename std::enable_if<!std::is_scalar<
		typename std::remove_all_extents<Ty>::type>::value, void
	>::type TypeRead(Ty (&arrStr)[N])
	{
		for(int i=0; i<N; ++i) {
			*this >>arrStr[i];
			if(ifs.fail())
				return;
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
		constexpr int64_t perBufSize = bufSize/sizeof(Ty);
		//������
		str.clear();
		for(; size>0; size-=perBufSize) {
			int64_t realSize = size<perBufSize? size:perBufSize;
			auto nowSize = str.size();
			str.resize((std::basic_string<Ty>::size_type)(nowSize+realSize));
			ifs.read(reinterpret_cast<char *>(&str[0])+nowSize*sizeof(Ty),
				realSize*sizeof(Ty));
			if(ifs.bad())
				throw std::runtime_error("Read File Bad Error");
			if(ifs.fail())
				return;
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
		constexpr int64_t perBufSize = bufSize/sizeof(Ty);
		//������
		vec.clear();
		for(; size>0; size-=perBufSize) {
			int64_t realSize = size<perBufSize? size:perBufSize;
			auto nowSize = vec.size();
			vec.resize(nowSize+realSize);
			ifs.read(reinterpret_cast<char *>(vec.data())+nowSize*sizeof(Ty),
				realSize*sizeof(Ty));
			if(ifs.bad())
				throw std::runtime_error("Read File Bad Error");
			if(ifs.fail())
				return;
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
		//������
		vec.clear();
		for(int i=0; i<size; ++i) {
			vec.emplace_back();
			*this >>vec.back();
			if(ifs.fail())
				return;
		}
	}
	//��array�������ͽ����ļ���ȡ
	template<typename Ty, size_t c_size>
	typename std::enable_if<std::is_scalar<Ty>::value, void
	>::type TypeRead(std::array<Ty, c_size> &arr)
	{
		//������
		ifs.read(reinterpret_cast<char *>(arr.data()), c_size*sizeof(Ty));
		if(ifs.bad())
			throw std::runtime_error("Read File Bad Error");
	}
	//��array���������ͽ����ļ���ȡ
	template<typename Ty, size_t c_size>
	typename std::enable_if<!std::is_scalar<Ty>::value, void
	>::type TypeRead(std::array<Ty, c_size> &arr)
	{
		//������
		for(int i=0; i<c_size; ++i) {
			*this >>arr[i];
			if(ifs.fail())
				return;
		}
	}
	//��deque�����ļ���ȡ����Ĭ�Ϲ��캯��
	template<typename Ty>
	void TypeRead(std::deque<Ty> &deq)
	{
		//���ߴ�
		int64_t size = -1;
		TypeRead(size);
		//������
		deq.clear();
		for(int i=0; i<size; ++i) {
			deq.emplace_back();
			*this >>deq.back();
			if(ifs.fail())
				return;
		}
	}
	//��list�����ļ���ȡ����Ĭ�Ϲ��캯��
	template<typename Ty>
	void TypeRead(std::list<Ty> &lis)
	{
		//���ߴ�
		int64_t size = -1;
		TypeRead(size);
		lis.clear();
		//������
		for(int i=0; i<size; ++i) {
			lis.emplace_back();
			*this >>lis.back();
			if(ifs.fail())
				return;
		}
	}
	//�Զ����������ļ���ȡ����Ĭ�Ϲ�����ƶ���ֵ
	template<typename Ty>
	void TypeRead(std::set<Ty> &bst)
	{
		//���ߴ�
		int64_t size = -1;
		TypeRead(size);
		bst.clear();
		//������
		for(int i=0; i<size; ++i) {
			Ty data;
			*this >>data;
			if(ifs.fail())
				return;
			if(!bst.insert(std::move(data)).second) {
				ifs.setstate(std::ios::failbit);
				return;
			}
		}
	}
	//�Զ������Խ����ļ���ȡ����Ĭ�Ϲ�����ƶ���ֵ
	template<typename Ty1, typename Ty2>
	void TypeRead(std::map<Ty1, Ty2> &bst)
	{
		//���ߴ�
		int64_t size = -1;
		TypeRead(size);
		bst.clear();
		//������
		for(int i=0; i<size; ++i) {
			Ty1 data;
			*this >>data;
			if(ifs.fail())
				return;
			auto res = bst.emplace(std::piecewise_construct,
				std::forward_as_tuple(std::move(data)), std::make_tuple());
			if(!res.second) {
				ifs.setstate(std::ios::failbit);
				return;
			}
			*this >>res.first->second;
		}
	}
	//�Թ�ϣ������ļ���ȡ����Ĭ�Ϲ�����ƶ���ֵ
	template<typename Ty>
	void TypeRead(std::unordered_set<Ty> &htb)
	{
		//���ߴ�
		int64_t size = -1;
		TypeRead(size);
		htb.clear();
		//������
		for(int i=0; i<size; ++i) {
			Ty data;
			*this >>data;
			if(ifs.fail())
				return;
			if(!htb.insert(std::move(data)).second) {
				ifs.setstate(std::ios::failbit);
				return;
			}
		}
	}
	//�Թ�ϣ��Խ����ļ���ȡ����Ĭ�Ϲ�����ƶ���ֵ
	template<typename Ty1, typename Ty2>
	void TypeRead(std::unordered_map<Ty1, Ty2> &htb)
	{
		//���ߴ�
		int64_t size = -1;
		TypeRead(size);
		htb.clear();
		//������
		for(int i=0; i<size; ++i) {
			Ty1 data;
			*this >>data;
			if(ifs.fail())
				return;
			auto res = htb.emplace(std::piecewise_construct,
				std::forward_as_tuple(std::move(data)), std::make_tuple());
			if(!res.second) {
				ifs.setstate(std::ios::failbit);
				return;
			}
			*this >>res.first->second;
		}
	}
};
