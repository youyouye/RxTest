#pragma once
#include <string>

class RxTest
{
public:
	RxTest() {}
	virtual ~RxTest() {}

	virtual std::wstring Run() = 0;
};
