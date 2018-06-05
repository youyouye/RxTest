#pragma once
#include <iostream>
#include "rx_base_test.hpp"
#include "../rx_flowable.hpp"
#include "../rx_subscriber.hpp"

class RxJustTest : public RxTest
{
public:
	RxJustTest() {}
	std::wstring Run()
	{
		auto rx = Flowable<int>::Just(11);
		auto subscriber = std::make_shared<FlowableSubscriber<int>>();
		subscriber->SetOnSubscribeCallback([](std::shared_ptr<Subscription> subscriber) {
			std::cout << "on subscribe!" << std::endl;
			subscriber->Request(1);
		});
		subscriber->SetOnNextCallback([](const int& item) {
			std::cout << "on next!" <<item << std::endl;
		});
		subscriber->SetOnCompleteCallback([]() {
			std::cout << "on complete!" << std::endl;
		});
		rx->Subscribe(subscriber);
		return L"finish";
	}
};