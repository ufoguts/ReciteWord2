#pragma once


/*TODO
改动业务逻辑框架
加入undo功能
抽象出搜索文件，搜索词汇表的代码
*/

//文件夹名
#define DATA_DIR_NAME "data"//数据文件夹
#define DATA_DIR_PATH "./" DATA_DIR_NAME//数据路径
#define DATA_DIR_PREFIX DATA_DIR_PATH "/"//数据路径前缀
#define DATA_FILE_EXT ".dat"//数据后缀名
#define INPUT_DIR_NAME "input"//输入文件夹
#define INPUT_DIR_PATH "./" INPUT_DIR_NAME//输入路径
#define INPUT_DIR_PREFIX INPUT_DIR_PATH "/"//输入路径前缀
#define INPUT_FILE_EXT ".txt"//数据后缀名
#define OUPPUT_DIR_NAME "output"//输出文件夹
#define OUPPUT_DIR_PATH "./" OUPPUT_DIR_NAME//输出路径
#define OUPPUT_DIR_PREFIX OUPPUT_DIR_PATH "/"//输出路径前缀
#define OUPPUT_FILE_EXT ".txt"//数据后缀名

//特殊单词表
#define NOW_LIST_NAME "now"//前置单词表
#define TEMP_LIST_NAME "temp"//缓存单词表

#define SHOW_DEFAULT_NUM 20//预览默认个数
#define INPUT_SEGMENT_SIZE 4096//认为导入一个单词最大长度

#define TAB_BLANK "     "//统一使用的空白符，5个空格
#define LONG_TAB_BLANK "        "//统一使用的双倍空白符，8个空格
#define OUT_BLANK ":    "//统一使用指出的空白符，冒号和4和空格
#define CLEAR_ENTER "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"//清屏用，15个回车


//单词
struct Word
{
	string english;
	string chinese;
public:
	Word() =
		default;
	Word(string O_(english), string O_(chinese)= ""):
		O_INIT_MOVE(english), O_INIT_MOVE(chinese)
	{
	}
};
bool operator ==(const Word &word1, const Word &word2)
{
	return word1.english==word2.english;
}
bool operator <(const Word &word1, const Word &word2)
{
	return word1.english<word2.english;
}
BinReadFile &operator >>(BinReadFile &brf, Word &word)
{
	return brf >>word.english >>word.chinese;
}
BinWriteFile &operator <<(BinWriteFile &bwf, const Word &word)
{
	return bwf <<word.english <<word.chinese;
}

using ListType = SequenceMultiSet<Word>;//单词表类型
using LibraryType = map<string, ListType>;//单词库类型
using ListNameSet = set<string>;//单词表名记录集合


//单词位置索引类
struct ItemIndex
{
	LibraryType::iterator itList;//词表迭代器
	ListType::Iter itWord;//词表内迭代器
	ItemIndex()
		= default;
	ItemIndex(LibraryType::iterator O_(itList), ListType::Iter O_(itWord)):
		O_INIT_MOVE(itList), O_INIT(itWord)
	{
	}
	//访问函数
	const Word &operator *() const
	{
		return *itWord;
	}
	const Word *operator ->() const
	{
		return &operator *();
	}
	//修改
	template<typename Ty>
	typename std::enable_if<IsRemoveCVRefSame<Word, Ty>::value, void
	>::type Modify(Ty &&word) const
	{
		itList->second.Modify(itWord, std::forward<Ty>(word));
	}
	template<typename Ty>
	typename std::enable_if<IsRemoveCVRefSame<Word, Ty>::value, bool
	>::type TryModify(Ty &&word) const
	{
		auto itFind = itList->second.Find(GetConstRef(word));
		if(itFind!=itList->second.ValueEnd() && itFind.GetIter()!=itWord)
			return false;
		Modify(std::forward<Ty>(word));
		return true;
	}
};


//将迭代器范围内的名字导入名字集合
template<typename TyIt>
inline void ItemIndexToListNameSet(ListNameSet &listName, TyIt st, TyIt ed)
{
	for(; st!=ed; ++st)
		listName.insert(st->itList->first);
}


//大小写规范字符串转换，使用下划线做转义字符，读写文件用
inline string FormToStr(const string &str)
{
	string strRes;
	bool bFlag = false;
	for(auto ch : str) {
		if(!bFlag && ch=='_') {
			bFlag = true;
		}
		else if(IsNumChar(ch) || IsLowChar(ch) || ch=='_') {
			if(bFlag && IsLowChar(ch))
				ch = LowCharToUppChar(ch);
			strRes <<ch;
			bFlag = false;
		}
	}
	return strRes;
}
inline string StrToForm(const string &str)
{
	string strRes;
	bool bFlag = false;
	for(auto ch : str) {
		if(IsUppChar(ch)) {
			strRes <<'_';
			ch = UppCharToLowChar(ch);
		}
		else if(ch=='_')
			strRes <<'_';
		strRes <<ch;
	}
	return strRes;
}


//查找重复并添加单词
inline bool ListAddWord(ListType &list, const Word &word)
{
	if(list.Count(word)!=0)
		return false;
	list.PushBack(word);
	return true;
}


//获取行的第一个字符串，成功获取为true
inline bool CinGetLineFirstString(string &str)
{
	str.clear();
	string strIn;
	std::getline(cin, strIn);
	istringstream iss(strIn);
	return (bool)(iss >>str);
}
//获取完整行作为字符串，判断非空为true
inline bool CinGetLineJudgeBlank(string *pstr= nullptr)
{
	string strTmp;
	if(pstr==nullptr)
		pstr = &strTmp;
	std::getline(cin, *pstr);
	return std::find_if(pstr->begin(), pstr->end(), IsNotBlankChar)
		!= pstr->end();
}


//获取正则
inline bool CinGetRegex(regex &rgx)
{
	string tmpstr;
	while(true) {
		if(!CinGetLineJudgeBlank(&tmpstr))
			return false;
		try {
			rgx.assign(tmpstr);
		}
		catch(regex_error &) {
			cout <<"正则不合法，重新输入: ";
			continue;
		}
		break;
	}
	return true;
}


//获取正则的子串编号
inline bool CinGetRegexIndex(regex &rgx, int &index)
{
	while(true) {
		if(!(cin >>index >>endl))
			return false;
		if(index<0 || IntGT(index, rgx.mark_count())) {
			cout <<"编号不符合范围，重新输入: \n";
			continue;
		}
		break;
	}
	return true;
}


//打印单词辅助类
class PrintWord
{
public:
	const Word &ref;
public:
	explicit PrintWord(const Word &O_(ref)):
		O_INIT(ref)
	{
	}
};
ostream &operator <<(ostream &os, const PrintWord &word)
{
	os <<word.ref.english <<LONG_TAB_BLANK <<word.ref.chinese;
	return os;
}

//打印序号和单词辅助类
class PrintIndexWord
{
public:
	int idx;
	const Word &ref;
public:
	explicit PrintIndexWord(int O_(idx), const Word &O_(ref)):
		O_INIT(idx), O_INIT(ref)
	{
	}
};
ostream &operator <<(ostream &os, const PrintIndexWord &word)
{
	os <<(word.idx+1) <<OUT_BLANK <<PrintWord(word.ref);
	return os;
}

//打印范围的序号和单词辅助类，末尾没有回车
class PrintRangeIndexWord
{
public:
	const ListType &ref;
	int st;
	int ed;
public:
	explicit PrintRangeIndexWord(ListType &O_(ref), int O_(st), int O_(ed)):
		O_INIT(ref), O_INIT(st), O_INIT(ed)
	{
	}
};
ostream &operator <<(ostream &os, const PrintRangeIndexWord &word)
{
	bool bFirst = true;
	if(word.st!=0) {
		cout <<PrintIndexWord(0, word.ref.Front()) <<"\n"
			<<"...";
		bFirst = false;
	}
	auto itWord = word.ref.GetIter(word.st);
	for(int i=word.st; i<word.ed; ++i, ++itWord) {
		if(!bFirst)
			cout <<"\n";
		bFirst = false;
		cout <<PrintIndexWord(i, *itWord);
	}
	if(word.ed!=word.ref.Size()) {
		if(!bFirst)
			cout <<"\n";
		bFirst = false;
		cout <<"...\n"
			<<PrintIndexWord(word.ref.Size()-1, word.ref.Back());
	}
	return os;
}

//打印序号和单词辅助类，序号是查找而来
class PrintFindIndexWord
{
public:
	const ListType &ref;
	ListType::Iter it;
public:
	explicit PrintFindIndexWord(const ListType &O_(ref),
		ListType::Iter O_(it)
	): O_INIT(ref), O_INIT(it)
	{
	}
};
ostream &operator <<(ostream &os, const PrintFindIndexWord &word)
{
	os <<(word.ref.GetIndex(word.it)+1) <<OUT_BLANK <<PrintWord(*word.it);
	return os;
}

//打印表名，序号和单词辅助类，序号是查找而来
class PrintListFindIndexWord
{
public:
	LibraryType::iterator ref;
	ListType::Iter it;
public:
	explicit PrintListFindIndexWord(LibraryType::iterator O_(ref),
		ListType::Iter O_(it)
	): O_INIT_MOVE(ref), O_INIT(it)
	{
	}
	explicit PrintListFindIndexWord(const ItemIndex &o_index):
		ref(o_index.itList), it(o_index.itWord)
	{
	}
};
ostream &operator <<(ostream &os, const PrintListFindIndexWord &word)
{
	os <<word.ref->first <<" - " <<PrintFindIndexWord(word.ref->second, word.it);
	return os;
}

//打印项索引，即表名和索引，不包括单词
class PrintItemFindIndex
{
public:
	const ItemIndex &ref;
public:
	explicit PrintItemFindIndex(const ItemIndex &O_(ref)):
		O_INIT(ref)
	{
	}
};
ostream &operator <<(ostream &os, const PrintItemFindIndex &word)
{
	os <<"(" <<word.ref.itList->first <<","
		<<(word.ref.itList->second.GetIndex(word.ref.itWord)+1) <<")";
	return os;
}

//打印项索引向量
class PrintItemFindIndexVector
{
public:
	const vector<ItemIndex> &ref;
public:
	explicit PrintItemFindIndexVector(const vector<ItemIndex> &O_(ref)):
		O_INIT(ref)
	{
	}
};
ostream &operator <<(ostream &os, const PrintItemFindIndexVector &word)
{
	for(int i=0; i!=word.ref.size(); ++i) {
		os <<PrintItemFindIndex(word.ref[i]);
		if(i!=word.ref.size()-1)
			os <<",";
	}
	return os;
}



