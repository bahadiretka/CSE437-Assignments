#include "Timer.h"

void Timer::threadLoop(){
    std::unique_lock<std::mutex> lock(_mutex);
    while (!_stopThread){
        if (_queue.empty()){
            _cv.wait(lock);
        }
        else{
            TimerEvent event = _queue.top();
            _queue.pop();
            _cv.wait_until(lock, event.next);
            if (!_stopThread){
                event.callback();
                if (event.period.count() > 0 && (!event.predicate || event.predicate())){
                    event.next = CLOCK::now() + event.period;
                    _queue.push(event);
                }
            }
        }
    }
}

void Timer::registerTimer(const Timepoint& tp, const TTimerCallback& cb){
    std::lock_guard<std::mutex> lock(_mutex);
    _queue.push(TimerEvent{tp, Millisecs(0), cb, nullptr});
    _cv.notify_all();
}

void Timer::registerTimer(const Millisecs& period, const TTimerCallback& cb){
    registerTimer(CLOCK::now() + period, period, cb);
}

void Timer::registerTimer(const Timepoint& tp, const Millisecs& period, const TTimerCallback& cb){
    std::lock_guard<std::mutex> lock(_mutex);
    _queue.push(TimerEvent{tp, period, cb, nullptr});
    _cv.notify_all();
}

void Timer::registerTimer(const TPredicate& pred, const Millisecs& period, const TTimerCallback& cb){
    std::lock_guard<std::mutex> lock(_mutex);
    _queue.push(TimerEvent{CLOCK::now() + period, period, cb, pred});
    _cv.notify_all();
}

void Timer::stopThread(){
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _stopThread = true;
    }
    _cv.notify_all();
    if (_timerThread.joinable()){
        _timerThread.join();
    }
}