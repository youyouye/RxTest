#include <iostream>
#include <thread>
#include "rx_base_test.hpp"
#include "../rx_flowable.hpp"
#include "../rx_subscriber.hpp"

class RxMergeTest : public RxTest
{
public:
	RxMergeTest() {}
	void test1() 
	{
		auto rx1 = Flowable<int>::Just(1, 2, 3);
		auto rx2 = Flowable<int>::Just(4, 5, 6);
		auto rx = Flowable<int>::Merge(rx1, rx2);
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
	}
	void test2() 
	{
		auto rx1 = Flowable<int>::Just(1, 2, 3)
			->SubscribeOn(ThreadType::k_IoThread)
			->ObserveOn(ThreadType::k_Pool);
		auto rx2 = Flowable<int>::Just(4, 5, 6)
			->SubscribeOn(ThreadType::k_MainThread)
			->ObserveOn(ThreadType::k_Pool);
		auto rx = Flowable<int>::Merge(rx1, rx2);
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
	}

	std::wstring Run()
	{
		//test1();
		test2();
		return L"finish";
	}
};
