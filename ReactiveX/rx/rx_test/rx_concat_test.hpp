#pragma once
#include <memory>
#include <thread>
#include "rx_base_test.hpp"
#include "../rx_subscriber.hpp"
#include "../rx_flowable.hpp"
#include "../rx_subscriber/lambda_subscriber.hpp"

class RxConcatTest : public RxTest
{
public:
	void test1()
	{
		auto subscriber = std::make_shared<LambdaSubscriber<std::string>>([](const std::string& item) {
			std::cout << std::this_thread::get_id() << "on next: " << item << std::endl;
		});
		auto rx1 = Flowable<std::string>::Just("1", "2", "3")
			->SubscribeOn(ThreadType::k_Pool)
			->ObserveOn(ThreadType::k_Pool);

		auto rx2 = Flowable<std::string>::Just("4", "5", "6")
			->SubscribeOn(ThreadType::k_Pool)
			->ObserveOn(ThreadType::k_Pool);
		Flowable<std::string>::Concat(rx1,rx2)
			->Subscribe(subscriber);
	}

	std::wstring Run()
	{
		test1();
		return L"finish";
	}

};
