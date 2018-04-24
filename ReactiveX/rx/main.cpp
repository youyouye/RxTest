#include "reactive_x.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

//test subscribe
void test_subscribe() 
{
	auto on_subscribe = std::make_shared<OnSubscribe<std::string>>();
	auto func = [](std::shared_ptr<Observer<std::string>> subsriber) {
		std::cout << "subsribe complete" << std::endl;
		subsriber->OnNext("hello world!");
	};
	on_subscribe->SetSubscribeCallback(func);
	auto flow = Flowable<std::string>::Instance(on_subscribe);

	//
	auto flow_subsciber = std::make_shared<FlowSubscribe<std::string>>();
	flow_subsciber->SetOnCompletion([]() { 
		std::cout << "completion!" << std::endl;
	});
	flow_subsciber->SetOnNext([](std::string param) {
		std::cout << "next!" << param << std::endl;
	});
	flow_subsciber->SetOnError([]() {
		std::cout << "error!" << std::endl;
	});

	flow->Subscribe(flow_subsciber);
}
//test map
void test_map() 
{
	auto on_subscribe = std::make_shared<OnSubscribe<std::string>>();
	auto func = [](std::shared_ptr<Observer<std::string>> subsriber) {
		std::cout << "subsribe complete" << std::endl;
		subsriber->OnNext("hello world!");
	};
	on_subscribe->SetSubscribeCallback(func);
	auto flow = Flowable<std::string>::Instance(on_subscribe);

	//
	auto flow_subsciber = std::make_shared<FlowSubscribe<int>>();
	flow_subsciber->SetOnCompletion([]() {
		std::cout << "completion!" << std::endl;
	});
	flow_subsciber->SetOnNext([](int param) {
		std::cout << "next!" << param << std::endl;
	});
	flow_subsciber->SetOnError([]() {
		std::cout << "error!" << std::endl;
	});

	auto transform = std::make_shared<Transformer<std::string,int>>();
	transform->setTransformCallback([](std::string param)->int {
		return 5;
	});

	flow->map(transform)->Subscribe(flow_subsciber);
}
//test subscribeOn and observerOn
void test3() 
{
	ScheduleManager::Instance()->Start(5);

	auto on_subscribe = std::make_shared<OnSubscribe<std::string>>();
	auto func = [](std::shared_ptr<Observer<std::string>> subsriber) {
		std::cout << "Onsubsribe :"<< std::this_thread::get_id() << std::endl;
		subsriber->OnNext("hello world!");
	};
	on_subscribe->SetSubscribeCallback(func);
	auto flow = Flowable<std::string>::Instance(on_subscribe);

	//
	auto flow_subsciber = std::make_shared<FlowSubscribe<std::string>>();
	flow_subsciber->SetOnCompletion([]() {
		std::cout << "completion!" << std::endl;
	});
	flow_subsciber->SetOnNext([](std::string param) {
		std::cout << "next!" << std::this_thread::get_id() << std::endl;
		std::cout << "next!" << param << std::endl;
	});
	flow_subsciber->SetOnError([]() {
		std::cout << "error!" << std::endl;
	});

	flow->SubscribeOn(ThreadType::k_IoThread)->ObserveOn(ThreadType::k_Pool)
		->Subscribe(flow_subsciber);
	
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	ScheduleManager::Instance()->Stop();
}

/*
//test_AsyncSubject
void test4() 
{
	auto subject = AsyncSubject<std::string>::Instance();
	//
	auto flow_subsciber = std::make_shared<FlowSubscribe<std::string>>();
	flow_subsciber->SetOnCompletion([]() {
		std::cout << "completion!" << std::endl;
	});
	flow_subsciber->SetOnNext([](std::string param) {
		std::cout << "next!" << std::this_thread::get_id() << std::endl;
		std::cout << "next!" << param << std::endl;
	});
	flow_subsciber->SetOnError([]() {
		std::cout << "error!" << std::endl;
	});
	subject->Subscribe(flow_subsciber);
}
*/
//test just
void test5() 
{
	auto flow_subsciber = std::make_shared<FlowSubscribe<std::string>>();
	flow_subsciber->SetOnCompletion([]() {
		std::cout << "completion!" << std::endl;
	});
	flow_subsciber->SetOnNext([](std::string param) {
		std::cout << "next!" << std::this_thread::get_id() << std::endl;
		std::cout << "next!" << param << std::endl;
	});
	flow_subsciber->SetOnError([]() {
		std::cout << "error!" << std::endl;
	});
	Flowable<std::string>::Just("hello world")->Subscribe(flow_subsciber);
}
//test just(...args)
void test6() 
{
	auto flow_subsciber = std::make_shared<FlowSubscribe<std::string>>();
	flow_subsciber->SetOnCompletion([]() {
		std::cout << "completion!" << std::endl;
	});
	flow_subsciber->SetOnNext([](std::string param) {
		std::cout << "next!" << std::this_thread::get_id() << std::endl;
		std::cout << "next!" << param << std::endl;
	});
	flow_subsciber->SetOnError([]() {
		std::cout << "error!" << std::endl;
	});
	Flowable<std::string>::Just("hello world","222","333","444","555")->Subscribe(flow_subsciber);
}

//test from array
void test7() 
{
	auto flow_subsciber = std::make_shared<FlowSubscribe<std::string>>();
	flow_subsciber->SetOnCompletion([]() {
		std::cout << "completion!" << std::endl;
	});
	flow_subsciber->SetOnNext([](std::string param) {
		std::cout << "next!" << std::this_thread::get_id() << std::endl;
		std::cout << "next!" << param << std::endl;
	});
	flow_subsciber->SetOnError([]() {
		std::cout << "error!" << std::endl;
	});
	Flowable<std::string>::From(std::vector<std::string>({ "hello world", "222", "333", "444", "555" }))->Subscribe(flow_subsciber);
}
//test interval
void test8() 
{

}
//test unsubscrible
void test9() 
{
	auto flow_subsciber = std::make_shared<FlowSubscribe<std::string>>();
	flow_subsciber->SetOnCompletion([]() {
		std::cout << "completion!" << std::endl;
	});
	flow_subsciber->SetOnNext([](std::string param) {
		std::cout << "next!" << std::this_thread::get_id() << std::endl;
		std::cout << "next!" << param << std::endl;
	});
	flow_subsciber->SetOnError([]() {
		std::cout << "error!" << std::endl;
	});
	auto on_subscribe = std::make_shared<OnSubscribe<std::string>>();
	auto func = [](std::shared_ptr<Observer<std::string>> subsriber) {
		std::cout << "Onsubsribe :" << std::this_thread::get_id() << std::endl;
		subsriber->OnNext("hello world!");
		subsriber->Cancel();
	};
	on_subscribe->SetSubscribeCallback(func);

	Flowable<std::string>::Instance(on_subscribe)
		->SubscribeOn(ThreadType::k_IoThread)
		->ObserveOn(ThreadType::k_Pool)
		->UnsubscribeOn(ThreadType::k_IoThread)
		->Subscribe(flow_subsciber);
}

//test startwith
void test10() 
{
	auto flow_subsciber = std::make_shared<FlowSubscribe<std::string>>();
	flow_subsciber->SetOnCompletion([]() {
		std::cout << "completion!" << std::endl;
	});
	flow_subsciber->SetOnNext([](std::string param) {
		std::cout << "next!" << std::this_thread::get_id() << std::endl;
		std::cout << "next!" << param << std::endl;
	});
	flow_subsciber->SetOnError([]() {
		std::cout << "error!" << std::endl;
	});
	auto on_subscribe = std::make_shared<OnSubscribe<std::string>>();
	auto func = [](std::shared_ptr<Observer<std::string>> subsriber) {
		std::cout << "subsribe complete"<< std::this_thread::get_id() << std::endl;
		subsriber->OnNext("hello world!");
	};
	on_subscribe->SetSubscribeCallback(func);
	Flowable<std::string>::Instance(on_subscribe)
		->StartWith("1")
		->SubscribeOn(ThreadType::k_Pool)
		->ObserveOn(ThreadType::k_IoThread)
		->Subscribe(flow_subsciber);
}

void main() 
{
	//current thread
	std::cout << std::this_thread::get_id() << std::endl;
	ScheduleManager::Instance()->Start(5);
	test10();
	while (true)
	{
	}
	ScheduleManager::Instance()->Stop();
}














