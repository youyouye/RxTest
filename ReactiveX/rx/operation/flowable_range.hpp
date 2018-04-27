#pragma once
#include "../reactive_x.hpp"

template<typename T> class Observer;
template<typename T> class OnSubscribe;
template<typename T> class Flowable;

template<typename T>
class FlowableRange : public Flowable<T>
{
public:
	FlowableRange(int start,int end,ThreadType type) 
		:start_(start),end_(end),type_(type)
	{
	}

	void SubscribeActual(std::shared_ptr<Observer<T>> subscriber) override
	{
		for (int i = start_;i <= end_;i++)
		{
			subscriber->OnNext(i);
		}
	}

private:
	size_t start_;
	size_t end_;
	ThreadType type_;
};