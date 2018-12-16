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

typedef struct Resident {
	int lump_num;//块号
	int page_num;//页号
};

class Add {//地址变换
private:
	vector<Table> page_table;
	vector<Resident> resident_set;//驻留集，大小为4
	string order;//指令(长度为9)
	int page_num;//根据指令得到的页号
	int page_offset;//页内偏移量
	int abs_add;//绝对地址
public:
	void show_page();//输出页表
	void show_resident();//输出驻留集
	void init();//对页表进行初始化操作
	void test();//测试函数
	void get_pagenum();//得到页号
	void get_pageoffset();//得到页内偏移量
	bool if_exist();//判断是否该页标志是否为1
	void interrupt();//进行中断处理
	void get_absadd();//得到绝对地址
};

void Add::show_page() {
	cout << "页号         标志         主存块号       外存地址     修改位" << endl;
	for (int i = 0; i < page_table.size(); i++) {
		cout.setf(ios::left);
		cout.width(14);
		cout <<i;
		cout.setf(ios::left);
		cout.width(14);
		cout << page_table[i].flag;
		cout.setf(ios::left);
		cout.width(14);
		cout << page_table[i].lump_num ;
		cout.setf(ios::left);
		cout.width(14);
		cout << page_table[i].out_add;
		cout.setf(ios::left);
		cout.width(14);
		cout << page_table[i].fix_flag;
		cout << endl;
	}
}

void Add::show_resident() {
	cout << "页号       主存块号" << endl;
	for (int i = 0; i < resident_set.size(); i++) {
		cout.setf(ios::left);
		cout.width(14);
		cout << resident_set[i].page_num;
		cout.setf(ios::left);
		cout.width(14);
		cout << resident_set[i].lump_num;
		cout << endl;
	}
}

void Add::get_absadd() {
	get_pageoffset();//得到页内偏移地址
	int lump_num = page_table[page_num].lump_num;
	abs_add = lump_num * 64 + page_offset;
	cout << "绝对地址为:" << abs_add << endl;
}

void Add::interrupt() {
	if (resident_set.size() <4) {//如果该作业还没有分配4个内存块
		Resident r;
		r.page_num = page_num;
		int buff[4] = { 5,8,9,10 };
		int i = 0;
		for (; i < 4; i++) {
			int j = 0;
			for (; j < resident_set.size(); j++) {
				if (buff[i] == resident_set[j].lump_num) break;
			}
			if (j == resident_set.size()) {//表示buff[i]不在驻留集中
				break;
			}
		}
		r.lump_num = buff[i];
		resident_set.insert(resident_set.begin(), r);
		cout << "调进页" << page_num << endl;
		page_table[resident_set[0].page_num].flag = 1;//修改页标志
		page_table[resident_set[0].page_num].lump_num = r.lump_num;//添加块号

	}
	else {
		Resident r;
		r.lump_num= resident_set[3].lump_num;
		r.page_num = page_num;
		page_table[resident_set[3].page_num].flag = 0; //将该块原来对应的页标志置为0，表示被置换出内存
		cout << "将页" << resident_set[3].page_num << "调出内存" << endl;
		page_table[resident_set[3].page_num].lump_num = 0;
		page_table[resident_set[3].page_num].fix_flag = 0; //将该块原来对应的修改位标志置为0，表示被置换出内存

		resident_set.erase(resident_set.begin() + 3);//除去驻留集中被调出的页信息

		if (page_table[resident_set[3].page_num].fix_flag) {
			cout << "调页" << resident_set[3].page_num << "到磁盘" << endl;
		}
		cout << "调进页" << page_num << endl;

		resident_set.insert(resident_set.begin(),r);//将所缺页换入
		page_table[resident_set[0].page_num].flag = 1;//将所缺页标志置为1
		page_table[resident_set[0].page_num].lump_num = resident_set[0].lump_num;
	}
}

bool Add::if_exist() {
	if (page_table[page_num].flag == 1) {//最近访问过，将其在驻留集中的位置置顶
		int i = 0;
		for (; i < resident_set.size(); i++)
			if (resident_set[i].page_num == page_num) break;
		Resident r;
		r.lump_num = resident_set[i].lump_num;
		r.page_num = resident_set[i].page_num;
		resident_set.erase(resident_set.begin() + i);
		resident_set.insert(resident_set.begin(), r);
		return true;
	}
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
			cout << "该页不在主存中，产生缺页中断" << endl;
			interrupt();
		}
			get_absadd();
			cout << "是否是\"存\"指令(1、是   2、否)" << endl;
			int save;
			cin >> save;
			if (save == 1) {
				page_table[page_num].fix_flag = 1;
			}
		int flag;
		cout <<endl<< "是否显示页表中内容(1、是     2、否)" << endl;
		cin >> flag;
		if (flag == 1) show_page();
		cout << endl << "是否显示驻留集中的内容（1、是  2、否）" << endl;
		cin >> flag;
		if (flag == 1) show_resident();
		cout << "还有后继指令吗?(1、有    2、无)" << endl;
		cin >> flag;
		if (flag == 2) break;
	}
}

int main() {
	Add a;
	a.test();
	
	return 0;
}