#include <iostream>
#include <string>
#include "schedule_manager.h"
#include "rx_test/rx_manager_test.hpp"

void test_just() 
{
	
}

void main()
{
	ScheduleManager::Instance()->Start(5);
	RxTestManager::Instance()->RunAllTest();
	while (true)
	{
	}
	ScheduleManager::Instance()->Stop();
}
