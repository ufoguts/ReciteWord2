#pragma once

//IO操作库

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




//先去除引用再去除常数易变属性
template<typename Ty>
struct _IoOperatorRemoveCVRef
{
	using type = typename std::remove_cv<typename std::remove_reference<Ty>::type>::type;
};

//后者去除引用常数易变后是否为前者
template<typename TyDst, typename TySrc>
struct _IoOperatorIsRemoveCVRefSame:
	std::is_same<TyDst, typename _IoOperatorRemoveCVRef<TySrc>::type>
{
};

//测试去除常易变易用属性后，是否为字符指针，字符数组或string类型
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

//字符空白符判断
inline constexpr bool _IoOperatorCharIsBlank(char ch)
{
	return ch==' ' || ch=='\t' || ch=='\r' || ch=='\n'
		|| ch=='\v' || ch=='\f';
}

//调用变量的析构函数
template<typename Ty>
inline void _IoOperatorCallDestruct(Ty &value)
{
	value.~Ty();
}



//按格式读取一行并返回字符串
inline bool _LineStringToInit(const std::string &strIn, std::string &strField,
	std::string &strValue)
{
	//若"(\t1)abc(\t2)(\t3)def(\t4)(\t5)ghi\r\n"
	//找有效字符头
	auto itSt = std::find_if_not(strIn.begin(), strIn.end(), _IoOperatorCharIsBlank);//a
	//判断空或注释'#'';'"//"
	if(itSt==strIn.end() || *itSt=='#' || *itSt==';'
		||(*itSt=='/' && itSt+1!=strIn.end() && itSt[1]=='/'))
		return false;
	//找字段尾，不要求后续
	auto itGap = std::find_if(itSt+1, strIn.end(), _IoOperatorCharIsBlank);//\t2
	strField.assign(itSt, itGap);
	//找后续词
	auto itWordSt = std::find_if_not(itGap, strIn.end(), _IoOperatorCharIsBlank);//d
	auto ritWordEd = std::find_if_not(strIn.rbegin(),
		std::string::const_reverse_iterator(itWordSt), _IoOperatorCharIsBlank);//i
	//存储值，可能为空
	if(itWordSt!=strIn.end() && itWordSt<ritWordEd.base())
		strValue.assign(itWordSt, ritWordEd.base());
	return true;
}



//输入流读取配置文件，有预定义字段范围，读取范围内的值，返回未找到的第一个字段
//配置文件格式应为去掉"#define"的宏定义格式，'#'';'"//"都为注释
//bReplace是否同一字段后者替换前者，若为重复集合且为否则重复添加
//bCheckNull是否拒绝读取值为空的情况
template<typename Ty1, typename Ty2>
inline Ty2 StreamReadPredefInit(std::istream &is, Ty1 &table,
	Ty2 itFieldSt, Ty2 itFieldEd, bool bReplace= true, bool bCheckNull= true)
{
	//将关键字段写入
	Ty2 itField;
	std::set<std::string> bstField;
	for(itField=itFieldSt; itField!=itFieldEd; ++itField) {
		bstField.emplace(*itField);
	}
	//从流读取关键字段
	std::string strIn, strField, strValue;
	while(std::getline(is, strIn)) {
		//解析行存入对应字符串
		if(!_LineStringToInit(strIn, strField, strValue))
			continue;
		//判断在表中
		if(bstField.find(strField)==bstField.end())
			continue;
		//判断值为空
		if(bCheckNull && strValue.empty())
			continue;
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
		if(res==table.end())
			break;
	}
	return itField;
}


//输入流读取配置文件，无预定义字段范围，读取所有文件的值，返回读取个数
//配置文件格式应为去掉"#define"的宏定义格式，'#'';'"//"都为注释
//bReplace是否同一字段后者替换前者，若为重复集合且为否则重复添加
//bCheckNull是否拒绝读取值为空的情况
template<typename Ty1>
inline int StreamReadAllInit(std::istream &is, Ty1 &table,
	bool bReplace= true, bool bCheckNull= true)
{
	//从流读取关键字段
	std::string strIn, strField, strValue;
	while(std::getline(is, strIn)) {
		//解析行存入对应字符串
		if(!_LineStringToInit(strIn, strField, strValue))
			continue;
		//判断值为空
		if(bCheckNull && strValue.empty())
			continue;
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
	return (int)table.size();
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



//输出流集合无锁标志，作为流输出表示无锁
class OstreamSetNoLock
{
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
		std::initializer_list<TyMtx *> intlMtx = {}
	): vecPOs(intlOs), vecPMtx(intlMtx)
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
	Hold<std::ostream &> operator <<(OstreamSetNoLock) const
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
		const OstreamSet<TyMtx> *o_pSet
	): vecRes(std::move(o_vecRes)), pSet(o_pSet)
	{
	}
	//删除拷贝
	Hold(const Hold &)
		= delete;
	Hold &operator =(const Hold &)
		= delete;
	//移动操作，移出数据不需析构解锁标志
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



//断言操作类输出选项
enum class AssertOption
{
	thrw_log,//抛出异常并输出日志
	thrw,//抛出异常
	asst_log,//调试断言并输出日志
	asst,//调试断言
	log,//输出日志
	none,//无操作
};

//断言操作类
template<typename TyOs>
class AssertOperator
{
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
	AssertOption option;//输出选项

public:
	//构造
	AssertOperator() =
		delete;
	explicit AssertOperator(TyOs &o_os, AssertOption o_option):
		os(o_os), option(o_option)
	{
	}
public:
	//指定选项进行断言
	bool operator()(AssertOption optionTmp,
		bool bCond, const std::string &str= "") const
	{
		if(bCond)
			return true;
		//按选项进行操作
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
	//使用默认选项进行断言
	bool operator()(bool bCond, const std::string &str= "") const
	{
		return operator()(option, bCond, str);
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
	BinWriteFile &WriteBuf(const void *buf, size_t size)
	{
		ofs.write(reinterpret_cast<const char *>(buf), size);
		if(ofs.bad())
			throw std::runtime_error("Can't Write File");
		return *this;
	}
	//对内置类型数组进行读取
	template<typename Ty>
	typename std::enable_if<std::is_scalar<Ty>::value, BinWriteFile &
	>::Type WriteArray(const Ty *pt, size_t count)
	{
		WriteBuf(pt, sizeof(Ty)*count);
		return *this;
	}
	//对非内置类型数组进行读取
	template<typename Ty>
	typename std::enable_if<!std::is_scalar<Ty>::value, BinWriteFile &
	>::Type WriteArray(const Ty *pt, size_t count)
	{
		for(; count!=0; --count, ++pt)
			*this <<*pt;
		return *this;
	}
	//写文件运算符重载
	template<typename Ty>
	BinWriteFile &operator <<(const Ty &r)
	{
		TypeWrite(r);
		return *this;
	}
public:
	//对内置类型进行文件写入，包括内置类型数组
	template<typename Ty>
	typename std::enable_if<std::is_scalar<
		typename std::remove_all_extents<Ty>::type>::value, void
	>::type TypeWrite(const Ty &r)
	{
		WriteBuf(&r, sizeof(r));
	}
	//对非内置类型数组进行文件写入
	template<typename Ty, size_t N>
	typename std::enable_if<!std::is_scalar<
		typename std::remove_all_extents<Ty>::type>::value, void
	>::type TypeWrite(const Ty (&arrStr)[N])
	{
		for(int64_t i=0; i<N; ++i)
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
		WriteBuf(&str[0], (size_t)(size*sizeof(Ty)));
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
		WriteBuf(vec.data(), size*sizeof(Ty));
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
		WriteBuf(arr.data(), c_size*sizeof(Ty));
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
	template<typename Ty, typename TyComp>
	void TypeWrite(const std::set<Ty, TyComp> &bst)
	{
		//写尺寸
		int64_t size = bst.size();
		TypeWrite(size);
		//写数据
		for(auto &r : bst)
			*this <<r;
	}
	//对多值二叉树进行文件写入
	template<typename Ty, typename TyComp>
	void TypeWrite(const std::multiset<Ty, TyComp> &bst)
	{
		//写尺寸
		int64_t size = bst.size();
		TypeWrite(size);
		//写数据
		for(auto &r : bst)
			*this <<r;
	}
	//对二叉树对进行文件写入
	template<typename Ty1, typename Ty2, typename TyComp>
	void TypeWrite(const std::map<Ty1, Ty2, TyComp> &bst)
	{
		//写尺寸
		int64_t size = bst.size();
		TypeWrite(size);
		//写数据
		for(auto &r : bst)
			*this <<r.first <<r.second;
	}
	//对多值二叉树对进行文件写入
	template<typename Ty1, typename Ty2, typename TyComp>
	void TypeWrite(const std::multimap<Ty1, Ty2, TyComp> &bst)
	{
		//写尺寸
		int64_t size = bst.size();
		TypeWrite(size);
		//写数据
		for(auto &r : bst)
			*this <<r.first <<r.second;
	}
	//对哈希表进行文件写入
	template<typename Ty, typename TyHash, typename TyComp>
	void TypeWrite(const std::unordered_set<Ty, TyHash, TyComp> &htb)
	{
		//写尺寸
		int64_t size = htb.size();
		TypeWrite(size);
		//写数据
		for(auto &r : htb)
			*this <<r;
	}
	//对多值哈希表进行文件写入
	template<typename Ty, typename TyHash, typename TyComp>
	void TypeWrite(const std::unordered_multiset<Ty, TyHash, TyComp> &htb)
	{
		//写尺寸
		int64_t size = htb.size();
		TypeWrite(size);
		//写数据
		for(auto &r : htb)
			*this <<r;
	}
	//对哈希表对进行文件写入
	template<typename Ty1, typename Ty2, typename TyHash, typename TyComp>
	void TypeWrite(const std::unordered_map<Ty1, Ty2, TyHash, TyComp> &htb)
	{
		//写尺寸
		int64_t size = htb.size();
		TypeWrite(size);
		//写数据
		for(auto &r : htb)
			*this <<r.first <<r.second;
	}
	//对多值哈希表对进行文件写入
	template<typename Ty1, typename Ty2, typename TyHash, typename TyComp>
	void TypeWrite(const std::unordered_multimap<Ty1, Ty2, TyHash, TyComp> &htb)
	{
		//写尺寸
		int64_t size = htb.size();
		TypeWrite(size);
		//写数据
		for(auto &r : htb)
			*this <<r.first <<r.second;
	}
};

//写类型二进制文件类迭代器
//是输出迭代器
template<typename Ty>
class BinWriteFileIter:
	public std::iterator<std::output_iterator_tag, void,
	void, void, void>
{
private://数据
	BinWriteFile *pBwf;//关联文件

public://基本函数
	//没有默认构造
	BinWriteFileIter()
		= delete;
	//关联构造
	BinWriteFileIter(BinWriteFile &bwf):
		pBwf(&bwf)
	{
	}
public://运算符重载
	//解引用操作，返回自身
	BinWriteFileIter &operator *() const
	{
		return *this;
	}
	//递增操作，什么也不做，返回自身
	BinWriteFileIter &operator ++()
	{
		return *this;
	}
	BinWriteFileIter &operator ++(int)
	{
		return *this;
	}
	//赋值操作，进行输出
	BinWriteFileIter &operator =(const Ty &data)
	{
		*pBwf <<data;
		return *this;
	}
};


template<typename Ty>
class BinReadFileIter;
//读类型二进制文件类
//不同环境的内置类型大小不一定相同，要保证使用大小无关变量
class BinReadFile
{
public:
	std::ifstream ifs;//文件流
	bool bFast= false;//快速模式，忽略可能的缓冲区溢出
	static constexpr int64_t bufSize = 4096;//读取缓存大小

public:
	//构造
	explicit BinReadFile(bool o_bFast= false):
		bFast(o_bFast)
	{
	}
	//打开文件构造
	template<typename Ty, typename= typename std::enable_if<
		_IoOperatorIsRemoveCVRefSameSzOrString<Ty>::value>::type
	> explicit BinReadFile(Ty &&name, bool o_bFast= false):
		bFast(o_bFast)
	{
		Open(std::forward<Ty>(name));
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
	BinReadFile &ReadBuf(void *buf, size_t size)
	{
		ifs.read(reinterpret_cast<char *>(buf), size);
		if(ifs.bad())
			throw std::runtime_error("Read File Bad Error");
		return *this;
	}
	//对内置类型数组进行读取
	template<typename Ty>
	typename std::enable_if<std::is_scalar<Ty>::value, BinReadFile &
	>::Type ReadArray(Ty *pt, size_t count)
	{
		ReadBuf(pt, sizeof(Ty)*count);
		return *this;
	}
	//对非内置类型数组进行读取
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
	//读文件运算符重载
	template<typename Ty>
	BinReadFile &operator >>(Ty &r)
	{
		TypeRead(r);
		return *this;
	}
public:
	//对内置类型进行文件读取，包括内置类型数组
	template<typename Ty>
	typename std::enable_if<std::is_scalar<
		typename std::remove_all_extents<Ty>::type>::value, void
	>::type TypeRead(Ty &r)
	{
		ReadBuf(&r, sizeof(r));
	}
	//对非内置类型数组进行文件读取
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
		if(ifs.fail())
			return;
		str.clear();
		//读数据
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
	//对vector内置类型进行文件读取
	template<typename Ty>
	typename std::enable_if<std::is_scalar<Ty>::value, void
	>::type TypeRead(std::vector<Ty> &vec)
	{
		//读尺寸
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		constexpr int64_t perBufSize = bufSize/sizeof(Ty);
		vec.clear();
		//读数据
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
	//对vector非内置类型进行文件读取，需默认构造函数
	template<typename Ty>
	typename std::enable_if<!std::is_scalar<Ty>::value, void
	>::type TypeRead(std::vector<Ty> &vec)
	{
		//读尺寸
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		//读数据
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
	//对array内置类型进行文件读取
	template<typename Ty, size_t c_size>
	typename std::enable_if<std::is_scalar<Ty>::value, void
	>::type TypeRead(std::array<Ty, c_size> &arr)
	{
		//读数据
		ReadBuf(arr.data(), c_size*sizeof(Ty));
	}
	//对array非内置类型进行文件读取
	template<typename Ty, size_t c_size>
	typename std::enable_if<!std::is_scalar<Ty>::value, void
	>::type TypeRead(std::array<Ty, c_size> &arr)
	{
		//读数据
		for(int64_t i=0; i<c_size; ++i) {
			if(ifs.fail())
				return;
			*this >>arr[i];
		}
	}
	//对deque进行文件读取，需默认构造函数
	template<typename Ty>
	void TypeRead(std::deque<Ty> &deq)
	{
		//读尺寸
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		deq.clear();
		//读数据
		for(int64_t i=0; i<size; ++i) {
			if(ifs.fail())
				return;
			deq.emplace_back();
			*this >>deq.back();
		}
	}
	//对list进行文件读取，需默认构造函数
	template<typename Ty>
	void TypeRead(std::list<Ty> &lis)
	{
		//读尺寸
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		lis.clear();
		//读数据
		for(int64_t i=0; i<size; ++i) {
			if(ifs.fail())
				return;
			lis.emplace_back();
			*this >>lis.back();
		}
	}
	//对二叉树进行文件读取，需默认构造和移动赋值
	template<typename Ty, typename TyComp>
	void TypeRead(std::set<Ty, TyComp> &bst)
	{
		//读尺寸
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		bst.clear();
		//读数据
		for(int64_t i=0; i<size; ++i) {
			if(ifs.fail())
				return;
			Ty data;
			*this >>data;
			bst.insert(bst.end(), std::move(data));
		}
	}
	//对多值二叉树进行文件读取，需默认构造和移动赋值
	template<typename Ty, typename TyComp>
	void TypeRead(std::multiset<Ty, TyComp> &bst)
	{
		//读尺寸
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		bst.clear();
		//读数据
		for(int64_t i=0; i<size; ++i) {
			if(ifs.fail())
				return;
			Ty data;
			*this >>data;
			bst.insert(bst.end(), std::move(data));
		}
	}
	//对二叉树对进行文件读取，需默认构造和移动赋值
	template<typename Ty1, typename Ty2, typename TyComp>
	void TypeRead(std::map<Ty1, Ty2, TyComp> &bst)
	{
		//读尺寸
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		bst.clear();
		//读数据
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
	//对多值二叉树对进行文件读取，需默认构造和移动赋值
	template<typename Ty1, typename Ty2, typename TyComp>
	void TypeRead(std::multimap<Ty1, Ty2, TyComp> &bst)
	{
		//读尺寸
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		bst.clear();
		//读数据
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
	//对哈希表进行文件读取，需默认构造和移动赋值
	template<typename Ty, typename TyHash, typename TyComp>
	void TypeRead(std::unordered_set<Ty, TyHash, TyComp> &htb)
	{
		//读尺寸
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		htb.clear();
		//读数据
		for(int64_t i=0; i<size; ++i) {
			if(ifs.fail())
				return;
			Ty data;
			*this >>data;
			htb.insert(std::move(data));
		}
	}
	//对多值哈希表进行文件读取，需默认构造和移动赋值
	template<typename Ty, typename TyHash, typename TyComp>
	void TypeRead(std::unordered_multiset<Ty, TyHash, TyComp> &htb)
	{
		//读尺寸
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		htb.clear();
		//读数据
		for(int64_t i=0; i<size; ++i) {
			if(ifs.fail())
				return;
			Ty data;
			*this >>data;
			htb.insert(std::move(data));
		}
	}
	//对哈希表对进行文件读取，需默认构造和移动赋值
	template<typename Ty1, typename Ty2, typename TyHash, typename TyComp>
	void TypeRead(std::unordered_map<Ty1, Ty2, TyHash, TyComp> &htb)
	{
		//读尺寸
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		htb.clear();
		//读数据
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
	//对多值哈希表对进行文件读取，需默认构造和移动赋值
	template<typename Ty1, typename Ty2, typename TyHash, typename TyComp>
	void TypeRead(std::unordered_multimap<Ty1, Ty2, TyHash, TyComp> &htb)
	{
		//读尺寸
		int64_t size = -1;
		TypeRead(size);
		if(ifs.fail())
			return;
		htb.clear();
		//读数据
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

//读类型二进制文件类迭代器
//是输入迭代器，存储类型必须可默认构造
template<typename Ty>
class BinReadFileIter:
	public std::iterator<std::input_iterator_tag, Ty,
	int64_t, Ty *, Ty &>
{
private://数据
	BinReadFile *pBrf;//关联文件
	int64_t size;//剩余写入大小，负数表示无限写入
	Ty data;//存储数据

public://基本函数
	//默认构造，成为尾后迭代器
	BinReadFileIter():
		pBrf(nullptr), size(0)
	{
	}
	//关联构造，初始进行读取，可选择的加入读取数量限制
	BinReadFileIter(BinReadFile &brf, int64_t o_size= -1):
		pBrf(&brf), size(o_size)
	{
		Read();
	}
public://运算符重载
	//解引用操作，返回数据引用，支持移动操作
	Ty &operator *() const
	{
		return data;
	}
	Ty *operator ->() const
	{
		return &operator *();
	}
	//递增操作，同时进行读取
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
	//比较操作，指向相同的文件就认为相同
	bool operator ==(const BinReadFileIter &other) const
	{
		return pBrf==other.pBrf;
	}
	bool operator !=(const BinReadFileIter &other) const
	{
		return !operator ==(other);
	}
private:
	//读取数据，每次读取是析构再默认构造一次数据，以支持移动操作
	void Read()
	{
		assert(pBrf!=nullptr);
		//若剩余0则转为尾后
		if(size==0) {
			pBrf = nullptr;
			return;
		}
		//清除
		_IoOperatorCallDestruct(data);
		new(&data) Ty();
		//读取
		*pBrf >>data;
		//若读取失败则转为尾后
		if(!*pBrf)
			pBrf = nullptr;
		//减少剩余
		if(size>0)
			-- size;
	}
};
