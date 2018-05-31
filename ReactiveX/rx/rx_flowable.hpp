#pragma once
#include "rx_base.hpp"
#include "schedule_manager.h"
#include "rx_operation/flowable_just.hpp"

template<typename T>
class Flowable : public Publisher<T>
{
public:
	Flowable() {}
	virtual ~Flowable() {}

	void Subscribe(std::shared_ptr<Subscriber<T>> subscriber) override
	{
		SubscribeActual(subscriber);
	}

	static std::shared_ptr<Flowable<T>> Just(const T& item)
	{
		return std::make_shared<FlowableJust<T>>(item);
	}

	std::shared_ptr<Flowable<T>> SubscribeOn(const ThreadType &type)
	{
		return nullptr;
	}

	std::shared_ptr<Flowable<T>> ObserveOn(const ThreadType &type) 
	{
		return nullptr;
	}

protected:

};
