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

#ifndef __TASK_MANAGER_HPP__
#define __TASK_MANAGER_HPP__

#include <vector>
#include <mutex>
#include <thread>
#include <map>
#include <memory>

#include "Task.hpp"

class ITaskManager
{
public:
  virtual void addTask(std::shared_ptr<Task> pTask) = 0;
  virtual void cancelTask(std::shared_ptr<Task> pTask, bool useJoin) = 0;
  virtual void cancelTask(std::shared_ptr<Task> pTask) = 0;

  virtual void executeAllTasks(void) = 0;
  virtual void stopAllTasks(void) = 0;
  virtual bool isRunning(void) = 0;
  virtual bool isRemainingTasks(void) = 0;
  virtual void finalize(void) = 0;
};

class TaskManager : public ITaskManager, public Task::ITaskNotifier, public std::enable_shared_from_this<TaskManager>
{
public:
  TaskManager(int nMaxThread = 4);
  virtual ~TaskManager();

public:
  virtual void addTask(std::shared_ptr<Task> pTask);
  virtual void cancelTask(std::shared_ptr<Task> pTask, bool useJoin);
  virtual void cancelTask(std::shared_ptr<Task> pTask){ cancelTask( pTask, false ); };

  virtual void executeAllTasks(void);
  virtual void stopAllTasks(void);
  virtual bool isRunning(void);
  virtual bool isRemainingTasks(void);
  virtual void finalize(void);

  // for task
public:
  virtual void onTaskCompletion(std::shared_ptr<ITask> pTask);

protected:
  int mMaxThread;
  std::atomic<bool> mStopping;

  std::vector<std::shared_ptr<Task>> mTasks;
  std::map<std::shared_ptr<Task>, std::shared_ptr<std::thread>> mThreads;
  std::mutex mMutexTasks;
  std::mutex mMutexThreads;
};

#endif /* __TASK_MANAGER_HPP__ */
