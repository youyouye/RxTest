#pragma once
#include <memory>
#include "../rx_base.hpp"
#include "../rx_flowable.hpp"

template<typename T>
class Flowable;

template<typename T>
class FlowableSubscribeOn : public Flowable<T>
{
public:
	FlowableSubscribeOn(std::shared_ptr<Flowable<T>> upstream,const ThreadType &type) 
		:upstream_(upstream),type_(type)
	{
	}

	void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) override 
	{
		auto rx_subscriber = std::make_shared<SubscribeOnSubscriber<T>>(subscriber, type_);
		subscriber->OnSubscribe(rx_subscriber);
		auto self = std::static_pointer_cast<FlowableSubscribeOn<T>>(shared_from_this());
		ScheduleManager::Instance()->PostThread(type_, [self, rx_subscriber]() {
			self->upstream_->Subscribe(rx_subscriber);
		});
	}
	
public:
	std::shared_ptr<Flowable<T>> upstream_;
	ThreadType type_;
};

template<typename T>
class SubscribeOnSubscriber : public Subscriber<T>,public Subscription,
	public std::enable_shared_from_this<SubscribeOnSubscriber<T>>
{
public:
	SubscribeOnSubscriber(std::shared_ptr<Subscriber<T>> actual,const ThreadType &type)
		:actual_(actual),type_(type)
	{
	}

	void Request(int n) override
	{
	}

	void Cancel() override
	{
	}

	void OnSubscribe(std::shared_ptr<Subscription> subscription) override
	{
		disposable_ = subscription;
	}

	void OnNext(const T &t) override
	{
		actual_->OnNext(t);
	}

	void OnComplete() override
	{
		actual_->OnComplete();
	}

private:
	std::shared_ptr<Subscriber<T>> actual_;
	ThreadType type_;
	std::shared_ptr<Subscription> disposable_;
};
