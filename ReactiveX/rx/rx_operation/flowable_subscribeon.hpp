#pragma once
#include "../rx_flowable.hpp"

template<typename T>
class FlowableSubscribeOn 
{
public:
	FlowableSubscribeOn(std::shared_ptr<Flowable<T>> source,ThreadType type)
		source_(source),type_(type)
	{
	}

	void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) override
	{
		auto temp = std::make_shared<SubscribeOnSubscriber<T>>(subscriber,type_,source_);
		subscriber->OnSubscribe(temp);
		ScheduleManager()->Instance()->PostThread(type_, [source_]() {
			source_->Subscribe(temp);
		});
	}

private:
	std::shared_ptr<Flowable<T>> source_;
	ThreadType type_;
};

template<typename T>
class SubscribeOnSubscriber :public Subscriber<T>,public Subscription
{
public:
	SubscribeOnSubscriber(std::shared_ptr<Subscriber<T>> actual, ThreadType type, std::shared_ptr<Flowable<T>> source)
		:actual_(actual),type_(type),source_(source)
	{
	}

	void OnSubscribe(std::shared_ptr<Subscription> subscription) override
	{
		int r = request_num_.fetch_add(-request_num_);
		if (r != 0)
		{
			RequestUpstream(r, subscription);
		}
	}

	void OnNext(const T& t) override
	{
		actual_->OnNext(t);
	}

	void OnComplete() override
	{
		actual_->OnComplete();
		//dispose
	}

	void OnCancel() override 
	{
		//dispose
	}

	void Request(long n) override 
	{
		RequestUpstream(n);
	}

	void RequestUpstream(long n, std::shared_ptr<Subscription> subscription)
	{
		//current thread
//		subscription->Request(n);
		ScheduleManager()->Instance()->PostThread(type_, [subscription]() {
			subscription->Request(n);
		});
	}

private:
	std::shared_ptr<Subscriber<T>> actual_;
	ThreadType type_;
	std::shared_ptr<Flowable<T>> source_;
	std::atomic_int request_num_;
};