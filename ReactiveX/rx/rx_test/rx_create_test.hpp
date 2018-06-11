#pragma once
#include <iostream>
#include "rx_base_test.hpp"
#include "../rx_flowable.hpp"
#include "../rx_subscriber.hpp"

class RxCreateTest : public RxTest 
{
public:
	RxCreateTest() {}
	std::wstring Run()
	{
		auto on_subscribe = std::make_shared<FlowableOnSubscribe<int>>();
		on_subscribe->SetSubscribeCallback([](std::shared_ptr<FlowableEmitter<int>> emitter) {
			emitter->OnNext(1);
			emitter->OnNext(2);
			emitter->OnNext(3);
			emitter->OnComplete();
			emitter->OnNext(4);
			emitter->OnComplete();
		});
		auto subscriber = std::make_shared<FlowableSubscriber<int>>();
		subscriber->SetOnSubscribeCallback([](std::shared_ptr<Subscription> subscriber) {
			std::cout << "on subscribe!" << std::endl;
			subscriber->Request(1);
		});
		subscriber->SetOnNextCallback([](const int& item) {
			std::cout << "on next!" << item << std::endl;
		});
		subscriber->SetOnCompleteCallback([]() {
			std::cout << "on complete!" << std::endl;
		});

		auto rx_test = Flowable<int>::Create(on_subscribe)
			->SubscribeOn(ThreadType::k_IoThread)
			->ObserveOn(ThreadType::k_MainThread);
		rx_test->Subscribe(subscriber);
		return L"finish";
	}
};
