#pragma once
#include <atomic>
#include "../rx_base.hpp"
#include "../rx_flowable.hpp"
#include "../safe_queue.hpp"
template<typename T>
class Flowable;

template<typename T>
class ConcatInnerSubscriber;

template<typename T>
class ConcatSubscriber;

template<typename T>
class FlowableConcat : public Flowable<T> 
{
public:
	FlowableConcat(std::vector<std::shared_ptr<Flowable<T>>> sources)
		:sources_(sources)
	{
	}

	void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) override
	{
		auto concat_subscriber = std::make_shared<ConcatSubscriber<T>>(subscriber,sources_);
		concat_subscriber->RunSubscribeTask();
	}

public:
	std::vector<std::shared_ptr<Flowable<T>>> sources_;
};

class ConcatSubscription : public Subscription 
{
public:
	void Request(int n) override 
	{
	}
	void Cancel() override 
	{
	}
};

template<typename T>
class ConcatSubscriber : public Subscriber<T>,
	public std::enable_shared_from_this<ConcatSubscriber<T>>
{
public:
	ConcatSubscriber(std::shared_ptr<Subscriber<T>> actual, std::vector<std::shared_ptr<Flowable<T>>> source)
		:actual_(actual),sources_(source)
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
		actual_->OnSubscribe(std::make_shared<ConcatSubscription>());
	}

	void OnNext(const T &t) 
	{
		RunTask();
	}

	void OnComplete() 
	{
		complete_flag = true;
		RunTask();
	}

	void RunTask() 
	{
		if (player_num_.fetch_add(1) != 0)
			return;
		for (;;)
		{
			if (!activeing_flag_)
			{
				if (cancel_flag)
					return;
				if (flowable_index_ == sources_.size())
				{
					cancel_flag = true;
					actual_->OnComplete();
					return;
				}
				if (flowable_index_ < sources_.size())
				{
					activeing_flag_ = true;
					int temp_index = flowable_index_;
					flowable_index_++;
					sources_[temp_index]->Subscribe(GetInnerSubscriber());
				}
			}
			if ((--player_num_) == 0)
			{
				break;
			}
		}
	}

	void RunSubscribeTask()
	{
		if (flowable_index_ < sources_.size())
		{
			auto temp_index = flowable_index_; 
			flowable_index_++;
			sources_[temp_index]->Subscribe(GetInnerSubscriber());
		}
	}

	std::shared_ptr<ConcatInnerSubscriber<T>> GetInnerSubscriber() 
	{
		if (inner_subscriber_ == nullptr)
		{
			inner_subscriber_ = std::make_shared<ConcatInnerSubscriber<T>>(actual_,shared_from_this());
		}
		return inner_subscriber_;
	}

public:
	std::shared_ptr<Subscriber<T>> actual_;
	std::vector<std::shared_ptr<Flowable<T>>> sources_;
	int flowable_index_ = 0;
	bool activeing_flag_ = false;
	std::shared_ptr<ConcatInnerSubscriber<T>> inner_subscriber_;
};

template<typename T>
class ConcatInnerSubscriber : public Subscriber<T>
{
public:
	ConcatInnerSubscriber(std::shared_ptr<Subscriber<T>> actual,
		std::shared_ptr<ConcatSubscriber<T>> parent)
		:actual_(actual),parent_(parent)
	{
	}
	void OnSubscribe(std::shared_ptr<Subscription> subscription) 
	{
	}
	void OnNext(const T &t) 
	{
		actual_->OnNext(t);
	}
	void OnComplete() 
	{
		parent_->activeing_flag_ = false;
		parent_->RunTask();
	}
private:
	std::shared_ptr<Subscriber<T>> actual_;
	std::shared_ptr<ConcatSubscriber<T>> parent_;
};
