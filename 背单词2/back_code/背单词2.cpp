// ������2.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Data.h"


//�����ݴ洢
ListType g_mapWord;



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
	lbdCreatDir(OUPPUT_DIR_PATH);

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
	g_mapWord[NOW_LIST_NAME];
	g_mapWord[TEMP_LIST_NAME];
}


//�����ļ�
void WriteDataFile(const ListNameSet &setSave)
{
	for(auto &oriName : setSave) {
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
void ChoiceList(const string &curName, ListNameSet &setSave)
{
	bool bFirst = true;
	//���������б�
	for(auto &list : g_mapWord) {
		if(!bFirst)
			cout <<"\n";
		else
			bFirst = false;
		cout <<list.first <<LONG_TAB_BLANK <<list.second.size();
	}
	cin >>endl;
}


//�л��ʻ��
void ChoiceChange(string &curName, ListNameSet &setSave)
{
	cout <<"�����л������ƣ����򷵻�: ";
	string name;
	if(!CinGetLineFirstString(name))
		return;
	std::replace_if(name.begin(), name.end(), IsNotBroadIdChar, '_');
	//�����������жϴ���
	if(g_mapWord.find(name)==g_mapWord.end()) {
		cout <<"\"" <<name <<"\"�����ڣ�����ǿհ״���: ";
		if(CinGetLineJudgeBlank()) {
			g_mapWord[name];
			setSave.insert(name);
			curName = name;
		}
	}
	//������ı�
	else
		curName = name;
}


//��ʾ��ǰ�ʻ��ʻ�
void ChoiceShow(const string &curName, ListNameSet &setSave)
{
	auto &curList = g_mapWord.at(curName);
	if(curList.empty())
		cout <<"���գ�";
	else {
		cout <<"������ĵ��ģ����Ϸ�����ʾȫ��: ";
		int st, ed;
		if(!(cin >>st >>ed >>endl)) {
			st = 1;
			ed = curList.size();
		}
		-- st;
		RANGE_LIMIT(st, 0, (int)curList.size()-1);
		RANGE_LIMIT(ed, st+1, (int)curList.size());
		bool bFirst = true;
		if(st!=0) {
			cout <<PrintIndexWord(curList, 0) <<"\n"
				<<"...";
			bFirst = false;
		}
		for(int i=st; i<ed; ++i) {
			if(!bFirst)
				cout <<"\n";
			bFirst = false;
			cout <<PrintIndexWord(curList, i);
		}
		if(ed!=curList.size()) {
			if(!bFirst)
				cout <<"\n";
			bFirst = false;
			cout <<"...\n"
				<<PrintIndexWord(curList, curList.size()-1);
		}
	}
	cin >>endl;
}


//��ǰ�ʻ����ӵ���
void ChoiceAdd(const string &curName, ListNameSet &setSave)
{
	auto &curList = g_mapWord.at(curName);
	bool bChange = false;
	while(true)
	{
		Word tmp;
		cout <<"����Ӣ�ģ����򷵻�: ";
		if(!CinGetLineJudgeBlank(&tmp.english))
			break;
		if(!CanListAddWord(curList, tmp)) {
			cout <<"�����ظ�\n";
			continue;
		}
		cout <<"�������Ľ��ͣ����򷵻�: ";
		if(!CinGetLineJudgeBlank(&tmp.chinese))
			break;
		bChange = true;
		curList.push_back(tmp);
		cout <<"��ӳɹ�\n";
	}
	if(bChange)
		setSave.insert(curName);
}


//��ǰ�ʻ��ɾ������
void ChoiceDelete(string &curName, ListNameSet &setSave)
{
	auto &curList = g_mapWord.at(curName);
	bool bChange = false;
	string retName = curName;
	while(true) {
		if(curList.empty()) {
			cout <<"���ʱ�Ϊ��";
			if(curName==NOW_LIST_NAME || curName==TEMP_LIST_NAME) {
				cin >>endl;
				break;
			}
			cout <<"\n�Ƿ�ɾ�����ʱ��ǿհ״���ɾ��: ";
			if(CinGetLineJudgeBlank()) {
				g_mapWord.erase(curName);
				bChange = true;
				cout <<"��ɾ�����ʱ�";
				retName = NOW_LIST_NAME;
				cin >>endl;
			}
			break;
		}
		cout <<"��ǰ��" <<curList.size() <<"���ʣ�����Ԥ����㣬���Ϸ��򷵻�: ";
		int num;
		if(!(cin >>num >>endl))
			break;
		-- num;
		RANGE_LIMIT(num, 0, (int)curList.size()-1);
		if(num!=0) {
			cout <<PrintIndexWord(curList, 0) <<"\n"
				<<"...\n";
		}
		for(int i=0; i<SHOW_DEFAULT_NUM && num!=curList.size(); ++i, ++num) {
			cout <<PrintIndexWord(curList, num) <<"\n";
		}
		if(num!=curList.size()) {
			cout <<"...\n"
				<<PrintIndexWord(curList, curList.size()-1) <<"\n";
		}
		cout <<"����ɾ����Χ��ţ����Ϸ��򷵻�: ";
		int st, ed;
		if(!(cin >>st >>ed >>endl))
			break;
		-- st;
		if(!IS_IN_RANGE(st, 0, (int)curList.size()-1)
			|| !IS_IN_RANGE(ed, st+1, (int)curList.size()))
		{
			cout <<"��Ŵ���\n";
			continue;
		}
		curList.erase(curList.begin()+st, curList.begin()+ed);
		bChange = true;
		cout <<"ɾ��" <<ed-st <<"�ɹ�\n";
	}
	if(bChange)
		setSave.insert(curName);
	curName = retName;
}


//���´ʻ����
void ChoiceUpdata(const string &curName, ListNameSet &setSave)
{
	while(true) {
		cout <<"�����������ʣ����򷵻�: ";
		Word target;
		if(!CinGetLineJudgeBlank(&target.english))
			return;
		vector<ItemIndex> vecIndex;//���ҽ����
		//���ҵ���
		for(auto itList=g_mapWord.begin(); itList!=g_mapWord.end(); ++itList) {
			auto it = std::find(itList->second.begin(), itList->second.end(), target);
			if(it!=itList->second.end()) {
				vecIndex.emplace_back(itList, it-itList->second.begin());
			}
		}
		if(vecIndex.size()==0) {
			cout <<"δ�ҵ�";
			cin >>endl;
			continue;
		}
		cout <<"���Ϊ��\n";
		for(auto &idx: vecIndex) {
			cout <<PrintListIndexWord(idx) <<"\n";
		}
		//��������
		cout <<"����Ӣ�ģ�Ϊ���򲻱�: ";
		string tmp;
		if(CinGetLineJudgeBlank(&tmp))
			target.english = tmp;
		cout <<"�������ģ�Ϊ����ȡ��: ";
		if(!CinGetLineJudgeBlank(&target.chinese)) {
			continue;
		}
		//�ı䵥��
		int num = 0;
		for(auto &idx: vecIndex) {
			auto itSt = idx.itList->second.begin(),
				itMd = idx.itList->second.begin()+idx.idx,
				itEd = idx.itList->second.end();
			if(std::find(itSt, itMd, target)!=itMd || std::find(itMd+1, itEd, target)!=itEd)
			{
				cout <<idx.itList->first <<"�����ظ�\n";
				continue;
			}
			*idx = target;
			++ num;
			setSave.insert(idx.itList->first);
		}
		cout <<"�޸�" <<num <<"���";
		cin >>endl;
	}
}


//��ǰ�ʻ������
void ChoiceRecite(const string &curName, ListNameSet &setSave)
{
	auto &curList = g_mapWord.at(curName);
	auto &wordsTemp = g_mapWord.at(TEMP_LIST_NAME);
	bool bChange = false;//��ǰ�ʱ��Ƿ�ı�
	bool bHasTemp = false;//temp�ʱ��Ƿ�ı�
	cout <<"ÿ�����ʰ����ı���������ʳ��ָ���Ϊ�������softmax����\n"
		<<"������������ʾ��ʼ������ͱ�������ޣ����Ϸ��򷵻�: ";
	int maxFlag, initFlag;//����Ǵ���
	if(!(cin >>initFlag >>maxFlag >>endl))
		return;
	if(!(initFlag>0 && maxFlag>=initFlag))
		return;
	double base;//ָ�����ʵ�
	cout <<"���븡������ʾsoftmax�������������Ϸ���ʹ��ȱʡֵ1.5: ";
	if(!(cin >>base >>endl))
		base = 1.5;
	vector<int> vecFlag(curList.size(), initFlag);//���ʱ������
	vector<double> vecPrb(curList.size());//��������
	for(int i=0; i!=curList.size(); ++i)
		vecPrb[i] = FastPower(base, vecFlag[i]);
	int sumFlag = curList.size()*initFlag;
	cout <<CLEAR_ENTER;
	cout <<"����d���ٱ�ǣ����Ϊ0��ɾ�����ʣ�����s���ֱ�ǣ�����+���뵽temp\n"
		<<"s��d����ͬʱ���֣���������ָ�һ�����������෵��";
	cin >>endl;
	cout <<"\n";
	mt19937 ran((unsigned)system_clock::now().time_since_epoch().count());
	while(true) {
		if(curList.empty()) {
			cout <<"���ʱ�Ϊ��";
			cin >>endl;
			break;
		}
		std::discrete_distribution<> dtbt(vecPrb.begin(), vecPrb.end());
		int index = dtbt(ran);
		cout <<TAB_BLANK <<curList[index].english <<TAB_BLANK;
		cin >>endl;
		cout <<index+1 <<"/" <<curList.size() <<OUT_BLANK <<curList[index].chinese <<"\n"
			<<"ʣ��" <<vecFlag[index] <<"/" <<sumFlag <<"������: ";
		string option;
		CinGetLineFirstString(option);
		bool bDel = option=="d" || option=="d+" || option=="+d";
		bool bSta = option=="s" || option=="s+" || option=="+s";
		bool bTemp = option=="+" || option=="d+" || option=="+d"
			|| option=="s+" || option=="+s";
		if(bTemp) {
			if(curName==TEMP_LIST_NAME)
				cout <<"ɾ���ʿ���tempΪͬһ��������tempû������\n";
			else {
				if(!bHasTemp) {
					wordsTemp.clear();
					bHasTemp = true;
				}
				if(ListAddWord(wordsTemp, curList[index]))
					cout <<"����temp�ɹ�\n";
				else
					cout <<"����temp�ظ�\n";
			}
		}
		if(bDel) {
			-- vecFlag[index];
			vecPrb[index] = FastPower(base, vecFlag[index]);
			-- sumFlag;
			if(vecFlag[index]==0) {
				curList.erase(curList.begin()+index);
				vecFlag.erase(vecFlag.begin()+index);
				vecPrb.erase(vecPrb.begin()+index);
				bChange = true;
				cout <<"������ɾ��\n";
			}
			else
				cout <<"���ٱ��\n";
		}
		else if(bSta) {
			cout <<"���ֱ��\n";
		}
		else {
			//����һ�������ֵ�Ĳ����ȡ��
			int tmp = (maxFlag-vecFlag[index]+1)/2;
			vecFlag[index] += tmp;
			vecPrb[index] = FastPower(base, vecFlag[index]);
			sumFlag += tmp;
		}
		if(!option.empty() && !bDel && !bTemp &&!bSta)
			break;
		cout <<CLEAR_ENTER;
	}
	if(bChange)
		setSave.insert(curName);
	if(bHasTemp)
		setSave.insert(TEMP_LIST_NAME);
}


//��ǰ�ʻ���鵥��
void ChoiceCheck(const string &curName, ListNameSet &setSave)
{
	cout <<"���뵼��ĵ��ʱ�Ϊ����Ĭ��now��";
	string inListName;
	if(!CinGetLineFirstString(inListName))
		inListName = NOW_LIST_NAME;
	if(g_mapWord.find(inListName)==g_mapWord.end()) {
		cout <<inListName <<"������\n";
		return;
	}
	bool bHasTemp = false;//�Ƿ��Ѿ������temp
	auto &wordsTemp = g_mapWord.at(TEMP_LIST_NAME);//temp�ʻ��
	cout <<CLEAR_ENTER;
	cout <<"�س���һ��������l(��ĸ)����" <<inListName <<"������+���뵽temp�����෵��";
	cin >>endl;
	cout <<"\n";
	vector<Word> tmpWords;
	bool bSameList = curName==inListName;
	auto &outWords = *(bSameList? &tmpWords:&g_mapWord.at(curName));//Դ�ʿ�
	auto &inWords = g_mapWord.at(inListName);//����ʿ�
	if(bSameList)
		swap(tmpWords, g_mapWord.at(curName));
	vector<int> vecIndex(outWords.size());//����������
	for(int i=0; i!=vecIndex.size(); ++i)
		vecIndex[i] = i;
	mt19937 ran((unsigned)system_clock::now().time_since_epoch().count());
	std::random_shuffle(vecIndex.begin(), vecIndex.end(),
		[&ran](unsigned n) {return ran()%n; });
	bool bUser = true;//�û�����״̬
	//checkѭ��
	for(int i=0; i!=outWords.size(); ++i) {
		int index = vecIndex[i];
		string option;
		bool bLead = false;
		bool bTemp = false;
		//�û�����ʱ
		if(bUser) {
			cout <<i+1 <<"/" <<outWords.size() <<OUT_BLANK <<outWords[index].english <<TAB_BLANK;
			cin >>endl;
			cout <<index+1 <<OUT_BLANK <<outWords[index].chinese <<"\n"
				<<"����: ";
			CinGetLineFirstString(option);
			bLead = option=="l" || option=="l+" || option=="+l";
			bTemp = option=="+" || option=="l+" || option=="+l";
		}
		//������tempѡ��
		if(bTemp) {
			if(inListName==TEMP_LIST_NAME || curName==TEMP_LIST_NAME)
				cout <<"���뵼���ʿ���tempΪͬһ��������tempû������\n";
			else {
				if(!bHasTemp) {
					wordsTemp.clear();
					bHasTemp = true;
				}
				if(ListAddWord(wordsTemp, outWords[index]))
					cout <<"����temp�ɹ�\n";
				else
					cout <<"����temp�ظ�\n";
			}
		}
		//������ѡ�����û�����
		if(!bUser || bLead) {
			if(bSameList) {
				inWords.push_back(std::move(outWords[index]));
				if(bUser)
					cout <<"����ɹ�\n";
			}
			else {
				if(ListAddWord(inWords, outWords[index]))
					cout <<"����ɹ�\n";
				else
					cout <<"�����ظ�\n";
			}
		}
		//���˳�����ͬlist���������û�����״̬
		if(bUser && !option.empty() && !bLead && !bTemp) {
			if(bSameList) {
				-- i;
				bUser = false;
			}
			else
				break;
		}
	}
	setSave.insert(inListName);
	if(bHasTemp)
		setSave.insert(TEMP_LIST_NAME);
}


//�뵱ǰ�ʻ���Ƶ���
void ChoiceCopy(const string &curName, ListNameSet &setSave)
{
	string newListName;//�����б���
	cout <<"����i���Ƶ���ǰ�ʻ������o�ӵ�ǰ�ʻ���ƣ����򷵻�: ";
	CinGetLineFirstString(newListName);
	if(newListName!="i" && newListName!="o")
		return;
	bool bLeadOut = newListName=="o";//�Ƿ��ǵ���
	vector<ListType::iterator> vecListFrom;//����Դ�ʻ���б�
	vector<Word> *listTo = nullptr;//����Ŀ�Ĵʻ��
	int leadFromSt= 0, leadFromEd= 0;//����Դ�ʻ㷶Χ
	bool bLeadFrom = false;//�Ƿ��շ�Χ����
	if(bLeadOut)
		vecListFrom.push_back(g_mapWord.find(curName));
	else
		listTo = &g_mapWord.at(curName);
	bool bLeadInAndRgx = false;//�Ƿ��ǵ�����ʹ������
	//������ѡ���Ƿ�����
	if(!bLeadOut) {
		cout <<"�Ƿ�ʹ������ѡ������Դ�ʻ���ǿհ�����Ϊ��: ";
		bLeadInAndRgx = CinGetLineJudgeBlank();
		//������
		if(bLeadInAndRgx) {
			regex rgxLeadIn;
			cout <<"������Դ�ʻ������ȫ��ƥ�䣬���򷵻�: \n";
			if(!CinGetRegex(rgxLeadIn))
				return;
			for(auto &itRes=g_mapWord.begin(); itRes!=g_mapWord.end(); ++itRes) {
				if(std::regex_match(itRes->first, rgxLeadIn))
					vecListFrom.push_back(itRes);
			}
			cout <<"�ҵ�ƥ��Ĵʻ��\n";
			for(auto &ref: vecListFrom)
				cout <<ref->first <<"\n";
			cout <<"��" <<vecListFrom.size() <<"��������ǿհ׼���: ";
			if(!CinGetLineJudgeBlank())
				return;
		}
	}
	//�����ǵ���������
	if(!bLeadInAndRgx) {
		//ѡ��ʿ�
		if(bLeadOut)
			cout <<"���뵼�뵽�Ĵʿ⣬�����뷵��: ";
		else
			cout <<"���뵼����Դ�ʿ⣬�����뷵��: ";
		ListType::iterator itRes;
		while(true) {
			if(!CinGetLineFirstString(newListName))
				return;
			itRes = g_mapWord.find(newListName);
			if(itRes==g_mapWord.end())
				cout <<"δ�ҵ��ʿ�\n";
			else
				break;
		}
		if(bLeadOut)
			listTo = &itRes->second;
		else
			vecListFrom.push_back(itRes);
		//ѡ���뷶Χ
		int size = (int)vecListFrom.front()->second.size();
		cout <<"������Դ�ʿ⹲��" <<size <<"��\n"
			<<"���뵼�����ʼ����ֹ��ţ����Ϸ���ѡ��ȫ��: ";
		while(true) {
			if(!(cin >>leadFromSt >>leadFromEd >>endl)) {
				leadFromSt = 1;
				leadFromEd = size;
			}
			-- leadFromSt;
			if(IS_IN_RANGE(leadFromSt, 0, size-1)
				|| IS_IN_RANGE(leadFromEd, leadFromSt+1, size))
			{
				bLeadFrom = true;
				break;
			}
			else
				cout <<"��Ŵ�����������: \n";
		}
	}
	//��ʼѰ��
	int success = 0, fail = 0;
	if(!bLeadFrom)
		leadFromSt = 0;
	for(auto &listFrom: vecListFrom) {
		if(!bLeadFrom) {
			leadFromEd = listFrom->second.size();
		}
		for(int i=leadFromSt; i!=leadFromEd; ++i) {
			if(ListAddWord(*listTo, listFrom->second[i]))
				++ success;
			else
				++ fail;
		}
	}
	setSave.insert(bLeadOut? newListName:curName);
	cout <<"����������" <<success <<"���ظ�" <<fail;
	cin >>endl;
}


//�����ʻ����
void ChoiceSearch(const string &curName, ListNameSet &setSave)
{
	regex rgxList;
	vector<ListType::iterator> vecListSearch;//����ƥ���
	cout <<"����ɸѡ�ʻ������ȫ��ƥ�䣬���򷵻�: \n";
	if(!CinGetRegex(rgxList))
		return;
	for(auto &itRes=g_mapWord.begin(); itRes!=g_mapWord.end(); ++itRes) {
		if(std::regex_match(itRes->first, rgxList))
			vecListSearch.push_back(itRes);
	}
	cout <<"�ҵ�ƥ��Ĵʻ��\n";
	for(auto &ref: vecListSearch)
		cout <<ref->first <<"\n";
	cout <<"��" <<vecListSearch.size() <<"��\n";
	cout <<"����������������ȫ��ƥ�䣬���򷵻�: \n";
	string substr;
	regex rgxWord;
	if(!CinGetRegex(rgxWord))
		return;
	struct Res {
		vector<ItemIndex> index;
		bool bWrong;
	};
	map<Word, Res> tmpList;//bool��ʾ�Ƿ���������
	for(auto &refIt: vecListSearch) {
		for(int i=0; i!=refIt->second.size(); ++i) {
			auto &word = refIt->second[i];
			if(std::regex_match(word.english, rgxWord)) {
				//���ʷ��������ж��Ƿ�����
				auto it = tmpList.find(word);
				if(it==tmpList.end()) {
					auto &refRes = tmpList.emplace(word, Res()).first->second;
					refRes.bWrong = false;
					refRes.index.emplace_back(refIt, i);
				}
				else {
					it->second.index.emplace_back(refIt, i);
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
	setSave.insert(TEMP_LIST_NAME);
	auto &oriTmpList = g_mapWord.at(TEMP_LIST_NAME);
	oriTmpList.clear();
	int wrongCnt = 0;
	cout <<"���Ϊ��\n";
	for(auto &pr : tmpList) {
		cout <<TAB_BLANK <<PrintWord(pr.first) <<"\n"
			<<PrintItemIndexVector(pr.second.index);
		if(pr.second.bWrong) {
			cout <<" ***���Ĳ�һ�£�***";
			++ wrongCnt;
		}
		cout <<"\n";
		oriTmpList.push_back(pr.first);
	}
	cout <<"������" <<tmpList.size() <<"�����Ѵ���\"" TEMP_LIST_NAME "\"�У�����"
		<<wrongCnt <<"�����Ĳ�һ��";
	cin >>endl;
}


//�������һ����
void ChoiceRegularize(const string &curName, ListNameSet &setSave)
{
	cout <<"������ĵ�һ���ԣ�����ǿհ׼���: ";
	if(!CinGetLineJudgeBlank())
		return;
	//������࣬{Ӣ��:[{����:[����]}]}
	using CheckResult = unordered_map<string, map<string, vector<ItemIndex>>>;
	auto lbdCheck = [&]()-> CheckResult {//�������е��ʵı���ؽ��
		CheckResult ret;
		for(auto itList=g_mapWord.begin(); itList!=g_mapWord.end(); ++itList) {
			auto &list = itList->second;
			for(int i=0; i!=list.size(); ++i) {
				auto &word = list[i];
				ret[word.english][word.chinese].emplace_back(itList, i);
			}
		}
		return ret;
	};
	auto lbdDropBlank = [](const string &str)-> string {//ȥ���ַ����Ŀհ׷�����û���򷵻ؿ�
		auto itSt = std::find_if(str.begin(), str.end(), IsNotBlankChar);
		auto ritEd = std::find_if(str.rbegin(), string::const_reverse_iterator(itSt),
			IsNotBlankChar);
		if(itSt==str.begin() && ritEd.base()==str.end())
			return string();
		else
			return string(itSt, ritEd.base());
	};
	//���м��
	auto htbBstVec = lbdCheck();
	cout <<"�ܼ��ҵ�����" <<htbBstVec.size() <<"��";
	cin >>endl;
	//�ȼ��Ӣ�Ŀհ׷�
	vector<pair<CheckResult::iterator, string>> vecEnglishBlank;//��������������͸ı���ַ���
	for(auto itEnglish=htbBstVec.begin(); itEnglish!=htbBstVec.end(); ++itEnglish) {
		string tmp = lbdDropBlank(itEnglish->first);
		if(!tmp.empty())
			vecEnglishBlank.emplace_back(itEnglish, tmp);
	}
	cout <<"��鵽��Ӣ��������пհ׷��������:\n";
	if(vecEnglishBlank.empty()) {
		cout <<"���գ�";
		cin >>endl;
	}
	else {
		for(auto &res: vecEnglishBlank) {
			cout <<TAB_BLANK "\"" <<res.first->first <<"\"\n";
			for(auto &prChinese: res.first->second) {
				cout <<PrintItemIndexVector(prChinese.second) <<",";
			}
			cout <<"\n";
		}
		cout <<"��" <<vecEnglishBlank.size() <<"�����Ƿ��޳��հ׷�������ǿ�����: ";
		if(CinGetLineJudgeBlank()) {
			//���Ŀհ׷�
			for(auto &res: vecEnglishBlank) {
				for(auto &prChinese: res.first->second) {
					for(auto &idx: prChinese.second) {
						idx->english = res.second;
						setSave.insert(idx.itList->first);
					}
				}
			}
			cout <<"�����޳��հ׷�";
			cin >>endl;
			//�����¼����
			htbBstVec = lbdCheck();
		}
	}
	//�ټ�����Ŀհ׷�
	struct EnglishBlankResult {//���������
		CheckResult::iterator itEnglish;
		CheckResult::mapped_type::iterator itChinese;
		string strChange;
	};
	vector<EnglishBlankResult> vecChineseBlank;//������������������͸ı���ַ���
	for(auto itEnglish=htbBstVec.begin(); itEnglish!=htbBstVec.end(); ++itEnglish) {
		for(auto itChinese=itEnglish->second.begin();
			itChinese!=itEnglish->second.end(); ++itChinese)
		{
			string tmp = lbdDropBlank(itChinese->first);
			if(!tmp.empty())
				vecChineseBlank.push_back(EnglishBlankResult{itEnglish, itChinese, tmp});
		}
	}
	cout <<"��鵽������������пհ׷��������:\n";
	if(vecChineseBlank.empty()) {
		cout <<"���գ�";
		cin >>endl;
	}
	else {
		for(auto &res: vecChineseBlank) {
			cout <<TAB_BLANK "\"" <<res.itEnglish->first <<"\""
				<<LONG_TAB_BLANK "\"" <<res.itChinese->first <<"\"\n";
			cout <<PrintItemIndexVector(res.itChinese->second) <<"\n";
		}
		cout <<"��" <<vecChineseBlank.size() <<"�����Ƿ��޳��հ׷�������ǿ�����: ";
		if(CinGetLineJudgeBlank()) {
			//���Ŀհ׷�
			for(auto &res: vecChineseBlank) {
				for(auto &idx: res.itChinese->second) {
					idx->chinese = res.strChange;
					setSave.insert(idx.itList->first);
				}
			}
			cout <<"�����޳��հ׷�";
			cin >>endl;
			//���¼����
			htbBstVec = lbdCheck();
		}
	}
	//�ٽ������Ĳ�һ�¼��
	cout <<"��鵽�����Ĳ�һ�������:\n";
	bool bFind = false;
	auto &listTemp = g_mapWord.at(TEMP_LIST_NAME);
	for(auto &prEnglish: htbBstVec) {
		//���ҵ���һ�µ�
		if(prEnglish.second.size()>1) {
			if(!bFind) {
				bFind = true;
				listTemp.clear();
			}
			cout <<TAB_BLANK <<prEnglish.first <<"\n";
			for(auto &prChinese: prEnglish.second) {
				cout <<prChinese.first <<OUT_BLANK
					<<PrintItemIndexVector(prChinese.second) <<"\n";
			}
			listTemp.emplace_back(prEnglish.first, prEnglish.second.begin()->first);
		}
	}

	if(bFind) {
		setSave.insert(TEMP_LIST_NAME);
		cout <<"��" <<listTemp.size() <<"�����Ѵ���" TEMP_LIST_NAME "��";
	}
	else
		cout <<"���գ�";
	cin >>endl;
}


//���ļ�����ʻ��
void ChoiceInput(const string &curName, ListNameSet &setSave)
{
	cout <<"���ļ�����\"" <<INPUT_DIR_NAME <<"\"�ļ����£���׺��Ϊ" <<INPUT_FILE_EXT <<"\n";
	cout <<"����ĵ��ʱ�����Ϊ�ļ�������������׺�������������»��ߵı���滻Ϊ�»���\n";
	cout <<"���뵼���ļ�ƥ������ȫ��ƥ�䣬��������׺�������򷵻�: \n";
	regex rgxFile;
	if(!CinGetRegex(rgxFile))
		return;
	vector<filesystem::path> vecFilePath;
	//�����ļ���ȡ
	filesystem::path path(INPUT_DIR_PATH);
	for(auto &file : filesystem::directory_iterator(path)) {
		//ƥ���ļ���
		if(filesystem::is_regular_file(file)
			&& file.path().extension().string()==INPUT_FILE_EXT
			&& std::regex_match(file.path().stem().string(), rgxFile)
			)
			vecFilePath.push_back(file.path());
	}
	cout <<"�ҵ����ϵ��ļ���\n";
	for(auto &ref: vecFilePath)
		cout <<ref.filename().string() <<"\n";
	cout <<"��" <<vecFilePath.size() <<"��\n";
	cout <<"����ǿհ׽��е���: ";
	if(!CinGetLineJudgeBlank())
		return;
	int cntFailOpen= 0, cntCreatList= 0, cntWord= 0, cntWrongWord= 0;
	//�����ļ���ȡ
	for(auto &filePath : vecFilePath) {
		ifstream ifs(filePath.string());
		if(ifs.is_open()) {
			//�淶���ļ������������ʱ�
			string name = filePath.stem().string();
			std::replace_if(name.begin(), name.end(), IsNotBroadIdChar, '_');
			auto prRes = g_mapWord.emplace(piecewise_construct,
				std::tie(name), make_tuple());
			if(prRes.second)
				++ cntCreatList;
			auto &newWords = prRes.first->second;
			setSave.insert(name);
			//������ȡ�ؼ��ֶ�
			std::string str;
			while(std::getline(ifs, str)) {
				Word word;
				//����Ч�ַ�ͷ
				auto itSt = std::find_if_not(str.begin(), str.end(), IsBlankChar);//a
				//�ж�ע��
				if(itSt==str.end() || *itSt=='#' || *itSt==';')
					continue;
				//���ֶ�β����Ҫ�����
				auto itGap = std::find_if(itSt+1, str.end(), IsBlankChar);//\t2
				word.english.assign(itSt, itGap);
				//�Һ�����
				auto itWordSt = std::find_if_not(itGap, str.end(), IsBlankChar);//d
				auto ritWordEd = std::find_if_not(str.rbegin(),
					std::string::reverse_iterator(itWordSt), IsBlankChar);//i
				//�洢ֵ������Ϊ��
				if(itWordSt!=str.end() && itWordSt<ritWordEd.base())
					word.chinese.assign(itWordSt, ritWordEd.base());
				if(word.chinese.empty())
					continue;
				//�ж��ظ�
				if(ListAddWord(newWords, word))
					++ cntWord;
				else
					++ cntWrongWord;
			}
		}
		else
			++ cntFailOpen;
	}
	int cntList = vecFilePath.size()-cntFailOpen;
	cout <<"��ɣ��ɹ����ļ�" <<cntList
		<<"�����ļ�ʧ��" <<cntFailOpen
		<<"�������б�" <<cntCreatList <<"���Ѵ����б�" <<cntList-cntCreatList <<"\n"
		 <<"���뵥��" <<cntWord <<"���ظ�����" <<cntWrongWord;
	cin >>endl;
}


//�߼����ļ�����ʻ��
void ChoiceAdInput(const string &curName, ListNameSet &setSave)
{
	cout <<"���ļ�����\"" <<INPUT_DIR_NAME <<"\"�ļ����£�������б�����������׺��\n";
	cout <<"����ĵ��ʱ�����Ϊ�ļ�������������׺�������������»��ߵı���滻Ϊ�»���\n";
	cout <<"���뵼���ļ�ƥ������ȫ��ƥ�䣬������׺�������򷵻�: \n";
	regex rgxFile;
	if(!CinGetRegex(rgxFile))
		return;
	vector<filesystem::path> vecFilePath;
	//�����ļ���ȡ
	filesystem::path path(INPUT_DIR_PATH);
	for(auto &file : filesystem::directory_iterator(path)) {
		//ƥ���ļ���
		if(filesystem::is_regular_file(file)
			&& std::regex_match(file.path().filename().string(), rgxFile)
			)
			vecFilePath.push_back(file.path());
	}
	cout <<"�ҵ����ϵ��ļ���\n";
	for(auto &ref: vecFilePath)
		cout <<ref.filename().string() <<"\n";
	cout <<"��" <<vecFilePath.size() <<"��\n";
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
	cout <<"�Ƿ���е��룿����ǿհ׼���: ";
	if(!CinGetLineJudgeBlank())
		return;
	int cntFailOpen= 0, cntCreatList= 0, cntWord= 0, cntWrongWord= 0;
	//�����ļ���ȡ
	for(auto &filePath : vecFilePath) {
		ifstream ifs(filePath.string());
		if(ifs.is_open()) {
			//�淶���ļ������������ʱ�
			string name = filePath.stem().string();
			std::replace_if(name.begin(), name.end(), IsNotBroadIdChar, '_');
			auto prRes = g_mapWord.emplace(piecewise_construct,
				std::tie(name), make_tuple());
			if(prRes.second)
				++ cntCreatList;
			auto &newWords = prRes.first->second;
			setSave.insert(name);
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
					if(ListAddWord(newWords, word))
						++ cntWord;
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
		else
			++ cntFailOpen;
	}
	int cntList = vecFilePath.size()-cntFailOpen;
	cout <<"��ɣ��ɹ����ļ�" <<cntList
		<<"�����ļ�ʧ��" <<cntFailOpen
		<<"�������б�" <<cntCreatList <<"���Ѵ����б�" <<cntList-cntCreatList <<"\n"
		 <<"���뵥��" <<cntWord <<"���ظ�����" <<cntWrongWord;
	cin >>endl;
}


//�ʻ�������ļ�
void ChoiceOutput(const string &curName, ListNameSet &setSave)
{
	regex rgxOutput;
	vector<ListType::iterator> vecListOutput;
	cout <<"���뵼���ʻ������ȫ��ƥ�䣬���򷵻�: \n";
	if(!CinGetRegex(rgxOutput))
		return;
	for(auto &itRes=g_mapWord.begin(); itRes!=g_mapWord.end(); ++itRes) {
		if(std::regex_match(itRes->first, rgxOutput))
			vecListOutput.push_back(itRes);
	}
	cout <<"�ҵ�ƥ��Ĵʻ��\n";
	for(auto &ref: vecListOutput)
		cout <<ref->first <<"\n";
	cout <<"��" <<vecListOutput.size() <<"��������ǿհ׼���: ";
	if(!CinGetLineJudgeBlank())
		return;
	for(auto &refIt: vecListOutput) {
		ofstream ofs(OUPPUT_DIR_PATH "/"+refIt->first+OUPPUT_FILE_EXT);
		for(auto &word: refIt->second)
			ofs <<word.english <<" " <<word.chinese <<"\n";
	}
	cout <<"��ɣ��ѵ�����\"" <<OUPPUT_DIR_NAME <<"\"�ļ���";
	cin >>endl;
}


//�ʻ�������ļ�
void ChoicePlan(const string &curName, ListNameSet &setSave)
{
	const string c_flagData = "#date";//���ڱ�־
	const string c_flagGroup = "#group";//ÿ���־
	const string c_flagBlank = "#blank";//�հױ�־
	const vector<int> vecPeriod = {0, 1, 3, 7, 14, 29};
	cout <<"���ɱ�����ʱ����ã�ÿ��ƻ����ĵ����ڵ�";
	for(int i: vecPeriod)
		cout <<i <<",";
	cout <<"���ϰ\n"
		<<"�������ļ����� \"" INPUT_DIR_PATH "\" Ŀ¼��\n"
		<<"�ļ�ÿ�д���һ��������Ŀ������Ϊ���¼���:\n"
		<<"1,\"(���ʱ���) [(��ʼ���)] [(�������)]\" ��ʾҪ����һ�����ʱ�ϵ��\n"
		<<"\t��\"lis 1 3\"��ʾlis1��lis2��lis3һ��3����\n"
		<<"\t��ʼ��źͽ�����ſ�ʡ�Ա�ʾֻ��һ����\n"
		<<"2,\"" <<c_flagData <<" (����)\" ��ʾ����ı�Ӹ��������ڿ�ʼ\n"
		<<"\t���ڱ�ʾ���ʽ \"2018-1-23\" ����ʼĬ��ʹ�����е��������\n"
		<<"3,\"" <<c_flagGroup <<" (����)\" ��ʾ����ÿ��ƻ����ж��ٸ����ʱ�\n"
		<<"\t��ʼĬ��Ϊ1������������ζ�ŷָ������ĵ��ʱ�\n"
		<<"\t���������ĵ��ʱ�һ�����µ�һ�쿪ʼ��\n"
		<<"4,\"" <<c_flagBlank <<" [(����)]\" ��ʾ�������ճ�������û�б��мƻ�\n"
		<<"\t������������0��ʾ�����ķָ������ĵ��ʱ�\n";
	cout <<"��������txt�ļ�������������׺�������򷵻�: ";
	string strFileName;
	if(!CinGetLineFirstString(strFileName))
		return;
	ifstream ifs(INPUT_DIR_PREFIX+strFileName+INPUT_FILE_EXT);
	if(!ifs.is_open()) {
		cout <<"δ�ҵ������ļ�";
		cin >>endl;
		return;
	}
	cout <<"�������txt�ļ�������������׺�������򷵻�: ";
	if(!CinGetLineFirstString(strFileName))
		return;
	ofstream ofs(OUPPUT_DIR_PREFIX+strFileName+OUPPUT_FILE_EXT);
	if(!ofs.is_open()) {
		cout <<"����ļ���ʧ��";
		cin >>endl;
		return;
	}
	vector<pair<time_t, string>> vecList;//��ŵ��ʱ�
	int groupSize = 1;//ÿ�챳����
	int nowSize = 0;//��ǰ���б�����
	int listCnt = 0;//���ʱ���
	auto nowTime = std::time(nullptr);//��ǰʱ���
	time_t timeOneDay = 60*60*24;
	bool bInitDate = true;//�Ƿ���Ĭ��ʱ��
	//��ʱ����䵽��
	auto tmStru = TimeNumToTimeStru(nowTime);
	tmStru.tm_sec = tmStru.tm_min = tmStru.tm_hour = 0;
	tmStru.tm_isdst = 0;
	nowTime = TimeStruToTimeNum(tmStru);
	//����ѭ��
	string strLine;
	for(int lineIdx=0; std::getline(ifs, strLine); ++lineIdx) {
		istringstream iss(std::move(strLine));
		string name, strTmp;
		int num1, num2, num3;
		char ch1, ch2;
		if(!(iss >>name))
			continue;
		//���ı�����
		if(name==c_flagData) {
			if(!(iss >>num1 >>ch1 >>num2 >>ch2 >>num3)
				|| ch1!='-' || ch2!='-')
			{
				cout <<"warning in line " <<lineIdx <<" with " <<name <<": "
					<<"δ��ʶ�����ڸ�ʽ�����Դ���\n";
				continue;
			}
			std::tm tmStruTmp {0,//sec
				0,//min
				0,//hour
				num3,//day
				num2-1,//mon
				num1-1900,//year
				0,//wday
				0,//yday
				0,};//isdst
			auto tmNumTmp = TimeStruToTimeNum(tmStruTmp);
			if(tmNumTmp<0) {
				cout <<"warning in line " <<lineIdx <<" with " <<name <<": "
					<<"���ڷ�Χ���󣬺��Դ���\n";
				continue;
			}
			//ʱ�䲻�Ϸ����ڲ�Ϊ��ʼδʹ��ʱ��������С���ϴ�ʱ��
			else if((!(bInitDate && vecList.empty()) && tmNumTmp<=nowTime)) {
				cout <<"warning in line " <<lineIdx <<" with " <<c_flagData <<": "
					<<"���ڳ��ֵ�����������Դ���\n";
				continue;
			}
			nowTime = tmNumTmp;
			nowSize = 0;
			bInitDate = false;
		}
		//���ı�ÿ������
		else if(name==c_flagGroup) {
			if(!(iss >>num1) || num1<=0) {
				cout <<"warning in line " <<lineIdx <<" with " <<name <<": "
					<<"δ�ṩ������������Ϊ�������Դ���\n";
				continue;
			}
			groupSize = num1;
			if(nowSize>0) {
				nowSize = 0;
				nowTime += timeOneDay;
			}
		}
		//����ӿհױ�
		else if(name==c_flagBlank) {
			if(!(iss >>num1))
				num1 = 0;
			if(num1<0) {
				cout <<"warning in line " <<lineIdx <<" with " <<name <<": "
					<<"�ṩ����Ϊ�������Դ���\n";
				continue;
			}
			if(nowSize>0) {
				nowSize = 0;
				nowTime += timeOneDay;
			}
			nowTime += num1*timeOneDay;
		}
		//����ӱ�
		else {
			bool bHaveIdx = true;
			if(!(iss >>num1)) {
				bHaveIdx = false;//��Ǳ�û�����
				num1 = 0, num2 = 0;
			}
			else if(num1<0) {
				cout <<"warning in line " <<lineIdx <<" with " <<name <<": "
					<<"�ṩ��һ������Ϊ�������Դ���\n";
				continue;
			}
			if(!(iss >>num2)) {
				num2 = num1;
			}
			else if(num2<num1) {
				cout <<"warning in line " <<lineIdx <<" with " <<name <<": "
					<<"�ṩ�ڶ�������С�ڵ�һ�����֣����Դ���\n";
				continue;
			}
			++ num2;
			listCnt += num2-num1;
			for(int i=num1; i<num2; ) {
				int lastSize = std::min(groupSize-nowSize, num2-i);
				string newName;
				//�ı�����
				if(!bHaveIdx)
					newName = name;
				else if(lastSize==1)
					newName <<name <<"(" <<std::to_string(i) <<")";
				else {
					newName <<name <<"(" <<std::to_string(i) <<"-"
						<<std::to_string(i+lastSize-1) <<")";
				}
				if(nowSize==0)
					vecList.emplace_back(nowTime, newName);
				else
					vecList.back().second <<", " <<newName;
				//ѭ������
				i += lastSize;
				nowSize += lastSize;
				if(nowSize>=groupSize) {
					nowTime += timeOneDay;
					nowSize = 0;
				}
			}
		} 
		if(iss >>strLine) {
			cout <<"warning in line " <<lineIdx <<" with " <<name <<": "
				<<"��ȡ��ɺ�����ʣ���ַ�\n";
		}
	}
	OstreamSet<> osst({&cout, &ofs});//��ͬ���
	osst <<"���ʱ���: " <<listCnt <<"\n"
		<<"���µ�������: " <<vecList.size() <<"\n";
	vector<string> vecOut;
	//���ɱ�
	if(!vecList.empty()) {
		time_t tmNum = vecList.front().first;//��ǰʱ��
		int idx = 0;//��ǰvecList����
		while(true) {
			string strTmp;
			bool bHaveNew = idx!=vecList.size() && vecList[idx].first==tmNum;//���µĵ��ʱ�
			if(bHaveNew)
				strTmp <<"��: " <<vecList[idx].second <<"\n";
			//�Ҹ�ϰ�ĵ��ʱ�
			int idxSt;//vecList��period��Χ�ڵ���ʼλ��
			for(idxSt = idx-1; idxSt>=0; --idxSt) {
				if(vecList[idxSt].first<tmNum-timeOneDay*vecPeriod.back())
					break;
			}
			int idxPeriod = vecPeriod.size()-1;//vecPeriod����
			for(++idxSt; idxSt<=idx; ++idxSt) {
				if(idxSt==vecList.size())
					break;
				for(; idxPeriod>=0
					&& vecList[idxSt].first>tmNum-timeOneDay*vecPeriod[idxPeriod]; --idxPeriod
					)
					;
				if(idxPeriod>=0
					&& vecList[idxSt].first==tmNum-timeOneDay*vecPeriod[idxPeriod])
				{
					strTmp <<"��ϰ: " <<vecList[idxSt].second <<"\n";
					-- idxPeriod;
				}
			}
			//��Ϊ������
			if(!strTmp.empty()) {
				vecOut.emplace_back();
				vecOut.back() <<std::to_string(vecOut.size()) <<",  "
					<<TimeToStr(tmNum, true, false, "%F") <<":\n"
					<< strTmp;
			}
			//���¼���
			tmNum += timeOneDay;
			if(bHaveNew)
				++ idx;
			else {//idx==0������뷢��
				//��������ϰ�������
				if(vecList[idx-1].first<tmNum-timeOneDay*vecPeriod.back()) {
					if(idx==vecList.size())
						break;
					else
						tmNum = vecList[idx].first;
				}
			}
		}
	}
	osst <<"�ܱ���������: " <<vecOut.size() <<"\n";
	ofs <<"�ƻ�������: " <<"\n\n";
	for(auto &str: vecOut)
		ofs <<str <<"\n";
	ofs.close();
	cout <<"������ɣ��ƻ����ѵ����ļ�";
	cin >>endl;
}


//�����ã��洢�����ļ�
void DebugSaveAll()
{
	auto lbd = [](ListType::iterator it)-> const string & {
		return it->first;
	};
	ListNameSet setSave(TieDerefIterWrapper(g_mapWord.begin(), lbd),
		TieDerefIterWrapper(g_mapWord.end(), lbd));
	WriteDataFile(setSave);
}



int main()
{
	//��ȡ�洢����
	ReadDataFile();

	//ҵ��ѭ��
	string curName = NOW_LIST_NAME;//��ǰ��
	ListNameSet setSave;//�����ļ��б�
	while(true) {
		string choice;
		cout <<"\n"
			<<"��ǰ���дʻ��: " <<g_mapWord.size() <<"\n"
			<<"��ǰ�ʻ��Ϊ: " <<curName <<"�����е���: " <<g_mapWord.at(curName).size() <<"\n"
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
					<<"delete����ǰ�ʻ��ɾ������\n"
					<<"updata�����´ʻ����\n"
					<<"recite����ǰ�ʻ�����ʣ��зŻ������ʾ��ѡ��ɾ��\n"
					<<"check����ǰ�ʻ���鵥�ʣ��޷Ż������ʾ��ѡ����\n"
					<<"copy���뵱ǰ�ʻ���Ƶ���\n"
					<<"search�������ʻ����\n"
					<<"regularize��������дʻ��������Ƿ�һ��\n"
					<<"input�����ļ�����ʻ��\n"
					<<"adinput���߼����ļ�����ʻ��\n"
					<<"plan�����ռ����������ɼƻ�"
					<<"output���ʻ�������ļ�\n";
			}
			else
				break;
		}
		if(choice=="list")
			ChoiceList(curName, setSave);
		else if(choice=="change")
			ChoiceChange(curName, setSave);
		else if(choice=="show")
			ChoiceShow(curName, setSave);
		else if(choice=="add")
			ChoiceAdd(curName, setSave);
		else if(choice=="delete")
			ChoiceDelete(curName, setSave);
		else if(choice=="updata")
			ChoiceUpdata(curName, setSave);
		else if(choice=="recite")
			ChoiceRecite(curName, setSave);
		else if(choice=="check")
			ChoiceCheck(curName, setSave);
		else if(choice=="copy")
			ChoiceCopy(curName, setSave);
		else if(choice=="search")
			ChoiceSearch(curName, setSave);
		else if(choice=="regularize")
			ChoiceRegularize(curName, setSave);
		else if(choice=="input")
			ChoiceInput(curName, setSave);
		else if(choice=="adinput")
			ChoiceAdInput(curName, setSave);
		else if(choice=="output")
			ChoiceOutput(curName, setSave);
		else if(choice=="plan")
			ChoicePlan(curName, setSave);
		else if(choice=="exit")
			break;

		//DebugSaveAll();

		//�����ļ�
		WriteDataFile(setSave);
		setSave.clear();
	}


	return 0;
}

