#pragma once
#include "../reactive_x.hpp"
#include "../common.hpp"

template<typename T> class Observer;
template<typename T> class OnSubscribe;
template<typename T> class Flowable;

template<typename T,typename... Args>
class FlowableMerge : public Flowable<T>
{
public:
	FlowableMerge(std::tuple<Args...> params)
		:params_(params)
	{
	}

	void SubscribeActual(std::shared_ptr<Observer<T>> subscriber) override
	{
		for_each(params_, [subscriber](auto flowable) {
			flowable->Subscribe(subscriber);
		});
	}

	std::tuple<Args...> params_;
};