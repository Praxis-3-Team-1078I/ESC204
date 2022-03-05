#ifndef THREAD_HPP
#define THREAD_HPP

#include "pico/multicore.h"
#include "pico/sync.h"
#include <functional>
#include <memory>
#include <queue>
#include <type_traits>

// An abstraction of the other core on the pico into a thread
// allows for us to pass functions to other core to run
class Thread
{
  private:
    struct _callQueue_t
    {
        std::function<void()> f;
        std::shared_ptr<bool> finished_ptr;
        _callQueue_t(std::function<void()> _f, std::shared_ptr<bool> _finished_ptr);
    };

    // need to keep track if core 1 has been started
    static bool _core1Started;
    // a queue of functions (with some metadata) for the other core to execute
    static std::queue<_callQueue_t> _callQueue;

    // core 1 will loop this function
    static void _core1_entry();

    // pointer to whether core 1 has finished running a function or not. used for sync
    std::shared_ptr<bool> _finished_ptr = std::make_shared<bool>(false);
    bool _joinable = false;

  public:
    // since this is templated code, must write implementation here
    template <class Callable, class... Args> Thread(Callable &&f, Args &&...args)
    {
        // make sure that the function we were given is callable witn the arguments given
        static_assert(std::is_invocable<typename std::decay<Callable>::type, typename std::decay<Args>::type...>::value,
                      "Thread arguments must be invocable after conversion to rvalues");

        if (!_core1Started)
        {
            // start up core 1 with our entry function
            multicore_launch_core1(this->_core1_entry);
            // push something over FIFO so core 1 knows its core 1
            multicore_fifo_push_blocking(1);
            _core1Started = true;
        }
        _joinable = true;
        // bundle the arguments with the function to give us one that takes no args
        std::function<void()> binded_f = std::bind(f, args...);
        // push to queue to other core
        _callQueue.push(_callQueue_t(binded_f, _finished_ptr));
    }

    ~Thread();

    // whether we can call join on this thread
    bool joinable();

    // wait for thread to stop running
    void join();

    // detach running process from thread object
    void detach();
};

#endif