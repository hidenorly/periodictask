/*
  Copyright (C) 2022 hidenorly

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef __TIMER_HPP__
#define __TIMER_HPP__

#include "Task.hpp"
#include "PeriodicTask.hpp"
#include "ThreadPool.hpp"
#include <mutex>
#include <memory>
#include <thread>
#include <chrono>

class Timer : public Task
{
protected:
  class DelayedTask : public Task
  {
  protected:
    std::shared_ptr<Task> mTask;
    int mDelayMsec;
  public:
    DelayedTask(std::shared_ptr<Task> pTask, int nDelayMsec) : mTask( pTask ), mDelayMsec( nDelayMsec ){};
    virtual ~DelayedTask(){};
    virtual void onExecute(void){
      if( mTask ){
        std::this_thread::sleep_for( std::chrono::milliseconds( mDelayMsec ) );
        if( mIsRunning && !mStopRunning ){
          mTask->onExecute();
        }
      }
    }
    virtual void cancel(void){
      if( mTask ){
        mTask->cancel();
      }
    }
  };

protected:
  inline static std::shared_ptr<IPeriodicTaskManager> mPeriodicTaskManager;
  inline static std::shared_ptr<ThreadPool> mThreadPool;
  inline static std::atomic<int> mTaskManRefCounter = 0;
  int mDelayMsec;
  bool mRepeat;

protected:
  std::shared_ptr<IPeriodicTaskManager> getTaskManager(void);
  std::shared_ptr<ThreadPool> getThreadPool(void);

public:
  Timer(int nDelayMsec, bool bRepeat = true);
  virtual ~Timer();

  virtual void schedule(void);
  virtual void cancelSchedule(void);

  virtual void onExecute(void) = 0;
};

#endif /* __TIMER_HPP__ */
