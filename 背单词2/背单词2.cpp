// ������2.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Data.h"


//�����ݴ洢
map<string, vector<Word>> g_mapWord;



//��ȡ�洢����
void ReadDataFile()
{
	//�����ļ���
	auto lbdCreatDir = [](const string &name) {
		filesystem::path path("./"+name);
		//�����ڷ�Ŀ¼��ɾ��
		if(filesystem::exists(path)) {
			if(!filesystem::is_directory(path))
				filesystem::remove(path);
		}
		//���������򴴽�
		if(!filesystem::exists(path)) {
			filesystem::create_directory(path);
		}
	};
	lbdCreatDir(DATA_DIR_NAME);
	lbdCreatDir(INPUT_DIR_NAME);

	//�����ļ���ȡ
	filesystem::path path(DATA_DIR_PATH);
	for(auto &file : filesystem::directory_iterator(path)) {
		if(filesystem::is_regular_file(file)) {
			if(file.path().extension().string()==DATA_FILE_EXT) {
				BinReadFile brf(file.path().string());
				string name = FormToStr(file.path().stem().string());
				if(g_mapWord.find(name)!=g_mapWord.end())
					throw runtime_error("�ļ����б��ظ�");
				brf >>g_mapWord[name];
			}
		}
	}
	//����Ĭ���б�
	g_mapWord[NOW_LIST];
	g_mapWord[TEMP_LIST];
}


//�����ļ�
void WriteDataFile(const vector<string> &vecName)
{
	for(auto &oriName : vecName) {
		string name = StrToForm(oriName);
		filesystem::path path(DATA_DIR_PATH);
		path /= name+DATA_FILE_EXT;
		//��������ɾ���ļ�
		if(g_mapWord.find(oriName)==g_mapWord.end()) {
			if(filesystem::exists(path))
				filesystem::remove(path);
		}
		//������д�ļ�
		else {
			BinWriteFile bwf(path.string());
			bwf <<g_mapWord.at(oriName);
		}
	}
}


//��ʾ���дʻ��
void ChoiceList(const string &list, vector<string> &vecSave)
{
	bool bFirst = true;
	//���������б�
	for(auto &list : g_mapWord) {
		if(!bFirst)
			cout <<"\n";
		else
			bFirst = false;
		cout <<list.first <<"      " <<list.second.size();
	}
	cin >>endl;
}


//�л��ʻ��
void ChoiceChange(string &list, vector<string> &vecSave)
{
	cout <<"�����л������ƣ����򷵻�: ";
	string name;
	if(!CinGetLineFirstString(name))
		return;
	std::replace_if(name.begin(), name.end(), CharIsNotBroadId, '_');
	//�����������жϴ���
	if(g_mapWord.find(name)==g_mapWord.end()) {
		cout <<"\"" <<name <<"\"�����ڣ�����ǿհ״���: ";
		if(CinGetLineJudgeBlank()) {
			g_mapWord[name];
			vecSave.push_back(name);
			list = name;
		}
	}
	//������ı�
	else
		list = name;
}


//��ʾ��ǰ�ʻ��ʻ�
void ChoiceShow(const string &list, vector<string> &vecSave)
{
	auto &words = g_mapWord.at(list);
	if(words.empty())
		cout <<"���գ�";
	else {
		cout <<"������ĵ��ģ����Ϸ�����ʾȫ��: ";
		int st, ed;
		if(!(cin >>st >>ed >>endl)) {
			st = 1;
			ed = words.size();
		}
		-- st;
		RANGE_LIMIT(st, 0, (int)words.size()-1);
		RANGE_LIMIT(ed, st+1, (int)words.size());
		bool bFirst = true;
		if(st!=0) {
			cout <<0+1 <<":   " <<words[0].english
				<<"      " <<words[0].chinese <<"\n"
				<<"...";
			bFirst = false;
		}
		for(int i=st; i<ed; ++i) {
			if(!bFirst)
				cout <<"\n";
			bFirst = false;
			cout <<i+1 <<":   " <<words[i].english
				<<"      " <<words[i].chinese;
		}
		if(ed!=words.size()) {
			if(!bFirst)
				cout <<"\n";
			bFirst = false;
			cout <<"...\n"
				<<words.size() <<":   " <<words[words.size()-1].english
				<<"      " <<words[words.size()-1].chinese;
		}
	}
	cin >>endl;
}


//��ǰ�ʻ����ӵ���
void ChoiceAdd(const string &list, vector<string> &vecSave)
{
	auto &words = g_mapWord.at(list);
	bool bChange = false;
	while(true)
	{
		Word tmp;
		cout <<"����Ӣ�ģ����򷵻�: ";
		if(!CinGetLineJudgeBlank(&tmp.english))
			break;
		if(std::find(words.begin(), words.end(), tmp)!=words.end()) {
			cout <<"�����ظ�\n";
			continue;
		}
		cout <<"�������Ľ��ͣ����򷵻�: ";
		if(!CinGetLineJudgeBlank(&tmp.chinese))
			break;
		bChange = true;
		words.push_back(tmp);
		cout <<"��ӳɹ�\n";
	}
	if(bChange)
		vecSave.push_back(list);
}


//��ǰ�ʻ��ɾ������
string ChoiceDel(const string &list, vector<string> &vecSave)
{
	auto &words = g_mapWord.at(list);
	bool bChange = false;
	string res = list;
	while(true) {
		if(words.empty()) {
			cout <<"���ʱ�Ϊ��\n";
			if(list==NOW_LIST || list==TEMP_LIST)
				break;
			cout <<"�Ƿ�ɾ�����ʱ��ǿհ״���ɾ��: ";
			if(CinGetLineJudgeBlank()) {
				g_mapWord.erase(list);
				bChange = true;
				cout <<"ɾ�����ʱ�\n";
				res = NOW_LIST;
			}
			break;
		}
		cout <<"��ǰ��" <<words.size() <<"���ʣ�����Ԥ����㣬���Ϸ��򷵻�: ";
		int num;
		if(!(cin >>num >>endl))
			break;
		-- num;
		RANGE_LIMIT(num, 0, (int)words.size()-1);
		if(num!=0) {
			cout <<1 <<":   " <<words[0].english
				<<"      " <<words[0].chinese <<"\n"
				<<"...\n";
		}
		for(int i=0; i<SHOW_DEFAULT_NUM && num!=words.size(); ++i, ++num) {
			cout <<num+1 <<":   " <<words[num].english
				<<"      " <<words[num].chinese <<"\n";
		}
		if(num!=words.size()) {
			cout <<"...\n"
				<<words.size() <<":   " <<words[words.size()-1].english
				<<"      " <<words[words.size()-1].chinese <<"\n";
		}
		cout <<"����ɾ����Χ��ţ����Ϸ��򷵻�: ";
		int st, ed;
		if(!(cin >>st >>ed >>endl))
			break;
		-- st;
		if(st<0 || st>=(int)words.size() || ed<=st || ed>(int)words.size()) {
			cout <<"��Ŵ���\n";
			continue;
		}
		words.erase(words.begin()+st, words.begin()+ed);
		bChange = true;
		cout <<"ɾ��" <<ed-st <<"�ɹ�\n";
	}
	if(bChange)
		vecSave.push_back(list);
	return res;
}


//��ǰ�ʻ������
void ChoiceRecite(const string &list, vector<string> &vecSave)
{
	auto &words = g_mapWord.at(list);
	bool bChange = false;
	cout <<"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	cout <<"�س���һ��������dɾ�������෵��";
	cin >>endl;
	mt19937 ran((unsigned)system_clock::now().time_since_epoch().count());
	while(true) {
		if(list.empty()) {
			cout <<"���ʱ�Ϊ��\n";
			cin >>endl;
			break;
		}
		int index= ran()%words.size();
		cout <<"      " <<words[index].english <<"    ";
		cin >>endl;
		cout <<index+1 <<":   " <<words[index].chinese <<"\n"
			<<"����: ";
		string option;
		CinGetLineFirstString(option);
		if(option=="d") {
			words.erase(words.begin()+index);
			bChange = true;
			cout <<"ɾ���ɹ�\n";
		}
		else if(!option.empty())
			break;
		cout <<"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	}
	if(bChange)
		vecSave.push_back(list);
}


//��ǰ�ʻ���鵥��
void ChoiceCheck(const string &list, vector<string> &vecSave)
{
	cout <<"���뵼��ĵ��ʱ�Ϊ����Ĭ��now��";
	string inList;
	if(!CinGetLineFirstString(inList))
		inList = NOW_LIST;
	if(g_mapWord.find(inList)==g_mapWord.end()) {
		cout <<inList <<"������\n";
		return;
	}
	cout <<"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	cout <<"�س���һ��������l(��ĸ)����" <<inList <<"�����෵��";
	cin >>endl;
	vector<Word> tmpWords;
	bool bSameList = list==inList;
	auto &words = *(bSameList? &tmpWords:&g_mapWord.at(list));//Դ�ʿ�
	auto &inWords = g_mapWord.at(inList);//����ʿ�
	if(bSameList)
		swap(tmpWords, g_mapWord.at(list));
	vector<int> vecIndex(words.size());//����������
	for(int i=0; i!=vecIndex.size(); ++i)
		vecIndex[i] = i;
	mt19937 ran((unsigned)system_clock::now().time_since_epoch().count());
	std::random_shuffle(vecIndex.begin(), vecIndex.end(),
		[&ran](unsigned n) {return ran()%n; });
	bool bUser = true;
	//checkѭ��
	for(int i=0; i!=words.size(); ++i) {
		int index = vecIndex[i];
		string option;
		//�û�����ʱ
		if(bUser) {
			cout <<i+1 <<"/" <<words.size() <<"    " <<words[index].english <<"   ";
			cin >>endl;
			cout <<index+1 <<":   " <<words[index].chinese <<"\n"
				<<"����: ";
			CinGetLineFirstString(option);
		}
		//������ѡ�����û�����
		if(!bUser || option=="l") {
			if(bSameList) {
				inWords.push_back(std::move(words[index]));
				if(bUser)
					cout <<"����ɹ�\n";
			}
			else {
				if(std::find(inWords.begin(), inWords.end(), words[index])
					==inWords.end())
				{
					inWords.push_back(words[index]);
					cout <<"����ɹ�\n";
				}
				else
					cout <<"�����ظ�\n";
			}
		}
		//���˳�����now���������û�����״̬
		else if(!option.empty()) {
			if(bSameList) {
				-- i;
				bUser = false;
			}
			else
				break;
		}
	}
	vecSave.push_back(inList);
}


//�뵱ǰ�ʻ���Ƶ���
void ChoiceCopy(const string &list, vector<string> &vecSave)
{
	string newList;
	cout <<"����i���Ƶ���ǰ�ʻ������o�ӵ�ǰ�ʻ���ƣ����򷵻�: ";
	CinGetLineFirstString(newList);
	if(newList!="i" && newList!="o")
		return;
	bool bFrom = newList=="o";
	if(bFrom)
		cout <<"���뵼�뵽�Ĵʿ⣬�����뷵��: ";
	else
		cout <<"���뵼����Դ�ʿ⣬�����뷵��: ";
	while(true) {
		if(!CinGetLineFirstString(newList))
			return;
		else if(g_mapWord.find(newList)==g_mapWord.end())
			cout <<"δ�ҵ��ʿ�\n";
		else
			break;
	}
	auto &listFrom = bFrom? g_mapWord.at(list):g_mapWord.at(newList);
	auto &listTo = bFrom? g_mapWord.at(newList):g_mapWord.at(list);
	int success = 0, fail = 0;
	for(auto &word : listFrom) {
		if(std::find(listTo.begin(), listTo.end(), word)
			==listTo.end())
		{
			listTo.push_back(word);
			++ success;
		}
		else
			++ fail;
	}
	vecSave.push_back(bFrom? newList:list);
	cout <<"����������" <<success <<"���ظ�" <<fail;
	cin >>endl;
}


//�뵱ǰ�ʻ��ѡ���Ƶ���
void ChoiceSelCopy(const string &list, vector<string> &vecSave)
{
}


//���´ʻ����
void ChoiceUpdata(const string &list, vector<string> &vecSave)
{
	cout <<"�����������ʣ����򷵻�: ";
	Word target;
	if(!CinGetLineJudgeBlank(&target.english))
		return;
	vector<int> vecIndex;
	for(auto &pair : g_mapWord) {
		auto it = std::find(pair.second.begin(), pair.second.end(), target);
		if(it!=pair.second.end()) {
			vecSave.push_back(pair.first);
			vecIndex.push_back(it-pair.second.begin());
		}
	}
	if(vecSave.size()==0) {
		cout <<"δ�ҵ�\n";
		cin >>endl;
		return;
	}
	cout <<"���Ϊ��\n";
	for(int i=0; i!=vecSave.size(); ++i) {
		if(i!=0) {
			cout <<"\n";
		}
		auto &word = g_mapWord.at(vecSave[i])[vecIndex[i]];
		cout <<vecSave[i] <<":" <<vecIndex[i]+1
			<<"      " <<word.english <<"      " <<word.chinese;
	}
	cin >>endl;
	cout <<"����Ӣ�ģ�Ϊ���򲻱�: ";
	string tmp;
	if(CinGetLineJudgeBlank(&tmp))
		target.english = tmp;
	cout <<"�������ģ�Ϊ���򷵻�: ";
	if(!CinGetLineJudgeBlank(&target.chinese)) {
		vecSave.clear();
		return;
	}
	int num = 0;
	for(int i=0; i!=vecSave.size(); ++i) {
		auto &words = g_mapWord.at(vecSave[i]);
		if(std::find(words.begin(), words.begin()+vecIndex[i], target)!=words.begin()+vecIndex[i]
			|| std::find(words.begin()+vecIndex[i]+1, words.end(), target)!=words.end())
		{
			cout <<vecSave[i] <<"�����ظ�\n";
			continue;
		}
		words[vecIndex[i]] = target;
		++ num;
	}
	cout <<"�޸�" <<num <<"���";
	cin >>endl;
}


//�����ʻ����
void ChoiceSearch(const string &list, vector<string> &vecSave)
{
	cout <<"������������ȫ��ƥ�䣬���򷵻�: \n";
	string substr;
	regex rgx;
	if(!CinGetRegex(rgx))
		return;
	struct Res {
		vector<pair<string, int>> index;
		bool bWrong;
	};
	map<Word, Res> tmpList;//bool��ʾ�Ƿ���������
	for(auto &list : g_mapWord) {
		for(int i=0; i!=list.second.size(); ++i) {
			auto &word = list.second[i];
			if(std::regex_match(word.english, rgx)) {
				//���ʷ��������ж��Ƿ�����
				auto it = tmpList.find(word);
				if(it==tmpList.end()) {
					auto &refRes = tmpList.emplace(word, Res()).first->second;
					refRes.bWrong = false;
					refRes.index.emplace_back(list.first, i+1);
				}
				else {
					it->second.index.emplace_back(list.first, i+1);
					if(word.chinese!=it->first.chinese)
						it->second.bWrong = true;
				}
			}
		}
	}
	if(tmpList.empty()) {
		cout <<"δ�ҵ�";
		cin >>endl;
		return;
	}
	vecSave.push_back(TEMP_LIST);
	auto &oriTmpList = g_mapWord.at(TEMP_LIST);
	oriTmpList.clear();
	int cnt = 0;
	cout <<"���Ϊ��\n";
	for(auto &pr : tmpList) {
		cout <<"    " <<pr.first.english <<"     "
			<<pr.first.chinese
			<<"\n";
		for(auto &res : pr.second.index) {
			cout <<"(" <<res.first <<"," <<res.second <<"), ";
			++ cnt;
		}
		if(pr.second.bWrong)
			cout <<"���Ĳ�һ�£�";
		cout <<"\n";
		oriTmpList.push_back(std::move(pr.first));
	}
	cout <<"������" <<cnt <<"����" <<tmpList.size()
		<<"���Ѵ���\"" TEMP_LIST "\"��";
	cin >>endl;
}


//���ļ�����ʻ��
void ChoiceInput(const string &list, vector<string> &vecSave)
{
	cout <<"���ļ�����\"" <<INPUT_DIR_NAME <<"\"�ļ����£���׺��Ϊ" <<INPUT_FILE_EXT <<"\n";
	cout <<"�ļ���������׺����Ϊ�б���������ǿհ׽��е���: ";
	if(!CinGetLineJudgeBlank())
		return;
	//�����ļ���ȡ
	int cntList= 0, cntWrongList= 0, cntWord= 0, cntWrongWord= 0;
	filesystem::path path(INPUT_DIR_PATH);
	for(auto &file : filesystem::directory_iterator(path)) {
		if(filesystem::is_regular_file(file)) {
			if(file.path().extension().string()==INPUT_FILE_EXT) {
				//���ļ�������
				ifstream ifs(file.path().string());
				string name = file.path().stem().string();
				std::replace_if(name.begin(), name.end(), CharIsNotBroadId, '_');
				++ cntList;
				if(g_mapWord.find(name)!=g_mapWord.end())
					++ cntWrongList;
				auto &newWords = g_mapWord[name];
				vecSave.push_back(name);
				//������ȡ�ؼ��ֶ�
				std::string str;
				while(std::getline(ifs, str)) {
					Word word;
					//����Ч�ַ�ͷ
					auto itSt = std::find_if_not(str.begin(), str.end(), CharIsBlank);//a
					//�ж�ע��
					if(itSt==str.end() || *itSt=='#' || *itSt==';')
						continue;
					//���ֶ�β����Ҫ�����
					auto itGap = std::find_if(itSt+1, str.end(), CharIsBlank);//\t2
					word.english.assign(itSt, itGap);
					//�Һ�����
					auto itWordSt = std::find_if_not(itGap, str.end(), CharIsBlank);//d
					auto ritWordEd = std::find_if_not(str.rbegin(),
						std::string::reverse_iterator(itWordSt), CharIsBlank);//i
					//�洢ֵ������Ϊ��
					if(itWordSt!=str.end() && itWordSt<ritWordEd.base())
						word.chinese.assign(itWordSt, ritWordEd.base());
					if(word.chinese.empty())
						continue;
					//�ж��ظ�
					if(std::find(newWords.begin(), newWords.end(), word)
						==newWords.end())
					{
						newWords.push_back(word);
						++ cntWord;
					}
					else
						++ cntWrongWord;
				}
			}
		}
	}
	cout <<"�������ҵ��б�" <<cntList <<"���ظ��б�" <<cntWrongList
		 <<"�����뵥��" <<cntWord <<"���ظ�����" <<cntWrongWord;
	cin >>endl;
}


////�߼����ļ�����ʻ��
//void ChoiceAdInput(const string &list, vector<string> &vecSave)
//{
//	cout <<"���ļ�����\"" <<INPUT_DIR_NAME <<"\"�ļ����£�������б�����������׺��\n";
//	cout <<"���뵼���ļ�ƥ������ȫ��ƥ�䣬������׺�������򷵻�: \n";
//	regex rgxFile;
//	if(!CinGetRegex(rgxFile))
//		return;
//	cout <<"���뵼�뵥��Ӣ�ﲿ�֣����ֵ���ƥ�䣬����ƥ�䣬ע������ѡ���Ӵ������򷵻�: \n";
//	regex rgxEng;
//	if(!CinGetRegex(rgxEng))
//		return;
//	cout <<"����Ӣ�ﲿ��ѡ���Ӵ���ţ�0Ϊȫѡ�����Ϸ����أ�";
//	int choiceEng;
//	if(!CinGetRegexIndex(rgxEng, choiceEng))
//		return;
//	cout <<"���뵼�뵥�����Ĳ��֣����ֵ���ƥ�䣬����ƥ�䣬ע������ѡ���Ӵ������򷵻�: \n";
//	regex rgxChin;
//	if(!CinGetRegex(rgxChin))
//		return;
//	cout <<"�������Ĳ���ѡ���Ӵ���ţ�0Ϊȫѡ�����Ϸ����أ�";
//	int choiceChin;
//	if(!CinGetRegexIndex(rgxChin, choiceChin))
//		return;
//	cout <<"�Ƿ���е��룿����հ׷���: ";
//	if(!CinGetLineJudgeBlank())
//		return;
//	//�����ļ���ȡ
//	int cntList= 0, cntWrongList= 0, cntWord= 0, cntWrongWord= 0;
//	filesystem::path path(INPUT_DIR_PATH);
//	for(auto &file : filesystem::directory_iterator(path)) {
//		if(filesystem::is_regular_file(file)) {
//			if(std::regex_match(file.path().filename().string(), rgxFile)) {
//				//���ļ�������
//				ifstream ifs(file.path().string());
//				string name = file.path().stem().string();
//				std::replace_if(name.begin(), name.end(), CharIsNotBroadId, '_');
//				++ cntList;
//				if(g_mapWord.find(name)!=g_mapWord.end())
//					++ cntWrongList;
//				auto &newWords = g_mapWord[name];
//				vecSave.push_back(name);
//				//������ȡ�ؼ��ֶ�
//				std::string str;
//				Word word;
//				bool bEng = true;
//				while(std::getline(ifs, str)) {
//					//ƥ�����
//					while(!str.empty()) {
//						smatch matchRes;
//						if(std::regex_search(str, matchRes, *(bEng? &rgxEng:&rgxChin))) {
//							//Ӣ�ﴦ��
//							if(bEng) {
//								word.english = matchRes[choiceEng].str();
//							}
//							//���Ĵ���
//							else {
//								word.chinese = matchRes[choiceEng].str();
//								//����ʿ�
//								//�ж��ظ�
//								if(std::find(newWords.begin(), newWords.end(), word)
//									==newWords.end())
//								{
//									newWords.push_back(word);
//									++ cntWord;
//								}
//								else
//									++ cntWrongWord;
//							}
//							//��ת״̬
//							bEng = !bEng;
//						}
//						str = matchRes.suffix().str();
//					}
//				}
//			}
//		}
//	}
//	cout <<"�������ҵ��б�" <<cntList <<"���ظ��б�" <<cntWrongList
//		 <<"�����뵥��" <<cntWord <<"���ظ�����" <<cntWrongWord;
//	cin >>endl;
//}


//�߼����ļ�����ʻ��
void ChoiceAdInput(const string &list, vector<string> &vecSave)
{
	cout <<"���ļ�����\"" <<INPUT_DIR_NAME <<"\"�ļ����£�������б�����������׺��\n";
	cout <<"���뵼���ļ�ƥ������ȫ��ƥ�䣬������׺�������򷵻�: \n";
	regex rgxFile;
	if(!CinGetRegex(rgxFile))
		return;
	cout <<"����ƥ�䵥�����򣬲��ֵ���ƥ�䣬ע��������Ӣ�Ӵ������򷵻�: \n";
	regex rgxWord;
	if(!CinGetRegex(rgxWord))
		return;
	cout <<"����Ӣ�ﲿ��ѡ���Ӵ���ţ�0Ϊȫѡ�����Ϸ����أ�";
	int choiceEng;
	if(!CinGetRegexIndex(rgxWord, choiceEng))
		return;
	cout <<"�������Ĳ���ѡ���Ӵ���ţ�0Ϊȫѡ�����Ϸ����أ�";
	int choiceChin;
	if(!CinGetRegexIndex(rgxWord, choiceChin))
		return;
	cout <<"�Ƿ���е��룿����հ׷���: ";
	if(!CinGetLineJudgeBlank())
		return;
	//�����ļ���ȡ
	int cntList= 0, cntWrongList= 0, cntWord= 0, cntWrongWord= 0;
	filesystem::path path(INPUT_DIR_PATH);
	for(auto &file : filesystem::directory_iterator(path)) {
		if(filesystem::is_regular_file(file)
			&& std::regex_match(file.path().filename().string(), rgxFile))
		{
			//���ļ�������
			ifstream ifs(file.path().string());
			string name = file.path().stem().string();
			std::replace_if(name.begin(), name.end(), CharIsNotBroadId, '_');
			++ cntList;
			if(g_mapWord.find(name)!=g_mapWord.end())
				++ cntWrongList;
			auto &newWords = g_mapWord[name];
			vecSave.push_back(name);
			//���ļ������ַ�������������󳤶�
			std::string strInput(3*INPUT_SEGMENT_SIZE, 0);
			int size = 0;
			while(true) {
				ifs.read(&strInput[size], INPUT_SEGMENT_SIZE*3-size);
				size += (int)ifs.gcount();
				strInput.resize(size);
				auto citSt= strInput.cbegin(), citEd= strInput.cend();
				//�ַ���ƥ��ѭ����ֱ��ʣ�೤�Ȳ�����󳤶�
				while(true) {
					smatch mth;
					std::regex_search(citSt, citEd, mth, rgxWord);
					//���û��ƥ��
					if(mth.empty()) {
						citSt += std::max<int>(0, citEd-citSt-INPUT_SEGMENT_SIZE);
						break;
					}
					//��ƥ��λ�üӺ�׺������󳤶����ļ�δ����
					else if(mth.length()+mth.suffix().length()<INPUT_SEGMENT_SIZE && !ifs.eof()) {
						citSt += std::max<int>(0, mth[0].second-citSt-INPUT_SEGMENT_SIZE);
						break;
					}
					Word word;
					word.english = mth[choiceEng].str();
					word.chinese = mth[choiceChin].str();
					//�ж��ظ�������ʿ�
					if(std::find(newWords.begin(), newWords.end(), word)
						==newWords.end())
					{
						newWords.push_back(word);
						++ cntWord;
					}
					else
						++ cntWrongWord;
					//ѭ��
					if(mth[0].first==strInput.end())
						break;
					citSt = std::max(mth.suffix().first, mth[0].first+1);
					//ʣ�೤�Ȳ�����󳤶����ļ�δ����
					if(citEd-citSt<INPUT_SEGMENT_SIZE && !ifs.eof())
						break;
				}
				if(ifs.eof())
					break;
				//����
				std::copy(citSt, citEd, strInput.begin());
				size = citEd-citSt;
			}
		}
	}
	cout <<"�������ҵ��б�" <<cntList <<"���ظ��б�" <<cntWrongList
		 <<"�����뵥��" <<cntWord <<"���ظ�����" <<cntWrongWord;
	cin >>endl;
}


//�ʻ�������ļ�
void ChoiceOutput(const string &list, vector<string> &vecSave)
{

}



int main()
{
	//��ȡ�洢����
	ReadDataFile();

	//ҵ��ѭ��
	string list = NOW_LIST;
	while(true) {
		//�����ļ��б�
		vector<string> vecSave;
		
		string choice;
		cout <<"\n"
			<<"��ǰ���дʻ��: " <<g_mapWord.size() <<"\n"
			<<"��ǰ�ʻ��Ϊ: " <<list <<"�����е���: " <<g_mapWord.at(list).size() <<"\n"
			<<"����help�鿴����ѡ��\n"
			<<"����exit���˳�\n";

		while(true) {
			cout <<"���룺" <<flush;
			CinGetLineFirstString(choice);
			if(choice=="help") {
				cout <<"����ѡ������\n"
					<<"list����ʾ���дʻ��\n"
					<<"change���л��ʻ���򴴽�\n"
					<<"show����ʾ��ǰ�ʻ��ʻ�\n"
					<<"add����ǰ�ʻ����ӵ���\n"
					<<"del����ǰ�ʻ��ɾ������\n"
					<<"recite����ǰ�ʻ�����ʣ��зŻ������ʾ��ѡ��ɾ��\n"
					<<"check����ǰ�ʻ���鵥�ʣ��޷Ż������ʾ��ѡ����\n"
					<<"copy���뵱ǰ�ʻ���Ƶ���\n"
					<<"selCopy���뵱ǰ�ʻ��ѡ�񲢸��Ƶ��ʣ�δ��ɣ�\n"
					<<"updata�����´ʻ����\n"
					<<"search�������ʻ����\n"
					<<"input�����ļ�����ʻ��\n"
					<<"adInput���߼����ļ�����ʻ��\n"
					<<"output���ʻ�������ļ���δ��ɣ�\n";
			}
			else
				break;
		}
		if(choice=="list")
			ChoiceList(list, vecSave);
		else if(choice=="change")
			ChoiceChange(list, vecSave);
		else if(choice=="show")
			ChoiceShow(list, vecSave);
		else if(choice=="add")
			ChoiceAdd(list, vecSave);
		else if(choice=="del")
			list = ChoiceDel(list, vecSave);
		else if(choice=="recite")
			ChoiceRecite(list, vecSave);
		else if(choice=="check")
			ChoiceCheck(list, vecSave);
		else if(choice=="copy")
			ChoiceCopy(list, vecSave);
		else if(choice=="selCopy")
			ChoiceSelCopy(list, vecSave);
		else if(choice=="updata")
			ChoiceUpdata(list, vecSave);
		else if(choice=="search")
			ChoiceSearch(list, vecSave);
		else if(choice=="input")
			ChoiceInput(list, vecSave);
		else if(choice=="adInput")
			ChoiceAdInput(list, vecSave);
		else if(choice=="output")
			ChoiceOutput(list, vecSave);
		else if(choice=="exit")
			break;


		//�����ļ�
		WriteDataFile(vecSave);
	}


	return 0;
}

