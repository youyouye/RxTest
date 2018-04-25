#pragma once
#include "../reactive_x.hpp"

template<typename T> class Observer;
template<typename T> class OnSubscribe;
template<typename T> class Flowable;
template<typename T,typename R>
class FlowableTransformer;

template<typename T,typename R>
class FlowableFlatMap : public Flowable<R> 
{
public:
	FlowableFlatMap(std::shared_ptr<Flowable<T>> source, std::shared_ptr<FlowableTransformer<T, R>> transformer)
		:actual_(source),transformer_(transformer)
	{
	}

	void SubscribeActual(std::shared_ptr<Observer<R>> subscriber) override
	{
		std::shared_ptr<Observer<T>> flatmap_subscriber = std::make_shared<FlatMapSubscribe<T,R>>(subscriber, transformer_);
		actual_->Subscribe(flatmap_subscriber);
	}

private:
	std::shared_ptr<Flowable<T>> actual_;
	std::shared_ptr<FlowableTransformer<T, R>> transformer_;
};

template<typename T,typename R>
class FlatMapSubscribe : public Observer<T> 
{
public:
	FlatMapSubscribe(std::shared_ptr<Observer<R>> subscirber, std::shared_ptr<FlowableTransformer<T, R>> transformer)
		:subscriber_(subscirber),transformer_(transformer)
	{
		Init();
	}
	void Init() 
	{
		this->on_complete_ = [this]() {
			subscriber_->OnCompleted();
		};
		this->on_error_ = [this]() {
			subscriber_->OnError();
		};
		this->on_next_ = [this](T var) {
			transformer_->func_(var)->Subscribe(subscriber_);
		};
	}
public:
	std::shared_ptr<Observer<R>> subscriber_;
	std::shared_ptr<FlowableTransformer<T, R>> transformer_;
};
