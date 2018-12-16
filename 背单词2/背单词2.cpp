// 背单词2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Data.h"


//主数据存储
map<string, vector<Word>> g_mapWord;



//读取存储数据
void ReadDataFile()
{
	//创建文件夹
	auto lbdCreatDir = [](const string &name) {
		filesystem::path path("./"+name);
		//若存在非目录则删除
		if(filesystem::exists(path)) {
			if(!filesystem::is_directory(path))
				filesystem::remove(path);
		}
		//若不存在则创建
		if(!filesystem::exists(path)) {
			filesystem::create_directory(path);
		}
	};
	lbdCreatDir(DATA_DIR_NAME);
	lbdCreatDir(INPUT_DIR_NAME);

	//遍历文件读取
	filesystem::path path(DATA_DIR_PATH);
	for(auto &file : filesystem::directory_iterator(path)) {
		if(filesystem::is_regular_file(file)) {
			if(file.path().extension().string()==DATA_FILE_EXT) {
				BinReadFile brf(file.path().string());
				string name = FormToStr(file.path().stem().string());
				if(g_mapWord.find(name)!=g_mapWord.end())
					throw runtime_error("文件与列表重复");
				brf >>g_mapWord[name];
			}
		}
	}
	//创建默认列表
	g_mapWord[NOW_LIST];
	g_mapWord[TEMP_LIST];
}


//保存文件
void WriteDataFile(const vector<string> &vecName)
{
	for(auto &oriName : vecName) {
		string name = StrToForm(oriName);
		filesystem::path path(DATA_DIR_PATH);
		path /= name+DATA_FILE_EXT;
		//不存在则删除文件
		if(g_mapWord.find(oriName)==g_mapWord.end()) {
			if(filesystem::exists(path))
				filesystem::remove(path);
		}
		//存在则写文件
		else {
			BinWriteFile bwf(path.string());
			bwf <<g_mapWord.at(oriName);
		}
	}
}


//显示所有词汇表
void ChoiceList(const string &list, vector<string> &vecSave)
{
	bool bFirst = true;
	//遍历单词列表
	for(auto &list : g_mapWord) {
		if(!bFirst)
			cout <<"\n";
		else
			bFirst = false;
		cout <<list.first <<"      " <<list.second.size();
	}
	cin >>endl;
}


//切换词汇表
void ChoiceChange(string &list, vector<string> &vecSave)
{
	cout <<"输入切换表名称，空则返回: ";
	string name;
	if(!CinGetLineFirstString(name))
		return;
	std::replace_if(name.begin(), name.end(), CharIsNotBroadId, '_');
	//若不存在则判断创建
	if(g_mapWord.find(name)==g_mapWord.end()) {
		cout <<"\"" <<name <<"\"不存在，输入非空白创建: ";
		if(CinGetLineJudgeBlank()) {
			g_mapWord[name];
			vecSave.push_back(name);
			list = name;
		}
	}
	//存在则改变
	else
		list = name;
}


//显示当前词汇表词汇
void ChoiceShow(const string &list, vector<string> &vecSave)
{
	auto &words = g_mapWord.at(list);
	if(words.empty())
		cout <<"（空）";
	else {
		cout <<"输入从哪到哪，不合法则显示全部: ";
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


//当前词汇表添加单词
void ChoiceAdd(const string &list, vector<string> &vecSave)
{
	auto &words = g_mapWord.at(list);
	bool bChange = false;
	while(true)
	{
		Word tmp;
		cout <<"输入英文，空则返回: ";
		if(!CinGetLineJudgeBlank(&tmp.english))
			break;
		if(std::find(words.begin(), words.end(), tmp)!=words.end()) {
			cout <<"单词重复\n";
			continue;
		}
		cout <<"输入中文解释，空则返回: ";
		if(!CinGetLineJudgeBlank(&tmp.chinese))
			break;
		bChange = true;
		words.push_back(tmp);
		cout <<"添加成功\n";
	}
	if(bChange)
		vecSave.push_back(list);
}


//当前词汇表删除单词
string ChoiceDel(const string &list, vector<string> &vecSave)
{
	auto &words = g_mapWord.at(list);
	bool bChange = false;
	string res = list;
	while(true) {
		if(words.empty()) {
			cout <<"单词表为空\n";
			if(list==NOW_LIST || list==TEMP_LIST)
				break;
			cout <<"是否删除单词表？非空白代表删除: ";
			if(CinGetLineJudgeBlank()) {
				g_mapWord.erase(list);
				bChange = true;
				cout <<"删除单词表\n";
				res = NOW_LIST;
			}
			break;
		}
		cout <<"当前有" <<words.size() <<"单词，输入预览起点，不合法则返回: ";
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
		cout <<"输入删除范围编号，不合法则返回: ";
		int st, ed;
		if(!(cin >>st >>ed >>endl))
			break;
		-- st;
		if(st<0 || st>=(int)words.size() || ed<=st || ed>(int)words.size()) {
			cout <<"序号错误\n";
			continue;
		}
		words.erase(words.begin()+st, words.begin()+ed);
		bChange = true;
		cout <<"删除" <<ed-st <<"成功\n";
	}
	if(bChange)
		vecSave.push_back(list);
	return res;
}


//当前词汇表背单词
void ChoiceRecite(const string &list, vector<string> &vecSave)
{
	auto &words = g_mapWord.at(list);
	bool bChange = false;
	cout <<"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	cout <<"回车下一步，输入d删除，其余返回";
	cin >>endl;
	mt19937 ran((unsigned)system_clock::now().time_since_epoch().count());
	while(true) {
		if(list.empty()) {
			cout <<"单词表为空\n";
			cin >>endl;
			break;
		}
		int index= ran()%words.size();
		cout <<"      " <<words[index].english <<"    ";
		cin >>endl;
		cout <<index+1 <<":   " <<words[index].chinese <<"\n"
			<<"输入: ";
		string option;
		CinGetLineFirstString(option);
		if(option=="d") {
			words.erase(words.begin()+index);
			bChange = true;
			cout <<"删除成功\n";
		}
		else if(!option.empty())
			break;
		cout <<"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	}
	if(bChange)
		vecSave.push_back(list);
}


//当前词汇表检查单词
void ChoiceCheck(const string &list, vector<string> &vecSave)
{
	cout <<"输入导入的单词表，为空则默认now：";
	string inList;
	if(!CinGetLineFirstString(inList))
		inList = NOW_LIST;
	if(g_mapWord.find(inList)==g_mapWord.end()) {
		cout <<inList <<"不存在\n";
		return;
	}
	cout <<"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	cout <<"回车下一步，输入l(字母)导入" <<inList <<"，其余返回";
	cin >>endl;
	vector<Word> tmpWords;
	bool bSameList = list==inList;
	auto &words = *(bSameList? &tmpWords:&g_mapWord.at(list));//源词库
	auto &inWords = g_mapWord.at(inList);//导入词库
	if(bSameList)
		swap(tmpWords, g_mapWord.at(list));
	vector<int> vecIndex(words.size());//随机序号数组
	for(int i=0; i!=vecIndex.size(); ++i)
		vecIndex[i] = i;
	mt19937 ran((unsigned)system_clock::now().time_since_epoch().count());
	std::random_shuffle(vecIndex.begin(), vecIndex.end(),
		[&ran](unsigned n) {return ran()%n; });
	bool bUser = true;
	//check循环
	for(int i=0; i!=words.size(); ++i) {
		int index = vecIndex[i];
		string option;
		//用户输入时
		if(bUser) {
			cout <<i+1 <<"/" <<words.size() <<"    " <<words[index].english <<"   ";
			cin >>endl;
			cout <<index+1 <<":   " <<words[index].chinese <<"\n"
				<<"输入: ";
			CinGetLineFirstString(option);
		}
		//若导入选项或非用户输入
		if(!bUser || option=="l") {
			if(bSameList) {
				inWords.push_back(std::move(words[index]));
				if(bUser)
					cout <<"导入成功\n";
			}
			else {
				if(std::find(inWords.begin(), inWords.end(), words[index])
					==inWords.end())
				{
					inWords.push_back(words[index]);
					cout <<"导入成功\n";
				}
				else
					cout <<"单词重复\n";
			}
		}
		//若退出，在now情况进入非用户输入状态
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


//与当前词汇表复制单词
void ChoiceCopy(const string &list, vector<string> &vecSave)
{
	string newList;
	cout <<"输入i复制到当前词汇表，输入o从当前词汇表复制，否则返回: ";
	CinGetLineFirstString(newList);
	if(newList!="i" && newList!="o")
		return;
	bool bFrom = newList=="o";
	if(bFrom)
		cout <<"输入导入到的词库，空输入返回: ";
	else
		cout <<"输入导入来源词库，空输入返回: ";
	while(true) {
		if(!CinGetLineFirstString(newList))
			return;
		else if(g_mapWord.find(newList)==g_mapWord.end())
			cout <<"未找到词库\n";
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
	cout <<"结束，导入" <<success <<"，重复" <<fail;
	cin >>endl;
}


//与当前词汇表选择复制单词
void ChoiceSelCopy(const string &list, vector<string> &vecSave)
{
}


//更新词汇表单词
void ChoiceUpdata(const string &list, vector<string> &vecSave)
{
	cout <<"输入搜索单词，空则返回: ";
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
		cout <<"未找到\n";
		cin >>endl;
		return;
	}
	cout <<"结果为：\n";
	for(int i=0; i!=vecSave.size(); ++i) {
		if(i!=0) {
			cout <<"\n";
		}
		auto &word = g_mapWord.at(vecSave[i])[vecIndex[i]];
		cout <<vecSave[i] <<":" <<vecIndex[i]+1
			<<"      " <<word.english <<"      " <<word.chinese;
	}
	cin >>endl;
	cout <<"输入英文，为空则不变: ";
	string tmp;
	if(CinGetLineJudgeBlank(&tmp))
		target.english = tmp;
	cout <<"输入中文，为空则返回: ";
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
			cout <<vecSave[i] <<"单词重复\n";
			continue;
		}
		words[vecIndex[i]] = target;
		++ num;
	}
	cout <<"修改" <<num <<"完成";
	cin >>endl;
}


//搜索词汇表单词
void ChoiceSearch(const string &list, vector<string> &vecSave)
{
	cout <<"输入搜索正则，全词匹配，空则返回: \n";
	string substr;
	regex rgx;
	if(!CinGetRegex(rgx))
		return;
	struct Res {
		vector<pair<string, int>> index;
		bool bWrong;
	};
	map<Word, Res> tmpList;//bool表示是否中文歧义
	for(auto &list : g_mapWord) {
		for(int i=0; i!=list.second.size(); ++i) {
			auto &word = list.second[i];
			if(std::regex_match(word.english, rgx)) {
				//单词符合正则，判断是否已有
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
		cout <<"未找到";
		cin >>endl;
		return;
	}
	vecSave.push_back(TEMP_LIST);
	auto &oriTmpList = g_mapWord.at(TEMP_LIST);
	oriTmpList.clear();
	int cnt = 0;
	cout <<"结果为：\n";
	for(auto &pr : tmpList) {
		cout <<"    " <<pr.first.english <<"     "
			<<pr.first.chinese
			<<"\n";
		for(auto &res : pr.second.index) {
			cout <<"(" <<res.first <<"," <<res.second <<"), ";
			++ cnt;
		}
		if(pr.second.bWrong)
			cout <<"中文不一致！";
		cout <<"\n";
		oriTmpList.push_back(std::move(pr.first));
	}
	cout <<"共搜索" <<cnt <<"个，" <<tmpList.size()
		<<"个已存入\"" TEMP_LIST "\"中";
	cin >>endl;
}


//从文件导入词汇表
void ChoiceInput(const string &list, vector<string> &vecSave)
{
	cout <<"将文件放在\"" <<INPUT_DIR_NAME <<"\"文件夹下，后缀名为" <<INPUT_FILE_EXT <<"\n";
	cout <<"文件名不含后缀将作为列表名，输入非空白进行导入: ";
	if(!CinGetLineJudgeBlank())
		return;
	//遍历文件读取
	int cntList= 0, cntWrongList= 0, cntWord= 0, cntWrongWord= 0;
	filesystem::path path(INPUT_DIR_PATH);
	for(auto &file : filesystem::directory_iterator(path)) {
		if(filesystem::is_regular_file(file)) {
			if(file.path().extension().string()==INPUT_FILE_EXT) {
				//打开文件并更新
				ifstream ifs(file.path().string());
				string name = file.path().stem().string();
				std::replace_if(name.begin(), name.end(), CharIsNotBroadId, '_');
				++ cntList;
				if(g_mapWord.find(name)!=g_mapWord.end())
					++ cntWrongList;
				auto &newWords = g_mapWord[name];
				vecSave.push_back(name);
				//从流读取关键字段
				std::string str;
				while(std::getline(ifs, str)) {
					Word word;
					//找有效字符头
					auto itSt = std::find_if_not(str.begin(), str.end(), CharIsBlank);//a
					//判断注释
					if(itSt==str.end() || *itSt=='#' || *itSt==';')
						continue;
					//找字段尾，不要求后续
					auto itGap = std::find_if(itSt+1, str.end(), CharIsBlank);//\t2
					word.english.assign(itSt, itGap);
					//找后续词
					auto itWordSt = std::find_if_not(itGap, str.end(), CharIsBlank);//d
					auto ritWordEd = std::find_if_not(str.rbegin(),
						std::string::reverse_iterator(itWordSt), CharIsBlank);//i
					//存储值，可能为空
					if(itWordSt!=str.end() && itWordSt<ritWordEd.base())
						word.chinese.assign(itWordSt, ritWordEd.base());
					if(word.chinese.empty())
						continue;
					//判断重复
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
	cout <<"结束，找到列表" <<cntList <<"，重复列表" <<cntWrongList
		 <<"，导入单词" <<cntWord <<"，重复单词" <<cntWrongWord;
	cin >>endl;
}


////高级从文件导入词汇表
//void ChoiceAdInput(const string &list, vector<string> &vecSave)
//{
//	cout <<"将文件放在\"" <<INPUT_DIR_NAME <<"\"文件夹下，导入后列表名不包含后缀名\n";
//	cout <<"输入导入文件匹配正则，全词匹配，包括后缀名，空则返回: \n";
//	regex rgxFile;
//	if(!CinGetRegex(rgxFile))
//		return;
//	cout <<"输入导入单词英语部分，部分迭代匹配，按行匹配，注意括起选择子串，空则返回: \n";
//	regex rgxEng;
//	if(!CinGetRegex(rgxEng))
//		return;
//	cout <<"输入英语部分选择子串序号，0为全选，不合法返回：";
//	int choiceEng;
//	if(!CinGetRegexIndex(rgxEng, choiceEng))
//		return;
//	cout <<"输入导入单词中文部分，部分迭代匹配，按行匹配，注意括起选择子串，空则返回: \n";
//	regex rgxChin;
//	if(!CinGetRegex(rgxChin))
//		return;
//	cout <<"输入中文部分选择子串序号，0为全选，不合法返回：";
//	int choiceChin;
//	if(!CinGetRegexIndex(rgxChin, choiceChin))
//		return;
//	cout <<"是否进行导入？输入空白返回: ";
//	if(!CinGetLineJudgeBlank())
//		return;
//	//遍历文件读取
//	int cntList= 0, cntWrongList= 0, cntWord= 0, cntWrongWord= 0;
//	filesystem::path path(INPUT_DIR_PATH);
//	for(auto &file : filesystem::directory_iterator(path)) {
//		if(filesystem::is_regular_file(file)) {
//			if(std::regex_match(file.path().filename().string(), rgxFile)) {
//				//打开文件并更新
//				ifstream ifs(file.path().string());
//				string name = file.path().stem().string();
//				std::replace_if(name.begin(), name.end(), CharIsNotBroadId, '_');
//				++ cntList;
//				if(g_mapWord.find(name)!=g_mapWord.end())
//					++ cntWrongList;
//				auto &newWords = g_mapWord[name];
//				vecSave.push_back(name);
//				//从流读取关键字段
//				std::string str;
//				Word word;
//				bool bEng = true;
//				while(std::getline(ifs, str)) {
//					//匹配迭代
//					while(!str.empty()) {
//						smatch matchRes;
//						if(std::regex_search(str, matchRes, *(bEng? &rgxEng:&rgxChin))) {
//							//英语处理
//							if(bEng) {
//								word.english = matchRes[choiceEng].str();
//							}
//							//中文处理
//							else {
//								word.chinese = matchRes[choiceEng].str();
//								//加入词库
//								//判断重复
//								if(std::find(newWords.begin(), newWords.end(), word)
//									==newWords.end())
//								{
//									newWords.push_back(word);
//									++ cntWord;
//								}
//								else
//									++ cntWrongWord;
//							}
//							//翻转状态
//							bEng = !bEng;
//						}
//						str = matchRes.suffix().str();
//					}
//				}
//			}
//		}
//	}
//	cout <<"结束，找到列表" <<cntList <<"，重复列表" <<cntWrongList
//		 <<"，导入单词" <<cntWord <<"，重复单词" <<cntWrongWord;
//	cin >>endl;
//}


//高级从文件导入词汇表
void ChoiceAdInput(const string &list, vector<string> &vecSave)
{
	cout <<"将文件放在\"" <<INPUT_DIR_NAME <<"\"文件夹下，导入后列表名不包含后缀名\n";
	cout <<"输入导入文件匹配正则，全词匹配，包括后缀名，空则返回: \n";
	regex rgxFile;
	if(!CinGetRegex(rgxFile))
		return;
	cout <<"输入匹配单词正则，部分迭代匹配，注意括起中英子串，空则返回: \n";
	regex rgxWord;
	if(!CinGetRegex(rgxWord))
		return;
	cout <<"输入英语部分选择子串序号，0为全选，不合法返回：";
	int choiceEng;
	if(!CinGetRegexIndex(rgxWord, choiceEng))
		return;
	cout <<"输入中文部分选择子串序号，0为全选，不合法返回：";
	int choiceChin;
	if(!CinGetRegexIndex(rgxWord, choiceChin))
		return;
	cout <<"是否进行导入？输入空白返回: ";
	if(!CinGetLineJudgeBlank())
		return;
	//遍历文件读取
	int cntList= 0, cntWrongList= 0, cntWord= 0, cntWrongWord= 0;
	filesystem::path path(INPUT_DIR_PATH);
	for(auto &file : filesystem::directory_iterator(path)) {
		if(filesystem::is_regular_file(file)
			&& std::regex_match(file.path().filename().string(), rgxFile))
		{
			//打开文件并更新
			ifstream ifs(file.path().string());
			string name = file.path().stem().string();
			std::replace_if(name.begin(), name.end(), CharIsNotBroadId, '_');
			++ cntList;
			if(g_mapWord.find(name)!=g_mapWord.end())
				++ cntWrongList;
			auto &newWords = g_mapWord[name];
			vecSave.push_back(name);
			//将文件读入字符串，按三倍最大长度
			std::string strInput(3*INPUT_SEGMENT_SIZE, 0);
			int size = 0;
			while(true) {
				ifs.read(&strInput[size], INPUT_SEGMENT_SIZE*3-size);
				size += (int)ifs.gcount();
				strInput.resize(size);
				auto citSt= strInput.cbegin(), citEd= strInput.cend();
				//字符串匹配循环，直到剩余长度不到最大长度
				while(true) {
					smatch mth;
					std::regex_search(citSt, citEd, mth, rgxWord);
					//如果没有匹配
					if(mth.empty()) {
						citSt += std::max<int>(0, citEd-citSt-INPUT_SEGMENT_SIZE);
						break;
					}
					//若匹配位置加后缀不到最大长度且文件未结束
					else if(mth.length()+mth.suffix().length()<INPUT_SEGMENT_SIZE && !ifs.eof()) {
						citSt += std::max<int>(0, mth[0].second-citSt-INPUT_SEGMENT_SIZE);
						break;
					}
					Word word;
					word.english = mth[choiceEng].str();
					word.chinese = mth[choiceChin].str();
					//判断重复，加入词库
					if(std::find(newWords.begin(), newWords.end(), word)
						==newWords.end())
					{
						newWords.push_back(word);
						++ cntWord;
					}
					else
						++ cntWrongWord;
					//循环
					if(mth[0].first==strInput.end())
						break;
					citSt = std::max(mth.suffix().first, mth[0].first+1);
					//剩余长度不到最大长度且文件未结束
					if(citEd-citSt<INPUT_SEGMENT_SIZE && !ifs.eof())
						break;
				}
				if(ifs.eof())
					break;
				//拷贝
				std::copy(citSt, citEd, strInput.begin());
				size = citEd-citSt;
			}
		}
	}
	cout <<"结束，找到列表" <<cntList <<"，重复列表" <<cntWrongList
		 <<"，导入单词" <<cntWord <<"，重复单词" <<cntWrongWord;
	cin >>endl;
}


//词汇表导出到文件
void ChoiceOutput(const string &list, vector<string> &vecSave)
{

}



int main()
{
	//读取存储数据
	ReadDataFile();

	//业务循环
	string list = NOW_LIST;
	while(true) {
		//保存文件列表
		vector<string> vecSave;
		
		string choice;
		cout <<"\n"
			<<"当前共有词汇表: " <<g_mapWord.size() <<"\n"
			<<"当前词汇表为: " <<list <<"，共有单词: " <<g_mapWord.at(list).size() <<"\n"
			<<"输入help查看操作选项\n"
			<<"输入exit，退出\n";

		while(true) {
			cout <<"输入：" <<flush;
			CinGetLineFirstString(choice);
			if(choice=="help") {
				cout <<"操作选项如下\n"
					<<"list，显示所有词汇表\n"
					<<"change，切换词汇表，或创建\n"
					<<"show，显示当前词汇表词汇\n"
					<<"add，当前词汇表添加单词\n"
					<<"del，当前词汇表删除单词\n"
					<<"recite，当前词汇表背单词，有放回随机显示，选择删除\n"
					<<"check，当前词汇表检查单词，无放回随机显示，选择导入\n"
					<<"copy，与当前词汇表复制单词\n"
					<<"selCopy，与当前词汇表选择并复制单词（未完成）\n"
					<<"updata，更新词汇表单词\n"
					<<"search，搜索词汇表单词\n"
					<<"input，从文件导入词汇表\n"
					<<"adInput，高级从文件导入词汇表\n"
					<<"output，词汇表导出到文件（未完成）\n";
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


		//保存文件
		WriteDataFile(vecSave);
	}


	return 0;
}

