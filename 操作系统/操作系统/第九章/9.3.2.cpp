
/*�ڶ����������������ݽṹʱ��������Щ���롣ԭ�������ϵĵ����ݽṹ�����ʵ���������ģ�ʵ������ǿ��Ը���ĳһȷ����ַ��
����ĳһ�ڴ��е����ݡ�����ģ��ʱ���޷��涨ĳһ�����ֵ�������������һ��ָ�룬���Ժ����в�һ�����뵽�Ľ���취�ǽ�һ������
��ģ���ڴ棬������Ƭ�ڴ��н�һ��α�����о���ô��ûʲô���壬��ֱ�Ӱ�ƽ����ʽ������
*/

#include"stdafx.h"
#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include<Windows.h>

using namespace std;
#if 0

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
typedef struct Leisure_node {
	int begin;//��ʼ��ַ
	int size;//����
	int state;//0������У�1�����ѷ���
	struct Leisure_node *behind;//ָ����һ���ڵ�
	struct Leisure_node *front;//ָ����һ���ڵ�
}Lei_node;

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
	cout << "��ʼ��ַ(KB)  ��ҵ��      ��ҵ��С(KB)" << endl;
	for (int i = 0; i < memory_list.size(); i++) {
		cout.setf(ios::left);
		cout.width(14);
		cout << memory_list[i].begin;
		cout.setf(ios::left);
		cout.width(14);
		cout << memory_list[i].name;
		cout.setf(ios::left);
		cout.width(14);
		cout << memory_list[i].size << endl;
	}
}

void Memory::init() {
	ifstream file;
	string mm = "memory.txt";
	file.open("memory.txt");
	if (!file) {
		cerr << mm << "�ļ���ʧ��" << endl;
		exit(1);
	}
	string str;
	getline(file, str);
	while (!file.eof()) {
		int pos1 = str.find(" ", 0);
		int begin;
		stringstream buff1;
		Memory_node node;
		buff1 << str.substr(0, pos1);
		buff1 >> begin;
		int pos2 = str.find(" ", pos1 + 1);
		string name = str.substr(pos1 + 1, pos2 - pos1);//pos2������ǿ����ĸ���������λ��
		stringstream buff2;
		buff2 << str.substr(pos2 + 1, str.size() - pos2);
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
	Lei_node *begin;//������˫��������ָ��
	void init();
	void show();
	int getbegin(job j);//�õ���ҵ��������Ӧ��ŵ��׵�ַ���������з�������и���
	void earse(Lei_node *node);
};

void Leisure::earse(Lei_node *node) {
	Lei_node *p = node->front;
	Lei_node *q = node->behind;
	if (p) {
		if (q) {
			q->front = p;
			p->behind = q;
		}
		else {
			begin = p;
			p->behind = q;
		}
	}
	else {
		if (q) {
			q->front = p;
		}
		else {
			begin = NULL;
		}
	}
	
}

void Leisure::show() {
	cout << "��ʼ��ַ(KB)  ����(KB)        ״̬" << endl;
	Lei_node *node = begin;
	while (node) {
		cout.setf(ios::left);
		cout.width(14);
		cout << node->begin;
		cout.setf(ios::left);
		cout.width(14);
		cout << node->size;
		cout.setf(ios::left);
		cout.width(14);
		if (node->state == 0) cout << "����" << endl;
		else cout << "�ѷ���" << endl;
		node = node->front;
	}
}


int Leisure::getbegin(job j)
{
	int return_num;
	Lei_node *node=begin;
	while (node) {
		if (node->size >=j.size) break;
		else node = node->front;
	}
	if (node == NULL) {
		return -1;
	}
	else {
		return_num = node->begin;
		if (node->size == j.size) {
			earse(node);
		}
		else {
			node->begin += j.size;
			node->size -= j.size;
		}
	}
	return return_num;
}


void Leisure::init() {
	begin = NULL;
	ifstream file;
	file.open("leisure1.txt");
	if (!file) {
		cerr << "�ļ���ʧ��" << endl;
		exit(1);
	}
	string str;
	getline(file, str);
	while (!file.eof()) {
		stringstream buff1;
		int pos1 = str.find(" ", 0);
		Lei_node *node = new Lei_node();
		buff1 << str.substr(0, pos1);
		buff1 >> node->begin;
		int pos2 = str.find(" ", pos1 + 1);
		stringstream buff2;
		buff2 << str.substr(pos1 + 1, pos2 - pos1);
		buff2 >> node->size;
		if (str.substr(pos2, str.size() - pos2).find("δ����") != string::npos) node->state = 0;
		else node->state = 1;
		
		//����ָ��ָ������
		if (begin == NULL) {
			node->behind = NULL;
			begin = node;
			node->front = NULL;
		}
		else {
			Lei_node *n = begin;
			while (n->front) n = n->front;
			node->behind= n;
			n->front = node;
		}
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
	Lei_node *up_node = NULL;//���ڿ������ı��
	Lei_node *down_node = NULL;//���ڿ������ı��
	Lei_node *node = lei.begin;
	while (node) {
		if ((node->begin + node->size) == recyle_begin) {
			down = true;
			down_node = node;
		}
		if ((recyle_begin + recyle_length) == node->begin) {
			up = true;
			up_node = node;
		}
		node = node->front;
	}
	if (up) {
		int length = recyle_length + up_node->size;
		if (down) {
			down_node->size += length;
			lei.earse(up_node);
			//lei.leisure_list.erase(lei.leisure_list.begin() + up_num);
		}
		else {
			up_node->begin = recyle_begin;
			up_node->size = length;
		}
	}
	else {
		if (down) {
			down_node->size += recyle_length;
		}
		else {//�ڿ����������¿�һ����Ŀ���루���׵�ַ˳����룬֮����������˳��
			Lei_node *l = new Lei_node();
			l->begin = recyle_begin;
			l->size = recyle_length;
			l->state =0;
			Lei_node *p = lei.begin;
			while (p) {
				if (p->begin > recyle_begin) break;
				else p = p->front;
			}
			if (p) {
				if (p->behind) {
					l->behind = p->behind;
					p->behind->front = l;
				}
				else {
					l->behind = NULL;
					lei.begin = l;
				}
				l->front = p;
				p->behind = l;
			}
			else {
				Lei_node *pp = lei.begin;
				if (!pp) lei.begin = l;
				else {
					while (pp->front) pp = pp->front;
					l->behind = pp;
					l->front = NULL;
					pp->front = l;
				}
			}
		//	lei.leisure_list.insert(lei.leisure_list.begin() + i, l);
		}
	}
}

int test::get_beginAndlength(string name) {
	int i;
	for (i = 0; i < mem.memory_list.size(); i++) {
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
	if (get_beginAndlength(name)) {
		update_lei();
		update_mem(name);
	}
	if (!wait.empty()) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE);
		cout << wait[0].name << "���ڵȴ�״̬�����份��" << endl;
		wake_up = true;
		apply();
	}
}


void test::apply() {
	job j;
	if (wake_up) {
		j = wait[0];
		wait.erase(wait.begin());
		wake_up = false;
	}
	else {
		cout << "������������ҵ������:" << endl;
		cin >> j.name;
		for (int i = 0; i<mem.memory_list.size(); i++)
			if (mem.memory_list[i].name.find(j.name) != string::npos) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
				cout << j.name << "�Ѿ������������У������ٴ�����" << endl;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
				return;
			}
		cout << "��������ҵ�Ĵ�С" << endl;
		cin >> j.size;
	}
	int begin = lei.getbegin(j);//�ӿ��������ҵ���ҵӦ�ò������ʼλ�ã����beginΪ-1�����ʾ��ҵ����ȴ����������wait����
	if (begin == -1) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		cout << "�޺��ʴ�С�Ŀ���������ҵ����ȴ�����" << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
		wait.push_back(j);
	}
	else
	{
		mem.update_memory(begin, j);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
		cout << "��ҵ�ɹ�װ��" << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
	}
}

void test::init() {
	wake_up = false;
	mem.init();
	lei.init();
}

void test::show_lei() {
	lei.show();
}

void test::show_mem() {
	mem.show();
}

int main()
{
	test t;
	t.init();

	int flag_num;
	while (true) {
		cout << endl << "********************************************************" << endl;
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
	return 0;
}

#endif // 0