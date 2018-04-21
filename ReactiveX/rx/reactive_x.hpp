#pragma once
#include <memory>
#include <functional>

template<typename T>
class Observer 
{
public:
	virtual ~Observer() {}
	virtual void OnStart() {}
	virtual void OnCompleted() 
	{
		if (on_complete_ != nullptr)
		{
			on_complete_();
		}
	}
	virtual void OnError() 
	{
		if (on_error_ != nullptr) 
		{
			on_error_();
		}
	}
	virtual void OnNext(T var) 
	{
		if (on_next_ != nullptr)
		{
			on_next_(var);
		}
	}
protected:
	std::function<void()> on_complete_;
	std::function<void()> on_error_;
	std::function<void(T)> on_next_;
};

template<typename T, typename R>
class Transformer
{
public:
	void setTransformCallback(const std::function<R(T)>& func) { function_ = func; }
public:
	std::function<R(T)> function_;
};

template<typename T>
class FlowSubscribe : public Observer<T>
{
public:
	void OnStart() override {}
	void SetOnCompletion(const std::function<void()>& func) { on_complete_ = func; }
	void SetOnError(const std::function<void()>& func) { on_error_ = func; }
	void SetOnNext(const std::function<void(T)>& func) { on_next_ = func; }
};

template<typename T>
class OnSubscribe
{
public:
	virtual ~OnSubscribe() {};
	void SetSubscribeCallback(const std::function<void(std::shared_ptr<Observer<T>>)>& func) { function_ = func; }
public:
	std::function<void(std::shared_ptr<Observer<T>>)> function_;
};

template<typename T,typename R>
class MapSubscribe : public Observer<R>
{
public:
	MapSubscribe(std::shared_ptr<Observer<T>> subscirber,std::shared_ptr<Transformer<R, T>> transform)
	{
		subscirber_ = subscirber;
		transformer_ = transform;
		Init();
	}
	void Init() 
	{
		this->on_complete_ = [this]() {
			subscirber_->OnCompleted();
		};
		this->on_error_ = [this]() {
			subscirber_->OnError();
		};
		this->on_next_ = [this](R var) {
			subscirber_->OnNext(transformer_->function_(var));
		};
	}
public:
	std::shared_ptr<Observer<T>> subscirber_;
	std::shared_ptr<Transformer<R, T>> transformer_;
};

template <typename T, typename R> class MapOnSubscribe;

template<typename T>
class Flowable : public std::enable_shared_from_this<Flowable<T>>
{
public:
	Flowable(std::shared_ptr<OnSubscribe<T>> onscriber) 
	{
		on_subscribe_ = onscriber;
	}
	virtual ~Flowable() {};

	static std::shared_ptr<Flowable<T>> Instance(std::shared_ptr<OnSubscribe<T>> onsubscribe)
	{
		return std::make_shared<Flowable<T>>(onsubscribe);
	}

	void Subscribe(std::shared_ptr<Observer<T>> subscriber) 
	{
		(*subscriber).OnStart();
		(*on_subscribe_).function_(subscriber);
	}
	template<typename R>
	std::shared_ptr<Flowable<R>> map(std::shared_ptr<Transformer<T,R>> transformer)
	{
		std::shared_ptr<OnSubscribe<R>> on_subscribe = std::make_shared<MapOnSubscribe<T,R>>(shared_from_this(), transformer);
		auto flowable = Flowable<R>::Instance(on_subscribe);
		return flowable;
	}

private:
	std::shared_ptr<OnSubscribe<T>> on_subscribe_;
};

template <typename T, typename R>
class MapOnSubscribe : public OnSubscribe<R>
{
public:
	MapOnSubscribe(std::shared_ptr<Flowable<T>> flowable, std::shared_ptr<Transformer<T, R>> transform)
	{
		flowable_ = flowable;
		transformer_ = transform;
		this->SetSubscribeCallback([this](std::shared_ptr<Observer<R>> observer) {
			std::shared_ptr<Observer<T>> map_subscribe = std::make_shared<MapSubscribe<R, T>>(observer, transformer_);
			flowable_->Subscribe(map_subscribe);
		});
	}
public:
	std::shared_ptr<Flowable<T>> flowable_;
	std::shared_ptr<Transformer<T, R>> transformer_;
};