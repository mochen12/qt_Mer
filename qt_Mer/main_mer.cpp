#pragma once

#include "main.h"


int main(int argc, char* argv[])
{
	//ʵ���������߶���
	menu mn;

	int choice = 0; //�����洢�û���ѡ��

	while (true)
	{
		//����չʾ�˵���Ա����
		mn.Show_Menu();

		cout << "����������ѡ�� " << endl;
		cin >> choice; // �����û���ѡ��

		switch (choice)
		{
		case 0:  //�˳�ϵͳ
			mn.ExitSystem();
			break;
		case 1:  //�޸�
			system("cls");
			mn.Modify("default");
			break;
		case 2:  //��ʼ��
			system("cls");
			mn.Initialize();
			break;
		case 3:  //����
			system("cls");
			mn.Start();
			break;
		default:
			system("cls"); //����
			break;
		}

	}

	system("pause");

	return 0;
}