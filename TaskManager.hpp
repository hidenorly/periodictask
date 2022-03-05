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

#ifndef __PERIODIC_TASK_HPP__
#define __PERIODIC_TASK_HPP__

#include <vector>
#include <mutex>
#include <thread>
#include <map>
#include <memory>
#include <functional>

class Task;

class TaskManager : public std::enable_shared_from_this<TaskManager>
{
public:
  TaskManager(int nMaxThread = 4);
  virtual ~TaskManager();

public:
  void addTask(std::shared_ptr<Task> pTask);
  void cancelTask(std::shared_ptr<Task> pTask, bool useJoin = false);
  void executeAllTasks(void);
  void stopAllTasks(void);
  bool isRunning(void);
  bool isRemainingTasks(void);
  void finalize(void);

  // for task
public:
  void _onTaskCompletion(std::shared_ptr<Task> pTask);

protected:
  int mMaxThread;
  std::atomic<bool> mStopping;

  std::vector<std::shared_ptr<Task>> mTasks;
  std::map<std::shared_ptr<Task>, std::shared_ptr<std::thread>> mThreads;
  std::mutex mMutexTasks;
  std::mutex mMutexThreads;

public:
  class TaskContext
  {
  protected:
    std::shared_ptr<TaskManager> mTaskManager;
    std::shared_ptr<Task> mTask;
  public:
    TaskContext(std::shared_ptr<TaskManager> pTaskManager, std::shared_ptr<Task> pTask) : mTaskManager(pTaskManager), mTask(pTask){};
    virtual ~TaskContext(){};

    std::shared_ptr<Task> getTask(void){ return mTask; };

    void callback(void){
      if( mTaskManager ){
        mTaskManager->_onTaskCompletion( mTask );
      }
    }
  };
};


class ITask
{
public:
  virtual void onExecute(void) = 0;
  virtual void onComplete(void){};
};

class Task : public std::enable_shared_from_this<Task>, public ITask
{
protected:
  std::atomic<bool> mIsRunning;
  std::atomic<bool> mStopRunning;

public:
  Task(void);
  virtual ~Task(void);

  // for task manager
public:
  static void execute(std::shared_ptr<TaskManager::TaskContext> pTaskContext);
  virtual void cancel(void);
  inline bool isRunning(void){ return mIsRunning; };

protected:
  void _execute(std::shared_ptr<TaskManager::TaskContext> pTaskContext);
};


#endif /* __PERIODIC_TASK_HPP__ */
