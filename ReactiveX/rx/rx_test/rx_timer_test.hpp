#pragma once
#include <iostream>
#include <thread>
#include "rx_base_test.hpp"
#include "../rx_flowable.hpp"

class RxTimerTest : public RxTest
{
public:
	RxTimerTest() {}
	std::wstring Run()
	{
		return L"finish";
	}
};