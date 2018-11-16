#pragma once
#include <memory>
#include <map>
#include <iostream>

class RxTestManager 
{
public:
	RxTestManager() 
	{
		Init();
	}
	~RxTestManager() {}
	
	static std::shared_ptr<RxTestManager> Instance()
	{
		if (manager_ == nullptr)
		{
			manager_ = std::make_shared<RxTestManager>();
		}
		return manager_;
	}

	void Init() 
	{
	}

	void RunAllTest() 
	{
		for (auto it = test_cases_.begin();it != test_cases_.end();it++)
		{
			auto temp_key = it->first;
			std::wcout <<temp_key.c_str() << L"!\t" << std::endl;
			auto result = it->second->Run();
			std::wcout << result.c_str() << L"!\t\n" << std::endl;
		}
	}
private:
	static std::shared_ptr<RxTestManager> manager_;
	std::map<std::wstring, std::shared_ptr<RxTest>> test_cases_;
};

std::shared_ptr<RxTestManager> RxTestManager::manager_ = nullptr;