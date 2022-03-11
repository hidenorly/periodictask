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

#ifndef __THREAD_POOL_HPP__
#define __THREAD_POOL_HPP__

#include <vector>
#include <mutex>
#include <thread>

#include "Task.hpp"

class ThreadPool
{
public:
  class TaskPool
  {
  protected:
    std::vector<std::shared_ptr<ITask>> mTasks;
    std::mutex mTaskMutex;

  public:
    TaskPool();
    virtual ~TaskPool();
    virtual void enqueue(std::shared_ptr<ITask> pTask);
    virtual std::shared_ptr<ITask> dequeue(void);
    virtual void erase(std::shared_ptr<ITask> pTask);
    virtual void clear(void);
  };

  class ThreadExector : public std::enable_shared_from_this<ThreadExector>
  {
  protected:
    std::shared_ptr<TaskPool> mTaskPool;
    std::shared_ptr<ITask> mCurrentRunningTask;
    std::shared_ptr<std::thread> mThread;
    std::atomic<bool> mStopping;

  public:
    ThreadExector(std::shared_ptr<TaskPool> pTaskPool);
    virtual ~ThreadExector();
    void execute(void);
    void terminate(void);
    void cancelTaskIfRunning(std::shared_ptr<ITask> pTask);

  protected:
    static void _execute( std::shared_ptr<ThreadExector> pThis );
    void onExecute(void);
  };

protected:
  int mMaxThreads;
  std::vector<std::shared_ptr<ThreadExector>> mThreads;
  std::shared_ptr<TaskPool> mTaskPool;

public:
  ThreadPool( int nNumOfThreads = std::thread::hardware_concurrency() );
  virtual ~ThreadPool();

  void addTask(std::shared_ptr<ITask> pTask);
  void canceTask(std::shared_ptr<ITask> pTask);

  void execute(void);
  void terminate(void);
};


#endif /* __THREAD_POOL_HPP__ */
