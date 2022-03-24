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

#include "Timer.hpp"

Timer::Timer(int nDelayMsec, bool bRepeat) : mDelayMsec( nDelayMsec ), mRepeat( bRepeat )
{
  mTaskManRefCounter++;
}

Timer::~Timer()
{
  mTaskManRefCounter--;
  if( mTaskManRefCounter<=0 ){
    mPeriodicTaskManager.reset();
    mTaskManRefCounter = 0;
  }
}

std::shared_ptr<IPeriodicTaskManager> Timer::getTaskManager(void)
{
  if( !mPeriodicTaskManager ){
    mPeriodicTaskManager = std::make_shared<PeriodicTaskManager>();
  }

  return mPeriodicTaskManager;
}


void Timer::start(void)
{
  std::shared_ptr<IPeriodicTaskManager> pTaskMan = getTaskManager();

  if( pTaskMan ){
    if( mRepeat ){
      pTaskMan->scheduleRepeat( shared_from_this(), mDelayMsec );
      pTaskMan->execute();
    }
  }
}

void Timer::stop(void)
{
  std::shared_ptr<IPeriodicTaskManager> pTaskMan = getTaskManager();

  if( pTaskMan ){
    if( mRepeat ){
      pTaskMan->cancelScheduleRepeat( shared_from_this() );
    }
  }
}
