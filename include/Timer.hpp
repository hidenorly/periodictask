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

#include "PeriodicTask.hpp"
#include <mutex>
#include <memory>

class Timer : public Task
{
protected:
  inline static std::shared_ptr<IPeriodicTaskManager> mPeriodicTaskManager;
  inline static std::atomic<int> mTaskManRefCounter = 0;
  int mDelayMsec;
  bool mRepeat;

protected:
  std::shared_ptr<IPeriodicTaskManager> getTaskManager(void);

public:
  Timer(int nDelayMsec, bool bRepeat = false);
  virtual ~Timer();

  virtual void start(void);
  virtual void stop(void);

  virtual void onExecute(void) = 0;
};

#endif /* __TIMER_HPP__ */
