#include"stdafx.h"
#include <map>
#include <iostream>
#include <string>
using namespace std;

#if 0
typedef struct PCB {
	string name;//������ 
	struct PCB *next;//��һ��PCB���׵�ַ 
	int arrive_time;//����ʱ�� 
	int run_time;//����ִ��ʱ�� 
	char status;//����״̬ ��ΪCʱ��ʾ�Ѿ�ִ����ϣ���ΪRʱ��ʾ����ִ�� 
}PCB;

map<string, int> my_map;//�����洢�������ؼ��ֶ�Ӧ����ֵ 
bool if_finsh = false;//�����Ƿ�ȫ����� 

					  //�����ж��Ƿ�Ҫ���������µĽ��� 
int if_continue() {
	cout << "��ǰ��������Ϊ�գ��Ƿ��������µĽ��̣�Y/N��?" << endl;
	char f;
	cin >> f;
	if (f == 'Y') return 1;
	else return 0;
}

PCB *begin = NULL;
PCB *end = NULL;
PCB *current;
int n;//������ 
int length_time;//ʱ��Ƭ���� 
int current_time;//��ǰʱ��

				 //�ҵ���ǰ������һ����� 
PCB* find_prev(PCB *p) {
	if (p != begin) {
		PCB *q = begin;
		while (q->next != p) q = q->next;
		return q;
	}
	else return end;
}

//�����µĽ���,remain��������������̻�ʣ��ʱ�䣬run_time������������Ѿ�ִ�е�ʱ�� 
void inputprocess(int remain[], int run_time[]) {
	for (int i = 0; i<n; i++) {
		PCB*p = new PCB();
		cout << "��������";
		cin >> p->name;
		my_map[p->name] = i;
		run_time[my_map[p->name]] = 0;

		cout << "����ʱ��:";
		cin >> p->arrive_time;
		cout << "��������ʱ�䣺";
		cin >> p->run_time;
		remain[my_map[p->name]] = p->run_time;
		p->status = 'R';
		if (begin == NULL) {
			begin = p;
			end = p;
			p->next = begin;
		}
		else {
			int flag = 1;
			PCB*q = begin;
			//Ѱ�Ҳ���λ�� 
			while (q->arrive_time<p->arrive_time) {
				q = q->next;
				if (q == end) break;
			}

			//��q������������п��ǣ�ͷ��β���� 
			if (q == begin) {
				if (q->arrive_time>p->arrive_time) {//����֮ǰ 
					p->next = q;
					begin = p;
					end->next = begin;
				}
				else {//����֮�� 
					p->next = q->next;
					q->next = p;
					end = p;
				}
			}
			else if (q == end) {
				if (q->arrive_time>p->arrive_time) {//����֮ǰ 
					PCB*prev = find_prev(end);
					prev->next = p;
					p->next = end;
				}
				else {//����֮�� 
					q->next = p;
					p->next = begin;
					end = p;
				}
			}
			else {
				PCB*prev = find_prev(q);
				p->next = prev->next;
				prev->next = p;
			}
		}
	}
	current = begin;
}

void runprocess(int remain[], int run_time[], int cycling_time[]) {
	//�����ǰ����Ϊ���̬����������н��̶��Ѿ���ɣ���ֱ���˳� 
	if (current->status == 'C') {
		if_finsh = true;
		return;
	}
	int runedtime = 0;//��¼��ǰ������ִ��ʱ�� 
					  /*��Ϊ��ǰ��ʱ����ܻ�С�ڵ�ǰ���̵ĵ���ʱ�䣬��ʱ��Ҫɨ�������������û����
					  ����Ľ��̣����������У�����������ĩβ����û�У��򽫵�ǰʱ�����++����*/
	int flag = 0;//���������ҵ�����
	int i;
	while (!flag) {
		if (current_time<current->arrive_time) {
			current = begin;
			if (current->arrive_time<current_time)  flag = 1;
		}
		else flag = 1;
		current_time++;
	}
	current_time -= 1;


	if (remain[my_map[current->name]]>length_time) {
		remain[my_map[current->name]] -= length_time;
		run_time[my_map[current->name]] += length_time;
		current_time += length_time;
		cout << "��ǰʱ��Ϊ:" << "     " << "�������еĽ�����" << "     " << "������ʱ��" << "    " << "��ʣʱ��" << "      " << "�����л�ʣ����" << endl;
		cout << current_time << "                 " << current->name << "                  " << run_time[my_map[current->name]] << "              " << remain[my_map[current->name]];
		PCB*p = current->next;
		cout << "          ";
		if (p != begin) {//����Ϊ�� 
			while (p != end) {
				cout << p->name << " ";
				p = p->next;
			}
			cout << p->name << endl;
		}
		else cout << "��";
		cout << endl;
	}
	else {
		current_time += remain[my_map[current->name]];
		current->status = 'C';
		run_time[my_map[current->name]] += remain[my_map[current->name]];
		remain[my_map[current->name]] = 0;
		cout << "��ǰʱ��Ϊ:" << "     " << "�������еĽ�����" << "     " << "������ʱ��" << "    " << "��ʣʱ��" << "      " << "�����л�ʣ����" << endl;
		cout << current_time << "                 " << current->name << "                  " << run_time[my_map[current->name]] << "              " << remain[my_map[current->name]];

		cout << "            ";
		PCB*p = current->next;
		if (p != begin) {//����������в�Ϊ�� 
			while (p != end) {
				cout << p->name << " ";
				p = p->next;
			}
			cout << p->name << endl;
		}
		else cout << "��";
		cout << endl;
		cout << "********************************************************" << endl;
		cout << "����" << current->name << "������ϣ��ӽ��̶����е���" << endl;
		cout << "********************************************************" << endl;

		cycling_time[my_map[current->name]] = current_time - current->arrive_time;//��������̵���תʱ�� 
		current->status = 'C';
		current = current->next;
	}
}

//��ʾ����֮��̸����̵ĳ�ʼ��Ϣ
void show_information(PCB *p) {
	cout << "****************************************" << endl;
	cout << "          �����̵Ļ�����Ϣ����          " << endl;
	cout << "****************************************" << endl;
	cout << "��������" << "     " << "���̵���ʱ��" << "     " << "���̹���ִ��ʱ��" << endl;
	for (int i = 0; i<n; i++) {
		cout << p->name << "                 ";
		cout << p->arrive_time << "                  ";
		cout << p->run_time;
		cout << endl;
		p = p->next;
	}
}

int main() {
	while (true) {
		if (if_continue()) {
			//	int n;
			current_time = 0;
			cout << "������Ҫ�����Ľ�����:" << endl;
			cin >> n;
			length_time = 5;
			int remain[n];
			int run_time[n];
			int cycling_time[n];//������תʱ�� 

								//			PCB pcb[n];//��Ϊ�ֲ�����գ�������Ҫ���������ж��� 
			inputprocess(remain, run_time);
			PCB *p = begin;
			show_information(p);
			cout << "ִ������ʱ��Ϊ:" << length_time << endl;
			while (!(if_finsh)) runprocess(remain, run_time, cycling_time);
			cout << "������    " << "���̵���תʱ��Ϊ:" << endl;
			PCB*q = begin;
			do {
				cout << q->name << "           " << cycling_time[my_map[q->name]] << endl;
				q = q->next;
			} while (q != begin);
			int time = 0;
			for (int i = 0; i<n; i++) time += cycling_time[i];
			cout << "ƽ����תʱ��Ϊ:" << time / n << endl;
		}//if
		else exit(1);
	}
	return 0;
}

#endif