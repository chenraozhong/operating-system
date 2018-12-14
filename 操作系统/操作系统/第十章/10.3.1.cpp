#include"stdafx.h"
#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<sstream>
using namespace std;

typedef struct Table {
	int table_num;//页号
	int flag;//标志
	int lump_num;//主存块号
	int out_add;//外存地址
	int fix_flag;//修改位
}Table;

class Add {//地址变换
private:
	vector<Table> page_table;
	string order;//指令(长度为9)
	int page_num;//根据指令得到的页号
	int page_offset;//页内偏移量
	int abs_add;//绝对地址
public:
	void init();//对页表进行初始化操作
	void test();//测试函数
	void get_pagenum();//得到页号
	void get_pageoffset();//得到页内偏移量
	bool if_exist();//判断是否该页标志是否为1
	void interrupt();//进行中断处理
	void get_absadd();//得到绝对地址
};

void Add::get_absadd() {
	get_pageoffset();//得到页内偏移地址
	int lump_num = page_table[page_num].lump_num;
	abs_add = lump_num * 64 + page_offset;
	cout << "绝对地址为:" << abs_add << endl;
}

void Add::interrupt() {
	cout << "暂时未写" << endl;
}

bool Add::if_exist() {
	if (page_table[page_num].flag == 1) return true;
	else return false;
}

void Add::get_pagenum() {
	string str = order.substr(0, 3);
	page_num = 0;
	for (int i = 0; i < 3; i++) {
		page_num = page_num * 2 + (str[i]-48);
	}
}

void Add::get_pageoffset() {
	string str = order.substr(3, 9);
	page_offset = 0;
	for (int i=0; i < 6; i++) {
		page_offset = page_offset * 2 + (str[i] - 48);
	}
}

void Add::init() {
	ifstream file;
	file.open("table.txt");
	if (!file) {
		cerr << "打开文件失败" << endl;
		exit(1);
	}
	string str;
	getline(file, str);
	while (!file.eof()) {
		Table *t=new Table();
		int pos1 = str.find(" ", 0);
		stringstream buff1;
		buff1 << str.substr(0, pos1 - 0);
		buff1 >> t->table_num;
		int pos2 = str.find(" ", pos1+1);
		stringstream buff2;
		buff2 << str.substr(pos1+1, pos2 - pos1);
		buff2 >> t->flag;
		int pos3 = str.find(" ", pos2+1);
		stringstream buff3;
		buff3 << str.substr(pos2+1, pos3 - pos2);
		buff3 >> t->lump_num;
		int pos4 = str.find(" ", pos3+1);
		stringstream buff4;
		buff4 << str.substr(pos3+1, pos4 - pos3);
		buff4 >> t->out_add;
		int pos5 = str.find(" ", pos4+1);
		stringstream buff5;
		buff5 << str.substr(pos4+1, pos5 - pos4);
		buff5 >> t->fix_flag;
		page_table.push_back(*t);
		getline(file, str);
	}
}

void Add::test() {
	init();
	while (true) {
		cout << "请输入指令" << endl;
		cin >> order;
		get_pagenum();
		if (!if_exist()) {
			cout << "该页不在主存中，产生缺页中断";
			interrupt();
		}
		else {
			get_absadd();
			cout << "是否是\"存\"指令(1、是   2、否)" << endl;
			int save;
			cin >> save;
			if (save == 1) {
				page_table[page_num].fix_flag = 1;
			}
		}
		cout << "还有后继指令吗?(1、有    2、无)" << endl;
		int flag;
		cin >> flag;
		if (flag == 2) break;
	}
}

int main() {
	Add a;
	a.test();
	
	return 0;
}