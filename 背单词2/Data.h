#pragma once


/*
改动业务逻辑框架
加入undo功能
格式化加入存储表名
改更新函数的存储表名依赖
改词汇表搜索功能
*/

//文件夹名
#define DATA_DIR_NAME "data"//数据文件夹
#define DATA_DIR_PATH "./" DATA_DIR_NAME//数据路径
#define DATA_FILE_EXT ".dat"//数据后缀名
#define INPUT_DIR_NAME "input"//输入文件夹
#define INPUT_DIR_PATH "./" INPUT_DIR_NAME//输入路径
#define INPUT_FILE_EXT ".txt"//数据后缀名
#define OUPPUT_DIR_NAME "output"//输出文件夹
#define OUPPUT_DIR_PATH "./" OUPPUT_DIR_NAME//输出路径
#define OUPPUT_FILE_EXT ".txt"//数据后缀名

//特殊单词表
#define NOW_LIST_NAME "now"//前置单词表
#define TEMP_LIST_NAME "temp"//缓存单词表

#define SHOW_DEFAULT_NUM 20//预览默认个数
#define INPUT_SEGMENT_SIZE 4096//认为导入一个单词最大长度

#define TAB_BLANK "     "//统一使用的空白符，5个空格
#define LONG_TAB_BLANK "        "//统一使用的双倍空白符，8个空格
#define OUT_BLANK ":    "//统一使用指出的空白符，冒号和4和空格


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

using ListType = map<string, vector<Word>>;//单词表类型
using ListNameSet = set<string>;//单词表名记录集合


//单词位置索引类
struct ItemIndex
{
	ListType::iterator itList;//词表迭代器
	int idx;//词表内位置
	ItemIndex()
		= default;
	ItemIndex(ListType::iterator O_(itList), int O_(idx)):
		O_INIT_MOVE(itList), O_INIT(idx)
	{
	}
	//访问函数
	Word &operator *() const
	{
		return itList->second[idx];
	}
	Word *operator ->() const
	{
		return &itList->second[idx];
	}
};


template<typename TyIt>
inline void ItemIndexToListNameSet(ListNameSet &listName, TyIt st, TyIt ed)
{
	for(; st!=ed; ++st)
		listName.insert(st->itList->first);
}


//大小写规范字符串转换
inline string FormToStr(const string &str)
{
	string strRes;
	bool bFlag = false;
	for(auto ch : str) {
		if(ch=='-') {
			bFlag = true;
		}
		else {
			if(ch>='A' && ch<='Z')
				ch = 'a'+(ch-'A');
			if(bFlag && ch>='a' && ch<='z')
				ch = 'A'+(ch-'a');
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
		if(ch>='A' && ch<='Z') {
			strRes <<'-';
			ch = 'a'+(ch-'A');
		}
		strRes <<ch;
	}
	return strRes;
}


inline bool CanListAddWord(vector<Word> &list, const Word &word)
{
	return std::find(list.begin(), list.end(), word)==list.end();
}
inline bool ListAddWord(vector<Word> &list, const Word &word)
{
	if(!CanListAddWord(list, word))
		return false;
	list.push_back(word);
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
	return std::find_if(pstr->begin(), pstr->end(), CharIsNotBlank)
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
		if(index<0 || IntLTE(index, rgx.mark_count())) {
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
	const vector<Word> &ref;
	int idx;
public:
	explicit PrintIndexWord(const vector<Word> &O_(ref), int O_(idx)):
		O_INIT(ref), O_INIT(idx)
	{
	}
};
ostream &operator <<(ostream &os, const PrintIndexWord &word)
{
	os <<(word.idx+1) <<OUT_BLANK <<PrintWord(word.ref[word.idx]);
	return os;
}

//打印表名，序号和单词辅助类
class PrintListIndexWord
{
public:
	ListType::iterator ref;
	int idx;
public:
	explicit PrintListIndexWord(ListType::iterator O_(ref), int O_(idx))
		:
		O_INIT_MOVE(ref), O_INIT(idx)
	{
	}
	explicit PrintListIndexWord(const ItemIndex &o_index):
		ref(o_index.itList), idx(o_index.idx)
	{
	}
};
ostream &operator <<(ostream &os, const PrintListIndexWord &word)
{
	os <<word.ref->first <<" - " <<PrintIndexWord(word.ref->second, word.idx);
	return os;
}

//打印项索引
class PrintItemIndex
{
public:
	const ItemIndex &ref;
public:
	explicit PrintItemIndex(const ItemIndex &O_(ref)):
		O_INIT(ref)
	{
	}
};
ostream &operator <<(ostream &os, const PrintItemIndex &word)
{
	os <<"(" <<word.ref.itList->first <<"," <<word.ref.idx+1 <<")";
	return os;
}

//打印项索引向量
class PrintItemIndexVector
{
public:
	const vector<ItemIndex> &ref;
public:
	explicit PrintItemIndexVector(const vector<ItemIndex> &O_(ref)):
		O_INIT(ref)
	{
	}
};
ostream &operator <<(ostream &os, const PrintItemIndexVector &word)
{
	for(int i=0; i!=word.ref.size(); ++i) {
		os <<PrintItemIndex(word.ref[i]);
		if(i!=word.ref.size()-1)
			os <<",";
	}
	return os;
}



