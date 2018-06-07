#pragma once
#include <atomic>
#include "../rx_base.hpp"
#include "../rx_flowable.hpp"
#include "../safe_queue.hpp"
template<typename T>
class Flowable;

template<typename T>
class FlowableConcat : public Flowable<T> 
{
public:
	FlowableConcat(std::shared_ptr<Flowable<T>> source)
		:source_(source)
	{
	}

	void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) override
	{
	}

public:
	std::shared_ptr<Flowable<T>> source_;
};

template<typename T>
class ConcatSubscriber : public Subscriber<T>, Subscription,
	public std::enable_shared_from_this<ConcatSubscriber<T>>
{
public:
	ConcatSubscriber(std::shared_ptr<Subscriber<T>> actual)
		:actual_(actual)
	{
	}

	void Request(int n)
	{
	}

	void Cancel()
	{
	}

	bool IsCancel() 
	{
	}

	void OnSubscribe(std::shared_ptr<Subscription> subscription) 
	{
	}

	void OnNext(const T &t) 
	{
	}

	void RunTask() 
	{
	}

	void OnComplete() 
	{
	}
public:
	std::shared_ptr<Subscriber<T>> actual_;
	SafeQueue<T> queue_;
};
