#include "thread.hpp"
#include <queue>

// initialize static variables
bool Thread::_core1Started = false;
std::queue<Thread::_callQueue_t> Thread::_callQueue;