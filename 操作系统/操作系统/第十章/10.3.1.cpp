#include"stdafx.h"
#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<sstream>
using namespace std;

typedef struct Table {
	int table_num;//ҳ��
	int flag;//��־
	int lump_num;//������
	int out_add;//����ַ
	int fix_flag;//�޸�λ
}Table;

typedef struct Resident {
	int lump_num;//���
	int page_num;//ҳ��
};

class Add {//��ַ�任
private:
	vector<Table> page_table;
	vector<Resident> resident_set;//פ��������СΪ4
	string order;//ָ��(����Ϊ9)
	int page_num;//����ָ��õ���ҳ��
	int page_offset;//ҳ��ƫ����
	int abs_add;//���Ե�ַ
public:
	void show_page();//���ҳ��
	void show_resident();//���פ����
	void init();//��ҳ����г�ʼ������
	void test();//���Ժ���
	void get_pagenum();//�õ�ҳ��
	void get_pageoffset();//�õ�ҳ��ƫ����
	bool if_exist();//�ж��Ƿ��ҳ��־�Ƿ�Ϊ1
	void interrupt();//�����жϴ���
	void get_absadd();//�õ����Ե�ַ
};

void Add::show_page() {
	cout << "ҳ��         ��־         ������       ����ַ     �޸�λ" << endl;
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
	cout << "ҳ��       ������" << endl;
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
	get_pageoffset();//�õ�ҳ��ƫ�Ƶ�ַ
	int lump_num = page_table[page_num].lump_num;
	abs_add = lump_num * 64 + page_offset;
	cout << "���Ե�ַΪ:" << abs_add << endl;
}

void Add::interrupt() {
	if (resident_set.size() <4) {//�������ҵ��û�з���4���ڴ��
		Resident r;
		r.page_num = page_num;
		int buff[4] = { 5,8,9,10 };
		int i = 0;
		for (; i < 4; i++) {
			int j = 0;
			for (; j < resident_set.size(); j++) {
				if (buff[i] == resident_set[j].lump_num) break;
			}
			if (j == resident_set.size()) {//��ʾbuff[i]����פ������
				break;
			}
		}
		r.lump_num = buff[i];
		resident_set.insert(resident_set.begin(), r);
		cout << "����ҳ" << page_num << endl;
		page_table[resident_set[0].page_num].flag = 1;//�޸�ҳ��־
		page_table[resident_set[0].page_num].lump_num = r.lump_num;//��ӿ��

	}
	else {
		Resident r;
		r.lump_num= resident_set[3].lump_num;
		r.page_num = page_num;
		page_table[resident_set[3].page_num].flag = 0; //���ÿ�ԭ����Ӧ��ҳ��־��Ϊ0����ʾ���û����ڴ�
		cout << "��ҳ" << resident_set[3].page_num << "�����ڴ�" << endl;
		page_table[resident_set[3].page_num].lump_num = 0;
		page_table[resident_set[3].page_num].fix_flag = 0; //���ÿ�ԭ����Ӧ���޸�λ��־��Ϊ0����ʾ���û����ڴ�

		resident_set.erase(resident_set.begin() + 3);//��ȥפ�����б�������ҳ��Ϣ

		if (page_table[resident_set[3].page_num].fix_flag) {
			cout << "��ҳ" << resident_set[3].page_num << "������" << endl;
		}
		cout << "����ҳ" << page_num << endl;

		resident_set.insert(resident_set.begin(),r);//����ȱҳ����
		page_table[resident_set[0].page_num].flag = 1;//����ȱҳ��־��Ϊ1
		page_table[resident_set[0].page_num].lump_num = resident_set[0].lump_num;
	}
}

bool Add::if_exist() {
	if (page_table[page_num].flag == 1) {//������ʹ���������פ�����е�λ���ö�
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
		cerr << "���ļ�ʧ��" << endl;
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
		cout << "������ָ��" << endl;
		cin >> order;
		get_pagenum();
		if (!if_exist()) {
			cout << "��ҳ���������У�����ȱҳ�ж�" << endl;
			interrupt();
		}
			get_absadd();
			cout << "�Ƿ���\"��\"ָ��(1����   2����)" << endl;
			int save;
			cin >> save;
			if (save == 1) {
				page_table[page_num].fix_flag = 1;
			}
		int flag;
		cout <<endl<< "�Ƿ���ʾҳ��������(1����     2����)" << endl;
		cin >> flag;
		if (flag == 1) show_page();
		cout << endl << "�Ƿ���ʾפ�����е����ݣ�1����  2����" << endl;
		cin >> flag;
		if (flag == 1) show_resident();
		cout << "���к��ָ����?(1����    2����)" << endl;
		cin >> flag;
		if (flag == 2) break;
	}
}

int main() {
	Add a;
	a.test();
	
	return 0;
}