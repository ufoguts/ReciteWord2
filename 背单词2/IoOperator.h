#pragma once

//IO操作库

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


//差输入函数对回车检测的封装


#define _IOOPERATOR_COPY_ASSIGN(Type, other) {\
	if(this!=&(other)) {\
		this->~Type();\
		new(this) Type(other);\
	}\
	return *this;}//拷贝赋值

#define _IOOPERATOR_MOVE_ASSIGN(Type, other) {\
	if(this!=&(other)) {\
		this->~Type();\
		new(this) Type(std::move(other));\
	}\
	return *this;}//移动赋值


//存储某类型，值类型
template<typename Ty>
struct _IoOperatorTypeHold
{
	Ty hold;
public:
	//构造
	_IoOperatorTypeHold(const Ty &o_hold):
		hold(o_hold)
	{
	}
	_IoOperatorTypeHold(Ty &&o_hold):
		hold(std::move(o_hold))
	{
	}
};
//存储某类型，左值引用特化
template<typename Ty>
struct _IoOperatorTypeHold<Ty &>
{
	Ty &hold;
public:
	//构造
	_IoOperatorTypeHold(Ty &o_hold):
		hold(o_hold)
	{
	}
};
//存储某类型，右值引用特化
template<typename Ty>
struct _IoOperatorTypeHold<Ty &&>
{
	Ty &&hold;
public:
	//构造
	_IoOperatorTypeHold(Ty &&o_hold):
		hold(std::move(o_hold))
	{
	}
};



//输入流读取配置文件，返回未找到的第一个字段
//会尽可能读取填充配置文件所有字段，即使为空，后者替换前者
//配置文件格式应为去掉"#define"的宏定义格式，'#'';'都为注释
template<typename Ty1, typename Ty2>
inline Ty2 StreamReadInit(std::istream &is, Ty1 &table,
	Ty2 itFieldSt, Ty2 itFieldEd, bool bCheckNull =true)
{
	//将关键字段写入
	Ty2 itField;
	std::set<std::string> bstField;
	for(itField=itFieldSt; itField!=itFieldEd; ++itField) {
		bstField.emplace(*itField);
	}
	auto lbdIsBlank = [](char ch) {
		return ch==' ' || ch=='\t' || ch=='\r' || ch=='\n';
	};
	//若"(\t1)abc(\t2)(\t3)def(\t4)(\t5)ghi\r\n"
	//从流读取关键字段
	std::string str;
	while(std::getline(is, str)) {
		std::string strField;
		//找有效字符头
		auto itSt = std::find_if_not(str.begin(), str.end(), lbdIsBlank);//a
		//判断注释
		if(itSt==str.end() || *itSt=='#' || *itSt==';')
			continue;
		//找字段尾，不要求后续
		auto itGap = std::find_if(itSt+1, str.end(), lbdIsBlank);//\t2
		strField.assign(itSt, itGap);
		//判断在表中
		if(bstField.find(strField)==bstField.end())
			continue;
		//找后续词
		auto itWordSt = std::find_if_not(itGap, str.end(), lbdIsBlank);//d
		auto ritWordEd = std::find_if_not(str.rbegin(),
			std::string::reverse_iterator(itWordSt), lbdIsBlank);//i
		//存储值，可能为空
		string strValue;
		if(itWordSt!=str.end() && itWordSt<ritWordEd.base())
			strValue.assign(itWordSt, ritWordEd.base());
		//替换模式下
		if(bReplace) {
			auto itRes = table.find(strField);
			if(itRes==table.end())
				table.emplace(std::move(strField), std::move(strValue));
			else
				itRes->second = std::move(strValue);
				
		}
		//无动作模式下
		else {
			table.emplace(std::move(strField), std::move(strValue));
		}
	}
	//判断填充完毕
	for(itField=itFieldSt; itField!=itFieldEd; ++itField) {
		std::string strTmp = *itField;
		auto res = table.find(strTmp);
		//若没找到字段或检查空且字段空
		if(res==table.end() || (bCheckNull && res->second.empty()))
			break;
	}
	return itField;
}


//输入流读取配置文件，无预置字段范围，返回读取个数
//替换或无动作，使用可重复集合时无动作为重复添加
//配置文件格式应为去掉"#define"的宏定义格式，'#'';'都为注释
template<typename Ty1>
inline int StreamReadAllInit(std::istream &is, Ty1 &table,
	bool bReplace =true)
{
	//若"(\t1)abc(\t2)(\t3)def(\t4)(\t5)ghi\r\n"
	//从流读取关键字段
	std::string str;
	while(std::getline(is, str)) {
		std::string strField;
		//找有效字符头
		auto itSt = std::find_if_not(str.begin(), str.end(), lbdIsBlank);//a
		//判断注释
		if(itSt==str.end() || *itSt=='#' || *itSt==';')
			continue;
		//找字段尾，不要求后续
		auto itGap = std::find_if(itSt+1, str.end(), lbdIsBlank);//\t2
		strField.assign(itSt, itGap);
		//找后续词
		auto itWordSt = std::find_if_not(itGap, str.end(), lbdIsBlank);//d
		auto ritWordEd = std::find_if_not(str.rbegin(),
			std::string::reverse_iterator(itWordSt), lbdIsBlank);//i
		//存储值，可能为空
		string strValue;
		if(itWordSt!=str.end() && itWordSt<ritWordEd.base())
			strValue.assign(itWordSt, ritWordEd.base());
		//替换模式下
		if(bReplace) {
			auto itRes = table.find(strField);
			if(itRes==table.end())
				table.emplace(std::move(strField), std::move(strValue));
			else
				itRes->second = std::move(strValue);
				
		}
		//无动作模式下
		else {
			table.emplace(std::move(strField), std::move(strValue));
		}
	}
	return table.size();
}


//输入流辅助函数
//使用flush刷新一行，使用ends清空状态，使用endl同时完成刷新和清空
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



//输出流保持类
//用于在输出后附加控制格式，二次输出时真正输出，需外部重载运算符
template<typename Ty>
class OstreamHold
{
	OstreamHold()
		= delete;
};
//输出流保持类左值引用特化
template<typename Ty>
class OstreamHold<Ty &>
{
private:
	std::ostream &os;//存储os
	Ty &ref;//存储引用
public:
	//构造函数
	explicit OstreamHold(std::ostream &o_os, Ty &o_ref):
		os(o_os), ref(o_ref)
	{
	}
	//删除拷贝
	OstreamHold(const OstreamHold<Ty &> &)
		= delete;
	OstreamHold<Ty &> &operator =(const OstreamHold<Ty &> &)
		= delete;
	//默认移动
	OstreamHold(OstreamHold<Ty &> &&)
		= default;
	OstreamHold<Ty &> &operator =(OstreamHold<Ty &> &&)
		= default;
	//流引用
	std::ostream &operator()() const
	{
		return os;
	}
	//引用和成员引用
	Ty &operator *() const
	{
		return ref;
	}
	Ty *operator ->() const
	{
		return &ref;
	}
	//转换常类型版本
	operator OstreamHold<const Ty &>() const
	{
		return OstreamHold<const Ty &>(os, ref);
	}
};
//输出流保持类右值引用特化
template<typename Ty>
class OstreamHold<Ty &&>
{
private:
	std::ostream &os;//存储os
	Ty data;//存储引用
public:
	//构造函数
	explicit OstreamHold(std::ostream &o_os, Ty &&o_data):
		os(o_os), data(std::move(o_data))
	{
	}
	//删除拷贝
	OstreamHold(const OstreamHold<Ty &&> &)
		= delete;
	OstreamHold<Ty &&> &operator =(const OstreamHold<Ty &&> &)
		= delete;
	//默认移动
	OstreamHold(OstreamHold<Ty &&> &&)
		= default;
	OstreamHold<Ty &&> &operator =(OstreamHold<Ty &&> &&)
		= default;
	//转换左值流
	 OstreamHold<const Ty &> operator()() const
	{
		return OstreamHold<const Ty &>(os, data);
	}
	 OstreamHold<Ty &> operator()()
	{
		return OstreamHold<Ty &>(os, data);
	}
	//引用和成员引用
	Ty &operator *() const
	{
		return data;
	}
	Ty *operator ->() const
	{
		return &data;
	}
};



//输出流集合类
//用于多个输出流同时输出，支持非流引用返回值的传递流操作
//附加互斥锁功能接口，要求参数可锁类型，同时此类也满足可锁
//若有锁则流操作自动，除非传递非锁标志，在一串流操作开始结束时进行锁定解锁
template<typename TyMtx= std::mutex>
class OstreamSet
{
public:
	//输出集合传递类型
	template<typename TyRes>
	class Hold;
	//标志类型，作为流输出表示无锁
	class NoLock
	{
	};
private:

public:
	std::vector<std::ostream *> vecPOs;//输出流，更改非线程安全
	std::vector<TyMtx *> vecPMtx;//锁，更改非线程安全

public:
	//默认构造
	OstreamSet()
		= default;
	//删除拷贝
	OstreamSet(const OstreamSet &)
		= delete;
	OstreamSet &operator =(const OstreamSet &)
		= delete;
	//默认移动，非线程安全
	OstreamSet(OstreamSet &&)
		= default;
	OstreamSet &operator =(OstreamSet &&)
		= default;
	//列表构造
	explicit OstreamSet(std::initializer_list<std::ostream *> intlOs,
		std::initializer_list<TyMtx *> intlMtx = {})
		:
		vecPOs(intlOs), vecPMtx(intlMtx)
	{
	}
	//迭代器构造
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
	//转发流操作，线程安全，按顺序调用锁，需保证不死锁
	template<typename Ty>
	auto operator <<(Ty &&arg) const->
		Hold<decltype(*vecPOs.front()<<std::forward<Ty>(arg))> 
	{
		//流结果类型数组
		using TyRes = decltype(*vecPOs.front()<<std::forward<Ty>(arg));
		std::vector<_IoOperatorTypeHold<TyRes>> vecRes;
		vecRes.reserve(vecPOs.size());
		//调用锁定
		Lock();
		//输出并存储
		for(auto pOs : vecPOs) {
			vecRes.emplace_back(*pOs <<std::forward<Ty>(arg));
		}
		//生成结果的Hold类型
		return Hold<TyRes>(std::move(vecRes), vecPMtx.empty()? nullptr:this);
	}
	//对操作符进行转发
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
	//对不锁标志进行流操作，流输出将不进行锁定和解锁
	Hold<std::ostream &> operator <<(NoLock) const
	{
		//流结果类型数组
		using TyRes = std::ostream &;
		std::vector<_IoOperatorTypeHold<TyRes>> vecRes;
		vecRes.reserve(vecPOs.size());
		//存储类型
		for(auto pOs : vecPOs) {
			vecRes.emplace_back(*pOs);
		}
		//生成结果的Hold类型
		return Hold<TyRes>(std::move(vecRes), nullptr);
	}
	//调用ostream的成员函数，进行锁定，忽略返回值
	template<typename Ty, typename ...Tys>
	void operator()(const Ty &pmf, Tys &&...args) const
	{
		//调用锁定
		Lock();
		//调用成员函数
		for(auto pOs : vecPOs) {
			(pOs->*pmf)(std::forward<Tys>(args)...);
		}
		//调用解锁
		Unlock();
	}
	//调用锁定
	void Lock() const
	{
		//顺序调用锁
		for(auto pMtx : vecPMtx)
			pMtx->lock();
	}
	void lock() const
	{
		Lock();
	}
	//调用解锁
	void Unlock() const
	{
		//反序解锁
		for(auto rit =vecPMtx.rbegin(); rit!=vecPMtx.rend(); ++rit)
			(*rit)->unlock();
	}
	void unlock() const
	{
		Unlock();
	}
	//尝试锁定
	bool TryLock() const
	{
		//顺序调用尝试锁
		unsigned i;
		for(i =0; i<vecPMtx.size(); ++i) {
			if(!vecPMtx[i]->try_lock())
				break;
		}
		//尝试失败则解锁
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

//输出流集合传递类型
template<typename TyMtx>
template<typename TyRes>
class OstreamSet<TyMtx>::Hold
{
	friend class OstreamSet<TyMtx>;
private:
	std::vector<_IoOperatorTypeHold<TyRes>> vecRes;//输出流的结果数组
	const OstreamSet<TyMtx> *pSet;//输出流集合指针，析构解锁标志

private:
	//构造，输出集合引用初始化
	explicit Hold(std::vector<_IoOperatorTypeHold<TyRes>> &&o_vecRes,
		const OstreamSet<TyMtx> *o_pSet)
		:
		vecRes(std::move(o_vecRes)), pSet(o_pSet)
	{
	}
	//删除拷贝
	Hold(const Hold &)
		= delete;
	Hold &operator =(const Hold &)
		= delete;
	//移动操作，移出数据不需析构解锁标志
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
	//析构函数，根据标志进行解锁，反序解锁
	~Hold()
	{
		if(pSet!=nullptr) {
			//调用解锁
			pSet->Unlock();
		}
	}
public:
	//转发流操作，转发后去除解锁标志
	template<typename Ty>
	auto operator <<(Ty &&arg)->
		Hold<decltype(vecRes.front().hold<<std::forward<Ty>(arg))>
	{
		//此对象不需析构解锁
		auto ptr = pSet;
		pSet = nullptr;
		//新的流操作结果类型
		using TyResTmp = decltype(vecRes.front().hold<<std::forward<Ty>(arg));
		std::vector<_IoOperatorTypeHold<TyResTmp>> vecResTmp;
		vecResTmp.reserve(vecRes.size());
		//流输出并存储
		for(auto &refRes : vecRes) {
			vecResTmp.emplace_back(refRes.hold <<std::forward<Ty>(arg));
		}
		//构造新的Hold类型，转义析构解锁标志
		return Hold<TyResTmp>(std::move(vecResTmp), ptr);
	}
	//对操作符进行转发
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



//断言操作类
template<typename TyOs =OstreamSet<>>
class AssertOperator
{
public:
	//输出选项
	enum class Option
	{
		thrw_log,//抛出异常并输出日志
		thrw,//抛出异常
		asst_log,//调试断言并输出日志
		asst,//调试断言
		log,//输出日志
		none,//无操作
	};
private:
	//非捕获异常
	class NoCatchError:
		private std::runtime_error
	{
		//继承基类构造函数（除默认拷贝构造）
		using std::runtime_error::runtime_error;
	public:
		//默认构造
		NoCatchError():
			std::runtime_error("")
		{
		}
		//使用继承what
		using std::runtime_error::what;
		//省略合成5拷贝
	};

private:
	TyOs &os;//输出类
public:
	Option option;//输出选项

public:
	//构造
	explicit AssertOperator(TyOs &o_os, Option o_option):
		os(o_os), option(o_option)
	{
	}
public:
	//指定选项进行断言
	bool operator()(Option optionTmp, bool bCond, const std::string &str) const
	{
		if(bCond)
			return true;
		//按选项进行操作
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
	//使用默认选项进行断言
	bool operator()(bool bCond, const std::string &str) const
	{
		return operator()(option, bCond, str);
	}
	//指定选项进行无参数断言
	bool Omit(Option optionTmp, bool bCond) const
	{
		if(bCond)
			return true;
		//按选项进行操作
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
	//使用默认选项进行无参数断言
	bool Omit(bool bCond) const
	{
		return Omit(option, bCond);
	}
};



//写类型二进制文件类
//不同环境的内置类型大小不一定相同，要保证使用大小无关变量
class BinWriteFile
{
public:
	std::ofstream ofs;//文件流

public:
	//构造
	BinWriteFile()
		= default;
	explicit BinWriteFile(const std::string &name, bool o_bAfter=false)
	{
		Open(name, o_bAfter);
	}
	//析构
	~BinWriteFile()
		= default;
	//删除拷贝
	BinWriteFile(const BinWriteFile &)
		= delete;
	BinWriteFile &operator =(BinWriteFile &)
		= delete;
	//移动构造
	BinWriteFile(BinWriteFile &&file)
		= default;
	//移动赋值
	BinWriteFile &operator =(BinWriteFile &&file)
		= default;

public:
	//打开文件，o_bAfter是否追加写
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
	//关闭文件
	bool Close()
	{
		ofs.close();
		if(ofs.bad())
			throw std::runtime_error("Can't Close File");
		return true;
	}
	//返回打开状态
	bool isopen() const
	{
		return ofs.is_open();
	}
	//文件流可用，一定可用
	bool IsFail() const
	{
		return !ofs.good();
	}
	operator bool() const
	{
		return ofs.good();
	}
	//清除错误标志
	void Clear()
	{
		ofs.clear();
	}
	//刷新文件缓冲区
	BinWriteFile &Fflush()
	{
		ofs.flush();
		return *this;
	}
	//重定位位置
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
	//查询位置
	std::ofstream::pos_type Tellp()
	{
		return ofs.tellp();
	}
	//对缓冲区进行文件写入
	BinWriteFile &Write(void *buf, size_t size)
	{
		ofs.write(reinterpret_cast<char *>(buf), size);
		if(ofs.bad())
			throw std::runtime_error("Can't Write File");
		return *this;
	}
	//写文件运算符重载
	template<typename Ty>
	BinWriteFile &operator <<(const Ty &r)
	{
		TypeWrite(r);
		return *this;
	}
private:
	//对内置类型进行文件写入，包括内置类型数组
	template<typename Ty>
	typename std::enable_if<std::is_scalar<
		typename std::remove_all_extents<Ty>::type>::value, void
	>::type TypeWrite(const Ty &r)
	{
		ofs.write(reinterpret_cast<const char *>(&r), sizeof(r));
		if(ofs.bad())
			throw std::runtime_error("Can't Write File");
	}
	//对非内置类型数组进行文件写入
	template<typename Ty, size_t N>
	typename std::enable_if<!std::is_scalar<
		typename std::remove_all_extents<Ty>::type>::value, void
	>::type TypeWrite(const Ty (&arrStr)[N])
	{
		for(int i=0; i<N; ++i)
			*this <<arrStr[i];
	}
	//对pair进行文件写入
	template<typename Ty1, typename Ty2>
	void TypeWrite(const std::pair<Ty1, Ty2> &pr)
	{
		*this <<pr.first <<pr.second;
	}
	//对tuple进行文件写入
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
	//对string进行文件写入
	template<typename Ty>
	void TypeWrite(const std::basic_string<Ty> &str)
	{
		//写尺寸
		int64_t size = str.size();
		TypeWrite(size);
		//写数据
		ofs.write(reinterpret_cast<const char *>(&str[0]), size*sizeof(Ty));
		if(ofs.bad())
			throw std::runtime_error("Can't Write File");
	}
	//对vector内置类型进行文件写入
	template<typename Ty>
	typename std::enable_if<std::is_scalar<Ty>::value, void
	>::type TypeWrite(const std::vector<Ty> &vec)
	{
		//写尺寸
		int64_t size = vec.size();
		TypeWrite(size);
		//写数据
		ofs.write(reinterpret_cast<const char *>(vec.data()), size*sizeof(Ty));
	}
	//对vector非内置类型进行文件写入
	template<typename Ty>
	typename std::enable_if<!std::is_scalar<Ty>::value, void
	>::type TypeWrite(const std::vector<Ty> &vec)
	{
		//写尺寸
		int64_t size = vec.size();
		TypeWrite(size);
		//写数据
		for(auto &r : vec)
			*this <<r;
	}
	//对array内置类型进行文件写入
	template<typename Ty, size_t c_size>
	typename std::enable_if<std::is_scalar<Ty>::value, void
	>::type TypeWrite(const std::array<Ty, c_size> &arr)
	{
		//写数据
		ofs.write(reinterpret_cast<const char *>(arr.data()), c_size*sizeof(Ty));
	}
	//对array非内置类型进行文件写入
	template<typename Ty, size_t c_size>
	typename std::enable_if<!std::is_scalar<Ty>::value, void
	>::type TypeWrite(const std::array<Ty, c_size> &arr)
	{
		//写数据
		for(auto &r : arr)
			*this <<r;
	}
	//对deque进行文件写入
	template<typename Ty>
	void TypeWrite(const std::deque<Ty> &deq)
	{
		//写尺寸
		int64_t size = deq.size();
		TypeWrite(size);
		//写数据
		for(auto &r : deq)
			*this <<r;
	}
	//对list进行文件写入
	template<typename Ty>
	void TypeWrite(const std::list<Ty> &lis)
	{
		//写尺寸
		int64_t size = lis.size();
		TypeWrite(size);
		//写数据
		for(auto &r : lis)
			*this <<r;
	}
	//对二叉树进行文件写入
	template<typename Ty>
	void TypeWrite(const std::set<Ty> &bst)
	{
		//写尺寸
		int64_t size = bst.size();
		TypeWrite(size);
		//写数据
		for(auto &r : bst)
			*this <<r;
	}
	//对二叉树对进行文件写入
	template<typename Ty1, typename Ty2>
	void TypeWrite(const std::map<Ty1, Ty2> &bst)
	{
		//写尺寸
		int64_t size = bst.size();
		TypeWrite(size);
		//写数据
		for(auto &r : bst)
			*this <<r.first <<r.second;
	}
	//对哈希表进行文件写入
	template<typename Ty>
	void TypeWrite(const std::unordered_set<Ty> &htb)
	{
		//写尺寸
		int64_t size = htb.size();
		TypeWrite(size);
		//写数据
		for(auto &r : htb)
			*this <<r;
	}
	//对哈希表对进行文件写入
	template<typename Ty1, typename Ty2>
	void TypeWrite(const std::unordered_map<Ty1, Ty2> &htb)
	{
		//写尺寸
		int64_t size = htb.size();
		TypeWrite(size);
		//写数据
		for(auto &r : htb)
			*this <<r.first <<r.second;
	}
};


//读类型二进制文件类
//不同环境的内置类型大小不一定相同，要保证使用大小无关变量
class BinReadFile
{
private:
	std::ifstream ifs;//文件流
	static constexpr int64_t bufSize = 1024;//读取缓存大小

public:
	//构造
	BinReadFile()
		= default;
	explicit BinReadFile(const std::string &name)
	{
		Open(name);
	}
	//析构
	~BinReadFile()
		= default;
	//删除拷贝
	BinReadFile(const BinReadFile &)
		= delete;
	BinReadFile &operator =(BinReadFile &)
		= delete;
	//移动构造
	BinReadFile(BinReadFile &&file)
		= default;
	//移动赋值
	BinReadFile &operator =(BinReadFile &&file)
		= default;

public:
	//打开文件
	BinReadFile &Open(const std::string &name)
	{
		ifs.close();
		ifs.open(name, std::ios::binary);
		if(ifs.bad())
			throw std::runtime_error("Open Read File Bad Error");
		return *this;
	}
	//覆盖Close函数，若检测结尾则返回检测结果
	bool Close(bool bTestEnd = false)
	{
		bool res = !bTestEnd;
		//检测结尾
		if(bTestEnd) {
			res = TestEof();
		}
		ifs.close();
		if(ifs.bad())
			throw std::runtime_error("Can't Close File");
		return res;
	}
	//返回打开状态
	bool IsOpen() const
	{
		return ifs.is_open();
	}
	//文件流可用
	bool IsFail() const
	{
		return !ifs.good();
	}
	operator bool() const
	{
		return ifs.good();
	}
	//清除错误标志
	void Clear()
	{
		ifs.clear();
	}
	//检测结尾，会进行一次读取操作
	bool TestEof()
	{
		ifs.get();
		if(ifs.bad())
			throw std::runtime_error("Read File Bad Error");
		return ifs.eof();
	}
	//重定位位置
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
	//查询位置
	std::ifstream::pos_type Tellg()
	{
		return ifs.tellg();
	}
	//对缓冲区进行文件读取
	BinReadFile &Read(void *buf, size_t size)
	{
		ifs.read(reinterpret_cast<char *>(buf), size);
		if(ifs.bad())
			throw std::runtime_error("Read File Bad Error");
		return *this;
	}
	//读文件运算符重载
	template<typename Ty>
	BinReadFile &operator >>(Ty &r)
	{
		TypeRead(r);
		return *this;
	}
	//对内置类型进行文件读取，包括内置类型数组
	template<typename Ty>
	typename std::enable_if<std::is_scalar<
		typename std::remove_all_extents<Ty>::type>::value, void
	>::type TypeRead(Ty &r)
	{
		ifs.read(reinterpret_cast<char *>(&r), sizeof(r));
		if(ifs.bad())
			throw std::runtime_error("Read File Bad Error");
	}
	//对非内置类型数组进行文件读取
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
	//对pair进行文件读取
	template<typename Ty1, typename Ty2>
	void TypeRead(std::pair<Ty1, Ty2> &pr)
	{
		*this >>pr.first >>pr.second;
	}
	//对tuple进行文件读取
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
	//对string进行文件读取
	template<typename Ty>
	void TypeRead(std::basic_string<Ty> &str)
	{
		//读尺寸
		int64_t size = -1;
		TypeRead(size);
		constexpr int64_t perBufSize = bufSize/sizeof(Ty);
		//读数据
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
	//对vector内置类型进行文件读取
	template<typename Ty>
	typename std::enable_if<std::is_scalar<Ty>::value, void
	>::type TypeRead(std::vector<Ty> &vec)
	{
		//读尺寸
		int64_t size = -1;
		TypeRead(size);
		constexpr int64_t perBufSize = bufSize/sizeof(Ty);
		//读数据
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
	//对vector非内置类型进行文件读取，需默认构造函数
	template<typename Ty>
	typename std::enable_if<!std::is_scalar<Ty>::value, void
	>::type TypeRead(std::vector<Ty> &vec)
	{
		//读尺寸
		int64_t size = -1;
		TypeRead(size);
		//读数据
		vec.clear();
		for(int i=0; i<size; ++i) {
			vec.emplace_back();
			*this >>vec.back();
			if(ifs.fail())
				return;
		}
	}
	//对array内置类型进行文件读取
	template<typename Ty, size_t c_size>
	typename std::enable_if<std::is_scalar<Ty>::value, void
	>::type TypeRead(std::array<Ty, c_size> &arr)
	{
		//读数据
		ifs.read(reinterpret_cast<char *>(arr.data()), c_size*sizeof(Ty));
		if(ifs.bad())
			throw std::runtime_error("Read File Bad Error");
	}
	//对array非内置类型进行文件读取
	template<typename Ty, size_t c_size>
	typename std::enable_if<!std::is_scalar<Ty>::value, void
	>::type TypeRead(std::array<Ty, c_size> &arr)
	{
		//读数据
		for(int i=0; i<c_size; ++i) {
			*this >>arr[i];
			if(ifs.fail())
				return;
		}
	}
	//对deque进行文件读取，需默认构造函数
	template<typename Ty>
	void TypeRead(std::deque<Ty> &deq)
	{
		//读尺寸
		int64_t size = -1;
		TypeRead(size);
		//读数据
		deq.clear();
		for(int i=0; i<size; ++i) {
			deq.emplace_back();
			*this >>deq.back();
			if(ifs.fail())
				return;
		}
	}
	//对list进行文件读取，需默认构造函数
	template<typename Ty>
	void TypeRead(std::list<Ty> &lis)
	{
		//读尺寸
		int64_t size = -1;
		TypeRead(size);
		lis.clear();
		//读数据
		for(int i=0; i<size; ++i) {
			lis.emplace_back();
			*this >>lis.back();
			if(ifs.fail())
				return;
		}
	}
	//对二叉树进行文件读取，需默认构造和移动赋值
	template<typename Ty>
	void TypeRead(std::set<Ty> &bst)
	{
		//读尺寸
		int64_t size = -1;
		TypeRead(size);
		bst.clear();
		//读数据
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
	//对二叉树对进行文件读取，需默认构造和移动赋值
	template<typename Ty1, typename Ty2>
	void TypeRead(std::map<Ty1, Ty2> &bst)
	{
		//读尺寸
		int64_t size = -1;
		TypeRead(size);
		bst.clear();
		//读数据
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
	//对哈希表进行文件读取，需默认构造和移动赋值
	template<typename Ty>
	void TypeRead(std::unordered_set<Ty> &htb)
	{
		//读尺寸
		int64_t size = -1;
		TypeRead(size);
		htb.clear();
		//读数据
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
	//对哈希表对进行文件读取，需默认构造和移动赋值
	template<typename Ty1, typename Ty2>
	void TypeRead(std::unordered_map<Ty1, Ty2> &htb)
	{
		//读尺寸
		int64_t size = -1;
		TypeRead(size);
		htb.clear();
		//读数据
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
