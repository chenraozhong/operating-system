
/*在定义空闲区链表的数据结构时和书中有些出入。原因是书上的的数据结构是针对实际情况定义的，实际情况是可以根据某一确定地址来
访问某一内存中的内容。但在模拟时，无法规定某一具体的值来限制链表的下一个指针，所以和书中不一样。想到的解决办法是建一个数组
来模拟内存，再在这片内存中建一个伪链表。感觉这么做没什么意义，就直接按平常方式建链表
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
typedef struct Leisure_node {
	int begin;//起始地址
	int size;//长度
	int state;//0代表空闲，1代表已分配
	struct Leisure_node *behind;//指向下一个节点
	struct Leisure_node *front;//指向上一个节点
}Lei_node;

class Memory {//主存空间
public:
	vector<Memory_node> memory_list;//用于保存主存空间占用情况
	void init();//对主存空间进行初始化
	void show();
	void update_memory(int begin, job j);//在分配的时候更新主存

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
		node.name = "空闲区";
		memory_list.insert(memory_list.begin() + i + 1, node);
	}
}

void Memory::show() {
	cout << "起始地址(KB)  作业名      作业大小(KB)" << endl;
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
		cerr << mm << "文件打开失败" << endl;
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
		string name = str.substr(pos1 + 1, pos2 - pos1);//pos2代表的是拷贝的个数，而非位置
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
	Lei_node *begin;//空闲区双向链表首指针
	void init();
	void show();
	int getbegin(job j);//得到作业在主存中应存放的首地址，并将空闲分区表进行更新
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
	cout << "起始地址(KB)  长度(KB)        状态" << endl;
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
		if (node->state == 0) cout << "空闲" << endl;
		else cout << "已分配" << endl;
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
		cerr << "文件打开失败" << endl;
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
		if (str.substr(pos2, str.size() - pos2).find("未分配") != string::npos) node->state = 0;
		else node->state = 1;
		
		//处理指针指向问题
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
	vector<job> wait;//等待队列
	int recyle_begin;//回收作业的首地址
	int recyle_length;//回收作业的长度
	bool up;//记录释放区与空闲区是否上邻接
	bool down;//记录释放区和空闲区是否下邻接
	bool wake_up;//是否将等待队列中的作业唤醒
public:
	Memory mem;
	Leisure lei;
	void init();
	void apply();//分配函数
	void recycle();//回收函数
	void show_mem();
	void show_lei();
	int get_beginAndlength(string name);//返回值为0的时候表示出错，为1表示正常
	void update_lei();//在回收的时候更新空闲区
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
			mem.memory_list.erase(mem.memory_list.begin() + i);//删除之后i+1的节点就变成了i节点
		}
		else mem.memory_list.erase(mem.memory_list.begin() + i);
	}
	else {
		mem.memory_list[i].name = "空闲区";
		if (up) {
			mem.memory_list[i].size += mem.memory_list[i + 1].size;
			mem.memory_list.erase(mem.memory_list.begin() + i + 1);
		}
	}
}

void test::update_lei() {
	down = false;
	up = false;
	Lei_node *up_node = NULL;//上邻空闲区的编号
	Lei_node *down_node = NULL;//下邻空闲区的编号
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
		else {//在空闲区表中新开一个表目插入（按首地址顺序插入，之后便无需调整顺序）
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
		cout << "主存中无此作业，操作错误" << endl;
		return 0;
	}
	else {
		recyle_begin = mem.memory_list[i].begin;
		recyle_length = mem.memory_list[i].size;
	}
	return 1;
}

void test::recycle() {
	cout << "请输入需要回收的作业名:" << endl;
	string name;
	cin >> name;
	if (get_beginAndlength(name)) {
		update_lei();
		update_mem(name);
	}
	if (!wait.empty()) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE);
		cout << wait[0].name << "处于等待状态，将其唤醒" << endl;
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
		cout << "请输入申请作业的名字:" << endl;
		cin >> j.name;
		for (int i = 0; i<mem.memory_list.size(); i++)
			if (mem.memory_list[i].name.find(j.name) != string::npos) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
				cout << j.name << "已经存在于主存中，无需再次申请" << endl;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
				return;
			}
		cout << "请输入作业的大小" << endl;
		cin >> j.size;
	}
	int begin = lei.getbegin(j);//从空闲区中找到作业应该插入的起始位置，如果begin为-1，则表示作业进入等待，将其插入wait队列
	if (begin == -1) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		cout << "无合适大小的空闲区，作业进入等待队列" << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
		wait.push_back(j);
	}
	else
	{
		mem.update_memory(begin, j);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
		cout << "作业成功装入" << endl;
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
		cout << endl << "主存空间的占用情况:" << endl;
		t.show_mem();
		cout << endl << "空闲区说明表:" << endl;
		t.show_lei();
		cout << endl << "********************************************************" << endl;
		cout << "操作选项:1、作业申请    2、作业回收    3、退出" << endl;
		cin >> flag_num;
		if (flag_num == 1) t.apply();
		else if (flag_num == 2) t.recycle();
		else break;
	}
	return 0;
	return 0;
}

#endif // 0