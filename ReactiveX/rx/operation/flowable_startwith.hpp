#pragma once
#include "../reactive_x.hpp"

template<typename T> class Observer;
template<typename T> class OnSubscribe;
template<typename T> class Flowable;

template<typename T>
class FlowableStartwith : public Flowable<T>
{
public:
	FlowableStartwith(std::shared_ptr<Flowable<T>> source, std::shared_ptr<Flowable<T>> startwith)
		:actual_(source),startwith_(startwith)
	{
	}

	void SubscribeActual(std::shared_ptr<Observer<T>> subscriber) override
	{
		auto parent = std::make_shared<StartwithSubscriber<T>>(subscriber, actual_);
		startwith_->Subscribe(parent);
	}
	std::shared_ptr<Flowable<T>> GetActual() override 
	{
		return actual_;
	}

private:
	std::shared_ptr<Flowable<T>> actual_;
	std::shared_ptr<Flowable<T>> startwith_;
};

template<typename T>
class StartwithSubscriber : public Observer<T>
{
public:
	StartwithSubscriber(std::shared_ptr<Observer<T>> actual,std::shared_ptr<Flowable<T>> next_flowable)
		:actual_(actual),next_flowable_(next_flowable)
	{
		Init();
	}

	void Init()
	{
		auto self = GetActual();
		this->SetOnCompletion([self, this]() {
			self->OnCompleted();
			this->next_flowable_->on_subscribe_->function_(self);
		});
		this->SetOnError([self]() {
			self->OnError();
		});
		this->SetOnNext([self,this](T var) {
			self->OnNext(var);
			this->OnCompleted();
		});
	}
	std::shared_ptr<Observer<T>> GetActual() override
	{
		return actual_;
	}
private:
	std::shared_ptr<Observer<T>> actual_;
	std::shared_ptr<Flowable<T>> next_flowable_;
};
