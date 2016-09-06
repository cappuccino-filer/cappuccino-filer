#include <QDebug> 
#include <iostream>
#include <limits>
#include "io_engine.h"

void io_engine::init()
{
	qDebug() << __PRETTY_FUNCTION__;
}

void io_engine::run()
{
	qDebug() << __PRETTY_FUNCTION__;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
}
