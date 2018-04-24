#pragma once
#include "../reactive_x.hpp"
#include "common.hpp"
#include <memory>
template<typename T> class Observer;
template<typename T> class OnSubscribe;
template<typename T> class Flowable;

template<typename T>
class FlowableSubscribeOn : public Flowable<T>
{
public:
	FlowableSubscribeOn(std::shared_ptr<Flowable<T>> source,ThreadType type)
		:actual_(source),type_(type)
	{
	}
	void SubscribeActual(std::shared_ptr<Observer<T>> subscriber) override
	{
		auto self = GetActual();
		ScheduleManager::Instance()->PostThread(type_, [self,subscriber]() {
			self->Subscribe(subscriber);
		});
	}

	std::shared_ptr<Flowable<T>> GetActual() 
	{
		return actual_;
	}

public:
	std::shared_ptr<Flowable<T>> actual_;
	ThreadType type_;
};