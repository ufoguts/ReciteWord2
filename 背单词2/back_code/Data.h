#pragma once


/*
�Ķ�ҵ���߼����
����undo����
*/

//�ļ�����
#define DATA_DIR_NAME "data"//�����ļ���
#define DATA_DIR_PATH "./" DATA_DIR_NAME//����·��
#define DATA_DIR_PREFIX DATA_DIR_PATH "/"//����·��ǰ׺
#define DATA_FILE_EXT ".dat"//���ݺ�׺��
#define INPUT_DIR_NAME "input"//�����ļ���
#define INPUT_DIR_PATH "./" INPUT_DIR_NAME//����·��
#define INPUT_DIR_PREFIX INPUT_DIR_PATH "/"//����·��ǰ׺
#define INPUT_FILE_EXT ".txt"//���ݺ�׺��
#define OUPPUT_DIR_NAME "output"//����ļ���
#define OUPPUT_DIR_PATH "./" OUPPUT_DIR_NAME//���·��
#define OUPPUT_DIR_PREFIX OUPPUT_DIR_PATH "/"//���·��ǰ׺
#define OUPPUT_FILE_EXT ".txt"//���ݺ�׺��

//���ⵥ�ʱ�
#define NOW_LIST_NAME "now"//ǰ�õ��ʱ�
#define TEMP_LIST_NAME "temp"//���浥�ʱ�

#define SHOW_DEFAULT_NUM 20//Ԥ��Ĭ�ϸ���
#define INPUT_SEGMENT_SIZE 4096//��Ϊ����һ��������󳤶�

#define TAB_BLANK "     "//ͳһʹ�õĿհ׷���5���ո�
#define LONG_TAB_BLANK "        "//ͳһʹ�õ�˫���հ׷���8���ո�
#define OUT_BLANK ":    "//ͳһʹ��ָ���Ŀհ׷���ð�ź�4�Ϳո�
#define CLEAR_ENTER "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"//�����ã�15���س�


//����
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

using ListType = map<string, vector<Word>>;//���ʱ�����
using ListNameSet = set<string>;//���ʱ�����¼����


//����λ��������
struct ItemIndex
{
	ListType::iterator itList;//�ʱ������
	int idx;//�ʱ���λ��
	ItemIndex()
		= default;
	ItemIndex(ListType::iterator O_(itList), int O_(idx)):
		O_INIT_MOVE(itList), O_INIT(idx)
	{
	}
	//���ʺ���
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


//��Сд�淶�ַ���ת����ʹ���»�����ת���ַ�
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
	return std::find_if(pstr->begin(), pstr->end(), IsNotBlankChar)
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
		if(index<0 || IntGT(index, rgx.mark_count())) {
			cout <<"��Ų����Ϸ�Χ����������: \n";
			continue;
		}
		break;
	}
	return true;
}


//��ӡ���ʸ�����
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

//��ӡ��ź͵��ʸ�����
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

//��ӡ��������ź͵��ʸ�����
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

//��ӡ������
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

//��ӡ����������
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



