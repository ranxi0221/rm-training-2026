/**
 *  stl_tutorial.cpp
 *  演示 vector、string、map 的基本用法
 * 包括：
 *   1. vector: 动态数组，可动态增长
 *   2. string: 字符串，支持各种操作
 *   3. map: 键值对映射，类似于字典
 */

#include <iostream>
#include <vector>   // 使用 vector 必须包含此头文件
#include <string>   // 使用 string 必须包含此头文件
#include <map>      // 使用 map 必须包含此头文件
using namespace std;

int main() {
    // 1. vector（动态数组）
    // vector（动态数组）
//操作	                      代码	                                 说明
//包含头文件	         #include <vector>	                        必须包含
//创建空vector	         vector<int> v;	                           元素类型可以是任意类型
//初始化	             vector<int> v = {1,2,3};                   列表初始化（C++11）
//指定个数和初值	       vector<int> v(5, 10);	                5个元素，每个都是10
//末尾添加元素	          v.push_back(100);	                        动态扩容
//访问元素	             v[0] 或 v.at(0)	                        下标从0开始
//获取大小	             v.size()	                                返回当前元素个数
//遍历	            for (int i=0; i<v.size(); ++i) 或 range-for	    两种方式
//修改元素	            v[2] = 99;	                                 直接赋值
//删除末尾元素	        v.pop_back();	                              移除最后一个
//清空	                v.clear();	                                 删除所有元素
    cout << "vector 示例 " << endl;
    
    // 1.1 创建和初始化
    vector<int> v1;                 // 空的int类型vector
    vector<int> v2 = {1, 2, 3, 4};  // 初始化为 {1,2,3,4}
    vector<int> v3(5, 10);          // 5个元素，每个都是10 → {10,10,10,10,10}
    
    // 1.2 添加元素
    v1.push_back(100);  // 末尾添加元素
    v1.push_back(200);
    v1.push_back(300);
    
    // 1.3 访问元素（使用下标，从0开始）
    cout << "v1[0] = " << v1[0] << endl;
    cout << "v1[1] = " << v1[1] << endl;
    
    // 1.4 获取大小
    cout << "v1 的大小: " << v1.size() << endl;
    
    // 1.5 遍历 vector（三种方式）
    cout << "v2 的元素: ";
    for (int i = 0; i < v2.size(); i++) {
        cout << v2[i] << " ";
    }
    cout << endl;
    
    // 范围 for 循环（C++11）
    cout << "v3 的元素: ";
    for (int val : v3) {
        cout << val << " ";
    }
    cout << endl;
    
    // 1.6 修改元素
    v2[2] = 99;  // 将第三个元素改为99
    cout << "修改后 v2: ";
    for (int val : v2) cout << val << " ";
    cout << endl;
    
    // 1.7 删除最后一个元素
    v1.pop_back();  // 删除最后一个元素（300）
    cout << "pop_back 后 v1 的大小: " << v1.size() << endl;  // 变为2
    
    
    //  2. string（字符串）
   //操作	          代码	             说明
//包含头文件	#include <string>	    必须包含
//创建	       string s = "Hello";	  类似字符数组但更安全
//拼接	       s1 + s2 或 s1 += s2	   直接用 + 或 +=
//获取长度	    s.length() 或 s.size()	返回字符个数
//字符访问	     s[0]	                下标从0开始
//查找子串	     s.find("fox")	       返回索引，未找到返回 string::npos
//提取子串	     s.substr(pos, len)	    从pos开始取len个字符
//读取整行	     getline(cin, s)	     包含空格
//比较	        s1 == s2	          可直接比较，字典序
    cout << "\nstring 示例 " << endl;
    
    // 2.1 创建和初始化
    string s1 = "Hello";
    string s2("World");
    string s3;   // 空字符串
    
    // 2.2 赋值和拼接
    s3 = s1 + " " + s2;   // 使用 + 拼接字符串
    cout << "s3 = " << s3 << endl;
    
    s1 += " C++";   // s1 变为 "Hello C++"
    cout << "s1 = " << s1 << endl;
    
    // 2.3 获取长度
    cout << "s3 的长度: " << s3.length() << endl;   // 或 s3.size()
    
    // 2.4 按字符访问
    cout << "s3 的第一个字符: " << s3[0] << endl;
    cout << "s3 的最后一个字符: " << s3[s3.length()-1] << endl;
    
    // 2.5 字符串查找
    string sentence = "The quick brown fox jumps over the lazy dog.";
    size_t pos = sentence.find("fox");
    if (pos != string::npos) {
        cout << "在位置 " << pos << " 找到 'fox'" << endl;
    } else {
        cout << "未找到" << endl;
    }
    
    // 2.6 提取子串 (substr)
    string sub = sentence.substr(10, 5);  // 从索引10开始取5个字符
    cout << "substr(10,5) = " << sub << endl;  // "brown"
    
    // 2.7 输入字符串（含空格）
    string input;
    cout << "请输入一行文字（含空格）: ";
    cin.ignore();  // 忽略之前可能的回车
    getline(cin, input);   // 读取整行
    cout << "你输入的是: " << input << endl;
    
    
    // 3. map（键值对映射）
    //操作	       代码	                         说明
//包含头文件	#include <map>	                必须包含
//创建	        map<string, int> m;	           键类型，值类型
//插入	         m["key"] = value;	          下标方式（若键不存在则创建）
//插入（避免创建）	m.insert({"key", value});	不覆盖已存在的键
//访问	          m["key"]	                  若键不存在会创建并赋默认值
//查找键是否存在	m.find("key") != m.end()	常用模式
//遍历	           for (auto &p : m)	       按键自动排序（默认升序）
//删除	           m.erase("key");	          删除指定键
//获取大小	        m.size()	              键值对个数
    cout << "\nmap 示例 " << endl;
    
    // 3.1 创建和初始化
    map<string, int> ages;   // 键类型 string，值类型 int
    map<string, string> capitals = {
        {"China", "Beijing"},
        {"Japan", "Tokyo"},
        {"France", "Paris"}
    };
    
    // 3.2 插入元素（三种方式）
    ages["Alice"] = 20;          // 方式1：使用下标
    ages.insert({"Bob", 22});    // 方式2：使用 insert + 初始化列表
    ages.insert(pair<string, int>("Charlie", 25));  // 方式3：pair
    
    // 3.3 访问元素（使用下标，若键不存在会创建该键并赋默认值）
    cout << "Alice 的年龄: " << ages["Alice"] << endl;
    cout << "Bob 的年龄: " << ages["Bob"] << endl;
    
    // 3.4 检查键是否存在
    if (ages.find("David") != ages.end()) {
        cout << "David 的年龄: " << ages["David"] << endl;
    } else {
        cout << "David 不存在于 map 中" << endl;
    }
    
    // 3.5 修改值
    ages["Alice"] = 21;   // 直接赋值修改
    cout << "修改后 Alice 的年龄: " << ages["Alice"] << endl;
    
    // 3.6 遍历 map（注意：按键自动排序）
    cout << "所有年龄信息: " << endl;
    for (const auto &pair : ages) {
        cout << "  " << pair.first << " -> " << pair.second << endl;
    }
    
    // 3.7 遍历 capitals
    cout << "首都信息: " << endl;
    for (auto it = capitals.begin(); it != capitals.end(); ++it) {
        cout << "  " << it->first << " : " << it->second << endl;
    }
    
    // 3.8 删除元素
    ages.erase("Charlie");
    cout << "删除 Charlie 后，map 大小: " << ages.size() << endl;
    
    return 0;
}