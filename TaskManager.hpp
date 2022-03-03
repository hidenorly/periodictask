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

class TaskManager;

class Task : public std::enable_shared_from_this<Task>
{
public:
  std::atomic<bool> isRunning;
  std::shared_ptr<TaskManager> mpTaskManager;

protected:
  std::atomic<bool> mStopRunning;


public:
  Task(void);
  virtual ~Task(void);

  // for override
  virtual void onExecute(void)=0;
  virtual void onComplete(void){};

  // for task manager
public:
  static void execute(std::shared_ptr<Task> pTask);
  virtual void cancel(void);


protected:
  void _onComplete(void);

};

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

  void _onTaskCompletion(std::shared_ptr<Task> pTask);

protected:
  int mMaxThread;
  std::atomic<bool> mStopping;

  std::vector<std::shared_ptr<Task>> mTasks;
  std::map<std::shared_ptr<Task>, std::shared_ptr<std::thread>> mThreads;
  std::mutex mMutexTasks;
  std::mutex mMutexThreads;
};

#endif /* __PERIODIC_TASK_HPP__ */
