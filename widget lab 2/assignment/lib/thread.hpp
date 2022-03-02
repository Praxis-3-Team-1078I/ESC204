#ifndef THREAD_HPP
#define THREAD_HPP

#include "pico/multicore.h"
#include "pico/sync.h"
#include <functional>
#include <memory>
#include <queue>
#include <type_traits>

class Thread
{
  private:
    struct _callQueue_t
    {
        std::function<void()> f;
        std::shared_ptr<bool> finished_ptr;
        _callQueue_t(std::function<void()> _f, std::shared_ptr<bool> _finished_ptr);
    };

    static bool _core1Started;
    static std::queue<_callQueue_t> _callQueue;

    static void _core1_entry();

    std::shared_ptr<bool> _finished_ptr = std::make_shared<bool>(false);
    bool _joinable = false;

  public:
    // since this is templated code, must write implementation here
    template <class Callable, class... Args> Thread(Callable &&f, Args &&...args)
    {
        static_assert(std::is_invocable<typename std::decay<Callable>::type, typename std::decay<Args>::type...>::value,
                      "thread arguments must be invocable after conversion to rvalues");

        if (!_core1Started)
        {
            multicore_launch_core1(this->_core1_entry);
            multicore_fifo_push_blocking(1);
            _core1Started = true;
        }
        _joinable = true;
        std::function<void()> binded_f = std::bind(f, args...);
        _callQueue.push(_callQueue_t(binded_f, _finished_ptr));
    }

    ~Thread();

    bool joinable();

    void join();

    void detach();
};

#endif