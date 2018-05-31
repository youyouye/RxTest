#pragma once
#include <atomic>
#include "../rx_base.hpp"
#include "../rx_flowable.hpp"
template<typename T>
class Flowable;

template<typename T>
class FlowableJust :public Flowable<T>
{
public:
	
	FlowableJust(const T& value)
	{
		value_ = value;
	}

	void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) override
	{
		subscriber->OnSubscribe(std::make_shared<JustSubscription<T>>(subscriber, value_));
	}
private:
	T value_;
};

template<typename T>
class JustSubscription : public Subscription
{
public:
	JustSubscription(std::shared_ptr<Subscriber<T>> subscriber, const T& value)
		:subscriber_(subscriber), value_(value)
	{
	}

	void Request(int n) override
	{
		if (request_state_.compare_exchange_strong(k_no_request, k_requested))
		{
			subscriber_->OnNext(value_);
			if (request_state_ != k_cancelled)
			{
				subscriber_->OnComplete();
			}
		}
	}

	void Cancel() override
	{
		request_state_ = k_cancelled;
	}

private:
	std::shared_ptr<Subscriber<T>> subscriber_;
	T value_;
};
