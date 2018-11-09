#pragma once
#include <atomic>
#include "../rx_base.hpp"
#include "../rx_flowable.hpp"
#include "../safe_queue.hpp"

template<typename T>
class Flowable;

template<typename T>
class FlowableTimer : public Flowable<T> 
{
public:
	FlowableTimer(int delay, const ThreadType &type) 
		:delay_(delay),thread_type_(type)
	{
	}

	void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) override 
	{
		
	}
private:
	int delay_;
	ThreadType thread_type_;
};