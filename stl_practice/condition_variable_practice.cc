#include <stdio.h>

#include <mutex>
#include <thread>
#include <condition_variable>

static std::mutex mtx;
static std::condition_variable cv;

static int current = 0;

void timer(int id) {
    // wait: 当前线程调用 wait() 后将被阻塞，直到另外某个线程调用 notify_* 唤醒当前线程；
    // 当前线程被阻塞时，该函数会自动调用 std::mutex 的 unlock() 函数释放锁，使得其他被阻塞在锁竞争上的线程得以继续执行。
    // 一旦当前线程被唤醒，wait() 函数也是自动调用 std::mutex 的 lock() 函数以获得锁。
    std::unique_lock<std::mutex> lck(mtx);
    cv.wait(lck, [id]{ return id == current; });        // 只有当 pred 条件为 false 时调用 wait() 才会阻塞当前线程，并且在收到其他线程的通知后只有当 pred 为 true 时才会被解除阻塞
    // 线程被唤醒后，自动获取到锁，从当前位置直到 unlock 之前的代码都处于加锁状态。

    printf("thread[%d] current:%d\n", id, current++);
    
    // notify 并不释放锁，只是告诉调用过 wait 方法的线程可以去参与获取锁的竞争了，但不是马上得到锁，因为锁还在别人手里，别人还没释放。
    // 将 notify_* 放在解锁之前：
    //      发出通知的线程：通知 -> 最终释放锁
    //      被通知的（所有）线程：唤醒 -> 尝试获取锁，但会失败 -> 阻塞直到锁可用 -> 当通知线程解锁后，被通知线程拿到锁
    // 在解锁后调用 notify_*：
    //      发出通知的线程：通知
    //      被通知的（所有）线程：唤醒 -> 尝试获取锁并成功拿到锁
    lck.unlock();
    cv.notify_all();
}

int main(void) {
    std::thread ths[10];
    for (int i = 0; i < 10; ++i)
        ths[i] = std::thread(timer, i);
    for (int i = 0; i < 10; ++i)
        ths[i].join();
    return 0;
}