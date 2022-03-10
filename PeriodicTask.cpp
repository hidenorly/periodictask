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
        if( mStopRunning ) break;
        pTask->onExecute();
      }
    mMutexTasks.unlock();

  }
}

void PeriodicTask::cancel(void)
{
  mStopRunning = true;
  mPeriodicMsec = 0;
  mMutexTasks.lock();
    mTasks.clear();
  mMutexTasks.unlock();
}


PeriodicTaskPool::PeriodicTaskPool(int nPeriodMSec) : mPeriodicMsec( nPeriodMSec )
{

}

PeriodicTaskPool::~PeriodicTaskPool()
{

}

std::shared_ptr<PeriodicTask> PeriodicTaskPool::getPeriodicTask(void)
{
  std::shared_ptr<PeriodicTask> result;

  mTaskMutex.lock();
    if( mTasks.empty() ){
      mTasks.push_back( std::make_shared<PeriodicTask>( mPeriodicMsec ) );
    }
    result = std::dynamic_pointer_cast<PeriodicTask>( mTasks.front() );
  mTaskMutex.unlock();

  return result;
}


void PeriodicTaskPool::enqueue(std::shared_ptr<ITask> pTask)
{
  std::shared_ptr<PeriodicTask> pPeriodTask = getPeriodicTask();

  mTaskMutex.lock();
    if( pPeriodTask ){
      std::shared_ptr<Task> theTask = std::dynamic_pointer_cast<Task>( pTask );
      if( theTask ){
        pPeriodTask->addTask( theTask );
      }
    }
  mTaskMutex.unlock();
}

std::shared_ptr<ITask> PeriodicTaskPool::dequeue(void)
{
  std::shared_ptr<ITask> result;

  mTaskMutex.lock();
    if( !mTasks.empty() ){
      result = mTasks.front();
    }
  mTaskMutex.unlock();

  return result;
}

void PeriodicTaskPool::erase(std::shared_ptr<ITask> pTask)
{
  std::shared_ptr<PeriodicTask> pPeriodTask = getPeriodicTask();

  if( pPeriodTask ){
    std::shared_ptr<Task> theTask = std::dynamic_pointer_cast<Task>( pTask );
    if( theTask ){
      pPeriodTask->cancelTask( theTask );
    }
  }
}

void PeriodicTaskPool::clear(void)
{
  if( !mTasks.empty() ){
    mTasks.clear();
    mTasks.push_back( std::make_shared<PeriodicTask>( mPeriodicMsec ) );
  }
}


PeriodicTaskManager::PeriodicTaskManager()
{

}

PeriodicTaskManager::~PeriodicTaskManager()
{

}

void PeriodicTaskManager::scheduleRepeat(std::shared_ptr<Task> pTask, int nPeriodMSec)
{
  if( !mTaskPool.contains( nPeriodMSec ) ){
    std::shared_ptr<ThreadPool::TaskPool> pTaskPool = std::make_shared<PeriodicTaskPool>( nPeriodMSec );
    mTaskPool.insert_or_assign( nPeriodMSec, pTaskPool );
    mThreads.insert_or_assign( nPeriodMSec, std::make_shared<ThreadPool::ThreadExector>( pTaskPool ) );
  }
  std::shared_ptr<ThreadPool::TaskPool> pTaskPool = mTaskPool[ nPeriodMSec ];
  if( pTaskPool ){
    pTaskPool->enqueue( pTask );
  }
}

void PeriodicTaskManager::cancelScheduleRepeat(std::shared_ptr<Task> pTask)
{
  for( auto& [ nPeriodMSec, pTaskPool] : mTaskPool ){
    if( pTaskPool ){
      pTaskPool->erase( pTask );
      // TODO: remove the nPeriod's TaskPool and the ThreadExecuter if task is empty.
    }
  }
}

void PeriodicTaskManager::execute(void)
{
  for( auto& [ nPeriodMSec, pThread ] : mThreads ){
    if( pThread ){
      pThread->execute();
    }
  }
}

void PeriodicTaskManager::terminate(void)
{
  for( auto& [ nPeriodMSec, pThread ] : mThreads ){
    if( pThread ){
      pThread->terminate();
    }
  }
  mThreads.clear();
  mTaskPool.clear();
}
