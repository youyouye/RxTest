#pragma once
#include <atomic>
#include "../rx_base.hpp"
#include "../rx_flowable.hpp"
#include "../safe_queue.hpp"

template<typename T>
class Flowable;

template<typename T, typename R>
class FlatMapSubscriber;

template<typename T, typename R>
class FlatMapInnerSubscriber;

template<typename T,typename R>
class FlowableFlatMap : public Flowable<R>
{
public:
	FlowableFlatMap(std::shared_ptr<Flowable<T>> source,
		const std::function<std::shared_ptr<Flowable<R>>(const T& item)> &mapper)
		:source_(source),mapper_(mapper)
	{
	}
	
	void SubscribeActual(std::shared_ptr<Subscriber<R>> subscriber) override
	{
		auto flatmap_subscriber = std::make_shared<FlatMapSubscriber<T,R>>(subscriber, mapper_);
		source_->Subscribe(flatmap_subscriber);
	}

public:
	std::shared_ptr<Flowable<T>> source_;
	std::function<std::shared_ptr<Flowable<R>>(const T& item)> mapper_;
};

template<typename T,typename R>
class FlatMapSubscriber : public Subscriber<T>,
	public std::enable_shared_from_this<FlatMapSubscriber<T,R>>
{
public:
	FlatMapSubscriber(std::shared_ptr<Subscriber<R>> subscriber,
		const std::function<std::shared_ptr<Flowable<R>>(const T& item)> &mapper)
		:actual_(subscriber), mapper_(mapper)
	{
	}
	//for next time
	void OnSubscribe(std::shared_ptr<Subscription> subscription) 
	{
	}

	void OnNext(const T &t) 
	{
		if (complete_flag)
			return;
		auto rx_flowable = mapper_(t);
		SubscribeInner(rx_flowable);
	}

	void OnComplete() 
	{
		if (complete_flag)
		{
			return;
		}
		complete_flag = true;
		Drain();
	}

	void SubscribeInner(std::shared_ptr<Flowable<R>> flowable) 
	{
		auto rx_inner = std::make_shared<FlatMapInnerSubscriber<T,R>>(shared_from_this());
		if (AddInnerSubscriber(rx_inner))
		{
			flowable->Subscribe(rx_inner);
		}
	}
	
	bool AddInnerSubscriber(std::shared_ptr<FlatMapInnerSubscriber<T, R>> inner)
	{
		inner_subscribers_.push_back(inner);
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
			for (int i = 0; i < inner_subscribers_.size(); i++)
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
			for (int i = 0; i < inner_subscribers_.size(); i++)
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

	void TryEmit(const R &value,std::shared_ptr<FlatMapInnerSubscriber<T,R>> inner)
	{
		if (player_num_ == 0 && player_num_.compare_exchange_strong(no_player,one_player))
		{
			actual_->OnNext(value);
			if ((--player_num_) == 0)
				return;
		}
		else 
		{
			inner->queue_.Put(value);
			if (player_num_.fetch_add(1) != 0)
			{
				return;
			}
		}
		DrainLoop();
	}
private:
	std::shared_ptr<Subscriber<R>> actual_;
	std::function<std::shared_ptr<Flowable<R>>(const T& item)> mapper_;
	std::vector<std::shared_ptr<FlatMapInnerSubscriber<T,R>>> inner_subscribers_;
	int no_player = 0;
	int one_player = 1;
};

template<typename T,typename R>
class FlatMapInnerSubscriber : public Subscriber<R>,
	public std::enable_shared_from_this<FlatMapInnerSubscriber<T,R>>
{
public:
	FlatMapInnerSubscriber(std::shared_ptr<FlatMapSubscriber<T,R>> parent)
		:parent_(parent)
	{
	}

	void OnSubscribe(std::shared_ptr<Subscription> subscription) 
	{
	}

	void OnNext(const R &t) 
	{
		parent_->TryEmit(t,shared_from_this());
	}

	void OnComplete() 
	{
		complete_flag = true;
		parent_->Drain();
	}
public:
	SafeQueue<R> queue_;
	bool inner_complete_ = false;
private:
	std::shared_ptr<FlatMapSubscriber<T, R>> parent_;
};