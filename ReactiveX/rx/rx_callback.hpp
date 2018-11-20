#pragma once
#include <functional>
#include "rx_base.hpp"

typedef std::function<void(std::shared_ptr<Subscription>)> SubscribeCallback;
typedef std::function<void()> CompleteCallback;
typedef std::function<void(std::shared_ptr<Error>)> ErrorCallabck;