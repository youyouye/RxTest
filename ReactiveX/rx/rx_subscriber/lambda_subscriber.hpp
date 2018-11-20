#pragma once
#include <memory>
#include <iostream>
#include <thread>
#include "../rx_base.hpp"

template<typename T>
class LambdaSubscriber : public Subscriber<T>
{
public:
	LambdaSubscriber(std::function<void(const T&)> on_next) 
		:on_next_(on_next)
	{
		on_subscribe_ = std::bind(&LambdaSubscriber::defaultOnSubscribe, this,std::placeholders::_1);
		on_complete_ = std::bind(&LambdaSubscriber::defaultOnComplete, this);
		on_error_ = std::bind(&LambdaSubscriber::defaultOnError, this, std::placeholders::_1);
	}
	
	void OnSubscribe(std::shared_ptr<Subscription> subscription) override
	{
		if (on_subscribe_)
		{
			on_subscribe_(subscription);
		}
	}
	void OnNext(const T &t) override 
	{
		if (on_next_) 
		{
			on_next_(t);
		}
	}
	void OnComplete() override
	{
		if (on_complete_)
		{
			on_complete_();
		}
	}
	void OnError(std::shared_ptr<Error> error) override 
	{
		if (on_error_)
		{
			on_error_(error);
		}
	}
private:
	void defaultOnComplete() 
	{
		std::cout << std::this_thread::get_id() << "on complete." << std::endl;
	}
	void defaultOnSubscribe(std::shared_ptr<Subscription> subscription)
	{
		std::cout << std::this_thread::get_id() << "on subscription." << std::endl;
		subscription->Request(1000);
	}
	void defaultOnError(std::shared_ptr<Error> error) 
	{
		std::cout << std::this_thread::get_id() << "on error." << std::endl;
	}
private:
	SubscribeCallback on_subscribe_;
	std::function<void(const T&)> on_next_;
	CompleteCallback on_complete_;
	ErrorCallabck on_error_;
};