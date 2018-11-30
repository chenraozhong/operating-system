#include"stdafx.h"
#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
using namespace std;

//��ҵ
struct job
{
	string name;
	int size;
};

//ÿһ���ڴ浥Ԫ
struct Memory_node { 
	int begin;//��ҵ��ʼλ��
	int size;//��ҵ��С
	string name;//��ҵ����
};

//����˵����Ԫ�ڵ�
struct Leisure_node {
	int begin;//��ʼ��ַ
	int size;//����
	string state;
};

class Memory {//����ռ�
public:
	vector<Memory_node> memory_list;//���ڱ�������ռ�ռ�����
	void init();//������ռ���г�ʼ��
	void test();//���ڲ���
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
		cerr << mm << "�ļ���ʧ��"<<endl;
		exit(1);
	}
	string str;
	stringstream buff;
	getline(file, str);
	while (!file.eof()) {
		int pos1=str.find(" ", 0);
		int begin;
		buff<< str.substr(0, pos1);
		buff >> begin;
		int pos2 = str.find(" ", pos1+1);
		string name = str.substr(pos1, pos2-pos1);//pos2������ǿ����ĸ���������λ��
		buff<<str.substr(pos2, str.size()-pos2);
		int size;
		buff >> size;
		Memory_node node;
		node.begin = begin;
		node.name = name;
		node.size = size;
		memory_list.push_back(node);
		getline(file, str);
	}
	file.close();
}

class Leisure {
public:
	vector<Leisure_node> leisure_list;//���ڱ�����������
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
	int i;
	int jj = leisure_list.size();
	for (i = 0; i < leisure_list.size(); i++)
	{
		if (leisure_list[i].state=="�ձ�Ŀ") return -1;
		if (leisure_list[i].size > j.size) break;
	}
	if (leisure_list[i].size == j.size)//������������Ⱥ���ҵ��С���
	{
		leisure_list.erase(leisure_list.begin() + i);
	}
	else {
		leisure_list[i].begin += j.size;
		leisure_list[i].size -= j.size;
	}
}

void Leisure::init() {
	ifstream file;
	file.open("leisure.txt");
	if (!file) {
		cerr << "�ļ���ʧ��" << endl;
		exit(1);
	}
	string str;
	stringstream buff;
	getline(file, str);
	while (!file.eof()) {
		int pos1 = str.find(" ", 0);
		Leisure_node node;
		buff <<str.substr(0, pos1);
		buff >> node.begin;
		int pos2 = str.find(" ", pos1 + 1);
		buff<< str.substr(pos1, pos2 - pos1);
		buff >> node.size;
		string state = str.substr(pos2, str.size() - pos2);
		node.state = state;
		leisure_list.push_back(node);
		getline(file, str);
	}
	file.close();
}

class test {
public:
	vector<job> wait;//�ȴ�����
	Memory mem;

	Leisure lei;
	void init();
	void apply();
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
		mem.memory_list.insert(mem.memory_list.begin() + i + 1, node);
	}
}

void test::apply() {
	job j;
	cout << "������������ҵ������:" << endl;
	cin >> j.name;
	cout << "��������ҵ�Ĵ�С" << endl;
	cin >> j.size;
	int begin = lei.getbegin(j);//�ӿ��������ҵ���ҵӦ�ò������ʼλ�ã����beginΪ-1�����ʾ��ҵ����ȴ����������wait����
	if (begin == -1) {
		cout << "�޺��ʴ�С�Ŀ���������ҵ����ȴ�����" << endl;
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
	t.apply();
	t.show_mem();
	return 0;
}