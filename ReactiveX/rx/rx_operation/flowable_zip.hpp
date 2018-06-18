#pragma once
#include <atomic>
#include "../rx_base.hpp"
#include "../rx_flowable.hpp"
#include "../safe_queue.hpp"

template<typename T>
class Flowable;

template<typename T1, typename T2, typename T>
class ZipSubscriber;

template<typename T1, typename T2, typename T, typename R>
class ZipInnerSubscriber;

template<typename T1,typename T2,typename T>
class FlowableZip : public Flowable<T>
{
public:
	FlowableZip(std::shared_ptr<Flowable<T1>> source1, std::shared_ptr<Flowable<T2>> source2,
		std::function<T(const T1&, const T2&)> zipper)
		:source1_(source1),source2_(source2),zipper_(zipper)
	{
	}
	void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) override
	{
		auto zip_subscriber = std::make_shared<ZipSubscriber<T1,T2,T>>(subscriber,source1_,source2_,
			zipper_);
		zip_subscriber->RunSubscribeTask();
	}
private:
	std::shared_ptr<Flowable<T1>> source1_;
	std::shared_ptr<Flowable<T2>> source2_;
	std::function<T(const T1&, const T2&)> zipper_;
};

class ZipSubscription : public Subscription 
{
public:
	ZipSubscription() {}
	void Request(int n) 
	{
	}
	void Cancel() 
	{
	}
};

template<typename T1,typename T2,typename T>
class ZipSubscriber : public Subscriber<T>,
	public std::enable_shared_from_this<ZipSubscriber<T1,T2,T>>
{
public:
	ZipSubscriber(std::shared_ptr<Subscriber<T>> actual,std::shared_ptr<Flowable<T1>> source1, 
		std::shared_ptr<Flowable<T2>> source2,std::function<T(const T1&, const T2&)> zipper)
		:actual_(actual),source1_(source1),source2_(source2),zipper_(zipper)
	{
	}
	
	void OnSubscribe(std::shared_ptr<Subscription> subscription) override
	{
	}

	void OnNext(const T &t) override
	{
		Drain();
	}

	void OnComplete() override
	{
		complete_flag = true;
		Drain();
	}

	void RunSubscribeTask() 
	{
		auto zip_subscription = std::make_shared<ZipSubscription>();
		actual_->OnSubscribe(zip_subscription);
		auto self = shared_from_this();
		inner_1_ = std::make_shared<ZipInnerSubscriber<T1, T2, T,T1>>(self);
		inner_2_ = std::make_shared<ZipInnerSubscriber<T1, T2, T,T2>>(self);
		for (int i = 0;i<2; i++)
		{
			if (cancel_flag)
				return;
			if (i == 0)
			{
				source1_->Subscribe(inner_1_);
			}
			else if (i == 1) 
			{
				source2_->Subscribe(inner_2_);
			}
		}
	}

	void Drain()
	{
		if (player_num_.fetch_add(1) != 0)
			return;
		int missing = 1;
		for (;;)
		{
			for (;;)
			{
				int count = 0;
				int empty_count = 0;
				for (int i = 0;i < 2;i++) 
				{
					if (i == 0)
					{
						if (!store_value_flag_1)
						{
							if (CheckTerminate(i))
								return;
							if (!inner_1_->queue_.Empty())
							{
								store_value_1_ = inner_1_->queue_.Take();
								store_value_flag_1 = true;
							}
							else 
								empty_count++;
						}
						count++;
					}
					else if (i == 1) 
					{
						if (!store_value_flag_2)
						{
							if (CheckTerminate(i))
								return;
							if (!inner_2_->queue_.Empty())
							{
								store_value_2_ = inner_2_->queue_.Take();
								store_value_flag_2 = true;
							}
							else
								empty_count++;
						}
						count++;
					}
				}
				if (empty_count != 0)
				{
					break;
				}
				T value = zipper_(store_value_1_,store_value_2_);
				actual_->OnNext(value);
				store_value_flag_1 = false;
				store_value_flag_2 = false;
			}
			if (--player_num_ == 0) 
			{
				return;
			}
		}
	}
	
	bool CheckTerminate(int index) 
	{
		if (cancel_flag)
			return true;
		if (index == 0)
		{
			if (inner_1_->complete_flag && inner_1_->queue_.Empty())
			{
				actual_->OnComplete();
				return true;
			}
		}
		else if (index == 1) 
		{
			if (inner_2_->complete_flag && inner_2_->queue_.Empty())
			{
				actual_->OnComplete();
				return true;
			}
		}
		return false;
	}

private:
	std::shared_ptr<Subscriber<T>> actual_;
	std::shared_ptr<Flowable<T1>> source1_;
	std::shared_ptr<Flowable<T2>> source2_;
	std::shared_ptr<ZipInnerSubscriber<T1, T2, T,T1>> inner_1_;
	std::shared_ptr<ZipInnerSubscriber<T1, T2, T,T2>> inner_2_;
	std::function<T(const T1&, const T2&)> zipper_;
	T1 store_value_1_;
	T2 store_value_2_;
	bool store_value_flag_1 = false;
	bool store_value_flag_2 = false;
};

template<typename T1,typename T2,typename T,typename R>
class ZipInnerSubscriber : public Subscriber<R>
{
public:
	ZipInnerSubscriber(std::shared_ptr<ZipSubscriber<T1,T2,T>> parent)
		:parent_(parent)
	{
	}
	void OnSubscribe(std::shared_ptr<Subscription> subscription) override
	{
	}
	void OnNext(const R &t) override
	{
		queue_.Put(t);
		parent_->Drain();
	}
	void OnComplete() override
	{
		complete_flag = true;
		parent_->Drain();
	}
public:
	SafeQueue<R> queue_;
private:
	std::shared_ptr<ZipSubscriber<T1, T2, T>> parent_;
};