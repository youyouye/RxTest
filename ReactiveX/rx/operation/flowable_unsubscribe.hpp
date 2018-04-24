#pragma once
#include "../reactive_x.hpp"

template<typename T> class Observer;
template<typename T> class OnSubscribe;
template<typename T> class Flowable;

template<typename T>
class FlowableUnsubscribeOn : public Flowable<T> 
{
public:
	FlowableUnsubscribeOn(std::shared_ptr<Flowable<T>> source,const ThreadType &type)
		:actual_(source),type_(type)
	{
	}
	
	void SubscribeActual(std::shared_ptr<Observer<T>> subscriber) override
	{
		actual_->Subscribe(std::make_shared<UnsubscribeSubscriber<T>>(subscriber,type_));
	}

	std::shared_ptr<Flowable<T>> GetActual() 
	{
		return actual_;
	}
private:
	std::shared_ptr<Flowable<T>> actual_;
	ThreadType type_;
};

template<typename T>
class UnsubscribeSubscriber : public Observer<T>
{
public:
	UnsubscribeSubscriber(std::shared_ptr<Observer<T>> actual,const ThreadType &type)
		:actual_(actual),type_(type)
	{
		Init();
	}

	void Init() 
	{
		auto self = GetActual();
		this->SetOnCompletion([self]() {
			self->OnCompleted();
		});
		this->SetOnError([self]() {
			self->OnError();
		});
		this->SetOnNext([self](T var) {
			self->OnNext(var);
		});
	}

	void Cancel() override
	{
		auto self = shared_from_this();
		ScheduleManager().Instance()->PostThread(type_, [self]() {
			self->Cancel();
		});
	}
	std::shared_ptr<Observer<T>> GetActual() override
	{
		return actual_;
	}
private:
	std::shared_ptr<Observer<T>> actual_;
	ThreadType type_;
};
