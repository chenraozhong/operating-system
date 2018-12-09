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
	void show();
	void update_memory(int begin, job j);//�ڷ����ʱ���������

};

void Memory::update_memory(int begin, job j) {
	int i;
	for (i = 0; i < memory_list.size(); i++) {
		if (memory_list[i].begin == begin) break;
	}
	if (memory_list[i].size == j.size) {
		memory_list[i].name = j.name;
	}
	else {
		int begin2 = begin + j.size;
		int size2 = memory_list[i].size - j.size;
		memory_list[i].name = j.name;
		memory_list[i].size = j.size;
		Memory_node node;
		node.begin = begin2;
		node.size = size2;
		node.name = "������";
		memory_list.insert(memory_list.begin() + i + 1, node);
	}
}

void Memory::show() {
	cout << "��ʼ��ַ      ��ҵ��      ��ҵ��С" << endl;
	for (int i = 0; i < memory_list.size(); i++) {
		cout.setf(ios::left);
		cout.width(14);
		cout << memory_list[i].begin;
		cout.setf(ios::left);
		cout.width(14);
		cout << memory_list[i].name;
		cout.setf(ios::left);
		cout.width(14);
		cout<< memory_list[i].size << endl;
	}
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
	getline(file, str);
	while (!file.eof()) {
		int pos1=str.find(" ", 0);
		int begin;
		stringstream buff1;
		Memory_node node;
		buff1<< str.substr(0, pos1);
		buff1>> begin;
		int pos2 = str.find(" ", pos1+1);
		string name = str.substr(pos1+1, pos2-pos1);//pos2������ǿ����ĸ���������λ��
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
	vector<Leisure_node> leisure_list;//���ڱ�����������
	void init();
	void show();
	int getbegin(job j);//�õ���ҵ��������Ӧ��ŵ��׵�ַ���������з�������и���
};

void Leisure::show() {
	cout << "��ʼ��ַ      ����           ״̬" << endl;
	for (int i = 0; i < leisure_list.size(); i++)
	{
		cout.setf(ios::left);
		cout.width(14);
		cout << leisure_list[i].begin; 
		cout.setf(ios::left);
		cout.width(14);
		cout << leisure_list[i].size; 
		cout.setf(ios::left);
		cout.width(14);
		cout << leisure_list[i].state << endl;
	}
		
}

int Leisure::getbegin(job j)
{
	int begin;
	int i;
	int jj = leisure_list.size();
	for (i = 0; i < leisure_list.size(); i++)
	{
		if (leisure_list[i].state.find("�ձ�Ŀ")!=string::npos) return -1;
		if (leisure_list[i].size >= j.size) break;
	}
	begin = leisure_list[i].begin;
	if (leisure_list[i].size == j.size)//������������Ⱥ���ҵ��С���
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
		cerr << "�ļ���ʧ��" << endl;
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
private:
	vector<job> wait;//�ȴ�����
	int recyle_begin;//������ҵ���׵�ַ
	int recyle_length;//������ҵ�ĳ���
	bool up;//��¼�ͷ�����������Ƿ����ڽ�
	bool down;//��¼�ͷ����Ϳ������Ƿ����ڽ�
	bool wake_up;//�Ƿ񽫵ȴ������е���ҵ����
public:
	Memory mem;
	Leisure lei;
	void init();
	void apply();//���亯��
	void recycle();//���պ���
	void show_mem();
	void show_lei();
	int get_beginAndlength(string name);//����ֵΪ0��ʱ���ʾ����Ϊ1��ʾ����
	void update_lei();//�ڻ��յ�ʱ����¿�����
	void update_mem(string name);
};

void test::update_mem(string name) {
	int i = 0;
	for (; i < mem.memory_list.size(); i++)
		if (mem.memory_list[i].name.find(name) != string::npos) break;
	if (down) {
		mem.memory_list[i - 1].size += recyle_length;
		if (up) {
			mem.memory_list[i - 1].size += mem.memory_list[i + 1].size;
			mem.memory_list.erase(mem.memory_list.begin() + i);
			mem.memory_list.erase(mem.memory_list.begin() + i);//ɾ��֮��i+1�Ľڵ�ͱ����i�ڵ�
		}
		else mem.memory_list.erase(mem.memory_list.begin() + i);
	}
	else {
		mem.memory_list[i].name = "������";
		if (up) {
			mem.memory_list[i].size += mem.memory_list[i + 1].size;
			mem.memory_list.erase(mem.memory_list.begin() + i + 1);
		}
	}
}

void test::update_lei() {
	down = false;
	up = false;
	int up_num=0;//���ڿ������ı��
	int down_num=0;//���ڿ������ı��
	for (int i = 0; i < lei.leisure_list.size(); i++) {
		if ((lei.leisure_list[i].begin + lei.leisure_list[i].size) == recyle_begin) {
			down_num = i;
			down = true;
		}
		if ((recyle_begin + recyle_length) == lei.leisure_list[i].begin) {
			up_num = i;
			up = true;
		}
	}
	if (up) {
		int length = recyle_length + lei.leisure_list[up_num].size;
		if (down) {
			lei.leisure_list[down_num].size += length;
			lei.leisure_list.erase(lei.leisure_list.begin() + up_num);
		}
		else {
			lei.leisure_list[up_num].begin = recyle_begin;
			lei.leisure_list[up_num].size = length;
		}
	}
	else {
		if (down) {
			lei.leisure_list[down_num].size += recyle_length;
		}
		else {//�ڿ����������¿�һ����Ŀ���루���׵�ַ˳����룬֮����������˳��
			Leisure_node l;
			l.begin = recyle_begin;
			l.size = recyle_length;
			l.state = "δ����";
			int i = 0;
			for (; i < lei.leisure_list.size(); i++) {
				if (lei.leisure_list[i].begin > l.begin) break;
			}
			lei.leisure_list.insert(lei.leisure_list.begin() + i, l);
		}
	}
}

int test::get_beginAndlength(string name) {
	int i;
	for ( i = 0; i < mem.memory_list.size(); i++) {
		if (mem.memory_list[i].name.find(name) != string::npos) break;
	}
	if (i == mem.memory_list.size()) {
		cout << "�������޴���ҵ����������" << endl;
		return 0;
	}
	else {
		recyle_begin = mem.memory_list[i].begin;
		recyle_length = mem.memory_list[i].size;
	}
	return 1;
}

void test::recycle() {
	cout << "��������Ҫ���յ���ҵ��:" << endl;
	string name;
	cin >> name;
	if(get_beginAndlength(name)){
		update_lei();
		update_mem(name);
	}
	if (!wait.empty()) {
		cout << wait[0].name << "�ȴ�װ�룬���份��" << endl;
		wake_up = true;
		apply();
	}
}

void test::show_lei() {
	lei.show();
}

void test::show_mem() {
	mem.show();
}

void test::apply() {
	job j;
	if (wake_up) {
		j = wait[0];
		wait.erase(wait.begin());
	}
	else {
		cout << "������������ҵ������:" << endl;
		cin >> j.name;
		cout << "��������ҵ�Ĵ�С" << endl;
		cin >> j.size;
	}
	int begin = lei.getbegin(j);//�ӿ��������ҵ���ҵӦ�ò������ʼλ�ã����beginΪ-1�����ʾ��ҵ����ȴ����������wait����
	if (begin == -1) {
		cout << "�޺��ʴ�С�Ŀ���������ҵ����ȴ�����" << endl;
		wait.push_back(j);
	}
	else
	{
		mem.update_memory(begin,j);
		cout << "��ҵ�ɹ�װ��" << endl;
	}
}

void test::init() {
	wake_up = false;
	mem.init();
	lei.init();
}

int main() {
	test t;
	t.init();
	
	int flag_num;
	while ( true) {
		cout << endl<<"********************************************************" << endl;
		cout << endl << "����ռ��ռ�����:" << endl;
		t.show_mem();
		cout << endl << "������˵����:" << endl;
		t.show_lei();
		cout << endl << "********************************************************" << endl;
		cout << "����ѡ��:1����ҵ����    2����ҵ����    3���˳�" << endl;
		cin >> flag_num;
		if (flag_num == 1) t.apply();
		else if (flag_num == 2) t.recycle();
		else break;
	}
	return 0;
}