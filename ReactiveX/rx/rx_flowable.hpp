#pragma once
#include "rx_base.hpp"
#include "rx_onsubscribe.hpp"
#include "schedule_manager.h"
#include "rx_operation/flowable_create.hpp"
#include "rx_operation/flowable_just.hpp"
#include "rx_operation/flowable_observeon.hpp"

enum class BackPressureStrategy
{
	kDrop = 1,
	kLatest,
};

template<typename T>
class Flowable : public Publisher<T>,
	public std::enable_shared_from_this<Flowable<T>>
{
public:
	virtual ~Flowable() {}

	void Subscribe(std::shared_ptr<Subscriber<T>> subscriber) override 
	{
		SubscribeActual(subscriber);
	}
	virtual void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) {}

	static std::shared_ptr<Flowable<T>> Create(std::shared_ptr<FlowableOnSubscribe<T>> source,BackPressureStrategy mode) 
	{
		return std::make_shared<FlowableCreate<T>>(source,mode);
	}
	
	static std::shared_ptr<Flowable<T>> Just(const T& item) 
	{
		return std::make_shared<FlowableJust<T>>(item);
	}
	
	std::shared_ptr<Flowable<T>> SubscribeOn(ThreadType type) 
	{
		auto self = shared_from_this();
		return std::make_shared<FlowableSubscribeOn<T>>(self,type);
	}

	std::shared_ptr<Flowable<T>> ObserveOn(ThreadType type)
	{
		auto self = shared_from_this();
		return std::make_shared<FlowableObserveOn<T>>(self,type);
	}

};
