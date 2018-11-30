#include"stdafx.h"
#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
using namespace std;

//作业
struct job
{
	string name;
	int size;
};

//每一个内存单元
struct Memory_node { 
	int begin;//作业起始位置
	int size;//作业大小
	string name;//作业名字
};

//空闲说明表单元节点
struct Leisure_node {
	int begin;//起始地址
	int size;//长度
	string state;
};

class Memory {//主存空间
public:
	vector<Memory_node> memory_list;//用于保存主存空间占用情况
	void init();//对主存空间进行初始化
	void test();//用于测试
	void show();

};

void Memory::test() {
	for (int i = 0; i < memory_list.size(); i++)
		cout << memory_list[i].name << endl;
}

void Memory::show() {
	for (int i = 0; i < memory_list.size(); i++)
		cout << memory_list[i].begin<<"  "<< memory_list[i].name <<"  "<<memory_list[i].size<<endl;
}

void Memory::init() {
	ifstream file;
	string mm ="memory.txt";
	file.open("memory.txt");
	if (!file) {
		cerr << mm << "文件打开失败"<<endl;
		exit(1);
	}
	string str;
	getline(file, str);
	while (!file.eof()) {
		int pos1=str.find(" ", 0);
		int begin;
		stringstream buff1;
		Memory_node node;
		buff1<< str.substr(0, pos1);
		buff1>> begin;
		int pos2 = str.find(" ", pos1+1);
		string name = str.substr(pos1+1, pos2-pos1);//pos2代表的是拷贝的个数，而非位置
		stringstream buff2;
		buff2<<str.substr(pos2+1, str.size()-pos2);
		buff2 >> node.size;
		node.begin = begin;
		node.name = name;
		memory_list.push_back(node);
		getline(file, str);
	}
	file.close();
}

class Leisure {
public:
	vector<Leisure_node> leisure_list;//用于保存空闲区情况
	void init();
	void show();
	int getbegin(job j);
};

void Leisure::show() {
	for (int i = 0; i < leisure_list.size(); i++)
		cout << leisure_list[i].begin << "  " << leisure_list[i].size << "  " << leisure_list[i].state << endl;
}

int Leisure::getbegin(job j)
{
	int begin;
	int i;
	int jj = leisure_list.size();
	for (i = 0; i < leisure_list.size(); i++)
	{
		if (leisure_list[i].state=="空表目") return -1;
		if (leisure_list[i].size >= j.size) break;
	}
	begin = leisure_list[i].begin;
	if (leisure_list[i].size == j.size)//如果空闲区长度和作业大小相等
	{	
		leisure_list.erase(leisure_list.begin() + i);
	}
	else {
		leisure_list[i].begin += j.size;
		leisure_list[i].size -= j.size;
	}
	return begin;
}

void Leisure::init() {
	ifstream file;
	file.open("leisure.txt");
	if (!file) {
		cerr << "文件打开失败" << endl;
		exit(1);
	}
	string str;
	getline(file, str);
	while (!file.eof()){
		stringstream buff1;
		int pos1 = str.find(" ", 0);
		Leisure_node node;
		buff1 <<str.substr(0, pos1);
		buff1 >> node.begin;
		int pos2 = str.find(" ", pos1 + 1);
		stringstream buff2;
		buff2<<str.substr(pos1+1, pos2 - pos1);
		buff2 >> node.size;
		string state = str.substr(pos2, str.size() - pos2);
		node.state = state;
		leisure_list.push_back(node);
		getline(file, str);
		
	}
	file.close();
}

class test {
public:
	vector<job> wait;//等待队列
	Memory mem;

	Leisure lei;
	void init();
	void apply();//分配函数
	void update_memory(int begin,job j);
	void show_mem();
	void show_lei();
};

void test::show_lei() {
	lei.show();
}

void test::show_mem() {
	mem.show();
}

void test::update_memory(int begin, job j) {
	int i;
	for (i = 0; i < mem.memory_list.size(); i++) {
		if (mem.memory_list[i].begin == begin) break;
	}
	if (mem.memory_list[i].size == j.size) {
		mem.memory_list[i].name = j.name;
	}
	else {
		int begin2 = begin + j.size;
		int size2 = mem.memory_list[i].size - j.size;
		mem.memory_list[i].name = j.name;
		mem.memory_list[i].size = j.size;
		Memory_node node;
		node.begin = begin2;
		node.size = size2;
		node.name = "空闲区";
		mem.memory_list.insert(mem.memory_list.begin() + i + 1, node);
	}
}

void test::apply() {
	job j;
	cout << "请输入申请作业的名字:" << endl;
	cin >> j.name;
	cout << "请输入作业的大小" << endl;
	cin >> j.size;
	int begin = lei.getbegin(j);//从空闲区中找到作业应该插入的起始位置，如果begin为-1，则表示作业进入等待，将其插入wait队列
	if (begin == -1) {
		cout << "无合适大小的空闲区，作业进入等待队列" << endl;
		wait.push_back(j);
	}
	else
	{
		update_memory(begin,j);
	}
}

void test::init() {
	mem.init();
	lei.init();
}

int main() {
	test t;
	t.init();
	t.show_mem();
	t.show_lei();
	t.apply();
	t.show_mem();
	t.show_lei();
	return 0;
}