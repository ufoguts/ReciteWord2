// 背单词2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Data.h"


//主数据存储
ListType g_mapWord;



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
	lbdCreatDir(OUPPUT_DIR_PATH);

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
	g_mapWord[NOW_LIST_NAME];
	g_mapWord[TEMP_LIST_NAME];
}


//保存文件
void WriteDataFile(const ListNameSet &setSave)
{
	for(auto &oriName : setSave) {
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
void ChoiceList(const string &curName, ListNameSet &setSave)
{
	bool bFirst = true;
	//遍历单词列表
	for(auto &list : g_mapWord) {
		if(!bFirst)
			cout <<"\n";
		else
			bFirst = false;
		cout <<list.first <<LONG_TAB_BLANK <<list.second.size();
	}
	cin >>endl;
}


//切换词汇表
void ChoiceChange(string &curName, ListNameSet &setSave)
{
	cout <<"输入切换表名称，空则返回: ";
	string name;
	if(!CinGetLineFirstString(name))
		return;
	std::replace_if(name.begin(), name.end(), IsNotBroadIdChar, '_');
	//若不存在则判断创建
	if(g_mapWord.find(name)==g_mapWord.end()) {
		cout <<"\"" <<name <<"\"不存在，输入非空白创建: ";
		if(CinGetLineJudgeBlank()) {
			g_mapWord[name];
			setSave.insert(name);
			curName = name;
		}
	}
	//存在则改变
	else
		curName = name;
}


//显示当前词汇表词汇
void ChoiceShow(const string &curName, ListNameSet &setSave)
{
	auto &curList = g_mapWord.at(curName);
	if(curList.empty())
		cout <<"（空）";
	else {
		cout <<"输入从哪到哪，不合法则显示全部: ";
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


//当前词汇表添加单词
void ChoiceAdd(const string &curName, ListNameSet &setSave)
{
	auto &curList = g_mapWord.at(curName);
	bool bChange = false;
	while(true)
	{
		Word tmp;
		cout <<"输入英文，空则返回: ";
		if(!CinGetLineJudgeBlank(&tmp.english))
			break;
		if(!CanListAddWord(curList, tmp)) {
			cout <<"单词重复\n";
			continue;
		}
		cout <<"输入中文解释，空则返回: ";
		if(!CinGetLineJudgeBlank(&tmp.chinese))
			break;
		bChange = true;
		curList.push_back(tmp);
		cout <<"添加成功\n";
	}
	if(bChange)
		setSave.insert(curName);
}


//当前词汇表删除单词
void ChoiceDelete(string &curName, ListNameSet &setSave)
{
	auto &curList = g_mapWord.at(curName);
	bool bChange = false;
	string retName = curName;
	while(true) {
		if(curList.empty()) {
			cout <<"单词表为空";
			if(curName==NOW_LIST_NAME || curName==TEMP_LIST_NAME) {
				cin >>endl;
				break;
			}
			cout <<"\n是否删除单词表？非空白代表删除: ";
			if(CinGetLineJudgeBlank()) {
				g_mapWord.erase(curName);
				bChange = true;
				cout <<"删除单词表";
				retName = NOW_LIST_NAME;
				cin >>endl;
			}
			break;
		}
		cout <<"当前有" <<curList.size() <<"单词，输入预览起点，不合法则返回: ";
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
		cout <<"输入删除范围编号，不合法则返回: ";
		int st, ed;
		if(!(cin >>st >>ed >>endl))
			break;
		-- st;
		if(!IS_IN_RANGE(st, 0, (int)curList.size()-1)
			|| !IS_IN_RANGE(ed, st+1, (int)curList.size()))
		{
			cout <<"序号错误\n";
			continue;
		}
		curList.erase(curList.begin()+st, curList.begin()+ed);
		bChange = true;
		cout <<"删除" <<ed-st <<"成功\n";
	}
	if(bChange)
		setSave.insert(curName);
	curName = retName;
}


//更新词汇表单词
void ChoiceUpdata(const string &curName, ListNameSet &setSave)
{
	while(true) {
		cout <<"输入搜索单词，空则返回: ";
		Word target;
		if(!CinGetLineJudgeBlank(&target.english))
			return;
		vector<ItemIndex> vecIndex;//查找结果类
		//查找单词
		for(auto itList=g_mapWord.begin(); itList!=g_mapWord.end(); ++itList) {
			auto it = std::find(itList->second.begin(), itList->second.end(), target);
			if(it!=itList->second.end()) {
				vecIndex.emplace_back(itList, it-itList->second.begin());
			}
		}
		if(vecIndex.size()==0) {
			cout <<"未找到";
			cin >>endl;
			continue;
		}
		cout <<"结果为：\n";
		for(auto &idx: vecIndex) {
			cout <<PrintListIndexWord(idx) <<"\n";
		}
		//请求输入
		cout <<"输入英文，为空则不变: ";
		string tmp;
		if(CinGetLineJudgeBlank(&tmp))
			target.english = tmp;
		cout <<"输入中文，为空则取消: ";
		if(!CinGetLineJudgeBlank(&target.chinese)) {
			continue;
		}
		//改变单词
		int num = 0;
		for(auto &idx: vecIndex) {
			auto itSt = idx.itList->second.begin(),
				itMd = idx.itList->second.begin()+idx.idx,
				itEd = idx.itList->second.end();
			if(std::find(itSt, itMd, target)!=itMd || std::find(itMd+1, itEd, target)!=itEd)
			{
				cout <<idx.itList->first <<"表单词重复\n";
				continue;
			}
			*idx = target;
			++ num;
			setSave.insert(idx.itList->first);
		}
		cout <<"修改" <<num <<"完成";
		cin >>endl;
	}
}


//当前词汇表背单词
void ChoiceRecite(const string &curName, ListNameSet &setSave)
{
	auto &curList = g_mapWord.at(curName);
	auto &wordsTemp = g_mapWord.at(TEMP_LIST_NAME);
	bool bChange = false;//当前词表是否改变
	bool bHasTemp = false;//temp词表是否改变
	cout <<"每个单词绑定正的标记数，单词出现概率为标记数的softmax函数\n"
		<<"输入正整数表示初始标记数和标记数上限，不合法则返回: ";
	int maxFlag, initFlag;//最大标记次数
	if(!(cin >>initFlag >>maxFlag >>endl))
		return;
	if(!(initFlag>0 && maxFlag>=initFlag))
		return;
	double base = 1.5;//指数概率底
	vector<int> vecFlag(curList.size(), initFlag);//单词标记数组
	vector<double> vecPrb(curList.size());//概率数组
	for(int i=0; i!=curList.size(); ++i)
		vecPrb[i] = FastPower(base, vecFlag[i]);
	int sumFlag = curList.size()*initFlag;
	cout <<CLEAR_ENTER;
	cout <<"输入d减少标记，标记为0则删除单词，输入s保持标记，输入+导入到temp\n"
		<<"s和d不能同时出现，不输入则恢复一半标记数，其余返回";
	cin >>endl;
	cout <<"\n";
	mt19937 ran((unsigned)system_clock::now().time_since_epoch().count());
	while(true) {
		if(curList.empty()) {
			cout <<"单词表为空";
			cin >>endl;
			break;
		}
		std::discrete_distribution<> dtbt(vecPrb.begin(), vecPrb.end());
		int index = dtbt(ran);
		cout <<TAB_BLANK <<curList[index].english <<TAB_BLANK;
		cin >>endl;
		cout <<index+1 <<"/" <<curList.size() <<OUT_BLANK <<curList[index].chinese <<"\n"
			<<"剩余" <<vecFlag[index] <<"/" <<sumFlag <<"，输入: ";
		string option;
		CinGetLineFirstString(option);
		bool bDel = option=="d" || option=="d+" || option=="+d";
		bool bSta = option=="s" || option=="s+" || option=="+s";
		bool bTemp = option=="+" || option=="d+" || option=="+d"
			|| option=="s+" || option=="+s";
		if(bTemp) {
			if(curName==TEMP_LIST_NAME)
				cout <<"删除词库与temp为同一个，导入temp没有意义\n";
			else {
				if(!bHasTemp) {
					wordsTemp.clear();
					bHasTemp = true;
				}
				if(ListAddWord(wordsTemp, curList[index]))
					cout <<"导入temp成功\n";
				else
					cout <<"导入temp重复\n";
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
				cout <<"单词已删除\n";
			}
			else
				cout <<"减少标记\n";
		}
		else if(bSta) {
			cout <<"保持标记\n";
		}
		else {
			//增加一半与最大值的差，向上取整
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


//当前词汇表检查单词
void ChoiceCheck(const string &curName, ListNameSet &setSave)
{
	cout <<"输入导入的单词表，为空则默认now：";
	string inListName;
	if(!CinGetLineFirstString(inListName))
		inListName = NOW_LIST_NAME;
	if(g_mapWord.find(inListName)==g_mapWord.end()) {
		cout <<inListName <<"不存在\n";
		return;
	}
	bool bHasTemp = false;//是否已经导入过temp
	auto &wordsTemp = g_mapWord.at(TEMP_LIST_NAME);//temp词汇表
	cout <<CLEAR_ENTER;
	cout <<"回车下一步，输入l(字母)导入" <<inListName <<"，输入+导入到temp，其余返回";
	cin >>endl;
	cout <<"\n";
	vector<Word> tmpWords;
	bool bSameList = curName==inListName;
	auto &outWords = *(bSameList? &tmpWords:&g_mapWord.at(curName));//源词库
	auto &inWords = g_mapWord.at(inListName);//导入词库
	if(bSameList)
		swap(tmpWords, g_mapWord.at(curName));
	vector<int> vecIndex(outWords.size());//随机序号数组
	for(int i=0; i!=vecIndex.size(); ++i)
		vecIndex[i] = i;
	mt19937 ran((unsigned)system_clock::now().time_since_epoch().count());
	std::random_shuffle(vecIndex.begin(), vecIndex.end(),
		[&ran](unsigned n) {return ran()%n; });
	bool bUser = true;//用户输入状态
	//check循环
	for(int i=0; i!=outWords.size(); ++i) {
		int index = vecIndex[i];
		string option;
		bool bLead = false;
		bool bTemp = false;
		//用户输入时
		if(bUser) {
			cout <<i+1 <<"/" <<outWords.size() <<OUT_BLANK <<outWords[index].english <<TAB_BLANK;
			cin >>endl;
			cout <<index+1 <<OUT_BLANK <<outWords[index].chinese <<"\n"
				<<"输入: ";
			CinGetLineFirstString(option);
			bLead = option=="l" || option=="l+" || option=="+l";
			bTemp = option=="+" || option=="l+" || option=="+l";
		}
		//若导入temp选项
		if(bTemp) {
			if(inListName==TEMP_LIST_NAME || curName==TEMP_LIST_NAME)
				cout <<"导入导出词库与temp为同一个，导入temp没有意义\n";
			else {
				if(!bHasTemp) {
					wordsTemp.clear();
					bHasTemp = true;
				}
				if(ListAddWord(wordsTemp, outWords[index]))
					cout <<"导入temp成功\n";
				else
					cout <<"导入temp重复\n";
			}
		}
		//若导入选项或非用户输入
		if(!bUser || bLead) {
			if(bSameList) {
				inWords.push_back(std::move(outWords[index]));
				if(bUser)
					cout <<"导入成功\n";
			}
			else {
				if(ListAddWord(inWords, outWords[index]))
					cout <<"导入成功\n";
				else
					cout <<"单词重复\n";
			}
		}
		//若退出，在同list情况进入非用户输入状态
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


//与当前词汇表复制单词
void ChoiceCopy(const string &curName, ListNameSet &setSave)
{
	string newListName;//存新列表名
	cout <<"输入i复制到当前词汇表，输入o从当前词汇表复制，否则返回: ";
	CinGetLineFirstString(newListName);
	if(newListName!="i" && newListName!="o")
		return;
	bool bLeadOut = newListName=="o";//是否是导出
	vector<ListType::iterator> vecListFrom;//导入源词汇表列表
	vector<Word> *listTo = nullptr;//导入目的词汇表
	int leadFromSt= 0, leadFromEd= 0;//导入源词汇范围
	bool bLeadFrom = false;//是否按照范围导入
	if(bLeadOut)
		vecListFrom.push_back(g_mapWord.find(curName));
	else
		listTo = &g_mapWord.at(curName);
	bool bLeadInAndRgx = false;//是否是导入且使用正则
	//导出先选择是否正则
	if(!bLeadOut) {
		cout <<"是否使用正则选择导入来源词汇表，非空白输入为是: ";
		bLeadInAndRgx = CinGetLineJudgeBlank();
		//正则处理
		if(bLeadInAndRgx) {
			regex rgxLeadIn;
			cout <<"输入来源词汇表正则，全词匹配，空则返回: \n";
			if(!CinGetRegex(rgxLeadIn))
				return;
			for(auto &itRes=g_mapWord.begin(); itRes!=g_mapWord.end(); ++itRes) {
				if(std::regex_match(itRes->first, rgxLeadIn))
					vecListFrom.push_back(itRes);
			}
			cout <<"找到匹配的词汇表：\n";
			for(auto &ref: vecListFrom)
				cout <<ref->first <<"\n";
			cout <<"共" <<vecListFrom.size() <<"个，输入非空白继续: ";
			if(!CinGetLineJudgeBlank())
				return;
		}
	}
	//导入或非导出正则处理
	if(!bLeadInAndRgx) {
		//选择词库
		if(bLeadOut)
			cout <<"输入导入到的词库，空输入返回: ";
		else
			cout <<"输入导入来源词库，空输入返回: ";
		ListType::iterator itRes;
		while(true) {
			if(!CinGetLineFirstString(newListName))
				return;
			itRes = g_mapWord.find(newListName);
			if(itRes==g_mapWord.end())
				cout <<"未找到词库\n";
			else
				break;
		}
		if(bLeadOut)
			listTo = &itRes->second;
		else
			vecListFrom.push_back(itRes);
		//选择导入范围
		int size = (int)vecListFrom.front()->second.size();
		cout <<"导入来源词库共有" <<size <<"词\n"
			<<"输入导入的起始和终止序号，不合法则选择全部: ";
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
				cout <<"序号错误，重新输入: \n";
		}
	}
	//开始寻找
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
	cout <<"结束，导入" <<success <<"，重复" <<fail;
	cin >>endl;
}


//搜索词汇表单词
void ChoiceSearch(const string &curName, ListNameSet &setSave)
{
	regex rgxList;
	vector<ListType::iterator> vecListSearch;//正则匹配表
	cout <<"输入筛选词汇表正则，全词匹配，空则返回: \n";
	if(!CinGetRegex(rgxList))
		return;
	for(auto &itRes=g_mapWord.begin(); itRes!=g_mapWord.end(); ++itRes) {
		if(std::regex_match(itRes->first, rgxList))
			vecListSearch.push_back(itRes);
	}
	cout <<"找到匹配的词汇表：\n";
	for(auto &ref: vecListSearch)
		cout <<ref->first <<"\n";
	cout <<"共" <<vecListSearch.size() <<"个\n";
	cout <<"输入搜索单词正则，全词匹配，空则返回: \n";
	string substr;
	regex rgxWord;
	if(!CinGetRegex(rgxWord))
		return;
	struct Res {
		vector<ItemIndex> index;
		bool bWrong;
	};
	map<Word, Res> tmpList;//bool表示是否中文歧义
	for(auto &refIt: vecListSearch) {
		for(int i=0; i!=refIt->second.size(); ++i) {
			auto &word = refIt->second[i];
			if(std::regex_match(word.english, rgxWord)) {
				//单词符合正则，判断是否已有
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
		cout <<"未找到";
		cin >>endl;
		return;
	}
	setSave.insert(TEMP_LIST_NAME);
	auto &oriTmpList = g_mapWord.at(TEMP_LIST_NAME);
	oriTmpList.clear();
	int wrongCnt = 0;
	cout <<"结果为：\n";
	for(auto &pr : tmpList) {
		cout <<TAB_BLANK <<PrintWord(pr.first) <<"\n"
			<<PrintItemIndexVector(pr.second.index);
		if(pr.second.bWrong) {
			cout <<" ***中文不一致！***";
			++ wrongCnt;
		}
		cout <<"\n";
		oriTmpList.push_back(pr.first);
	}
	cout <<"共搜索" <<tmpList.size() <<"个，已存入\"" TEMP_LIST_NAME "\"中，其中"
		<<wrongCnt <<"个中文不一致";
	cin >>endl;
}


//检查中文一致性
void ChoiceRegularize(const string &curName, ListNameSet &setSave)
{
	cout <<"检查中文的一致性，输入非空白继续: ";
	if(!CinGetLineJudgeBlank())
		return;
	//检查结果类，{英文:[{中文:[索引]}]}
	using CheckResult = unordered_map<string, map<string, vector<ItemIndex>>>;
	auto lbdCheck = [&]()-> CheckResult {//生成所有单词的表查重结果
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
	auto lbdDropBlank = [](const string &str)-> string {//去除字符串的空白符，若没有则返回空
		auto itSt = std::find_if(str.begin(), str.end(), IsNotBlankChar);
		auto ritEd = std::find_if(str.rbegin(), string::const_reverse_iterator(itSt),
			IsNotBlankChar);
		if(itSt==str.begin() && ritEd.base()==str.end())
			return string();
		else
			return string(itSt, ritEd.base());
	};
	//进行检查
	auto htbBstVec = lbdCheck();
	//先检查英文空白符
	vector<pair<CheckResult::iterator, string>> vecEnglishBlank;//检查结果，迭代器和改变后字符串
	for(auto itEnglish=htbBstVec.begin(); itEnglish!=htbBstVec.end(); ++itEnglish) {
		string tmp = lbdDropBlank(itEnglish->first);
		if(!tmp.empty())
			vecEnglishBlank.emplace_back(itEnglish, tmp);
	}
	cout <<"检查到的英文两侧带有空白符的情况有:\n";
	if(vecEnglishBlank.empty()) {
		cout <<"（空）";
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
		cout <<"共" <<vecEnglishBlank.size() <<"个，是否剔除空白符，输入非空则是: ";
		if(CinGetLineJudgeBlank()) {
			//更改空白符
			for(auto &res: vecEnglishBlank) {
				for(auto &prChinese: res.first->second) {
					for(auto &idx: prChinese.second) {
						idx->english = res.second;
						setSave.insert(idx.itList->first);
					}
				}
			}
			cout <<"以已剔除空白符";
			cin >>endl;
			//并更新检查结果
			htbBstVec = lbdCheck();
		}
	}
	//再检查中文空白符
	struct EnglishBlankResult {//检查结果类型
		CheckResult::iterator itEnglish;
		CheckResult::mapped_type::iterator itChinese;
		string strChange;
	};
	vector<EnglishBlankResult> vecChineseBlank;//检查结果，两个迭代器和改变后字符串
	for(auto itEnglish=htbBstVec.begin(); itEnglish!=htbBstVec.end(); ++itEnglish) {
		for(auto itChinese=itEnglish->second.begin();
			itChinese!=itEnglish->second.end(); ++itChinese)
		{
			string tmp = lbdDropBlank(itChinese->first);
			if(!tmp.empty())
				vecChineseBlank.push_back(EnglishBlankResult{itEnglish, itChinese, tmp});
		}
	}
	cout <<"检查到的中文两侧带有空白符的情况有:\n";
	if(vecChineseBlank.empty()) {
		cout <<"（空）";
		cin >>endl;
	}
	else {
		for(auto &res: vecChineseBlank) {
			cout <<TAB_BLANK "\"" <<res.itEnglish->first <<"\""
				<<LONG_TAB_BLANK "\"" <<res.itChinese->first <<"\"\n";
			cout <<PrintItemIndexVector(res.itChinese->second) <<"\n";
		}
		cout <<"共" <<vecChineseBlank.size() <<"个，是否剔除空白符，输入非空则是: ";
		if(CinGetLineJudgeBlank()) {
			//更改空白符
			for(auto &res: vecChineseBlank) {
				for(auto &idx: res.itChinese->second) {
					idx->chinese = res.strChange;
					setSave.insert(idx.itList->first);
				}
			}
			cout <<"以已剔除空白符";
			cin >>endl;
			//更新检查结果
			htbBstVec = lbdCheck();
		}
	}
	//再进行中文不一致检查
	cout <<"检查到的中文不一致情况有:\n";
	bool bFind = false;
	auto &listTemp = g_mapWord.at(TEMP_LIST_NAME);
	for(auto &prEnglish: htbBstVec) {
		//若找到不一致的
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
		cout <<"共" <<listTemp.size() <<"个，已存入" TEMP_LIST_NAME "中";
	}
	else
		cout <<"（空）";
	cin >>endl;
}


//从文件导入词汇表
void ChoiceInput(const string &curName, ListNameSet &setSave)
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
				std::replace_if(name.begin(), name.end(), IsNotBroadIdChar, '_');
				++ cntList;
				if(g_mapWord.find(name)!=g_mapWord.end())
					++ cntWrongList;
				auto &newWords = g_mapWord[name];
				setSave.insert(name);
				//从流读取关键字段
				std::string str;
				while(std::getline(ifs, str)) {
					Word word;
					//找有效字符头
					auto itSt = std::find_if_not(str.begin(), str.end(), IsBlankChar);//a
					//判断注释
					if(itSt==str.end() || *itSt=='#' || *itSt==';')
						continue;
					//找字段尾，不要求后续
					auto itGap = std::find_if(itSt+1, str.end(), IsBlankChar);//\t2
					word.english.assign(itSt, itGap);
					//找后续词
					auto itWordSt = std::find_if_not(itGap, str.end(), IsBlankChar);//d
					auto ritWordEd = std::find_if_not(str.rbegin(),
						std::string::reverse_iterator(itWordSt), IsBlankChar);//i
					//存储值，可能为空
					if(itWordSt!=str.end() && itWordSt<ritWordEd.base())
						word.chinese.assign(itWordSt, ritWordEd.base());
					if(word.chinese.empty())
						continue;
					//判断重复
					if(ListAddWord(newWords, word))
						++ cntWord;
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


//高级从文件导入词汇表
void ChoiceAdInput(const string &curName, ListNameSet &setSave)
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
	cout <<"是否进行导入？输入非空白继续: ";
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
			std::replace_if(name.begin(), name.end(), IsNotBroadIdChar, '_');
			++ cntList;
			if(g_mapWord.find(name)!=g_mapWord.end())
				++ cntWrongList;
			auto &newWords = g_mapWord[name];
			setSave.insert(name);
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
					if(ListAddWord(newWords, word))
						++ cntWord;
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
void ChoiceOutput(const string &curName, ListNameSet &setSave)
{
	regex rgxOutput;
	vector<ListType::iterator> vecListOutput;
	cout <<"输入导出词汇表正则，全词匹配，空则返回: \n";
	if(!CinGetRegex(rgxOutput))
		return;
	for(auto &itRes=g_mapWord.begin(); itRes!=g_mapWord.end(); ++itRes) {
		if(std::regex_match(itRes->first, rgxOutput))
			vecListOutput.push_back(itRes);
	}
	cout <<"找到匹配的词汇表：\n";
	for(auto &ref: vecListOutput)
		cout <<ref->first <<"\n";
	cout <<"共" <<vecListOutput.size() <<"个，输入非空白继续: ";
	if(!CinGetLineJudgeBlank())
		return;
	for(auto &refIt: vecListOutput) {
		ofstream ofs(OUPPUT_DIR_PATH "/"+refIt->first+OUPPUT_FILE_EXT);
		for(auto &word: refIt->second)
			ofs <<word.english <<" " <<word.chinese <<"\n";
	}
	cout <<"完成，已导出到\"" <<OUPPUT_DIR_NAME <<"\"文件夹";
	cin >>endl;
}


//词汇表导出到文件
void ChoicePlan(const string &curName, ListNameSet &setSave)
{
	const string c_flagData = "#date";//日期标志
	const string c_flagGroup = "#group";//每组标志
	const string c_flagBlank = "#blank";//空白标志
	const vector<int> vecPeriod = {0, 1, 3, 7, 14, 29};
	cout <<"生成背单词时间表用，每天计划背的单词在第";
	for(int i: vecPeriod)
		cout <<i <<",";
	cout <<"天后复习\n"
		<<"将输入文件放在 \"" INPUT_DIR_PATH "\" 目录下\n"
		<<"文件每行代表一个输入项目，可以为如下几种:\n"
		<<"1,\"(单词表名) [(开始编号)] [(结束编号)]\" 表示要背的一个单词表系列\n"
		<<"\t如\"lis 1 3\"表示lis1、lis2、lis3一共3个表\n"
		<<"\t开始编号和结束编号可省略表示只有一个表\n"
		<<"2,\"" <<c_flagData <<" (日期)\" 表示下面的表从给出的日期开始\n"
		<<"\t日期表示如格式 \"2018-1-23\" ，初始默认使用运行当天的日期\n"
		<<"3,\"" <<c_flagGroup <<" (数量)\" 表示下面每天计划背诵多少个单词表\n"
		<<"\t初始默认为1，给出这行意味着分隔上下文单词表\n"
		<<"\t即接下来的单词表一定从新的一天开始背\n"
		<<"4,\"" <<c_flagBlank <<" [(数量)]\" 表示接下来空出多少天没有背诵计划\n"
		<<"\t不填数量或填0表示单纯的分隔上下文单词表\n";
	cout <<"输入输入txt文件名，不包括后缀名，空则返回: ";
	string strFileName;
	if(!CinGetLineFirstString(strFileName))
		return;
	ifstream ifs(INPUT_DIR_PREFIX+strFileName+INPUT_FILE_EXT);
	if(!ifs.is_open()) {
		cout <<"未找到输入文件";
		cin >>endl;
		return;
	}
	cout <<"输入输出txt文件名，不包括后缀名，空则返回: ";
	if(!CinGetLineFirstString(strFileName))
		return;
	ofstream ofs(OUPPUT_DIR_PREFIX+strFileName+OUPPUT_FILE_EXT);
	if(!ofs.is_open()) {
		cout <<"输出文件打开失败";
		cin >>endl;
		return;
	}
	vector<pair<time_t, string>> vecList;//存放单词表
	int groupSize = 1;//每天背诵数
	int nowSize = 0;//当前已有背诵数
	int listCnt = 0;//单词表数
	auto nowTime = std::time(nullptr);//当前时间点
	time_t timeOneDay = 60*60*24;
	bool bInitDate = true;//是否是默认时间
	//将时间对其到天
	auto tmStru = TimeNumToTimeStru(nowTime);
	tmStru.tm_sec = tmStru.tm_min = tmStru.tm_hour = 0;
	tmStru.tm_isdst = 0;
	nowTime = TimeStruToTimeNum(tmStru);
	//输入循环
	string strLine;
	for(int lineIdx=0; std::getline(ifs, strLine); ++lineIdx) {
		istringstream iss(std::move(strLine));
		string name, strTmp;
		int num1, num2, num3;
		char ch1, ch2;
		if(!(iss >>name))
			continue;
		//若改变日期
		if(name==c_flagData) {
			if(!(iss >>num1 >>ch1 >>num2 >>ch2 >>num3)
				|| ch1!='-' || ch2!='-')
			{
				cout <<"warning in line " <<lineIdx <<" with " <<name <<": "
					<<"未能识别日期格式，忽略此行\n";
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
					<<"日期范围错误，忽略此行\n";
				continue;
			}
			//时间不合法，在不为初始未使用时间的情况下小于上次时间
			else if((!(bInitDate && vecList.empty()) && tmNumTmp<=nowTime)) {
				cout <<"warning in line " <<lineIdx <<" with " <<c_flagData <<": "
					<<"日期出现倒退情况，忽略此行\n";
				continue;
			}
			nowTime = tmNumTmp;
			nowSize = 0;
			bInitDate = false;
		}
		//若改变每天数量
		else if(name==c_flagGroup) {
			if(!(iss >>num1) || num1<=0) {
				cout <<"warning in line " <<lineIdx <<" with " <<name <<": "
					<<"未提供数量或数量不为正，忽略此行\n";
				continue;
			}
			groupSize = num1;
			if(nowSize>0) {
				nowSize = 0;
				nowTime += timeOneDay;
			}
		}
		//若添加空白表
		else if(name==c_flagBlank) {
			if(!(iss >>num1))
				num1 = 0;
			if(num1<0) {
				cout <<"warning in line " <<lineIdx <<" with " <<name <<": "
					<<"提供数量为负，忽略此行\n";
				continue;
			}
			if(nowSize>0) {
				nowSize = 0;
				nowTime += timeOneDay;
			}
			nowTime += num1*timeOneDay;
		}
		//若添加表
		else {
			bool bHaveIdx = true;
			if(!(iss >>num1)) {
				bHaveIdx = false;//标记表没有序号
				num1 = 0, num2 = 0;
			}
			else if(num1<0) {
				cout <<"warning in line " <<lineIdx <<" with " <<name <<": "
					<<"提供第一个数字为负，忽略此行\n";
				continue;
			}
			if(!(iss >>num2)) {
				num2 = num1;
			}
			else if(num2<num1) {
				cout <<"warning in line " <<lineIdx <<" with " <<name <<": "
					<<"提供第二个数字小于第一个数字，忽略此行\n";
				continue;
			}
			++ num2;
			listCnt += num2-num1;
			for(int i=num1; i<num2; ) {
				int lastSize = std::min(groupSize-nowSize, num2-i);
				string newName;
				//改变名字
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
				//循环更新
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
				<<"读取完成后仍有剩余字符\n";
		}
	}
	OstreamSet<> osst({&cout, &ofs});//共同输出
	osst <<"单词表数: " <<listCnt <<"\n"
		<<"背新单词天数: " <<vecList.size() <<"\n";
	vector<string> vecOut;
	//生成表
	if(!vecList.empty()) {
		time_t tmNum = vecList.front().first;//当前时间
		int idx = 0;//当前vecList索引
		while(true) {
			string strTmp;
			bool bHaveNew = idx!=vecList.size() && vecList[idx].first==tmNum;//有新的单词表
			if(bHaveNew)
				strTmp <<"背: " <<vecList[idx].second <<"\n";
			//找复习的单词表
			int idxSt;//vecList在period范围内的起始位置
			for(idxSt = idx-1; idxSt>=0; --idxSt) {
				if(vecList[idxSt].first<tmNum-timeOneDay*vecPeriod.back())
					break;
			}
			int idxPeriod = vecPeriod.size()-1;//vecPeriod索引
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
					strTmp <<"复习: " <<vecList[idxSt].second <<"\n";
					-- idxPeriod;
				}
			}
			//不为空则导入
			if(!strTmp.empty()) {
				vecOut.emplace_back();
				vecOut.back() <<std::to_string(vecOut.size()) <<",  "
					<<TimeToStr(tmNum, true, false, "%F") <<":\n"
					<< strTmp;
			}
			//更新计数
			tmNum += timeOneDay;
			if(bHaveNew)
				++ idx;
			else {//idx==0不会进入发生
				//若超出复习最大周期
				if(vecList[idx-1].first<tmNum-timeOneDay*vecPeriod.back()) {
					if(idx==vecList.size())
						break;
					else
						tmNum = vecList[idx].first;
				}
			}
		}
	}
	osst <<"总背单词天数: " <<vecOut.size() <<"\n";
	ofs <<"计划表如下: " <<"\n\n";
	for(auto &str: vecOut)
		ofs <<str <<"\n";
	ofs.close();
	cout <<"生成完成，计划表已导入文件";
	cin >>endl;
}


//调试用，存储所有文件
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
	//读取存储数据
	ReadDataFile();

	//业务循环
	string curName = NOW_LIST_NAME;//当前表
	ListNameSet setSave;//保存文件列表
	while(true) {
		string choice;
		cout <<"\n"
			<<"当前共有词汇表: " <<g_mapWord.size() <<"\n"
			<<"当前词汇表为: " <<curName <<"，共有单词: " <<g_mapWord.at(curName).size() <<"\n"
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
					<<"delete，当前词汇表删除单词\n"
					<<"updata，更新词汇表单词\n"
					<<"recite，当前词汇表背单词，有放回随机显示，选择删除\n"
					<<"check，当前词汇表检查单词，无放回随机显示，选择导入\n"
					<<"copy，与当前词汇表复制单词\n"
					<<"search，搜索词汇表单词\n"
					<<"regularize，检查所有词汇表的中文是否一致\n"
					<<"input，从文件导入词汇表\n"
					<<"adinput，高级从文件导入词汇表\n"
					<<"plan，按照记忆曲线生成计划"
					<<"output，词汇表导出到文件\n";
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

		//保存文件
		WriteDataFile(setSave);
		setSave.clear();
	}


	return 0;
}

