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

#include "TaskManager.hpp"

#include <vector>
#include <mutex>

class IPeriodicTaskManager
{
public:
  virtual void scheduleRepeat(std::shared_ptr<Task> pTask, int nPeriodMSec) = 0;
  virtual void cancelScheduleRepeat(std::shared_ptr<Task> pTask) = 0;
};

class PeriodicTask : public Task
{
protected:
  int mPeriodicMsec;

  std::vector<std::shared_ptr<Task>> mTasks;
  std::mutex mMutexTasks;

public:
  PeriodicTask(int nPeriodMSec): mPeriodicMsec(nPeriodMSec){};
  virtual ~PeriodicTask(){};

  virtual void addTask(std::shared_ptr<Task> pTask);
  virtual void cancelTask(std::shared_ptr<Task> pTask);

  virtual void onExecute(void);
};

#endif /* __PERIODIC_TASK_HPP__ */
