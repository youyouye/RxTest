#pragma once
#include <atomic>
#include "../rx_base.hpp"
#include "../rx_flowable.hpp"
template<typename T>
class Flowable;

template<typename T>
class FlowableSubscribeOn : public Flowable<T> 
{
public:
	FlowableSubscribeOn(std::shared_ptr<Flowable<T>> source, ThreadType type) 
		:source_(source),type_(type)
	{
	}

	void SubscribeActual(std::shared_ptr<Subscriber<T>> subscriber) override
	{
		auto sos = std::make_shared<SubscribeOnSubscriber<T>>(subscriber,type_,source_);
		subscriber->OnSubscribe(sos);
		sos->RunInThread();
	}
	std::shared_ptr<Flowable<T>> source_;
	ThreadType type_;
};

template<typename T>
class SubscribeOnSubscriber : public Subscriber<T>, public Subscription,
	public std::enable_shared_from_this<SubscribeOnSubscriber<T>>
{
public:
	SubscribeOnSubscriber(std::shared_ptr<Subscriber<T>> actual, ThreadType type, std::shared_ptr<Publisher<T>>
		source) 
		:downstream_(actual),type_(type), source_(source)
	{
	}

	void RunInThread() 
	{
		std::shared_ptr<Publisher<T>> src = source_;
		source_ = nullptr;
		src->Subscribe(shared_from_this());
	}

	void OnSubscribe(std::shared_ptr<Subscription> subscription) override
	{
		this->upstream_ = subscription;
		int r = request_num_;
		if (request_num_ != 0)
		{
			request_num_ = 0;
			RequestUpstream(r,subscription);
		}
	}
	void OnNext(const T &t) override
	{
		downstream_->OnNext(t);
	}
	void OnComplete() override
	{
		downstream_->OnComplete();
	}
	void OnError(std::shared_ptr<Error> error) override
	{
		downstream_->OnError(error);
	}
	void Request(int n) override
	{
		if (upstream_ != nullptr)
			RequestUpstream(n, upstream_);
		else 
			request_num_ += n;
	}
	void Cancel() override
	{
		upstream_->Cancel();
	}
	void RequestUpstream(int n, std::shared_ptr<Subscription> s) 
	{
		auto self = shared_from_this();
		ScheduleManager::Instance()->PostThread(type_,[self,n]() {
			self->upstream_->Request(n);
		});
	}

private:
	std::shared_ptr<Subscriber<T>> downstream_ = nullptr;
	ThreadType type_;
	std::shared_ptr<Publisher<T>> source_ = nullptr;
	std::shared_ptr<Subscription> upstream_ = nullptr;
	std::atomic_int request_num_ = 0;
};
