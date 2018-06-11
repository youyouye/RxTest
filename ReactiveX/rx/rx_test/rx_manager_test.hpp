#pragma once
#include <memory>
#include <map>
#include <iostream>
#include "rx_just_test.hpp"
#include "rx_subscribe_on_test.hpp"
#include "rx_observe_on_test.hpp"
#include "rx_concat_test.hpp"
#include "rx_merge_test.hpp"

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
		auto rx_just_test = std::make_shared<RxJustTest>();
//		test_cases_.insert({L"just test",rx_just_test});
		auto rx_subscribe_on_test = std::make_shared<RxSubscribeOnTest>();
//		test_cases_.insert({ L"subscribeOn test",rx_subscribe_on_test });
		auto rx_observe_on_test = std::make_shared<RxObserveOnTest>();
//		test_cases_.insert({L"observeOn test",rx_observe_on_test});
		auto rx_concat_test = std::make_shared<RxConcatTest>();
//		test_cases_.insert({L"concat test",rx_concat_test});
		auto rx_merge = std::make_shared<RxMergeTest>();
		test_cases_.insert({L"merge test", rx_merge});
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