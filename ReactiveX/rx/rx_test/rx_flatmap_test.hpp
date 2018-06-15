#pragma once
#include <iostream>
#include <functional>
#include "rx_base_test.hpp"
#include "../rx_flowable.hpp"
#include "../rx_subscriber.hpp"

class RxFlatMap : public RxTest
{
public:
	RxFlatMap() {}

	void test1() 
	{
		auto on_subscribe = std::make_shared<FlowableOnSubscribe<std::string>>();
		on_subscribe->SetSubscribeCallback([](std::shared_ptr<FlowableEmitter<std::string>> emitter) {
			emitter->OnNext("one");
			emitter->OnComplete();
		});
		auto rx_one = Flowable<std::string>::Create(on_subscribe)
			->SubscribeOn(ThreadType::k_IoThread)
			->ObserveOn(ThreadType::k_MainThread);

		std::function<std::shared_ptr<Flowable<int>>(const std::string& item)> func = [](const std::string& item)->std::shared_ptr<Flowable<int>> {
			auto on_subscribe_two = std::make_shared<FlowableOnSubscribe<int>>();
			on_subscribe_two->SetSubscribeCallback([](std::shared_ptr<FlowableEmitter<int>> emitter) {
				emitter->OnNext(5);
				emitter->OnComplete();
			});
			auto rx_two = Flowable<int>::Create(on_subscribe_two)
				->SubscribeOn(ThreadType::k_MainThread)
				->ObserveOn(ThreadType::k_IoThread);
			return rx_two;
		};

		auto subscriber = std::make_shared<FlowableSubscriber<int>>();
		subscriber->SetOnSubscribeCallback([](std::shared_ptr<Subscription> subscriber) {
			std::cout << "on subscribe!" << std::endl;
		});
		subscriber->SetOnNextCallback([](const int& item) {
			std::cout << "on next!" << item << std::endl;
		});
		subscriber->SetOnCompleteCallback([]() {
			std::cout << "on complete!" << std::endl;
		});

		rx_one->FlatMap(func)
			->Subscribe(subscriber);
	}
	
	void test2() 
	{
		auto rx_just_1 = Flowable<int>::Just(1, 2, 3, 4, 5)
			->SubscribeOn(ThreadType::k_IoThread)
			->ObserveOn(ThreadType::k_MainThread);
		std::function<std::shared_ptr<Flowable<std::string>>(const int& item)> func = [](const int& item)->std::shared_ptr<Flowable<std::string>> {
			auto on_subscribe_two = std::make_shared<FlowableOnSubscribe<std::string>>();
			on_subscribe_two->SetSubscribeCallback([item](std::shared_ptr<FlowableEmitter<std::string>> emitter) {
				emitter->OnNext(std::to_string(item)+"xx");
				emitter->OnComplete();
			});
			auto rx_two = Flowable<std::string>::Create(on_subscribe_two)
				->SubscribeOn(ThreadType::k_MainThread)
				->ObserveOn(ThreadType::k_IoThread);
			return rx_two;
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

		rx_just_1->FlatMap(func)
			->Subscribe(subscriber);
	}

	std::wstring Run()
	{
//		test1();
		test2();
		return L"finish";
	}
};
