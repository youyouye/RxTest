#pragma once
#include "../reactive_x.hpp"

template<typename T> class Observer;
template<typename T> class OnSubscribe;
template<typename T> class Flowable;

template<typename T>
class FlowableInterval :public Flowable<T>
{
public:
	FlowableInterval(int init_delay, int period, ThreadType type)
	{
		init_delay_ = init_delay;
		period_ = period;
		type_ = type;
	}
	
	void SubscribeActual(std::shared_ptr<Observer<T>> subscriber) override
	{
	}

private:
	int init_delay_ = 0;
	int period_ = 0;
	ThreadType type_;
	size_t index_ = 0;
};