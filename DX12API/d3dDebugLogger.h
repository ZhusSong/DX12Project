#pragma once

#ifndef D3DDEBUGLOGGER_H
#define D3DDEBUGLOGGER_H

#include <iostream>  
#include <iomanip>  
#include <fstream>  
#include <string>  
#include <cstdlib>  
#include <stdint.h>  
#include <sstream> 
#include <Windows.h>
#include <vector>


#define qDebug MessageLogger(__FILE__, __FUNCTION__, __LINE__).debug
#define qInfo MessageLogger(__FILE__, __FUNCTION__, __LINE__).info
#define qWarning MessageLogger(__FILE__, __FUNCTION__, __LINE__).warning
#define qERROR MessageLogger(__FILE__, __FUNCTION__, __LINE__).critical
#define qFatal MessageLogger(__FILE__, __FUNCTION__, __LINE__).fatal

#endif

