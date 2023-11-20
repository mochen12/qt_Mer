#pragma once
#include "mainWindow.h"
#include <QtWidgets/QApplication>
#include "main.h"



int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	//MyWidget w;
	//qt_Mer w;
	MainWindow w;
	w.show();

	return a.exec();

}

