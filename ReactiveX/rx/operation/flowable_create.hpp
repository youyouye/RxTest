#pragma once
#include "../reactive_x.hpp"

template<typename T> class Observer;
template<typename T> class OnSubscribe;
template<typename T> class Flowable;

template<typename T>
class FlowableCreate : public Flowable<T> 
{
public:
	FlowableCreate(std::shared_ptr<OnSubscribe<T>> on_subscribe) 
	{
		this->on_subscribe_ = on_subscribe;
	}
	void SubscribeActual(std::shared_ptr<Observer<T>> subscriber) override 
	{
		(*on_subscribe_).function_(subscriber);
	}
private:
};