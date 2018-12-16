#pragma once


/*
改行输入
差导出单词
改动业务逻辑框架
*/

//文件夹名
#define DATA_DIR_NAME "data"//数据文件夹
#define DATA_DIR_PATH "./" DATA_DIR_NAME//数据路径
#define DATA_FILE_EXT ".dat"//数据后缀名
#define INPUT_DIR_NAME "input"//输入文件夹
#define INPUT_DIR_PATH "./" INPUT_DIR_NAME//输入路径
#define INPUT_FILE_EXT ".txt"//数据后缀名

//特殊单词表
#define NOW_LIST "now"//前置单词表
#define TEMP_LIST "temp"//缓存单词表

#define SHOW_DEFAULT_NUM 25//预览默认个数
#define INPUT_SEGMENT_SIZE 4096//认为导入一个单词最大长度


//单词
struct Word
{
	string english;
	string chinese;
public:
	Word() =
		default;
	Word(const string &O_(english), const string &O_(chinese)= ""):
		O_INIT(english), O_INIT(chinese)
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
		if(index<0 || SafeLTE(index, rgx.mark_count())) {
			cout <<"编号不符合范围，重新输入: \n";
			continue;
		}
		break;
	}
	return true;
}


