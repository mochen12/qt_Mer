#pragma once

#include "main.h"


int main(int argc, char* argv[])
{
	//实例化管理者对象
	menu mn;

	int choice = 0; //用来存储用户的选项

	while (true)
	{
		//调用展示菜单成员函数
		mn.Show_Menu();

		cout << "请输入您的选择： " << endl;
		cin >> choice; // 接受用户的选项

		switch (choice)
		{
		case 0:  //退出系统
			mn.ExitSystem();
			break;
		case 1:  //修改
			system("cls");
			mn.Modify("default");
			break;
		case 2:  //初始化
			system("cls");
			mn.Initialize();
			break;
		case 3:  //启动
			system("cls");
			mn.Start();
			break;
		default:
			system("cls"); //清屏
			break;
		}

	}

	system("pause");

	return 0;
}