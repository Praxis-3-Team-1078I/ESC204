#include "thread.hpp"
#include <functional>
#include <queue>

Thread::_callQueue_t::_callQueue_t(std::function<void()> _f, std::shared_ptr<bool> _finished_ptr)
{
    f = _f;
    finished_ptr = _finished_ptr;
}

void Thread::_core1_entry()
{
    while (true)
    {
        // wait for instructions to arrive
        while (_callQueue.empty())
        {
            tight_loop_contents();
        }
        // pop it off the queue, call, and then mark as done running
        _callQueue_t toDo = _callQueue.front();
        std::invoke(toDo.f);
        *(toDo.finished_ptr) = true;
        _callQueue.pop();
    }
}

Thread::~Thread()
{
    // thread not detached or joined by the time we go out of scope, error
    if (_joinable)
    {
        std::terminate();
    }
}

bool Thread::joinable()
{
    return _joinable;
}

void Thread::join()
{
    // cant join if already joined or detached
    if (!_joinable)
    {
        std::terminate();
    }
    // cant join from core 1
    else if (multicore_fifo_rvalid())
    {
        std::terminate();
    }
    // wait for core 1 to finish running our function
    while (!(*_finished_ptr))
    {
        tight_loop_contents();
    }
    _joinable = false;
}

void Thread::detach()
{
    // cant detach something we have already joined or detached
    if (!_joinable)
    {
        std::terminate();
    }
    _joinable = false;
}

// initialize static variables
bool Thread::_core1Started = false;
std::queue<Thread::_callQueue_t> Thread::_callQueue;