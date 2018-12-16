#pragma once


/*
��������
�������
�Ķ�ҵ���߼����
*/

//�ļ�����
#define DATA_DIR_NAME "data"//�����ļ���
#define DATA_DIR_PATH "./" DATA_DIR_NAME//����·��
#define DATA_FILE_EXT ".dat"//���ݺ�׺��
#define INPUT_DIR_NAME "input"//�����ļ���
#define INPUT_DIR_PATH "./" INPUT_DIR_NAME//����·��
#define INPUT_FILE_EXT ".txt"//���ݺ�׺��

//���ⵥ�ʱ�
#define NOW_LIST "now"//ǰ�õ��ʱ�
#define TEMP_LIST "temp"//���浥�ʱ�

#define SHOW_DEFAULT_NUM 25//Ԥ��Ĭ�ϸ���
#define INPUT_SEGMENT_SIZE 4096//��Ϊ����һ��������󳤶�


//����
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


//��Сд�淶�ַ���ת��
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


//��ȡ�еĵ�һ���ַ������ɹ���ȡΪtrue
inline bool CinGetLineFirstString(string &str)
{
	str.clear();
	string strIn;
	std::getline(cin, strIn);
	istringstream iss(strIn);
	return (bool)(iss >>str);
}
//��ȡ��������Ϊ�ַ������жϷǿ�Ϊtrue
inline bool CinGetLineJudgeBlank(string *pstr= nullptr)
{
	string strTmp;
	if(pstr==nullptr)
		pstr = &strTmp;
	std::getline(cin, *pstr);
	return std::find_if(pstr->begin(), pstr->end(), CharIsNotBlank)
		!= pstr->end();
}


//��ȡ����
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
			cout <<"���򲻺Ϸ�����������: ";
			continue;
		}
		break;
	}
	return true;
}


//��ȡ������Ӵ����
inline bool CinGetRegexIndex(regex &rgx, int &index)
{
	while(true) {
		if(!(cin >>index >>endl))
			return false;
		if(index<0 || SafeLTE(index, rgx.mark_count())) {
			cout <<"��Ų����Ϸ�Χ����������: \n";
			continue;
		}
		break;
	}
	return true;
}


