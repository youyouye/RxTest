#pragma once
#include <iostream>
#include <thread>
#include "rx_base_test.hpp"
#include "../rx_flowable.hpp"
#include "../rx_subscriber.hpp"

class RxZipTest : public RxTest 
{
public:
	RxZipTest() {}

	void test1() 
	{
		auto rx1 = Flowable<int>::Just(11,22,33)
			->SubscribeOn(ThreadType::k_IoThread)
			->ObserveOn(ThreadType::k_Pool);
		auto rx2 = Flowable<std::string>::Just("11","22","33")
			->SubscribeOn(ThreadType::k_MainThread)
			->ObserveOn(ThreadType::k_Pool);
		std::function<std::string(const int&, const std::string&)> func = [](const int& item1, const std::string& item2)->std::string {
			return std::to_string(item1) + item2;
		};
		
		auto subscriber = std::make_shared<FlowableSubscriber<std::string>>();
		subscriber->SetOnSubscribeCallback([](std::shared_ptr<Subscription> subscriber) {
			std::cout << "on subscribe!" << std::endl;
		});
		subscriber->SetOnNextCallback([](const std::string& item) {
			std::cout << "on next!" << item << std::endl;
		});
		subscriber->SetOnCompleteCallback([]() {
			std::cout << "on complete!" << std::endl;
		});
		Flowable<std::string>::Zip(rx1, rx2, func)
			->Subscribe(subscriber);
	}

	std::wstring Run()
	{
		test1();
		return L"finish";
	}
};