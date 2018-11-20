#pragma once
#include <atomic>
#include "../rx_base.hpp"
#include "../rx_flowable.hpp"
#include "../rx_subscription/subscription_arbiter.hpp"

template<typename T>
class Flowable;

template<typename T>
class FlowableConcat : public Flowable<T>
{
public:
	FlowableConcat(const std::vector<std::shared_ptr<Publisher<T>>> &sources) 
		:sources_(sources)
	{
	}

	void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) override
	{
		auto concat_subscriber = std::make_shared<ConcatSubscriber<T>>(sources_, subscriber);
		subscriber->OnSubscribe(concat_subscriber);
		concat_subscriber->OnComplete();
	}
private:
	std::vector<std::shared_ptr<Publisher<T>>> sources_;
};

template<typename T>
class ConcatSubscriber : public Subscriber<T>, public SubscriptionArbiter,
	public std::enable_shared_from_this<ConcatSubscriber<T>>
{
public:
	ConcatSubscriber(std::vector<std::shared_ptr<Publisher<T>>> source,std::shared_ptr<Subscriber> downstream) 
		:sources_(source),downstream_(downstream)
	{
	}
	
	void OnSubscribe(std::shared_ptr<Subscription> subscription) override
	{
		SetSubscription(subscription);
	}

	void OnNext(const T &t) override
	{
		produced_++;
		downstream_->OnNext(t);
	}

	void OnComplete() override
	{
		if (process_num_.fetch_add(1) == 0)
		{
			int n = sources_.size();
			int i = index;
			for (;;)
			{
				if (i == n)
				{
					downstream_->OnComplete();
					return;
				}
				std::shared_ptr<Publisher<T>> p = sources_.at(i);
				int r = produced_;
				if (r != 0)
				{
					produced_ = 0;
					Produced(r);
				}
				auto self = shared_from_this();
				p->Subscribe(self);
				index = ++i;
				if ((process_num_ = process_num_ - 1) == 0)
				{
					break;
				}
			}
		}
	}

	void OnError(std::shared_ptr<Error> error) override
	{
		downstream_->OnError(error);
	}

private:
	std::vector<std::shared_ptr<Publisher<T>>> sources_ = nullptr;
	std::shared_ptr<Subscriber<T>> downstream_ = nullptr;
	int index = 0;
	std::atomic_int process_num_ = 0;
	int produced_ = 0;
};








