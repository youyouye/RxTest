#pragma once
#include "../rx_flowable.hpp"

template<typename T>
class Flowable;

template<typename T>
class FlowableJust : public Flowable<T> 
{
public:
	FlowableJust(const T& value) 
		:value_(value)
	{
	}
	
	void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) override
	{
		auto subscription = std::make_shared<JustSubscription<T>>(subscriber, value_);
		subscriber->OnSubscribe(subscription);
		subscription->Request(100);
	}

private:
	T value_;
};

template<typename T>
class JustSubscription : public Subscription 
{
public:
	JustSubscription(std::shared_ptr<Subscriber<T>> subscriber,const T& value) 
		:subscriber_(subscriber),value_(value)
	{
	}
	
	void Request(long n) override
	{
		subscriber_->OnNext(value_);
		subscriber_->OnComplete();
	}

	void Cancel() override
	{
	
	}

private:
	T value_;
	std::shared_ptr<Subscriber<T>> subscriber_;
};
