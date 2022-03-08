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

#include "PeriodicTask.hpp"
#include <iostream>

void PeriodicTask::addTask(std::shared_ptr<Task> pTask)
{
  mMutexTasks.lock();
    mTasks.push_back( pTask );
  mMutexTasks.unlock();
}

void PeriodicTask::cancelTask(std::shared_ptr<Task> pTask)
{
  mMutexTasks.lock();
    std::erase( mTasks, pTask );
  mMutexTasks.unlock();
}

void PeriodicTask::onExecute(void)
{
  std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();

  while( mIsRunning && !mStopRunning ){
    // Adjust the next periodic with the actual execution time
    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now() - lastTime );
    std::chrono::milliseconds delayMsec = std::chrono::milliseconds( mPeriodicMsec ) - duration;
    if( delayMsec.count() < 0 ){
      delayMsec = std::chrono::milliseconds( 0 );
      std::cout << "warning: execution time is exceeded than the periodic duration" << std::endl;
    }
    std::this_thread::sleep_for( delayMsec );

    lastTime = std::chrono::high_resolution_clock::now();
    mMutexTasks.lock();
      for( auto& pTask : mTasks ){
        pTask->onExecute();
      }
    mMutexTasks.unlock();

  }
}
