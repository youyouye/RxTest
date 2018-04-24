#pragma once
#include "../reactive_x.hpp"
template<typename T> class Observer;
template<typename T> class OnSubscribe;
template<typename T> class Flowable;

template<typename T>
class FlowableConcat : public Flowable<T>
{
public:

	FlowableConcat(const std::vector<std::shared_ptr<Flowable<T>>> &params)
		:params_(params)
	{
	}

	void SubscribeActual(std::shared_ptr<Observer<T>> subscriber) override
	{
		auto concat_subscriber = std::make_shared<ConcatSubscriber<T>>(params_, subscriber);
		if (params_.size() > 0)
		{
			params_[0]->on_subscribe_->function_(concat_subscriber);
		}
	}

private:
	std::vector<std::shared_ptr<Flowable<T>>> params_;
};

template<typename T>
class ConcatSubscriber : public Observer<T>
{
public:
	ConcatSubscriber(const std::vector<std::shared_ptr<Flowable<T>>> &params,
		std::shared_ptr<Observer<T>> actual)
		:params_(params),count_(params.size()),actual_(actual)
	{
		Init();
	}

	void Init()
	{
		this->SetOnCompletion([this]() {
			if (index_ < count_)
			{
				index_++;
				params_[index_]->on_subscribe_->function_(shared_from_this());
			}
		});
	
		this->SetOnError([this]() {

		});

		this->SetOnNext([this](T var) {
			actual_->OnNext(var);
		});

	}
private:
	size_t index_ = 1;
	size_t count_ = 0;
	std::vector<std::shared_ptr<Flowable<T>>> params_;
	std::shared_ptr<Observer<T>> actual_;
};
