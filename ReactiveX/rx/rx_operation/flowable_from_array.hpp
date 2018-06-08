#pragma once
#include <atomic>
#include "../rx_base.hpp"
#include "../rx_flowable.hpp"
template<typename T>
class Flowable;

template<typename T>
class FlowableFromArray : public Flowable<T> 
{
public:
	FlowableFromArray(const std::vector<T> &values) 
		:item_values_(values)
	{
	}

	void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) override
	{
		auto from_array_subscriber = std::make_shared<FromArraySubscriber<T>>(subscriber,item_values_);
		subscriber->OnSubscribe(from_array_subscriber);
	}

public:
	std::vector<T> item_values_;
};

template<typename T>
class FromArraySubscriber :  public Subscription,
	public std::enable_shared_from_this<FromArraySubscriber<T>>
{
public:
	FromArraySubscriber(std::shared_ptr<Subscriber<T>> actual,const std::vector<T>& values)
		:actual_(actual),values_(values)
	{
	}

	void Request(int n) 
	{
	}
	bool IsCancel() 
	{
	}
	void Cancel() 
	{
	}

	void Run()
	{

	}

public:
	std::shared_ptr<Subscriber<T>> actual_;
	std::vector<T> values_;
};
