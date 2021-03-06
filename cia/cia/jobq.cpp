// cia.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//cia - concurrency in action

#include "pch.h"
#include <iostream>
#include <memory>
#include <functional>
#include <algorithm>
#include <mutex>
#include <thread>
#include <type_traits>
#include <deque>
#include <future>
#include <condition_variable>
#include <atomic>

struct IJobItem {
	virtual void invoke() = 0;
	virtual ~IJobItem(){}
};

using JobPtr = std::shared_ptr<IJobItem>;

template <typename ReturnType>
class JobItem : public IJobItem {
protected:
	std::function<ReturnType()> func;
	std::promise<ReturnType> prms;
public:
	template <typename F, typename... Args>
	JobItem(F&& f, Args... args)
		: func(std::bind(std::forward<F>(f), std::forward<Args>(args)...))
	{}
	void invoke() {
		try {
			ReturnType res = ReturnType(func());
			prms.set_value(res);
		}
		catch (...) {
			prms.set_exception(std::current_exception());
		}
	}
	std::future<ReturnType> future() {
		return prms.get_future();
	}
};

class JobQ {
private:
	std::deque<JobPtr> q;
	std::mutex mut;
	std::condition_variable cv;
public:
	void push(const JobPtr& job) {
		std::lock_guard<std::mutex> gaurd(mut);
		q.push_front(job);
		cv.notify_one();
	}

	JobPtr pop() {
		std::unique_lock < std::mutex> lck(mut);
		cv.wait(lck, [this] {return !q.empty(); });
		JobPtr job = q.back();
		q.pop_back();
		return job;
	}

	size_t size() {
		std::lock_guard < std::mutex> gaurd(mut);
		return q.size();
	}

	void clear() {
		std::lock_guard<std::mutex> gaurd(mut);
		q.clear();
	}
};

class JobThread {
private:
	std::atomic<bool> stopped;
	std::atomic<bool> busy;
	std::thread thrd;
	JobQ& q;
public:
	JobThread(JobQ& _q):
		q(_q),
		stopped(false),
		busy(false)
	{}
	~JobThread() {
		if (thrd.joinable()) {
			thrd.join();
		}
	}
	void start() {
		thrd = std::thread([this] {
			while (!stopped) {
				auto task = q.pop();
				if (!stopped) break;
				busy = true;
				task->invoke();
				busy = false;
			}
		});
	}

	std::thread::id getId() const {
		return thrd.get_id();
	}

	void signalStop() {
		stopped = true;
	}

	bool isBusy() {
		return busy;
	}
};

class JobExecutor {
private:
	int thrd_cnt;
	JobQ q;
	std::vector<std::shared_ptr<JobThread>> thrds;
public:
	JobExecutor(int _thrd_cnt = 1):thrd_cnt(_thrd_cnt)
	{}
	~JobExecutor() {
		stop();
	}
	void start() {
		for (int i = 0; i < thrd_cnt; i++) {
			auto thrd = std::make_shared<JobThread>(q);
			thrds.push_back(thrd);
			thrd->start();
		}
	}
	void stop(bool force = false) {
		for (int i = 0; i < thrds.size(); i++) {
			if (force) thrds[i]->signalStop();
			q.push(std::make_shared<JobItem<int>>([this]() {
				for (int j = 0; j < thrds.size(); j++) {
					if (thrds[j]->getId() == std::this_thread::get_id()) {
						thrds[j]->signalStop();
					}
				}
				return 0; }));
		}
		thrds.clear();
		q.clear();
	}

	bool isBusy() {
		if (q.size() > 0) {
			return true;
		}
		for (const auto& thrd : thrds) {
			if (thrd->isBusy()) {
				return true;
			}
		}
		return false;
	}

	template<typename F, typename... Args>
	std::future<typename std::result_of<F(Args...)>::type> operator()(F&&f, Args... args) {
		if (thrds.empty()) throw std::logic_error("None active");
		auto item = std::make_shared < JobItem<typename std::result_of<F(Args...)>::type>>(std::forward<F>(f), std::forward<Args>(args)...);
		std::future<typename std::result_of<F(Args...)>::type> fut = item->future();
		q.push(item);
		return fut;
	}
};

int main()
{
	JobExecutor exe;
	exe.start();
	auto res = exe([]() {return true; });
}

