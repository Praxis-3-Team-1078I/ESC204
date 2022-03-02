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
        while (_callQueue.empty())
        {
            tight_loop_contents();
        }
        _callQueue_t toDo = _callQueue.front();
        std::invoke(toDo.f);
        *(toDo.finished_ptr) = true;
        _callQueue.pop();
    }
}

Thread::~Thread()
{
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
    if (!_joinable)
    {
        std::terminate();
    }
    else if (multicore_fifo_rvalid())
    {
        std::terminate();
    }
    while (!(*_finished_ptr))
    {
        tight_loop_contents();
    }
    _joinable = false;
}

void Thread::detach()
{
    if (!_joinable)
    {
        std::terminate();
    }
    _joinable = false;
}

// initialize static variables
bool Thread::_core1Started = false;
std::queue<Thread::_callQueue_t> Thread::_callQueue;