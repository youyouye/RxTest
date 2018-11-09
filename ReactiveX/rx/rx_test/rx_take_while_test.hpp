#pragma once
#include <iostream>
#include <thread>
#include "rx_base_test.hpp"
#include "../rx_flowable.hpp"
#include "../rx_subscriber.hpp"

class RxTakeWhileTest : public RxTest
{
public:
	RxTakeWhileTest() {}

	void Test1() 
	{
		auto rx2 = Flowable<int>::Just(1, 2, 3);
		auto subscriber2 = std::make_shared<FlowableSubscriber<int>>();
		subscriber2->SetOnSubscribeCallback([](std::shared_ptr<Subscription> subscriber) {
			std::cout << "on subscribe!" << std::endl;
			subscriber->Request(1);
		});
		subscriber2->SetOnNextCallback([](const int& item) {
			std::cout << "on next!" << item << std::endl;
		});
		subscriber2->SetOnCompleteCallback([]() {
			std::cout << "on complete!" << std::endl;
		});
		auto rx3 = rx2->TakeWhile([](const int & item)->bool {
			return item <= 2;
		});
		rx3->Subscribe(subscriber2);
	}

	void Test2() 
	{
		auto rx2 = Flowable<int>::Just(1, 2, 3)
			->SubscribeOn(ThreadType::k_Pool)
			->ObserveOn(ThreadType::k_Pool);
		auto subscriber2 = std::make_shared<FlowableSubscriber<int>>();
		subscriber2->SetOnSubscribeCallback([](std::shared_ptr<Subscription> subscriber) {
			std::cout << "on subscribe!  " << std::this_thread::get_id() << std::endl;
			subscriber->Request(1);
		});
		subscriber2->SetOnNextCallback([](const int& item) {
			std::cout << "on next!" << item << "   " << std::this_thread::get_id() << std::endl;
		});
		subscriber2->SetOnCompleteCallback([]() {
			std::cout << "on complete!   " << std::this_thread::get_id() << std::endl;
		});
		auto rx3 = rx2->TakeWhile([](const int & item)->bool {
			return item <= 2;
		});
		rx3->Subscribe(subscriber2);
	}

	std::wstring Run()
	{
		Test2();
		return L"finish";
	}
};