#pragma once
#include "../reactive_x.hpp"
#include "common.hpp"
#include <memory>
template<typename T> class Observer;
template<typename T> class OnSubscribe;
template<typename T> class Flowable;

template<typename T>
class FlowableObserveOn : public Flowable<T>
{
public:
	FlowableObserveOn(std::shared_ptr<Flowable<T>> source, ThreadType type)
		:actual_(source), type_(type)
	{
	}

	void SubscribeActual(std::shared_ptr<Observer<T>> subscriber) override 
	{
		auto observer_onsubscriber = std::make_shared<ObserverOnSubscriber<T>>(subscriber, type_);
		actual_->Subscribe(observer_onsubscriber);
	}

	std::shared_ptr<Flowable<T>> GetActual()
	{
		return actual_;
	}
public:
	std::shared_ptr<Flowable<T>> actual_;
	ThreadType type_;
};

template<typename T>
class ObserverOnSubscriber : public Observer<T>
{
public:
	ObserverOnSubscriber(std::shared_ptr<Observer<T>> actual,const ThreadType &type)
		:actual_(actual),type_(type)
	{
		Init();
	}
	void Init() 
	{
		auto self = GetActual();
		auto temp_type = type_;
		this->SetOnCompletion([self, temp_type]() {
			ScheduleManager::Instance()->PostThread(temp_type, [self]() {
				self->OnCompleted();
			});
		});
		this->SetOnError([self, temp_type]() {
			ScheduleManager::Instance()->PostThread(temp_type, [self]() {
				self->OnError();
			});
		});
		this->SetOnNext([self, temp_type](T var) {
			ScheduleManager::Instance()->PostThread(temp_type, [self, var]() {
				self->OnNext(var);
			});
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