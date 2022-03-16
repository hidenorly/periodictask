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

#ifndef __TASK_HPP__
#define __TASK_HPP__

#include <mutex>
#include <memory>

class ITask
{
public:
  virtual void onExecute(void) = 0;
  virtual void onComplete(void){};
};

class Task : public ITask, public std::enable_shared_from_this<Task>
{
public:
  class ITaskNotifier
  {
  public:
    virtual void onTaskCompletion(std::shared_ptr<ITask> pTask) = 0;
  };

protected:
  std::atomic<bool> mIsRunning;
  std::atomic<bool> mStopRunning;

public:
  Task(void);
  virtual ~Task(void);
  void execute(void);
  static void executeThreadFunc(std::shared_ptr<ITask> pTask, std::shared_ptr<ITaskNotifier> pNotifier);
  virtual void cancel(void);
  inline bool isRunning(void){ return mIsRunning; };

protected:
  void _execute(std::shared_ptr<ITaskNotifier> pNotifier);
};

#endif /* __TASK_HPP__ */
