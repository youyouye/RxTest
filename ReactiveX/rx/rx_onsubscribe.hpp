#pragma once
#include "rx_emitter.hpp"

template<typename T>
class FlowableOnSubscribe 
{
public:
	virtual ~FlowableOnSubscribe() {}
	virtual void Subscribe(std::shared_ptr<FlowableEmitter<T>> emitter) {}
};