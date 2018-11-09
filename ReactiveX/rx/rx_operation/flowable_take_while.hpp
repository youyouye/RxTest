#pragma once
#include <atomic>
#include "../rx_base.hpp"
#include "../rx_flowable.hpp"
#include "../safe_queue.hpp"

template<typename T>
class Flowable;

template<typename T>
class TakeWhileSubscriber;

template<typename T>
class FlowableTakeWhile : public Flowable<T>
{
public:
	FlowableTakeWhile(std::shared_ptr<Flowable<T>> source,
		const std::function<bool(const T& item)> &predicate)
		:source_(source), predicate_(predicate)
	{
	}

	void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) override
	{
		auto take_while_subscriber = std::make_shared<TakeWhileSubscriber<T>>(subscriber,predicate_);
		source_->Subscribe(take_while_subscriber);
	}

private:
	std::shared_ptr<Flowable<T>> source_;
	std::function<bool(const T& item)> predicate_;
};

template<typename T>
class TakeWhileSubscriber : public Subscriber<T> ,
	public Subscription,
	public std::enable_shared_from_this<TakeWhileSubscriber<T>>
{
public:
	TakeWhileSubscriber(std::shared_ptr<Subscriber<T>> actual,
		const std::function<bool(const T& item)> predicate)
		:downstream_(actual),predicate_(predicate)
	{
	}
	void Request(int n) override {}
	void Cancel() override {}

	void OnSubscribe(std::shared_ptr<Subscription> subscription) override
	{
		upstream_ = subscription;
//		downstream_->OnSubscribe(shared_from_this());
	}

	void OnNext(const T &t) override
	{
		if (done_)
			return;
		bool result = predicate_(t);
		if (!result)
		{
			done_ = true;
			upstream_->Cancel();
			return;
		}
		downstream_->OnNext(t);
	}

	void OnComplete() override
	{
		if (done_)
			return;
		done_ = true;
		downstream_->OnComplete();
	}

private:
	std::shared_ptr<Subscriber<T>> downstream_;
	std::function<bool(const T& item)> predicate_;
	std::shared_ptr<Subscription> upstream_;
	bool done_ = false;
};

















