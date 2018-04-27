#pragma once
#include "../reactive_x.hpp"

template<typename T> class Observer;
template<typename T> class OnSubscribe;
template<typename T> class Flowable;
template<typename T> class Callable;

template<typename T>
class FlowableDefer : public Flowable<T>
{
public:
	FlowableDefer(std::shared_ptr<Callable<T>> func)
		:func_(func)
	{
	}
	void SubscribeActual(std::shared_ptr<Observer<T>> subscriber) override
	{
		auto flowable = func_->func_();
		flowable->Subscribe(subscriber);
	}
private:
	std::shared_ptr<Callable<T>> func_;
};



