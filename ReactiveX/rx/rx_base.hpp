#pragma once
#include <memory>
#include <atomic>
#include "rx_common.hpp"

enum class SubscriptionType 
{
	k_queue_type,
	k_normal_type,
};

class Subscription
{
public:
	virtual ~Subscription() {}
	virtual void Request(int n) = 0;
	virtual void Cancel() = 0;
	void SetFusionMode(bool fusion_flag) { fusion_mode_flag = fusion_flag; }
	bool GetFusionMode() { return fusion_mode_flag; }
public:
	SubscriptionType type_ = SubscriptionType::k_normal_type;
protected:
	std::atomic_int request_state_ = 0;	//0 k_no_request,1 k_requested,2 k_cancelled
	//test num
	int k_start = 0;
	int k_on_next = 1;
	int k_on_complete = 2;
	int k_cancelled = 3;
	bool fusion_mode_flag = false;
};

template<typename T>
class Subscriber
{
public:
	virtual ~Subscriber() {}
	virtual void OnSubscribe(std::shared_ptr<Subscription> subscription) = 0;
	virtual void OnNext(const T &t) = 0;
	virtual void OnComplete() = 0;
	
	std::atomic_bool complete_flag = false;
	std::atomic_bool cancel_flag = false;
	std::atomic_int request_num_ = 0;
	std::atomic_int player_num_ = 0;
};

template<typename T>
class Publisher
{
public:
	virtual ~Publisher() {}
	virtual void Subscribe(std::shared_ptr<Subscriber<T>> subscriber) = 0;
};

template<typename T, typename R>
class Processor : public Subscriber<T>, Publisher<R>
{
	virtual ~Processor() {}
};