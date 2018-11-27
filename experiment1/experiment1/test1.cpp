#include "stdafx.h"
#include<iostream>
#include <string>
#include<fstream>
using namespace std;

ifstream fin;
ofstream fout;
//�����ļ� test  Ҫ���ļ� pragram
char *inFileName = "code.txt";
char *outFileName = "result.txt";

#define TABLE_COLUMN 12  //��
#define TABLE_ROW 13	 //��
int posLine = 1;//��¼�к�
int errors = 1;//��¼������
string str_temp = " ";

//keywords
string keyWord[8] = {
	"if", "else", "for", "while", "do", "int", "write", "read"
};

//�������״̬
typedef enum {
	S0,  //��ʼ״̬
	S1,S2,S3,S4,S5,S6,//�м�״̬
	S7,S8,S9,S10,S11,
	ERR// ����״̬
} STATE;

//״̬ת����
int transform_table[][12] = {
	//		��ĸ	����	0		�ָ���  +-       /		 *		 !		 =		><		other�������ո� 
	{ -1,	'a',	1,		0,		'(',	'+',	'/',	'*',	'!',	'=',	'>',	'o' },
	{ S0,	S1,		S2,		S3,		S4,		S6,		S8,		S6,	    S5,		S7,		S7 ,	ERR },
	{ S1,	S1,		S1,		S1,		ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR },//��
	{ S2,	ERR,	S2,		S2,		ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR },//��
	{ S3,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR },//��
	{ S4,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR },//��
	{ S5,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	S6,		ERR,	ERR },
	{ S6,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR },//��
	{ S7,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	S6,		ERR,	ERR },//��
	{ S8,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	S9,		ERR,	ERR,	ERR,	ERR },//��
	{ S9,	S9,		S9,		S9,		S9,		S9,		S9,		S10,	S9,		S9,		S9 ,	S9 },
	{ S10,	S9,		S9,		S9,		S9,		S9,		S11,	S10,	S9,		S9,		S9 ,	S9 },
	{ S11,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR,	ERR },//��
};

//״̬ע��
char *state_class[100];
void init_state_class() {
	state_class[S1] = "ID";
	state_class[S2] = "NUM";
	state_class[S3] = "NUM";
	state_class[S4] = "Delimiter";
	//state_class[S5] = "!";
	state_class[S6] = "Operator";
	state_class[S7] = "Operator";
	state_class[S8] = "Operator";
	//state_class[S9] = "ע��";
	//state_class[S10] = "*";
	state_class[S11] = "ע��";
}

//�������״̬
STATE end_state[] = {
	S1,S2,S3,S4,S6,S7,S8,S11,
};
int end_state_len = 8;

//�����������ת��Ϊ table�� �ڼ���
int get_column(char ch) {
	if (ch >= '1' && ch <= '9') {
		return 2;//��ĸ		
	}
	else if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z') {
		return 1;//����
	}
	else if (ch == '0') {
		return 3;//0
	}
	else if (ch == '(' || ch == ')' || ch == '{' || ch == '}' || ch == ';' || ch == ',') {
		return 4;//�ָ���
	}
	else if (ch == '+' || ch == '-') {
		return 5;//+-
	}
	else if (ch == '/') {
		return 6;// /	
	}
	else if (ch == '*') {
		return 7;//*	
	}
	else if (ch == '!') {
		return 8;//!
	}
	else if (ch == '=') {
		return 9;//=	
	}
	else if (ch == '<' || ch == '>') {
		return 10;//><
	}
	else
	{
		return 11;//other
	}
}

//�Ƿ�����̬��
int in_end_state(STATE now) {
	for (int i = 0; i < end_state_len; i++) {
		if (end_state[i] == now) {
			return 1; // ����̬������
		}
	}
	return 0; // ������̬������
}

//״̬ת��  ���뵱ǰ״̬ ����һ���ַ�  ��������״̬
STATE jump(STATE now, char c) {
	STATE new_state;
	// �����ǰ״̬�к�
	int row_idx = 0;
	for (int i = 1; i < TABLE_ROW; i++) {
		if (now == transform_table[i][0]) {
			row_idx = i;
			break;
		}
	}
	if (row_idx == 0)
	{
		return ERR;
	}
	//����ַ����ڵڼ���
	int col_idx = get_column(c);
	new_state = (STATE)transform_table[row_idx][col_idx];
	return new_state;
}

//�����star��end���ַ�
string changeToStr(char *str, int start, int end) {
	string s ="";
	for (int i = start; i <= end; i++) {
		s+= str[i];		
		
	}
	//cout << s;
	return s;
}

//�ж��Ƿ�Ϊ�ؼ���
int isKeywords(string str)
{
	for (int i=0;i<8;i++)
	{
		if (str == keyWord[i])
		{
			return i;
		}
	}
	return -1;
}

void analyseWord(char *line) {
	STATE now = S0;//��ʼ״̬
	int index = 0; //��¼��ǰ�ַ���λ��  �������
	int len = strlen(line);
	for (int i = 0; i < len; i++) {
		char c = line[i];
		//�ո�����
		if (c == ' ' || c == '\n' || c == '\t') {
			if (index == i) {
				index++;
			}
			continue; //��ȡ��һ���ַ�
		}
		now = jump(now, line[i]);
							 
		//����һ����״̬�Ĵ���  �����ȷ�ַ�  ����  ����  ��ע
	/*	if (jump(now, line[i + 1]) == ERR && get_column(line[i + 1])==11&& line[i + 1]!=' '&&line[i + 1] != '\0')
		{
			cout << "error:"<<"|"<< line[i + 1]<<"|"<< endl;

		}*/

		if (in_end_state(now) && jump(now, line[i+1]) == ERR) {//��ǰ�ַ�����̬��accept��  ������һ���ַ�״̬ΪERR
			string str = changeToStr(line, index, i);
			
			//�ж��ǲ���keywords
			if (isKeywords(str)>=0)
			{	
				fout << keyWord[isKeywords(str)] << str_temp;
				//cout << "\t  S" << now << "\tkeyWord  " << keyWord[isKeywords(str)] << endl;
				fout << str << str_temp  << endl;//��ŵ��ļ��� << posLine
			}
			else if(now != S11)
			{
				if (now != S1&& now != S2&& now != S3)
				{
					fout << str << str_temp << str << str_temp << endl;//��ŵ��ļ���  << posLine
				}
				else
				{
					fout << state_class[now] << str_temp;
					//cout << "\t  S" << now << "\t" << state_class[now] << endl;
					fout << str << str_temp  << endl;//��ŵ��ļ��� << posLine
				}

			}
			
			//�ص���ʼ̬
			now = S0;
			index = i + 1;
			
		}
		//�����Խ��� ��ǰ���м�״̬ ����һ��״̬Ϊerr ��ʾ��·����,��ǰ�ַ����Ϸ�
		else if (!in_end_state(now)&&jump(now, line[i + 1]) == ERR)
		{
			cout <<"errors:"<<errors++<<"  line:"<< posLine<< "\tδ֪���Ż򲻺Ϸ��ַ��� " << line[i] << endl;
			now = S0;
			index = i + 1;
		}
		//���ע�Ͳ�ȫ����   ���һ���ַ�����һ���ַ����ַ���β \0  ��ǰ�ַ���״̬������̬ 
		else if (line[i+1] =='\0' && !in_end_state(now))
		{
			cout << "errors:" << errors++ << "  line:" << posLine << "\tע�Ͳ�ȫ: " << changeToStr(line, index, i) << endl;
		}

	}

}


int main() {	
	//���ļ�
	fin.open(inFileName,ios::in);
	fout.open(outFileName, ios::out);
	cout << "Ҫ�����ԭ�ļ�Ϊ��"<<inFileName<<endl;
	cout << "������ɵ��ļ�Ϊ��" << outFileName << endl << endl;
	char buffer[256];
	init_state_class();
	while (!fin.eof()) {
		fin.getline(buffer, 256, '\n');//��ȡһ��
		analyseWord(buffer);
		posLine++;
	}
	cout << "�������������" << errors-1<< "������" << endl << endl;

	//�ر��ļ�
	fin.close();
	fout.close();
	system("pause");
	return 0;
}

void lexical_analysis()
{
	fin.open(inFileName, ios::in);
	fout.open(outFileName, ios::out);
	cout << "Ҫ�����ԭ�ļ�Ϊ��" << inFileName << endl;
	cout << "������ɵ��ļ�Ϊ��" << outFileName << endl << endl;
	char buffer[256];
	init_state_class();
	while (!fin.eof()) {
		fin.getline(buffer, 256, '\n');//��ȡһ��
		analyseWord(buffer);
		posLine++;
	}
	cout << "�������������" << errors - 1 << "������" << endl << endl;

	//�ر��ļ�
	fin.close();
	fout.close();
}
