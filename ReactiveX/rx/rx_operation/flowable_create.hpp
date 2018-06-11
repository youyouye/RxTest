#pragma once
#include <atomic>
#include "../rx_base.hpp"
#include "../rx_flowable.hpp"
template<typename T>
class Flowable;

template<typename T>
class FlowableCreate : public Flowable<T>
{
public:
	FlowableCreate(std::shared_ptr<FlowableOnSubscribe<T>> source)
		:source_(source)
	{
	}

	void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) override
	{
		auto create_subscriber = std::make_shared<CreateSubscriber<T>>(subscriber);
		subscriber->OnSubscribe(create_subscriber);
		source_->Subscribe(create_subscriber);
	}

private:
	std::shared_ptr<FlowableOnSubscribe<T>> source_;
};

template<typename T>
class CreateSubscriber : public FlowableEmitter<T>,
	public Subscription
{
public:
	CreateSubscriber(std::shared_ptr<Subscriber<T>> actual)
		:actual_(actual)
	{
	}

	void Request(int n) override 
	{
	}

	bool IsCanceled()
	{
		return false;
	}

	void Cancel() override 
	{
	}

	void OnNext(const T &t) override
	{
		if (!IsCanceled())
		{
			actual_->OnNext(t);
		}
	}

	void OnComplete() override
	{
		if (!IsCanceled())
		{
			actual_->OnComplete();
		}
	}
private:
	std::shared_ptr<Subscriber<T>> actual_;
};
