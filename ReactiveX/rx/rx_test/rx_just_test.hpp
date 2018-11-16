#pragma once
#include <memory>
#include "rx_base_test.hpp"
#include "../rx_subscriber.hpp"
#include "../rx_flowable.hpp"

class RxJustTest : public RxTest 
{
public:
	void test1() 
	{
		Flowable<std::string>::Just("1", "2", "3");
	}

	std::wstring Run() 
	{
		test1();
		return L"finish";
	}

};
