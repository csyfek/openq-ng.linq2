#include "LinqWindow.h"
#include "LinqMain.h"
#include "LinqMain.h"

LinqWindow::LinqWindow(int type)
{
	this->type = type;
	linqMain->windowList.push_back(this);
}

LinqWindow::~LinqWindow()
{
	linqMain->windowList.remove(this);
}
