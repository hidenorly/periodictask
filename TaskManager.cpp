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
#include <chrono>

#include <iostream>

Task::Task(void) : mIsRunning(false), mStopRunning(false)
{

}

Task::~Task(void)
{

}

// -- Task
void Task::_execute(std::shared_ptr<TaskManager::TaskContext> pTaskContext)
{
  if( pTaskContext && pTaskContext->getTask() == shared_from_this() ){
    mStopRunning = false;
    mIsRunning = true;
      onExecute();
      onComplete();
    mIsRunning = false;
    mStopRunning = false;
    pTaskContext->callback();
  }
}

void Task::execute(std::shared_ptr<TaskManager::TaskContext> pTaskContext)
{
  if( pTaskContext ){
    std::shared_ptr pTask = pTaskContext->getTask();
    if( pTask ){
      pTask->_execute( pTaskContext );
    }
  }
}

void Task::cancel(void)
{
  if( mIsRunning ) {
    mStopRunning = true;
  }
}

// --- TaskManager
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
          candidateTasks.push_back(pTask);
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
        mThreads.insert_or_assign( pTask, std::make_shared<std::thread>(&Task::execute, std::make_shared<TaskManager::TaskContext>( shared_from_this(), pTask) ) );
      }
    }
    mMutexThreads.unlock();
  }
}

void TaskManager::stopAllTasks(void)
{
  mStopping = true;
  while( isRunning() ) {
    mMutexTasks.lock();
    {
      for( auto& pTask : mTasks ) {
        if( pTask->isRunning() ){
          pTask->cancel();
        }
      }
      mTasks.clear();
    }
    mMutexTasks.unlock();
    mMutexThreads.lock();
      for(auto& [pTask, pThread] : mThreads ){
        if( pThread->joinable() ){
          pThread->join();
        }
      }
      mThreads.clear();
    mMutexThreads.unlock();
    std::this_thread::sleep_for(std::chrono::microseconds(1000)); // 1 msec
  }
}

void TaskManager::_onTaskCompletion(std::shared_ptr<Task> pTask)
{
  if( pTask ) {
    cancelTask( pTask );
    if( !mStopping ) {
      executeAllTasks();
    }
  }
}

bool TaskManager::isRunning(void)
{
  bool bRunning = false;
  mMutexTasks.lock();
  {
    for ( auto& pTask : mTasks ) {
      bRunning |= pTask->isRunning();
      if (bRunning) break;
    }
  }
  mMutexTasks.unlock();
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
}
