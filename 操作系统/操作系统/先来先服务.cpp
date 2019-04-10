#include"stdafx.h"
#include <map>
#include <iostream>
#include <string>
using namespace std;

#if 0
typedef struct PCB {
	string name;//进程名 
	struct PCB *next;//下一个PCB的首地址 
	int arrive_time;//到达时间 
	int run_time;//估计执行时间 
	char status;//进程状态 当为C时表示已经执行完毕，当为R时表示还在执行 
}PCB;

map<string, int> my_map;//用来存储进程名关键字对应的数值 
bool if_finsh = false;//进程是否全部完成 

					  //用于判断是否要继续建立新的进程 
int if_continue() {
	cout << "当前就绪队列为空，是否建立建立新的进程（Y/N）?" << endl;
	char f;
	cin >> f;
	if (f == 'Y') return 1;
	else return 0;
}

PCB *begin = NULL;
PCB *end = NULL;
PCB *current;
int n;//进程数 
int length_time;//时间片长度 
int current_time;//当前时间

				 //找到当前结点的上一个结点 
PCB* find_prev(PCB *p) {
	if (p != begin) {
		PCB *q = begin;
		while (q->next != p) q = q->next;
		return q;
	}
	else return end;
}

//建立新的进程,remain数组用来保存进程还剩的时间，run_time用来保存进程已经执行的时间 
void inputprocess(int remain[], int run_time[]) {
	for (int i = 0; i<n; i++) {
		PCB*p = new PCB();
		cout << "进程名：";
		cin >> p->name;
		my_map[p->name] = i;
		run_time[my_map[p->name]] = 0;

		cout << "到达时间:";
		cin >> p->arrive_time;
		cout << "估计运行时间：";
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
			//寻找插入位置 
			while (q->arrive_time<p->arrive_time) {
				q = q->next;
				if (q == end) break;
			}

			//对q的三种情况进行考虑：头、尾、中 
			if (q == begin) {
				if (q->arrive_time>p->arrive_time) {//插在之前 
					p->next = q;
					begin = p;
					end->next = begin;
				}
				else {//插在之后 
					p->next = q->next;
					q->next = p;
					end = p;
				}
			}
			else if (q == end) {
				if (q->arrive_time>p->arrive_time) {//插在之前 
					PCB*prev = find_prev(end);
					prev->next = p;
					p->next = end;
				}
				else {//插在之后 
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
	//如果当前进程为完成态，则表明所有进程都已经完成，则直接退出 
	if (current->status == 'C') {
		if_finsh = true;
		return;
	}
	int runedtime = 0;//记录当前程序已执行时间 
					  /*因为当前的时间可能会小于当前进程的到达时间，此时需要扫描进程序列中有没有先
					  到达的进程，将其先运行，并排至队列末尾。若没有，则将当前时间进行++操作*/
	int flag = 0;//代表有无找到进程
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
		cout << "当前时间为:" << "     " << "正在运行的进程名" << "     " << "已运行时间" << "    " << "还剩时间" << "      " << "队列中还剩程序" << endl;
		cout << current_time << "                 " << current->name << "                  " << run_time[my_map[current->name]] << "              " << remain[my_map[current->name]];
		PCB*p = current->next;
		cout << "          ";
		if (p != begin) {//代表不为空 
			while (p != end) {
				cout << p->name << " ";
				p = p->next;
			}
			cout << p->name << endl;
		}
		else cout << "空";
		cout << endl;
	}
	else {
		current_time += remain[my_map[current->name]];
		current->status = 'C';
		run_time[my_map[current->name]] += remain[my_map[current->name]];
		remain[my_map[current->name]] = 0;
		cout << "当前时间为:" << "     " << "正在运行的进程名" << "     " << "已运行时间" << "    " << "还剩时间" << "      " << "队列中还剩程序" << endl;
		cout << current_time << "                 " << current->name << "                  " << run_time[my_map[current->name]] << "              " << remain[my_map[current->name]];

		cout << "            ";
		PCB*p = current->next;
		if (p != begin) {//代表就绪队列不为空 
			while (p != end) {
				cout << p->name << " ";
				p = p->next;
			}
			cout << p->name << endl;
		}
		else cout << "空";
		cout << endl;
		cout << "********************************************************" << endl;
		cout << "进程" << current->name << "运行完毕，从进程队列中弹出" << endl;
		cout << "********************************************************" << endl;

		cycling_time[my_map[current->name]] = current_time - current->arrive_time;//计算出进程的周转时间 
		current->status = 'C';
		current = current->next;
	}
}

//显示输入之后程各进程的初始信息
void show_information(PCB *p) {
	cout << "****************************************" << endl;
	cout << "          各进程的基本信息如下          " << endl;
	cout << "****************************************" << endl;
	cout << "进程名字" << "     " << "进程到达时间" << "     " << "进程估计执行时间" << endl;
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
			cout << "请输入要建立的进程数:" << endl;
			cin >> n;
			length_time = 5;
			int remain[n];
			int run_time[n];
			int cycling_time[n];//进程周转时间 

								//			PCB pcb[n];//因为局部性清空，所以需要在主函数中定义 
			inputprocess(remain, run_time);
			PCB *p = begin;
			show_information(p);
			cout << "执行周期时长为:" << length_time << endl;
			while (!(if_finsh)) runprocess(remain, run_time, cycling_time);
			cout << "进程名    " << "进程的周转时间为:" << endl;
			PCB*q = begin;
			do {
				cout << q->name << "           " << cycling_time[my_map[q->name]] << endl;
				q = q->next;
			} while (q != begin);
			int time = 0;
			for (int i = 0; i<n; i++) time += cycling_time[i];
			cout << "平均周转时间为:" << time / n << endl;
		}//if
		else exit(1);
	}
	return 0;
}

#endif