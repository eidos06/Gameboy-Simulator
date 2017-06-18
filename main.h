#pragma once
#include <iostream>
#include <string>
#include "GB_FileOperator.h"
#include "GB_memory.h"
#include "GB_Window.h"
#include "GB_Const.h"
#include "GB_CPU.h"
#include "timer.cpp"

GB_memory memory_;
GB_CPU CPU;
GB_Window MainWindow;
TIMER timer;

void PrintInfo(GB_memory &GB_ROM);
