#pragma once
#include <memory>
#include <functional>
#include <tuple>
#include <utility>
#include "schedule_manager.h"

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
	void SetOnCompletion(const std::function<void()>& func) { on_complete_ = func; }
	void SetOnError(const std::function<void()>& func) { on_error_ = func; }
	void SetOnNext(const std::function<void(T)>& func) { on_next_ = func; }

public:
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
};

template<typename T>
class OnSubscribe
{
public:
	OnSubscribe() {}
	OnSubscribe(std::function<void(std::shared_ptr<Observer<T>>)> function) { function_ = function; }
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
	template<std::size_t I = 0, typename FuncT, typename... Tp>
	static inline typename std::enable_if<I == sizeof...(Tp), void>::type
		for_each(std::tuple<Tp...> &, FuncT) // Unused arguments are given no names.
	{ }

	template<std::size_t I = 0, typename FuncT, typename... Tp>
	static inline typename std::enable_if < I < sizeof...(Tp), void>::type
		for_each(std::tuple<Tp...>& t, FuncT f)
	{
		f(std::get<I>(t));
		for_each<I + 1, FuncT, Tp...>(t, f);
	}

	Flowable(std::shared_ptr<OnSubscribe<T>> onscriber) 
	{
		on_subscribe_ = onscriber;
	}
	virtual ~Flowable() {};
	
	void SetObSubcribe(std::shared_ptr<OnSubscribe<T>> onsubscribe)
	{
		this->on_subscribe_ = onsubscribe;
	}

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

	std::shared_ptr<Flowable<T>> SubscribeOn(const ThreadType& type)
	{
		auto on_subscribe = std::make_shared<OnSubscribe<T>>();
		std::shared_ptr<Flowable<T>> self = shared_from_this();
		auto prev_subscribe = self->on_subscribe_;
		on_subscribe->SetSubscribeCallback([prev_subscribe,type](std::shared_ptr<Observer<T>> observer) {
			observer->OnStart();
			ScheduleManager::Instance()->PostThread(type, [prev_subscribe,observer]() {
				prev_subscribe->function_(observer);
			});
		});
		this->SetObSubcribe(on_subscribe);
		return shared_from_this();
	}

	std::shared_ptr<Flowable<T>> ObserveOn(const ThreadType& type)
	{
		auto on_subscribe = std::make_shared<OnSubscribe<T>>();
		auto prev_subscribe = this->on_subscribe_;
		on_subscribe->SetSubscribeCallback([type,prev_subscribe](std::shared_ptr<Observer<T>> observer) {
			observer->OnStart();
			auto prev_on_completion = observer->on_complete_;
			auto prev_on_error = observer->on_error_;
			auto prev_on_next = observer->on_next_;
			observer->SetOnCompletion([type, observer, prev_on_completion]() {
				ScheduleManager::Instance()->PostThread(type, [prev_on_completion]() {
					prev_on_completion();
				});
			});

			observer->SetOnError([type, observer, prev_on_error]() {
				ScheduleManager::Instance()->PostThread(type, [observer, prev_on_error]() {
					prev_on_error();
				});
			});
			observer->SetOnNext([type, observer, prev_on_next](T var) {
				ScheduleManager::Instance()->PostThread(type, [observer, var, prev_on_next]() {
					prev_on_next(var);
				});
			});
			prev_subscribe->function_(observer);
		});
		this->SetObSubcribe(on_subscribe);
		return shared_from_this();
	}

	static std::shared_ptr<Flowable<T>> Just(T item) 
	{
		auto on_subscrice = std::make_shared<OnSubscribe<T>>();
		auto func = [item](std::shared_ptr<Observer<T>> subsriber) {
			subsriber->OnNext(item);
		};
		on_subscrice->SetSubscribeCallback(func);
		return Flowable<T>::Instance(on_subscrice);
	}
	template<typename ...Types>
	static std::shared_ptr<Flowable<T>> Just(Types ...args) 
	{
		auto on_subscrice = std::make_shared<OnSubscribe<T>>();
		auto func = [args...](std::shared_ptr<Observer<T>> subsriber) {
			auto arg = std::forward_as_tuple(args...);
			for_each(arg, [subsriber](auto x) {
				subsriber->OnNext(x);
			});
		};
		on_subscrice->SetSubscribeCallback(func);
		return Flowable<T>::Instance(on_subscrice);
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


template<typename T>
class Subject : public Observer<T>, public Flowable<T> 
{
public:
	virtual ~Subject() {}
	bool HasObservers() { return false; }
	bool HasComplete() { return false; }
};
//不懂什么用,暂时不写了;
/*
template<typename T>
class AsyncSubject : public Subject<T> 
{
public:
	AsyncSubject() {}
	~AsyncSubject() {}
	static std::shared_ptr<AsyncSubject<T>> Instance()
	{
		return std::make_shared<AsyncSubject<T>>();
	}

	void Init() 
	{
		this->SetOnNext([](T var) {
			value_ = var;
		});
		this->SetOnCompletion([]() {
			
		});
		this->SetOnError(()[] {
			
		});
	}


private:
	std::shared_ptr<Observer> subscribers_;
	T value_;
};
*/















