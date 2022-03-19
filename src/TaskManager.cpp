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


#include "TaskManager.hpp"
#include "Task.hpp"
#include <chrono>

#include <iostream>


TaskManager::TaskManager(int nMaxThread) : mMaxThread(nMaxThread), mStopping(false)
{

}

TaskManager::~TaskManager()
{

}

void TaskManager::addTask(std::shared_ptr<Task> pTask)
{
  mMutexTasks.lock();
  {
    mTasks.push_back(pTask);
  }
  mMutexTasks.unlock();
}

void TaskManager::cancelTask(std::shared_ptr<Task> pTask, bool useJoin)
{
  if( pTask ){
    // cancel notify & wait & remove the task from mThreads
    mMutexTasks.lock();
    {
      std::erase( mTasks, pTask );
    }
    mMutexTasks.unlock();

    std::shared_ptr<std::thread> pThread;
    mMutexThreads.lock();
    {
      pThread = mThreads[pTask];
      mThreads.erase( pTask );
    }
    mMutexThreads.unlock();

    if( pTask->isRunning() ){
      pTask->cancel();
      while( pTask->isRunning() ){
        std::this_thread::sleep_for(std::chrono::microseconds(1000)); // 1 msec
      }
    }
    if( pThread ){
      if( pThread->joinable() ){
        if( useJoin ){
          pThread->join();
        } else {
          pThread->detach();
        }
      }
    }
  }
}

void TaskManager::executeAllTasks(void)
{
  mStopping = false;

  // extract candidate tasks to execute
  int nNumOfRunningTasks = mThreads.size();
  if( nNumOfRunningTasks < mMaxThread ){
    // enumerate candidate task to run
    std::vector<std::shared_ptr<Task>> candidateTasks;
    mMutexTasks.lock();
    {
      for( auto& pTask : mTasks ){
        if( !pTask->isRunning() ){
          candidateTasks.push_back( pTask );
          nNumOfRunningTasks++;
          if( nNumOfRunningTasks >= mMaxThread ){
            break;
          }
        }
      }
    }
    mMutexTasks.unlock();

    // execute the extracted candidate tasks
    mMutexThreads.lock();
    {
      for( auto& pTask : candidateTasks ) {
        mMutexTasks.lock();
        {
          std::erase( mTasks, pTask );
        }
        mMutexTasks.unlock();
        mThreads.insert_or_assign( pTask, std::make_shared<std::thread>( &Task::executeThreadFunc, pTask, shared_from_this() ) );
      }
    }
    mMutexThreads.unlock();
  }
}

void TaskManager::stopAllTasks(void)
{
  mStopping = true;

  mTasks.clear();

  while( isRunning() ) {
    mMutexThreads.lock();
    {
      std::vector<std::shared_ptr<Task>> tasks;
      for( auto& [ pTask, pThread ] : mThreads ){
        if( pTask->isRunning() ){
          pTask->cancel();
        } else {
          tasks.push_back( pTask );
        }
      }

      for( auto& pTask : tasks ){
        mThreads.erase( pTask );
      }
    }
    mMutexThreads.unlock();
    std::this_thread::yield();
  }

  mThreads.clear();
}

void TaskManager::onTaskCompletion(std::shared_ptr<ITask> pTask)
{
  if( pTask ) {
    cancelTask( std::dynamic_pointer_cast<Task>( pTask ) );
    if( !mStopping ) {
      executeAllTasks();
    }
  }
}

bool TaskManager::isRunning(void)
{
  bool bRunning = false;
  mMutexThreads.lock();
  {
    for( auto& [ pTask, pThread ] : mThreads ){
      bRunning |= pTask->isRunning();
      if (bRunning) break;
    }
  }
  mMutexThreads.unlock();
  return bRunning;
}

bool TaskManager::isRemainingTasks(void)
{
  return !mTasks.empty();
}

void TaskManager::finalize(void)
{
  stopAllTasks();

  // remove all remaining tasks
  mMutexTasks.lock();
  {
    mTasks.clear();
  }
  mMutexTasks.unlock();

  // remove all threads
  mMutexThreads.lock();
  {
    mThreads.clear();
  }
  mMutexThreads.unlock();
}
