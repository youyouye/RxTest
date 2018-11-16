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
		:values_({value})
	{
	}
	FlowableJust(const T& item1, const T& item2)
		:values_({ item1,item2 })
	{
	}
	FlowableJust(const T& item1, const T& item2, const T& item3)
		:values_({ item1,item2,item3 })
	{
	}
	FlowableJust(const T& item1, const T& item2, const T& item3, const T& item4)
		:values_({ item1,item2,item3,item4 })
	{
	}
	FlowableJust(const T& item1, const T& item2, const T& item3, const T& item4, const T& item5)
		:values_({ item1,item2,item3,item4,item5 })
	{
	}

	void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) override
	{
		auto subscription = std::make_shared<JustSubscription<T>>(subscriber, values_);
		subscriber->OnSubscribe(subscription);
	}
private:
	std::vector<T> values_;
};

template<typename T>
class JustSubscription : public Subscription
{
public:
	JustSubscription(std::shared_ptr<Subscriber<T>> actual, const std::vector<T>& values)
		:downstream_(subscriber), values_(values)
	{
	}

	void Request(int r) override
	{
		request_num_ += r;
		SlowPath(r);
	}
	
	void SlowPath(int r) 
	{
		int e = 0;
		int f = values_.size();
		int i = index_;
		for (;;)
		{
			while (e != r && i != f)
			{
				if (cancelled_)
					return;
				T t = values_[i];
				downstream_->OnNext(t);
				e++;
				i++;
			}
			if (i == f)
			{
				if (!cancelled_)
				{
					downstream_->OnComplete();
				}
				return;
			}
			if (e == request_num_)
			{
				index_ = i;
				r = request_num_ - e;
				if (r == 0)
				{
					return;
				}
				e = 0;
			}
		}
	}

	void Cancel() override
	{
		cancelled_ = true;
	}

private:
	std::shared_ptr<Subscriber<T>> downstream_;
	std::vector<T> values_;
	std::atomic_bool cancelled_ = false;
	int index_ = 0;
	std::atomic_int request_num_ = 0;
};
