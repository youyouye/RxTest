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
		values_.push_back(value);
	}

	FlowableJust(const std::vector<T>& values)
		:values_(values)
	{
	}

	void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) override
	{
		auto subscription = std::make_shared<JustSubscription<T>>(subscriber, values_);
		subscriber->OnSubscribe(subscription);
		subscription->ChangeState();
	}
private:
	std::vector<T> values_;
};

template<typename T>
class JustSubscription : public Subscription
{
public:
	JustSubscription(std::shared_ptr<Subscriber<T>> subscriber, const std::vector<T>& values)
		:subscriber_(subscriber), values_(values)
	{
	}

	void Request(int n) override
	{
	}

	void Cancel() override
	{
		request_state_ = k_cancelled;
	}
	
	bool IsCancel() 
	{
		if (request_state_ == k_cancelled)
		{
			return true;
		}
		return false;
	}

	void ChangeState() 
	{
		if (request_state_.compare_exchange_strong(k_start,k_on_next))
		{
			for (int i = 0;i <values_.size() && !IsCancel();i++)
			{
				T value = values_[i];
				subscriber_->OnNext(value);
			}

			if (!IsCancel())
			{
				request_state_ = k_on_complete;
				subscriber_->OnComplete();
			}
		}
	}

private:
	std::shared_ptr<Subscriber<T>> subscriber_;
	std::vector<T> values_;
};
