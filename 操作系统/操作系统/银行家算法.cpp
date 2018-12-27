#include"stdafx.h"
#include<iostream>
#include<map>
#include<string>
#include<vector>
#include<string> 
#include<fstream>
#include<sstream>
using namespace std;

#if 1
map<string, int> my_map;//用来记录各进程名对应的数值，以方便访问各资源的总量

typedef struct Source {
	string name;//资源名称 
	int amount;//系统资源总量 
}source;

typedef struct list {
	string name;//进程名 
	vector<string> array;//将序列用一个string字符串表示，在使用的时候只要直接从string数组中提取然后减去48就行了 
}list;

typedef struct PCB {
	string name;
	int state;//进程的状态：当为0时表示，处于就绪状态，当为1时表示处于完成状态，当为-1时表示处于等待（阻塞）状态 
	vector<int> request_am;//当前进程的申请量
	vector<int> total_am;//资源需求的总量
	vector<int> allocation_am;//已占资源总量
	int finish;//能执行完的标志：当为0时表示还没有进行设置，当为1时表示能够执行完成，当为-1时，表示不能够执行完成 
}PCB;

class test {
private:
	int index;//表示上一次被分配的进程序号 
	vector<source> total_am;//系统资源总量
	vector<list>  sequence;//各进程各资源请求序列 
    //副本，用于备份恢复 
	vector<source> total_am_copy;
	vector<list> sequence_copy;
	vector<PCB> pcb_copy;
public:
	vector<PCB> pcb;//各进程的PCB序列 
	test();//初始化系统各资源总量的信息 
	void init();//输入各进程的请求序列 ，并对各进程的各资源最大的需求量进行初始化
	void init_pcb();//初始化各进程的PCB信息 
	string get_sourcelist(string source_name, string process_name){
		return sequence[my_map[process_name]].array[my_map[source_name]];
	}
	void bank_check();//银行家算法 
	void random_allocation();//随机算法
	bool if_all_finish();//在随机算法中使用，当就绪队列为空时，判断进程是否都已完成
	bool satisfy_request(int i);//i为被检验进程的编号,检验当前系统量是否满足进程i的申请需求 
	bool if_safe(int i);//对申请资源的进程i模拟分配资源，并判断分配是否安全 
	void restore();//将pcb和系统资源信息恢复到模拟分配之前的状态 
	void allocate(int i);//对进程i进行实际分配 
	bool if_finish(int i);//进程i是否已经得到全部的资源并完成 
	void show();//显示系统资源信息和pcb中各进程的信息
	void restore_wait();//从等待队列中将满足要求的进程恢复置就绪态
};

test::test() {
	ifstream file("source.txt");
	if (!file) {
		cerr << "打开文件错误" << endl;
		exit(1);
	}
	int i = 0;
	//从source文件中读取各系统资源重量的信息 
	while (!file.eof())
	{
		string str;
		getline(file, str);
		source p;
		p.name = str;
		my_map[str] = i++;
		string number;
		getline(file, number);
		stringstream ss;
		ss << number;
		ss >> p.amount;
		total_am.push_back(p);
	}
	file.close();
}

void test::init() {
	cout << "请输入测试序列文件号：1、sequence1.txt   2、sequence2.txt " << endl;
	int option;
	string name;
	cin >> option;
	if (option == 1) name = "sequence1.txt";
	else if (option == 2) name = "sequence2.txt";
	else {
		cerr << "输入错误" << endl;
		exit(1);
	}
	ifstream file(name);
	if (!file) {
		cerr << "打开文件错误" << endl;
		exit(1);
	}
	int i = 0;
	string str;
	getline(file, str);
	//cout<<str<<endl; 
	while (!file.eof()) {
		list l;
		l.name = str.substr(0, 2);//进程的名字，直接从sequence.txt文档中载入 
		my_map[l.name] = i++;
		PCB pcb_buff;
		pcb_buff.name = l.name;
		getline(file, str);
		//	cout<<str<<endl;
		do {
			string sour_name;//资源序列的名称 
			sour_name = str.substr(0, 1);
			string sour_list;//资源序列 
			sour_list = str.substr(1, str.size());
			//判断序列总和是否超过资源总量，超过即退出
			int sum = 0;
			for (int i = 0; i<sour_list.size(); i++) 	sum += sour_list[i] - 48;
			if (sum>total_am[my_map[sour_name]].amount) {
				cerr << l.name << "的" << sour_name << "资源的申请序列超过了系统资源的最大量，请重新设置文本" << endl;
				exit(1);
			}
			pcb_buff.total_am.push_back(sum);//初始化进程PCB中的资源需求总量 
			l.array.push_back(sour_list);
			if (file.eof()) break;//在调试的过程中发现，运行到最后一行时会出现文件已为空但还会继续进行访问的情况，所以在这加上判断条件。 
			getline(file, str);
		} while (str[str.size() - 1] != ':');
		sequence.push_back(l);
		pcb.push_back(pcb_buff);
	}
	file.close();
}

void test::init_pcb() {
	for (int i = 0; i<pcb.size(); i++) {
		pcb[i].state = 0;
		pcb[i].finish = 0;
		for (int j = 0; j<total_am.size(); j++) {
			pcb[i].allocation_am.push_back(0);
			string p = sequence[my_map[pcb[i].name]].array[my_map[total_am[j].name]];
			pcb[i].request_am.push_back(p[0] - 48);
			if (p.size() != 1)
				sequence[my_map[pcb[i].name]].array[my_map[total_am[j].name]] = p.substr(1, p.size());
			else sequence[my_map[pcb[i].name]].array[my_map[total_am[j].name]] = "";
		}
	}
}

void test::bank_check() {
	//计算pcb序列中从上一个被分配的进程开始的第一个处于就绪状态的进程的编号
	while (true) {
		int i = 0;
		while (i<pcb.size()) {
			if (pcb[i].state == 0) {
				if (i == index) i++;
				else break;
			}
			else i++;
		}
		if (i == pcb.size()) {
			i = 0;
			while (i<pcb.size()) {
				if (pcb[i].state == 0) break;
				else i++;
			}
		}
		if (i == pcb.size()) {
			cout << endl << "就绪队列中已无进程" << endl;
			break;
		}
		else {
			cout << "*****************************************" << endl;
			cout << endl << "被分配的进程名             分配状态" << endl;
			cout << "    " << pcb[i].name << "                    ";
			if (satisfy_request(i)) {
				index = i;
				//对信息进行备份
				pcb_copy = pcb;
				sequence_copy = sequence;
				total_am_copy = total_am;
				if (if_safe(i)) {
					cout << "可安全分配" << endl;
					allocate(i);
					if (if_finish(i)) {
						//归还全部资源，并将状态置为完成态,将request置0 
						for (int j = 0; j<total_am.size(); j++) {
							total_am[j].amount += pcb[i].allocation_am[j];
							pcb[i].request_am[j] = 0;
						}
						pcb[i].state = 1;
					}
					else {
						//更新进程i的request 
						for (int j = 0; j<total_am.size(); j++) {
							int amount = sequence[i].array[j][0] - 48;
							pcb[i].request_am[j] = amount;
							pcb[i].state = 0;
						}
					}
					show();
					restore_wait();//从等待进程中恢复满足条件的进程 
				}
				else {
					cout << "不能安全分配,进程设置为等待态" << endl;
					pcb[i].state = -1;
					show();
				}
			}
			else {
				cout << "系统各资源总量小于申请量，将" << pcb[i].name << "进程置为等待态" << endl;
				pcb[i].state = -1;
				show();
			}
		}
	}
}

void test::random_allocation() {
	//计算pcb序列中从上一个被分配的进程开始的第一个处于就绪状态的进程的编号
	while (true) {
		int i = 0;
		while (i<pcb.size()) {
			if (pcb[i].state == 0) {
				if (i == index) i++;//如果该进程上次执行过，则不应再次被选中
				else break;
			}
			else i++;
		}

		//解决当就绪队列中只存在一个进程，但上次分配过而不会在分配问题
		if (i == pcb.size()) {
			i = 0;
			while (i<pcb.size()) {
				if (pcb[i].state == 0) break;
				else i++;
			}
		}

		if (i == pcb.size()) {
			if (if_all_finish()) cout << endl << "未出现死锁，能正常执行" << endl;
			else cout << endl << "出现死锁" << endl;
			break;
		}
		else {
			cout << "*****************************************" << endl;
			cout << endl << "被分配的进程名             分配状态" << endl;
			cout << "    " << pcb[i].name << "                    ";
			if (satisfy_request(i)) {
				index = i;//记录这次被分配的进程，下一次应选其他进程
				cout << "成功分配" << endl;
				allocate(i);//满足该进程的当前申请
				if (if_finish(i)) {
					//归还全部资源，并将状态置为完成态,将request置0 
					for (int j = 0; j<total_am.size(); j++) {
						total_am[j].amount += pcb[i].allocation_am[j];
						pcb[i].request_am[j] = 0;
					}
					pcb[i].state = 1;
					restore_wait();//从等待进程中恢复满足条件的进程 
				}
				else {
					//更新进程i的request 
					for (int j = 0; j<total_am.size(); j++) {
						int amount = sequence[i].array[j][0] - 48;
						pcb[i].request_am[j] = amount;
						pcb[i].state = 0;
					}
				}
				show();
			}
			else {
				cout << "系统各资源总量小于申请量，将" << pcb[i].name << "进程置为等待态" << endl;
				pcb[i].state = -1;
				show();
			}
		}
	}
}

bool test::if_all_finish() {
	for (int i = 0; i < pcb.size(); i++)
		if (pcb[i].state != 1) return false;
	return true;
}

bool test::satisfy_request(int i) {
	bool satify = true;
	for (int j = 0; j<total_am.size() && satify; j++) {
		if (pcb[i].request_am[j]>total_am[j].amount) satify = false;
	}
	return satify;
}

bool test::if_safe(int i) {
	//对系统各资源进行更新操作
	for (int j = 0; j<total_am.size(); j++) {
		total_am[j].amount -= pcb[i].request_am[j];
		pcb[i].allocation_am[j] += pcb[i].request_am[j];
	}
	//寻找一个系统能满足其需求的进程
	while (true) {
		bool flag0 = false;//表示不能找到 
		int process;
		for (int p = 0; p<pcb.size() && !flag0; p++) {
			if (p == pcb.size()) break;
			while (pcb[p].finish == 1) {
				p++;
				if (p == pcb.size()) break;
			}
			if (p == pcb.size()) break;
			bool flag1 = true;//p进程是否能被满足 
			for (int pp = 0; pp<total_am.size() && flag1; pp++)
				if ((pcb[p].total_am[pp] - pcb[p].allocation_am[pp])>total_am[pp].amount) flag1 = false;

			if (flag1) {
				process = p;
				flag0 = flag1;
			}
		}
		if (flag0) {
			pcb[process].finish = 1;
			for (int j = 0; j<total_am.size(); j++)
				total_am[j].amount += pcb[process].allocation_am[j];
		}
		else break;
	}
	bool safe = true;
	for (int j = 0; j<pcb.size() && safe; j++)
	{
		if (j == i) j++;
		if (j == pcb.size()) break;
		if (pcb[j].finish == 0) safe = false;
	}
	restore();//进行恢复，恢复至模拟分配之前的状态 
	return safe;
}

void test::restore() {
	sequence = sequence_copy;
	pcb = pcb_copy;
	total_am = total_am_copy;
}

void  test::allocate(int i) {
	for (int j = 0; j<total_am.size(); j++)
	{
		total_am[j].amount -= pcb[i].request_am[j];
		pcb[i].allocation_am[j] += pcb[i].request_am[j];
	}
}

bool test::if_finish(int i) {
	bool finish = true;
	for (int j = 0; j<total_am.size(); j++) {
		if (pcb[i].total_am != pcb[i].allocation_am) finish = false;
	}
	return finish;
}

void test::show() {
	//	cout<<"系统资源剩余量："<<endl;
	cout << endl << "资源名             剩余量 " << endl;
	for (int j = 0; j<total_am.size(); j++)
	{
		cout << total_am[j].name << "                     " << total_am[j].amount << endl;
	}
	//	cout<<"各进程PCB 信息："<<endl;
	cout << endl << "进程名" << "     " << "已获得的资源" << "   " << "当前申请量" << "   " << "还需资源     进程状态" << endl;

	for (int i = 0; i<pcb.size(); i++) {
		cout << pcb[i].name << "          ";
		for (int j = 0; j<total_am.size(); j++)
			cout << pcb[i].allocation_am[j] << " ";
		cout << "          ";
		for (int j = 0; j<total_am.size(); j++)
			cout << pcb[i].request_am[j] << " ";
		cout << "       ";
		for (int j = 0; j<total_am.size(); j++)
			cout << pcb[i].total_am[j] - pcb[i].allocation_am[j] << " ";
		switch (pcb[i].state) {
		case 0:cout << "      就绪" << endl; break;
		case 1:cout << "      完成" << endl; break;
		case -1:cout << "      等待" << endl; break;
		}
	}
	cout << "*****************************************" << endl;
}

void test::restore_wait() {
	int i = 0;
	while (i<pcb.size()) {
		if (pcb[i].state == -1){
			bool flag = true;
			for (int j = 0; j<total_am.size() && flag; j++) {
				if (pcb[i].request_am[j]>total_am[j].amount) flag = false;
			}
			if (flag) { pcb[i].state = 0; 
			cout << endl << "进程P" << i << "满足资源要求，置为就绪态" << endl;
			}
		}
		i++;
	}
}

int main() {
	test t;
	//	for(int i=0;i<t.total_am.size();i++) cout<<t.total_am[i].amount<<endl;
	t.init();//从文件中写入各进程的各资源的申请序列 
			 /*以下为测试各进程的各资源申请序列是否写入成功，经下面的代码测试，写入成功。*/
			 /*
			 cout<<"能执行到这"<<endl;
			 string process;
			 string sour;
			 process="P1";
			 sour="B";
			 cout<<t.get_sourcelist(sour,process)<<endl;//在对序列进行外赋值的时候出现错误
			 */
			 /*以下为测试PCB是否初始化成功(初始化成功)*/
			 /*
			 t.init_pcb();
			 for(int i=0;i<t.pcb.size();i++)
			 {
			 cout<<t.pcb[i].name<<":"<<t.pcb[i].total_am[1]<<endl;
			 }
			 */
	t.init_pcb();
	int option;
	cout << "1、银行家算法      2、随机算法" << endl;
	cin >> option;
	cout << endl << "初始状态:" << endl;
	t.show();
	if (option == 1){
		t.bank_check();
	}
	else if(option==2){
		t.random_allocation();
	}
	else {
		cerr << "输入错误" << endl;
		exit(1);
	}
	return 0;
}
#endif