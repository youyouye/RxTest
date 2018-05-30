#pragma once
#include "rx_callable.hpp"

template<typename T>
class FlowableEmitter 
{
public:
	virtual ~FlowableEmitter(){}

	virtual void SetCancellable(CancelCallback callback) {}

	virtual int Requested() {}

	virtual bool IsCancelled() {}

	virtual std::shared_ptr<FlowableEmitter<T>> Serialize() {}

	virtual void OnNext(const T& value) {}

	virtual void OnError() {}

	virtual void OnComplete() {}
};

