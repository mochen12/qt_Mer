#pragma once
#include "mainWindow.h"
#include <QtWidgets/QApplication>
#include "main.h"



int main(int argc, char* argv[])
{
	std::cout << "main线程的ID为: " << std::this_thread::get_id() << std::endl;
	QApplication a(argc, argv);
	//MyWidget w;
	//qt_Mer w;
	MainWindow w;
	w.show();

	return a.exec();

}

