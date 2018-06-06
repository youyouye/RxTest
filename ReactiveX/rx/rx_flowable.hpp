#pragma once
#include <vector>
#include "rx_base.hpp"
#include "schedule_manager.h"
#include "rx_operation/flowable_just.hpp"
#include "rx_operation/flowable_subscribe_on.hpp"
#include "rx_operation/flowable_observe_on.hpp"

template<typename T>
class Flowable : public Publisher<T>,
	public std::enable_shared_from_this<Flowable<T>>
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

	static std::shared_ptr<Flowable<T>> Just(const std::vector<T> items) 
	{
		return std::make_shared<FlowableJust<T>>(items);
	}
	
	std::shared_ptr<Flowable<T>> SubscribeOn(const ThreadType &type)
	{
		return std::make_shared<FlowableSubscribeOn<T>>(shared_from_this(),type);
	}

	std::shared_ptr<Flowable<T>> ObserveOn(const ThreadType &type) 
	{
		return std::make_shared<FlowableObserveOn<T>>(shared_from_this(),type);
	}

protected:
	virtual void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) {}
};
