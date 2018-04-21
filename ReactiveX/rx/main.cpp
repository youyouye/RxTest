#include "reactive_x.hpp"
#include <iostream>
#include <string>

void test1() 
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

void test2() 
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

void main() 
{
	test2();
}