#pragma once
#include <iostream>
#include <thread>
#include "rx_base_test.hpp"
#include "../rx_flowable.hpp"
#include "../rx_subscriber.hpp"

class RxObserveOnTest : public RxTest 
{
public:
	RxObserveOnTest() {}

	std::wstring Run()
	{
		auto rx = Flowable<int>::Just(1,2,3,4,5)
			->SubscribeOn(ThreadType::k_Pool)
			->ObserveOn(ThreadType::k_IoThread);

		auto subscriber = std::make_shared<FlowableSubscriber<int>>();
		subscriber->SetOnSubscribeCallback([](std::shared_ptr<Subscription> subscriber) {
			std::cout << "on subscribe!  " << std::this_thread::get_id() << std::endl;
		});
		subscriber->SetOnNextCallback([](const int& item) {
			std::cout << "on next!" << item << "   " << std::this_thread::get_id() << std::endl;
		});
		subscriber->SetOnCompleteCallback([]() {
			std::cout << "on complete!   " << std::this_thread::get_id() << std::endl;
		});
		rx->Subscribe(subscriber);

		return L"finish";
	}
};
