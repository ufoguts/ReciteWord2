// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

//屏蔽时间转换警告
#define _CRT_SECURE_NO_WARNINGS


#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO:  在此处引用程序需要的其他头文件

//c库
#include <cstdio>//cio
#include <cassert>//断言
#include <cstdlib>//杂项
#include <cctype>//字符分类
#include <cstring>//字符串处理
#include <cmath>//数学
#include <clocale>//本地化
#include <cstdint>//类型扩展
#include <ctime>//时间
#include <cstdarg>//变长参数

//io相关
#include <iostream>//c++io
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::ends;
using std::ios_base;
using std::istream;
using std::ostream;
#include <sstream>//字符串io
using std::istringstream;
using std::ostringstream;
using std::stringstream;
#include <fstream>//文件io
using std::ifstream;
using std::ofstream;
using std::fstream;
#include <iomanip>//高级io

//泛型相关
#include <type_traits>//类型处理
#include<limits>//数值极限
using std::numeric_limits;
#include <algorithm>//算法
#include <initializer_list>//列表
using std::initializer_list;
#include <utility>//基础容器
using std::swap;
using std::pair;
using std::make_pair;
using std::piecewise_construct;
#include <tuple>//元组
using std::tuple;
using std::tuple_size;
using std::tuple_element;
using std::make_tuple;
//using std::tie;
using std::forward_as_tuple;
#include <functional>//函数
using std::function;
using std::hash;
using std::reference_wrapper;
namespace placeholders = std::placeholders;
#include <iterator>//迭代器
using std::reverse_iterator;
using std::back_insert_iterator;
using std::front_insert_iterator;
using std::insert_iterator;
using std::move_iterator;
using std::make_move_iterator;

//容器相关
#include <string>//字符串
using std::string;
using std::wstring;
#include <vector>//向量
using std::vector;
#include <array>//数组
using std::array;
#include <deque>//双向向量
using std::deque;
#include <list>//链表
using std::list;
#include <set>//稳定排序二叉树
using std::set;
using std::multiset;
#include <map>//稳定排序二叉树对
using std::map;
using std::multimap;
#include <unordered_set>//哈希表
using std::unordered_set;
using std::unordered_multiset;
#include <unordered_map>//哈希表对
using std::unordered_map;
using std::unordered_multimap;
#include <queue>//队列
using std::priority_queue;

//工具类相关
#include <memory>//内存
using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using std::make_unique;
#include <exception>//异常
using std::exception;
#include <stdexcept>//更多异常
using std::runtime_error;
#include <random>//随机数
using std::default_random_engine;
using std::mt19937;
using std::mt19937_64;
#include <ratio>//比例
using std::ratio;
#include <complex>//复数
using std::complex;
#include <regex>//正则表达式
using std::regex;
using std::wregex;
using std::regex_error;
using std::cmatch;
using std::smatch;
using std::wcmatch;
using std::wsmatch;
using std::cregex_iterator;
using std::sregex_iterator;
using std::wcregex_iterator;
using std::wsregex_iterator;
using std::csub_match;
using std::ssub_match;
using std::wcsub_match;
using std::wssub_match;

//跨平台相关
#include <chrono>//时钟
using std::chrono::time_point;
using std::chrono::time_point_cast;
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::system_clock;
using std::chrono::steady_clock;
namespace chrono = std::chrono;
#include <thread>//线程
using std::thread;
namespace this_thread = std::this_thread;
#include <mutex>//互斥
using std::mutex;
using std::timed_mutex;
using std::lock_guard;
using std::unique_lock;
using std::defer_lock;
using std::try_to_lock;
using std::adopt_lock;
#include <condition_variable>//线程同步
using std::condition_variable;
using std::condition_variable_any;
using std::cv_status;
#include <future>//期货
using std::future;
using std::shared_future;




/*当前主机和版本参考路径：
Boost 1.66.0
附加包含目录：
C:\Program Files\boost_1_66_0
附加库目录：
C:\Program Files\boost_1_66_0\stage\lib
附加依赖项：
环境：
建议使用静态编译，代码生成-运行库-MTd/MT
除非编译动态链接库，静态链接不易保证位置无关
*/

//filesystem，文件系统库
#include <boost/filesystem.hpp>
namespace boost { namespace filesystem { using boost::system::error_code; }; };
namespace filesystem = boost::filesystem;



//自制库
#include "IoOperator.h"
#include "TypeExtend.h"
#include "UsedMacro.h"
#include "StruAndAlgo.h"
