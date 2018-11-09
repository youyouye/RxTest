#pragma once
#include <atomic>
#include "../rx_base.hpp"
#include "../rx_flowable.hpp"
#include "../safe_queue.hpp"

template<typename T>
class Flowable;

template<typename T>
class MergeSubscriber;

template<typename T>
class MergeInnerSubscriber;

template<typename T>
class FlowableMerge : public Flowable<T> 
{
public:
	FlowableMerge(std::vector<std::shared_ptr<Flowable<T>>> sources)
		:sources_(sources)
	{
	}
	void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) override
	{
		auto merge_subscriber = std::make_shared<MergeSubscriber<T>>(subscriber, sources_);
		merge_subscriber->RunSubscribeTask();
	}

public:
	std::vector<std::shared_ptr<Flowable<T>>> sources_;
};

template<typename T>
class MergeSubscriber : public Subscriber<T>,
	public std::enable_shared_from_this<MergeSubscriber<T>>
{
public:
	MergeSubscriber(std::shared_ptr<Subscriber<T>> actual,
		const std::vector<std::shared_ptr<Flowable<T>>> &sources)
		:actual_(actual), sources_(sources)
	{
	}
	void OnSubscribe(std::shared_ptr<Subscription> subscription)
	{
	}
	void OnNext(const T &t)
	{
	}
	void OnComplete()
	{

	}
	void RunSubscribeTask()
	{
		wip_ = sources_.size();
		for (int i = 0; i < sources_.size() && !cancel_flag; i++)
		{
			SubscribeInner(sources_[i],i);
		}
	}
	void SubscribeInner(std::shared_ptr<Flowable<T>> source,int index)
	{
		auto inner_subscriber = std::make_shared<MergeInnerSubscriber<T>>(shared_from_this());
		if (AddInnerSubscriber(inner_subscriber,index))
		{
			source->Subscribe(inner_subscriber);
		}
	}
	bool AddInnerSubscriber(std::shared_ptr<MergeInnerSubscriber<T>> inner,int index)
	{
		inner_subscribers_.push_back(inner);
		inner->SetBindIndex(inner_subscribers_.size()-1,index);
		return true;
	}
	void Drain()
	{
		if (player_num_.fetch_add(1) == 0)
		{
			DrainLoop();
		}
	}
	void DrainLoop()
	{
		for (;;)
		{
			if (CheckTerminate())
				return;
			for (int i = 0;i < inner_subscribers_.size();i++)
			{
				if (inner_subscribers_[i]->queue_.Empty())
					continue;
				for (;;)
				{
					auto value = inner_subscribers_[i]->queue_.Take();
					actual_->OnNext(value);
					if (CheckTerminate())
						return;
					if (inner_subscribers_[i]->queue_.Empty())
						break;
				}
			}
			for (int i = 0;i < inner_subscribers_.size();i++)
			{
				if (inner_subscribers_[i]->complete_flag && inner_subscribers_[i]->queue_.Empty())
				{
					inner_subscribers_[i]->inner_complete_ = true;
				}
			}
			bool complete_flag = true;
			for (int i = 0; i < inner_subscribers_.size(); i++)
			{
				if (!inner_subscribers_[i]->inner_complete_)
				{
					complete_flag = false;
					break;
				}
			}
			if (complete_flag)
			{
				actual_->OnComplete();
				return;
			}
			if (player_num_.fetch_sub(1) == 1)
			{
				break;
			}
		}
	}

	bool CheckTerminate() 
	{
		if (cancel_flag)
		{
			return true;
		}
		return false;
	}

public:
	std::shared_ptr<Subscriber<T>> actual_;
	std::vector<std::shared_ptr<Flowable<T>>> sources_;
	std::atomic_int wip_;
	std::vector<std::shared_ptr<MergeInnerSubscriber<T>>> inner_subscribers_;
};

template<typename T>
class MergeInnerSubscriber : public Subscriber<T> 
{
public:
	MergeInnerSubscriber(std::shared_ptr<MergeSubscriber<T>> parent)
		:parent_(parent)
	{
	}
	void OnSubscribe(std::shared_ptr<Subscription> subscription) 
	{
	}
	void OnNext(const T &t) 
	{
		queue_.Put(t);
		parent_->Drain();
	}
	void OnComplete() 
	{
		complete_flag = true;
		parent_->Drain();
	}
	void SetBindIndex(int self_index,int bind_flowable_index) 
	{
		self_index_ = self_index;
		bind_flowable_index_ = bind_flowable_index;
	}
public:
	SafeQueue<T> queue_;
	bool inner_complete_ = false;
private:
	std::shared_ptr<MergeSubscriber<T>> parent_;
	int self_index_ = 0;
	int bind_flowable_index_ = 0;
};
