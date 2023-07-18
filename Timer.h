// Timer.h
#ifndef TIMER_H
#define TIMER_H

#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <chrono>
#include <condition_variable>

// Define useful aliases for timer related datatypes
using CLOCK = std::chrono::high_resolution_clock; 
using TTimerCallback = std::function<void()>; 
using Millisecs = std::chrono::milliseconds;
using Timepoint = CLOCK::time_point;
using TPredicate = std::function<bool()>;

// ITimer class defines the interface for a timer
class ITimer { 
public:
    // Register a one-shot timer that fires at a specific point in time
    virtual void registerTimer(const Timepoint& tp, const TTimerCallback& cb) = 0;
    // Register a periodic timer that fires every 'period' milliseconds
    virtual void registerTimer(const Millisecs& period, const TTimerCallback& cb) = 0;
    // Register a periodic timer that starts at a specific point in time and fires every 'period' milliseconds
    virtual void registerTimer(const Timepoint& tp, const Millisecs& period, const TTimerCallback& cb) = 0;
    // Register a conditional periodic timer that fires every 'period' milliseconds as long as 'pred' returns true
    virtual void registerTimer(const TPredicate& pred, const Millisecs& period, const TTimerCallback& cb) = 0;
};

// Struct representing a timer event
struct TimerEvent{
    Timepoint next; // The next time point when the event should be triggered
    Millisecs period; // The period between consecutive events. If this is zero, the event is one-shot
    TTimerCallback callback; // The function to call when the event is triggered
    TPredicate predicate; // A predicate that determines if the event should be triggered again
};

// Class to compare two TimerEvent instances
class TimerEventComparison{
public:
    // Return true if the 'next' field of lhs is later than that of rhs
    bool operator()(const TimerEvent& lhs, const TimerEvent& rhs) const{return lhs.next > rhs.next;}
};

// Timer class implements the ITimer interface
class Timer : public ITimer{
public:
    
    // Constructor starts a new thread for handling timer events
    Timer() : _timerThread(&Timer::threadLoop, this), _stopThread(false) {}
    // Destructor stops the timer thread
    ~Timer() { stopThread(); }

    // Implementation of ITimer interface
    void registerTimer(const Timepoint& tp, const TTimerCallback& cb) override;
    void registerTimer(const Millisecs& period, const TTimerCallback& cb) override;
    void registerTimer(const Timepoint& tp, const Millisecs& period, const TTimerCallback& cb) override;
    void registerTimer(const TPredicate& pred, const Millisecs& period, const TTimerCallback& cb) override;

private:
    // Stop the timer thread
    void stopThread();
    // Main loop of the timer thread
    void threadLoop();

    std::thread _timerThread; // The timer thread
    bool _stopThread; // Flag to indicate if the timer thread should stop
    std::mutex _mutex; // Mutex for thread synchronization
    std::condition_variable _cv; // Condition variable for thread synchronization
    std::priority_queue<TimerEvent, std::vector<TimerEvent>, TimerEventComparison> _queue; // Priority queue of timer events
};

#endif // TIMER_H
