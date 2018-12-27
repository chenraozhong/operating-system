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
map<string, int> my_map;//������¼����������Ӧ����ֵ���Է�����ʸ���Դ������

typedef struct Source {
	string name;//��Դ���� 
	int amount;//ϵͳ��Դ���� 
}source;

typedef struct list {
	string name;//������ 
	vector<string> array;//��������һ��string�ַ�����ʾ����ʹ�õ�ʱ��ֻҪֱ�Ӵ�string��������ȡȻ���ȥ48������ 
}list;

typedef struct PCB {
	string name;
	int state;//���̵�״̬����Ϊ0ʱ��ʾ�����ھ���״̬����Ϊ1ʱ��ʾ�������״̬����Ϊ-1ʱ��ʾ���ڵȴ���������״̬ 
	vector<int> request_am;//��ǰ���̵�������
	vector<int> total_am;//��Դ���������
	vector<int> allocation_am;//��ռ��Դ����
	int finish;//��ִ����ı�־����Ϊ0ʱ��ʾ��û�н������ã���Ϊ1ʱ��ʾ�ܹ�ִ����ɣ���Ϊ-1ʱ����ʾ���ܹ�ִ����� 
}PCB;

class test {
private:
	int index;//��ʾ��һ�α�����Ľ������ 
	vector<source> total_am;//ϵͳ��Դ����
	vector<list>  sequence;//�����̸���Դ�������� 
    //���������ڱ��ݻָ� 
	vector<source> total_am_copy;
	vector<list> sequence_copy;
	vector<PCB> pcb_copy;
public:
	vector<PCB> pcb;//�����̵�PCB���� 
	test();//��ʼ��ϵͳ����Դ��������Ϣ 
	void init();//��������̵��������� �����Ը����̵ĸ���Դ�������������г�ʼ��
	void init_pcb();//��ʼ�������̵�PCB��Ϣ 
	string get_sourcelist(string source_name, string process_name){
		return sequence[my_map[process_name]].array[my_map[source_name]];
	}
	void bank_check();//���м��㷨 
	void random_allocation();//����㷨
	bool if_all_finish();//������㷨��ʹ�ã�����������Ϊ��ʱ���жϽ����Ƿ������
	bool satisfy_request(int i);//iΪ��������̵ı��,���鵱ǰϵͳ���Ƿ��������i���������� 
	bool if_safe(int i);//��������Դ�Ľ���iģ�������Դ�����жϷ����Ƿ�ȫ 
	void restore();//��pcb��ϵͳ��Դ��Ϣ�ָ���ģ�����֮ǰ��״̬ 
	void allocate(int i);//�Խ���i����ʵ�ʷ��� 
	bool if_finish(int i);//����i�Ƿ��Ѿ��õ�ȫ������Դ����� 
	void show();//��ʾϵͳ��Դ��Ϣ��pcb�и����̵���Ϣ
	void restore_wait();//�ӵȴ������н�����Ҫ��Ľ��ָ̻��þ���̬
};

test::test() {
	ifstream file("source.txt");
	if (!file) {
		cerr << "���ļ�����" << endl;
		exit(1);
	}
	int i = 0;
	//��source�ļ��ж�ȡ��ϵͳ��Դ��������Ϣ 
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
	cout << "��������������ļ��ţ�1��sequence1.txt   2��sequence2.txt " << endl;
	int option;
	string name;
	cin >> option;
	if (option == 1) name = "sequence1.txt";
	else if (option == 2) name = "sequence2.txt";
	else {
		cerr << "�������" << endl;
		exit(1);
	}
	ifstream file(name);
	if (!file) {
		cerr << "���ļ�����" << endl;
		exit(1);
	}
	int i = 0;
	string str;
	getline(file, str);
	//cout<<str<<endl; 
	while (!file.eof()) {
		list l;
		l.name = str.substr(0, 2);//���̵����֣�ֱ�Ӵ�sequence.txt�ĵ������� 
		my_map[l.name] = i++;
		PCB pcb_buff;
		pcb_buff.name = l.name;
		getline(file, str);
		//	cout<<str<<endl;
		do {
			string sour_name;//��Դ���е����� 
			sour_name = str.substr(0, 1);
			string sour_list;//��Դ���� 
			sour_list = str.substr(1, str.size());
			//�ж������ܺ��Ƿ񳬹���Դ�������������˳�
			int sum = 0;
			for (int i = 0; i<sour_list.size(); i++) 	sum += sour_list[i] - 48;
			if (sum>total_am[my_map[sour_name]].amount) {
				cerr << l.name << "��" << sour_name << "��Դ���������г�����ϵͳ��Դ��������������������ı�" << endl;
				exit(1);
			}
			pcb_buff.total_am.push_back(sum);//��ʼ������PCB�е���Դ�������� 
			l.array.push_back(sour_list);
			if (file.eof()) break;//�ڵ��ԵĹ����з��֣����е����һ��ʱ������ļ���Ϊ�յ�����������з��ʵ������������������ж������� 
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
	//����pcb�����д���һ��������Ľ��̿�ʼ�ĵ�һ�����ھ���״̬�Ľ��̵ı��
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
			cout << endl << "�������������޽���" << endl;
			break;
		}
		else {
			cout << "*****************************************" << endl;
			cout << endl << "������Ľ�����             ����״̬" << endl;
			cout << "    " << pcb[i].name << "                    ";
			if (satisfy_request(i)) {
				index = i;
				//����Ϣ���б���
				pcb_copy = pcb;
				sequence_copy = sequence;
				total_am_copy = total_am;
				if (if_safe(i)) {
					cout << "�ɰ�ȫ����" << endl;
					allocate(i);
					if (if_finish(i)) {
						//�黹ȫ����Դ������״̬��Ϊ���̬,��request��0 
						for (int j = 0; j<total_am.size(); j++) {
							total_am[j].amount += pcb[i].allocation_am[j];
							pcb[i].request_am[j] = 0;
						}
						pcb[i].state = 1;
					}
					else {
						//���½���i��request 
						for (int j = 0; j<total_am.size(); j++) {
							int amount = sequence[i].array[j][0] - 48;
							pcb[i].request_am[j] = amount;
							pcb[i].state = 0;
						}
					}
					show();
					restore_wait();//�ӵȴ������лָ����������Ľ��� 
				}
				else {
					cout << "���ܰ�ȫ����,��������Ϊ�ȴ�̬" << endl;
					pcb[i].state = -1;
					show();
				}
			}
			else {
				cout << "ϵͳ����Դ����С������������" << pcb[i].name << "������Ϊ�ȴ�̬" << endl;
				pcb[i].state = -1;
				show();
			}
		}
	}
}

void test::random_allocation() {
	//����pcb�����д���һ��������Ľ��̿�ʼ�ĵ�һ�����ھ���״̬�Ľ��̵ı��
	while (true) {
		int i = 0;
		while (i<pcb.size()) {
			if (pcb[i].state == 0) {
				if (i == index) i++;//����ý����ϴ�ִ�й�����Ӧ�ٴα�ѡ��
				else break;
			}
			else i++;
		}

		//���������������ֻ����һ�����̣����ϴη�����������ڷ�������
		if (i == pcb.size()) {
			i = 0;
			while (i<pcb.size()) {
				if (pcb[i].state == 0) break;
				else i++;
			}
		}

		if (i == pcb.size()) {
			if (if_all_finish()) cout << endl << "δ����������������ִ��" << endl;
			else cout << endl << "��������" << endl;
			break;
		}
		else {
			cout << "*****************************************" << endl;
			cout << endl << "������Ľ�����             ����״̬" << endl;
			cout << "    " << pcb[i].name << "                    ";
			if (satisfy_request(i)) {
				index = i;//��¼��α�����Ľ��̣���һ��Ӧѡ��������
				cout << "�ɹ�����" << endl;
				allocate(i);//����ý��̵ĵ�ǰ����
				if (if_finish(i)) {
					//�黹ȫ����Դ������״̬��Ϊ���̬,��request��0 
					for (int j = 0; j<total_am.size(); j++) {
						total_am[j].amount += pcb[i].allocation_am[j];
						pcb[i].request_am[j] = 0;
					}
					pcb[i].state = 1;
					restore_wait();//�ӵȴ������лָ����������Ľ��� 
				}
				else {
					//���½���i��request 
					for (int j = 0; j<total_am.size(); j++) {
						int amount = sequence[i].array[j][0] - 48;
						pcb[i].request_am[j] = amount;
						pcb[i].state = 0;
					}
				}
				show();
			}
			else {
				cout << "ϵͳ����Դ����С������������" << pcb[i].name << "������Ϊ�ȴ�̬" << endl;
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
	//��ϵͳ����Դ���и��²���
	for (int j = 0; j<total_am.size(); j++) {
		total_am[j].amount -= pcb[i].request_am[j];
		pcb[i].allocation_am[j] += pcb[i].request_am[j];
	}
	//Ѱ��һ��ϵͳ������������Ľ���
	while (true) {
		bool flag0 = false;//��ʾ�����ҵ� 
		int process;
		for (int p = 0; p<pcb.size() && !flag0; p++) {
			if (p == pcb.size()) break;
			while (pcb[p].finish == 1) {
				p++;
				if (p == pcb.size()) break;
			}
			if (p == pcb.size()) break;
			bool flag1 = true;//p�����Ƿ��ܱ����� 
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
	restore();//���лָ����ָ���ģ�����֮ǰ��״̬ 
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
	//	cout<<"ϵͳ��Դʣ������"<<endl;
	cout << endl << "��Դ��             ʣ���� " << endl;
	for (int j = 0; j<total_am.size(); j++)
	{
		cout << total_am[j].name << "                     " << total_am[j].amount << endl;
	}
	//	cout<<"������PCB ��Ϣ��"<<endl;
	cout << endl << "������" << "     " << "�ѻ�õ���Դ" << "   " << "��ǰ������" << "   " << "������Դ     ����״̬" << endl;

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
		case 0:cout << "      ����" << endl; break;
		case 1:cout << "      ���" << endl; break;
		case -1:cout << "      �ȴ�" << endl; break;
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
			cout << endl << "����P" << i << "������ԴҪ����Ϊ����̬" << endl;
			}
		}
		i++;
	}
}

int main() {
	test t;
	//	for(int i=0;i<t.total_am.size();i++) cout<<t.total_am[i].amount<<endl;
	t.init();//���ļ���д������̵ĸ���Դ���������� 
			 /*����Ϊ���Ը����̵ĸ���Դ���������Ƿ�д��ɹ���������Ĵ�����ԣ�д��ɹ���*/
			 /*
			 cout<<"��ִ�е���"<<endl;
			 string process;
			 string sour;
			 process="P1";
			 sour="B";
			 cout<<t.get_sourcelist(sour,process)<<endl;//�ڶ����н����⸳ֵ��ʱ����ִ���
			 */
			 /*����Ϊ����PCB�Ƿ��ʼ���ɹ�(��ʼ���ɹ�)*/
			 /*
			 t.init_pcb();
			 for(int i=0;i<t.pcb.size();i++)
			 {
			 cout<<t.pcb[i].name<<":"<<t.pcb[i].total_am[1]<<endl;
			 }
			 */
	t.init_pcb();
	int option;
	cout << "1�����м��㷨      2������㷨" << endl;
	cin >> option;
	cout << endl << "��ʼ״̬:" << endl;
	t.show();
	if (option == 1){
		t.bank_check();
	}
	else if(option==2){
		t.random_allocation();
	}
	else {
		cerr << "�������" << endl;
		exit(1);
	}
	return 0;
}
#endif