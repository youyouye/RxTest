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
	FlowableFromArray(const std::vector<std::shared_ptr<Flowable<T>>>& values) 
		:flowable_values_(values)
	{
	}

	FlowableFromArray(const std::vector<T> &values) 
		:item_values_(values)
	{
	}

	void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) override
	{
		auto array_subscription = std::make_shared<FromArraySubscriber<T>>(subscriber, item_values_);
		subscriber->OnSubscribe(array_subscription);
		if (array_subscription->GetFusionMode())
			return;
		array_subscription->Run();
	}

public:
	std::vector<std::shared_ptr<Flowable<T>>> flowable_values_;
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
		return (request_state_ == k_cancelled);
	}
	void Cancel() 
	{
		request_state_ = k_cancelled;
	}

	void Run()
	{
		for (int i = 0 ;i <values_.size() && !IsCancel();i++)
		{
			actual_->OnNext(values_[i]);
		}
		if (!IsCancel())
		{
			actual_->OnComplete();
		}
	}

public:
	std::shared_ptr<Subscriber<T>> actual_;
	std::vector<T> values_;
};
