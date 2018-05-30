#include <iostream>
#include <string>
#include "rx_flowable.hpp"
#include "rx_subscriber.hpp"
#include "schedule_manager.h"

//test just
void test1() 
{
	auto subscriber = std::make_shared<FlowableSubscriber<std::string>>();
	subscriber->SetOnNext([](const std::string& item) {
		std::cout << "on next:" << item << std::endl;
	});

	Flowable<std::string>::Just("123")
		->Subscribe(subscriber);
}
//test create
void test2() 
{
	auto subscriber = std::make_shared<FlowableSubscriber<int>>();
	subscriber->SetOnNext([](const int& item) {
		std::cout << "on next:" << item << std::endl;
	});
	auto on_subscribe = std::make_shared<FlowableOnSubscribe<int>>();
//	Flowable<int>::Create()
//		->Subscribe(subscriber);
}

void main() 
{
	ScheduleManager::Instance()->Start(5);
	test2();
	while (true)
	{
	}
	ScheduleManager::Instance()->Stop();
}
