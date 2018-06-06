#pragma once
#include "../rx_base.hpp"
#include "../rx_flowable.hpp"
#include "../safe_queue.hpp"

template<typename T>
class Flowable;

template<typename T>
class FlowableObserveOn : public Flowable<T>
{
public:
	FlowableObserveOn(std::shared_ptr<Flowable<T>> upstream, const ThreadType &type)
		:upstream_(upstream), type_(type)
	{
	}

	void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) override
	{
		auto down_subscriber = std::make_shared<ObserveOnSubscriber<T>>(subscriber, type_);
		upstream_->Subscribe(down_subscriber);
	}

private:
	std::shared_ptr<Flowable<T>> upstream_;
	ThreadType type_;
};

template<typename T>
class ObserveOnSubscriber : public Subscriber<T>, public Subscription,
	public std::enable_shared_from_this<ObserveOnSubscriber<T>>
{
public:
	ObserveOnSubscriber(std::shared_ptr<Subscriber<T>> actual, const ThreadType &type)
		:actual_(actual), type_(type)
	{
	}

	void Request(int n)
	{
	}

	void Cancel()
	{
	}

	void OnSubscribe(std::shared_ptr<Subscription> subscription) override
	{
		disposable_ = subscription;
		actual_->OnSubscribe(shared_from_this());
	}

	void OnNext(const T &t) override
	{
		if (complete_flag)
			return;
		queue_.Put(t);
		TrySchedule();
	}

	void OnComplete() override
	{
		if (!complete_flag)
		{
			complete_flag = true;
			TrySchedule();
		}
	}

	void TrySchedule()
	{
		if (player_num_.fetch_add(1) != 0)
			return;
		auto self = shared_from_this();
		ScheduleManager::Instance()->PostThread(type_, [self]() {
			self->RunAsync();
		});
	}

	void RunAsync()
	{
		std::atomic_int missed = 1;
		for (;;)
		{
			if (CheckTerminal(queue_.Empty()))
				return;
			for (;;)
			{
				T value = queue_.Take();
				actual_->OnNext(value);
				if (CheckTerminal(queue_.Empty()))
					return;
			}
			missed -= missed;
			if (missed == 0)
				break;
		}
	}
	//because the empty is unreal
	bool CheckTerminal(bool empty)
	{
		if (cancel_flag)
		{
			queue_.Clear();
			return true;
		}
		if (complete_flag)
		{
			if (empty)
			{
				actual_->OnComplete();
				return true;
			}
		}
		return false;
	}

	void Clear()
	{
		queue_.Clear();
	}

private:
	std::shared_ptr<Subscriber<T>> actual_;
	ThreadType type_;

	int produced_ = 0;
	SafeQueue<T> queue_;
	std::shared_ptr<Subscription> disposable_;
};
