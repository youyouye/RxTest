#pragma once
#include <functional>
#include "rx_base.hpp"
#include "rx_callback.hpp"

template<typename T>
class FlowableSubscriber : public Subscriber<T>
{
public:
	FlowableSubscriber() {}
	void SetOnSubscribeCallback(const SubscribeCallback &callback)
	{
		on_subscribe_ = callback;
	}

	void SetOnNextCallback(const std::function<void(const T&)> &callback)
	{
		on_next_ = callback;
	}

	void SetOnCompleteCallback(const CompleteCallback &callback)
	{
		on_complete_ = callback;
	}
protected:
	void OnSubscribe(std::shared_ptr<Subscription> subscription) override 
	{
		if (on_subscribe_ != nullptr)
		{
			on_subscribe_(subscription);
		}
	}

	void OnNext(const T &t) override
	{
		if (on_next_ != nullptr)
		{
			on_next_(t);
		}
	}

	void OnComplete() override
	{
		if (on_complete_ != nullptr)
		{
			on_complete_();
		}
	}

private:
	SubscribeCallback on_subscribe_;
	std::function<void(const T&)> on_next_;
	CompleteCallback on_complete_;
};
