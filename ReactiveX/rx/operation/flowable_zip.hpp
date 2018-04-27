#pragma once
#include "../reactive_x.hpp"

template<typename T> class Observer;
template<typename T> class OnSubscribe;
template<typename T> class Flowable;

template<typename T, typename T1, typename T2>
class ZipSubscriber2;

template<typename T, typename R,typename T1,typename T2>
class ZipWrap : public Observer<T>
{
public:
	ZipWrap(std::shared_ptr<ZipSubscriber2<R,T1,T2>> subscriber)
		:subscriber_(subscriber)
	{
		Init();
	}
	void Init() 
	{
		this->SetOnCompletion([this]() {
			subscriber_->OnCompleted();
		});
		this->SetOnError([this]() {
			subscriber_->OnError();
		});
		this->SetOnNext([this](T var) {
			subscriber_->ReceiveParam(var);
		});
	}

private:
	std::shared_ptr<ZipSubscriber2<R, T1, T2>> subscriber_;
};

template<typename T,typename T1,typename T2>
class FlowableZip2 :public Flowable<T> 
{
public:
	FlowableZip2(std::shared_ptr<Flowable<T1>> observer_one, std::shared_ptr<Flowable<T2>> observer_two, std::function<T(T1, T2)> func)
		:t1_(observer_one),t2_(observer_two),func_(func)
	{
	}

	void SubscribeActual(std::shared_ptr<Observer<T>> subscriber) override
	{
		auto zip_subscriber = std::make_shared<ZipSubscriber2<T,T1,T2>>(subscriber,t1_,t2_,func_);
		std::shared_ptr<Observer<T1>> temp_t1 = std::make_shared<ZipWrap<T1,T, T1, T2>>(zip_subscriber);
		std::shared_ptr<Observer<T2>> temp_t2 = std::make_shared<ZipWrap<T2, T, T1, T2>>(zip_subscriber);
		t1_->Subscribe(temp_t1);
		t2_->Subscribe(temp_t2);
	}

private:
	std::shared_ptr<Flowable<T1>> t1_;
	std::shared_ptr<Flowable<T2>> t2_;
	std::function<T(T1, T2)> func_;
};

template<typename T,typename T1,typename T2>
class ZipSubscriber2 : public Observer<T>
{
public:
	ZipSubscriber2(std::shared_ptr<Observer<T>> subscriber,std::shared_ptr<Flowable<T1>> t1,std::shared_ptr<Flowable<T2>> t2,std::function<T(T1,T2)> func)
		:subscriber_(subscriber),t1_(t1),t2_(t2),func_(func),total_(2)
	{
		Init();
	}
	void Init() 
	{
		this->SetOnCompletion([this]() {
			subscriber_->OnCompleted();
		});
		this->SetOnError([this]() {
			subscriber_->OnError();
		});
		this->SetOnNext([this](T var) {
			subscriber_->OnNext(var);
		});
	}
	void ReceiveParam(T1 t1) 
	{
		num_++;
		receive_one_ = t1;
		CheckReceiveAll();
	}
	void ReceiveParam(T2 t2) 
	{
		num_++;
		receive_two_ = t2;
		CheckReceiveAll();
	}
private:
	void CheckReceiveAll() 
	{
		if (num_ == total_)
		{
			num_ = 0;
			OnNext(func_(receive_one_, receive_two_));
		}
	}
private:
	size_t num_ = 0;
	size_t total_;
	T1 receive_one_;
	T2 receive_two_;
	std::shared_ptr<Observer<T>> subscriber_;
	std::shared_ptr<Flowable<T1>> t1_;
	std::shared_ptr<Flowable<T2>> t2_;
	std::function<T(T1, T2)> func_;
};



