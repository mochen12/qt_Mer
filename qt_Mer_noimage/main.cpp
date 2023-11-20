#pragma once
#include "qt_Mer.h"
#include <QtWidgets/QApplication>
#include "main.h"


int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	qt_Mer w;
	w.show();

	return a.exec();

}