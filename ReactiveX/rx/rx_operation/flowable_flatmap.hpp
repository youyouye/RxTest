#pragma once
#include <atomic>
#include "../rx_base.hpp"
#include "../rx_flowable.hpp"
template<typename T>
class Flowable;

template<typename R,typename T>
class FlowableFlatMap : public Flowable<R>
{
public:
	FlowableFlatMap(std::shared_ptr<Flowable<T>> source,
		const std::function<std::shared_ptr<Flowable<R>>(const T& item)>& mapper)
		:source_(source),mapper_(mapper)
	{
	}

	void SubscribeActual(std::shared_ptr<Subscriber<R>> subscriber) 
	{
	}

private:
	std::shared_ptr<Flowable<T>> source_;
	std::function<std::shared_ptr<Flowable<R>>(const T& item)> mapper_;
};

template<typename R,typename T>
class MergeSubscriber : public Subscriber<R>, public Subscription 
{
public:
	MergeSubscriber(std::shared_ptr<Subscriber<R>> actual,
		const std::function<std::shared_ptr<Flowable<R>>(const T& item)>& mapper)
		:downstream_(actual),mapper_(mapper)
	{
	}

	void OnSubscribe(std::shared_ptr<Subscription> subscription) override
	{
		
	}
	void OnNext(const T &t) override
	{
	}
	void OnComplete() override
	{
	}
	void OnError(std::shared_ptr<Error> error) override
	{
	}
	void Request(int n) override
	{
	}
	void Cancel() override
	{
	}

private:
	std::shared_ptr<Subscriber<R>> downstream_;
	std::function<std::shared_ptr<Flowable<R>>(const T& item)>& mapper_;
	std::shared_ptr<Subscription> upstream_;

	bool done_;
	bool cancelled_;
};
