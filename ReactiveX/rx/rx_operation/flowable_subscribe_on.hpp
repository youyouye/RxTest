#pragma once
#include "../rx_base.hpp"
#include "../rx_flowable.hpp"

template<typename T>
class Flowable;

template<typename T>
class FlowableSubscribeOn : public Flowable<T>
{
public:
	FlowableSubscribeOn(std::shared_ptr<Flowable<T>> upstream,const ThreadType &type) 
		:upstream_(upstream),type_(type)
	{
	}

	void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) override 
	{
		auto subscribe_on = std::make_shared<>();
	}

private:
	std::shared_ptr<Flowable<T>> upstream_;
	ThreadType type_;
};

template<typename T>
class SubscribeOnSubscription :public Subscription
{
public:
	SubscribeOnSubscription() 
	{
	}

	void Request(int n) 
	{
	}

	void Cancel() 
	{
	}

};
