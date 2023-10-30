#include <iostream>
#include <list>
#include <vector>
#include <unordered_set>
#include <memory>
#include <cassert>
#include <unistd.h>
#include <functional>
 /*定时任务类*/
using TaskFunc = std::function<void()>;
// 它是一个使用 std::function 模板类实现的函数指针。
//这里的函数指针是指可以指向任意函数的指针类型，其参数类型为 void()，表示该函数不接受任何参数，返回类型为 void。
using ReleaseFunc = std::function<void()>; 
 
class TimeTask {
    private:
        uint64_t _id;  // 定时器任务对象
        uint64_t _timeout; // 定时任务的超时时间
        bool _canceled;     // false-表示没有被取消， true-表示被取消
        TaskFunc _task_cb;  // 定时器对象要执行的定时任务
        ReleaseFunc _release; //用于删除TimerWheel中保存的定时器对象信息
    public:
        // 1.构造函数
        TimeTask(uint64_t id,uint32_t delay,const TaskFunc &cb) : _id(id),_timeout(delay),_task_cb(cb) {}
        // 2.析构函数
         ~TimerTask() { 
            if (_canceled == false) _task_cb(); 
            _release(); 
        }
        void Cancel() { _canceled = true; }
        void SetRelease(const ReleaseFunc &cb) { _release = cb; }
        uint32_t DelayTime() { return _timeout; }
};

class TimeWheel {
    private:
        using WeakTask = std::weak_ptr<TimeTask>; 
    // std::weak_ptr 是 C++11 标准库中引入的一种智能指针，
    // 它提供了对指针所指向对象的弱引用。当弱引用超出作用域或者对象被销毁时，
    // 智能指针会自动设置为 nullptr，从而避免了悬空指针（dangling pointer）的问题。
        using PtrTask = std::share_ptr<TimeTask>;
        std::vector<std::vector<PtrTask>> _wheel;
        int _tick; // 当前的秒针
        int _capacity; // 表盘最大数量 ——其实就是最大延迟时间
        std::unordered_map<uint64_t,WeakTask> _timers;
    private:
        void RomoveTimer(uint64_t id) {
            auto it = _timers.find(id);
            if (it != _timers.find(id)) {
                _timers.arase(it);
            }
        }
    public:
        Wheel() _capacity(60),_tick(0),_wheel(_capacity) {}
        
}
