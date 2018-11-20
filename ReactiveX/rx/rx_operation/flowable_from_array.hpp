#pragma once
#include <atomic>
#include "../rx_base.hpp"
#include "../rx_flowable.hpp"
template<typename T>
class Flowable;

template<typename T>
class FlowableFromArray
{
public:
	FlowableFromArray(const std::vector<T> &items)
		:items_(items)
	{
	}

	void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) override
	{
		auto array_subscription = std::make_shared<FromArraySubscription>(subscriber,items_);
		subscriber->OnSubscribe(array_subscription);
	}

private:
	std::vector<T> items_;
};

template<typename T>
class FromArraySubscription : public Subscription 
{
public:
	FromArraySubscription(std::shared_ptr<Subscriber<T>> actual,const std::vector<T> &items)
		:downstream_(actual),items_(items)
	{
	}
	
	void Request(int n) override
	{
		request_num_.fetch_add(n);
		SlowPath(n);
	}
	
	void Cancel() override
	{
		cancelled_ = true;
	}

	void SlowPath(int n) 
	{
		int e = 0;
		int f = items_.size();
		int i = index_;
		for (;;)
		{
			while (e != n && i != f)
			{
				if (cancelled_)
					return;
				T t = items_[i];
				downstream_->OnNext(t);
				e++;
				i++;
			}
			if (i == f)
			{
				if (!cancelled_)
					downstream_->OnComplete();
				return;
			}
			if (e == request_num_)
			{
				index_ = i;
				request_num_ = request_num_ - e;
				if (request_num_ == 0)
					return;
				e = 0;
			}
		}
	}

private:
	std::shared_ptr<Subscriber<T>> downstream_ = nullptr;
	std::vector<T> items_;
	bool cancelled_ = false;
	std::atomic_int request_num_ = 0;
	int index_ = 0;
};

