// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

//����ʱ��ת������
#define _CRT_SECURE_NO_WARNINGS


#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�

//c��
#include <cstdio>//cio
#include <cassert>//����
#include <cstdlib>//����
#include <cctype>//�ַ�����
#include <cstring>//�ַ�������
#include <cmath>//��ѧ
#include <clocale>//���ػ�
#include <cstdint>//������չ
#include <ctime>//ʱ��
#include <cstdarg>//�䳤����

//io���
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
#include <sstream>//�ַ���io
using std::istringstream;
using std::ostringstream;
using std::stringstream;
#include <fstream>//�ļ�io
using std::ifstream;
using std::ofstream;
using std::fstream;
#include <iomanip>//�߼�io

//�������
#include <type_traits>//���ʹ���
#include<limits>//��ֵ����
using std::numeric_limits;
#include <algorithm>//�㷨
#include <initializer_list>//�б�
using std::initializer_list;
#include <utility>//��������
using std::swap;
using std::pair;
using std::make_pair;
using std::piecewise_construct;
#include <tuple>//Ԫ��
using std::tuple;
using std::tuple_size;
using std::tuple_element;
using std::make_tuple;
//using std::tie;
using std::forward_as_tuple;
#include <functional>//����
using std::function;
using std::hash;
using std::reference_wrapper;
namespace placeholders = std::placeholders;
#include <iterator>//������
using std::reverse_iterator;
using std::back_insert_iterator;
using std::front_insert_iterator;
using std::insert_iterator;
using std::move_iterator;
using std::make_move_iterator;

//�������
#include <string>//�ַ���
using std::string;
using std::wstring;
#include <vector>//����
using std::vector;
#include <array>//����
using std::array;
#include <deque>//˫������
using std::deque;
#include <list>//����
using std::list;
#include <set>//�ȶ����������
using std::set;
using std::multiset;
#include <map>//�ȶ������������
using std::map;
using std::multimap;
#include <unordered_set>//��ϣ��
using std::unordered_set;
using std::unordered_multiset;
#include <unordered_map>//��ϣ���
using std::unordered_map;
using std::unordered_multimap;
#include <queue>//����
using std::priority_queue;

//���������
#include <memory>//�ڴ�
using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using std::make_unique;
#include <exception>//�쳣
using std::exception;
#include <stdexcept>//�����쳣
using std::runtime_error;
#include <random>//�����
using std::default_random_engine;
using std::mt19937;
using std::mt19937_64;
#include <ratio>//����
using std::ratio;
#include <complex>//����
using std::complex;
#include <regex>//������ʽ
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

//��ƽ̨���
#include <chrono>//ʱ��
using std::chrono::time_point;
using std::chrono::time_point_cast;
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::system_clock;
using std::chrono::steady_clock;
namespace chrono = std::chrono;
#include <thread>//�߳�
using std::thread;
namespace this_thread = std::this_thread;
#include <mutex>//����
using std::mutex;
using std::timed_mutex;
using std::lock_guard;
using std::unique_lock;
using std::defer_lock;
using std::try_to_lock;
using std::adopt_lock;
#include <condition_variable>//�߳�ͬ��
using std::condition_variable;
using std::condition_variable_any;
using std::cv_status;
#include <future>//�ڻ�
using std::future;
using std::shared_future;




/*��ǰ�����Ͱ汾�ο�·����
Boost 1.66.0
���Ӱ���Ŀ¼��
C:\Program Files\boost_1_66_0
���ӿ�Ŀ¼��
C:\Program Files\boost_1_66_0\stage\lib
���������
������
����ʹ�þ�̬���룬��������-���п�-MTd/MT
���Ǳ��붯̬���ӿ⣬��̬���Ӳ��ױ�֤λ���޹�
*/

//filesystem���ļ�ϵͳ��
#include <boost/filesystem.hpp>
namespace boost { namespace filesystem { using boost::system::error_code; }; };
namespace filesystem = boost::filesystem;



//���ƿ�
#include "IoOperator.h"
#include "TypeExtend.h"
#include "UsedMacro.h"
#include "StruAndAlgo.h"
