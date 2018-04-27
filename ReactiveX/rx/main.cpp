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

//test concat
void test11() 
{
	auto memory = Flowable<std::string>::Instance([](std::shared_ptr<Observer<std::string>> subsriber) {
		if (true)
		{
			subsriber->OnNext("123");
		}
		else 
		{
			subsriber->OnCompleted();
		}
	});

	auto disk = Flowable<std::string>::Instance([](std::shared_ptr<Observer<std::string>> subsriber) {
		if (false)
		{
			subsriber->OnNext("456");
		}
		else 
		{
			subsriber->OnCompleted();
		}
	});

	auto network = Flowable<std::string>::Just("789");
	Flowable<std::string>::Concat(std::vector<std::shared_ptr<Flowable<std::string>>>({ memory, disk, network }))
		->Subscribe(std::make_shared<Observer<std::string>>(
			[](std::string var) { std::cout << var << std::endl; },
			[]() {},
			[]() {}
		));
}

//test flatmap
void test12() 
{
	class Student 
	{
	public:
		Student(std::string n) { name = n; }
		std::string name;
	};
	class Course
	{
	public:
		Course(std::string n) { name = n; }
		std::string name;
	};
	std::vector<Student> students;
	auto transfrom = std::make_shared<FlowableTransformer<Student, Course>>();
	transfrom->SetCallback([](Student var)->std::shared_ptr<Flowable<Course>> {
		return Flowable<Course>::Just(Course(var.name));
	});
	//subscriber
	auto subscriber = std::make_shared<FlowSubscribe<Course>>();
	subscriber->SetOnNext([](Course var) {
		std::cout << "ThreadId:" << std::this_thread::get_id() << std::endl;
		std::cout << "Course:" + var.name<< std::endl;
	});
	subscriber->SetOnCompletion([]() {
	});
	subscriber->SetOnError([]() {
	});
	students.push_back(Student("xxx"));
	students.push_back(Student("yyy"));
	students.push_back(Student("zzz"));
	Flowable<Student>::From(students)
		->FlatMap(transfrom)
		->ObserveOn(ThreadType::k_Pool)
		->Subscribe(subscriber);
}

//test13 merge
void test13()
{
	auto flowable_one = Flowable<std::string>::Just("1.")
		->SubscribeOn(ThreadType::k_Pool)
		->ObserveOn(ThreadType::k_Pool);
	auto flowable_two = Flowable<std::string>::Just("2.")
		->SubscribeOn(ThreadType::k_Pool)
		->ObserveOn(ThreadType::k_Pool);
	auto flowable_three = Flowable<std::string>::Just("3.")
		->SubscribeOn(ThreadType::k_Pool)
		->ObserveOn(ThreadType::k_Pool);

	auto subscriber = std::make_shared<FlowSubscribe<std::string>>();
	subscriber->SetOnNext([](std::string var) {
		std::cout << "ThreadId:" << std::this_thread::get_id() << std::endl;
		std::cout << "Value:" + var << std::endl;
	});
	subscriber->SetOnCompletion([]() {
	});
	subscriber->SetOnError([]() {
	});
	Flowable<std::string>::Merge(flowable_one, flowable_two, flowable_three)
		->Subscribe(subscriber);
}
//test14 zip
void test14() 
{
	auto first = Flowable<std::string>::From({ "1.","2.","3." })
		->SubscribeOn(ThreadType::k_Pool);
	auto second = Flowable<int>::From({ 1,2,3 })
		->SubscribeOn(ThreadType::k_Pool);
	auto subscriber = std::make_shared<FlowSubscribe<float>>();
	subscriber->SetOnNext([](float var) {
		std::cout << "ThreadId:" << std::this_thread::get_id() << std::endl;
		std::cout << "Value:" << var << std::endl;
	});
	subscriber->SetOnCompletion([]() {
	});
	subscriber->SetOnError([]() {
	});
	std::function<float(std::string,int)> func = [](std::string one,int two)->float{
		return 2.5f;
	};
	Flowable<float>::Zip(first, second, func)
		->Subscribe(subscriber);
}

//test zip2
void test15()
{
	auto first = std::make_shared<Flowable<std::string>>([](std::shared_ptr<Observer<std::string>> observer) {
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		observer->OnNext("hello world");
	})
		->SubscribeOn(ThreadType::k_Pool);
	class Student 
	{
	public:
		Student() {}
		Student(std::string n) { name = n; }
		std::string name;
	};
	auto second = std::make_shared<Flowable<Student>>([](std::shared_ptr<Observer<Student>> observer) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		observer->OnNext(Student("xxx"));
	})
		->SubscribeOn(ThreadType::k_Pool);
	std::function<std::string(std::string, Student)> func = [](std::string word, Student student)->std::string {
		return word + " " + student.name;
	};
	auto subscriber = std::make_shared<FlowSubscribe<std::string>>();
	subscriber->SetOnNext([](std::string var) {
		std::cout << "ThreadId:" << std::this_thread::get_id() << std::endl;
		std::cout << "Value:" << var << std::endl;
	});
	subscriber->SetOnCompletion([]() {
	});
	subscriber->SetOnError([]() {
	});
	Flowable<std::string>::Zip(first, second, func)
		->Subscribe(subscriber);
}

//test16 concat under thread
void test16() 
{
	auto first = Flowable<std::string>::Instance([](std::shared_ptr<Observer<std::string>> observer) {
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			observer->OnNext("123");
			observer->OnCompleted();
	})->SubscribeOn(ThreadType::k_Pool);
	auto second = Flowable<std::string>::Instance([](std::shared_ptr<Observer<std::string>> observer) {
		observer->OnNext("456");
		observer->OnCompleted();
	})->SubscribeOn(ThreadType::k_IoThread);
	auto subscriber = std::make_shared<FlowSubscribe<std::string>>();
	subscriber->SetOnNext([](std::string var) {
		std::cout << "ThreadId:" << std::this_thread::get_id() << std::endl;
		std::cout << "Value:" << var << std::endl;
	});
	subscriber->SetOnCompletion([]() {
	});
	subscriber->SetOnError([]() {
	});
	Flowable<std::string>::Concat({ first,second })
		->Subscribe(subscriber);
}

void main() 
{
	//current thread
	std::cout << std::this_thread::get_id() << std::endl;
	ScheduleManager::Instance()->Start(5);
	test16();
	while (true)
	{
	}
	ScheduleManager::Instance()->Stop();
}
