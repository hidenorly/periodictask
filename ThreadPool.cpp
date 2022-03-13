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

#include "ThreadPool.hpp"

ThreadPool::TaskPool::TaskPool()
{
}

ThreadPool::TaskPool::~TaskPool()
{
}

void ThreadPool::TaskPool::enqueue(std::shared_ptr<ITask> pTask)
{
  mTaskMutex.lock();
    mTasks.push_back( pTask );
  mTaskMutex.unlock();
}

std::shared_ptr<ITask> ThreadPool::TaskPool::dequeue(void)
{
  std::shared_ptr<ITask> result;

  mTaskMutex.lock();
    if( !mTasks.empty() ){
      result = mTasks.front();
      std::erase( mTasks, result );
    }
  mTaskMutex.unlock();

  return result;
}

void ThreadPool::TaskPool::erase(std::shared_ptr<ITask> pTask)
{
  mTaskMutex.lock();
    std::erase( mTasks, pTask );
  mTaskMutex.unlock();
}

void ThreadPool::TaskPool::clear(void)
{
  mTaskMutex.lock();
    mTasks.clear();
  mTaskMutex.unlock();
}


ThreadPool::ThreadExector::ThreadExector(std::shared_ptr<TaskPool> pTaskPool) : mTaskPool( pTaskPool ), mStopping( false )
{
}

ThreadPool::ThreadExector::~ThreadExector()
{
  terminate();
}

void ThreadPool::ThreadExector::execute(void)
{
  if( !mThread ){
    mThread = std::make_shared<std::thread>( &ThreadPool::ThreadExector::_execute, shared_from_this() );
  }
}

void ThreadPool::ThreadExector::terminate(void)
{
  if( mThread ){
    mStopping = true;
    if( mCurrentRunningTask ){
      std::shared_ptr<Task> pFullTask = std::dynamic_pointer_cast<Task>( mCurrentRunningTask );
      if( pFullTask ){
        pFullTask->cancel();
      }
    }
    if( mThread->joinable() ){
      mThread->join();
    }
    mStopping = false;
  }
  mCurrentRunningTask.reset();
  mTaskPool.reset();
  mThread.reset();
}

void ThreadPool::ThreadExector::_execute( std::shared_ptr<ThreadExector> pThis )
{
  if( pThis ){
    pThis->onExecute();
  }
}

void ThreadPool::ThreadExector::onExecute(void)
{
  while( !mStopping && mTaskPool ){
    mCurrentRunningTask = mTaskPool->dequeue();
    if( mCurrentRunningTask ){
      std::shared_ptr<Task> pFullTask = std::dynamic_pointer_cast<Task>( mCurrentRunningTask );
      if( pFullTask ){
        pFullTask->execute();
      } else {
        mCurrentRunningTask->onExecute();
        mCurrentRunningTask->onComplete();
        mCurrentRunningTask.reset();
      }
    } else {
      std::this_thread::yield();
    }
  }
}

void ThreadPool::ThreadExector::cancelTaskIfRunning(std::shared_ptr<ITask> pTask)
{
  if( mCurrentRunningTask == pTask ){
    std::shared_ptr<Task> pFullTask = std::dynamic_pointer_cast<Task>( mCurrentRunningTask );
    if( pFullTask ){
      pFullTask->cancel();
    }
  }
}

ThreadPool::ThreadPool( int nNumOfThreads ) : mMaxThreads( nNumOfThreads )
{
  mTaskPool = std::make_shared<ThreadPool::TaskPool>();

  for( int i = 0; i < nNumOfThreads; i++ ){
    mThreads.push_back( std::make_shared<ThreadPool::ThreadExector>( mTaskPool ) );
  }
}

ThreadPool::~ThreadPool()
{
  terminate();
}

void ThreadPool::addTask(std::shared_ptr<ITask> pTask)
{
  if( mTaskPool ){
    mTaskPool->enqueue( pTask );
  }
}

void ThreadPool::canceTask(std::shared_ptr<ITask> pTask)
{
  if( mTaskPool ){
    mTaskPool->erase( pTask );
    for( auto& pThread : mThreads ){
      pThread->cancelTaskIfRunning( pTask );
    }
  }
}

void ThreadPool::execute(void)
{
  if( mTaskPool ){
    for( auto& pThread : mThreads ){
      pThread->execute();
    }
  }
}

void ThreadPool::terminate(void)
{
  if( mTaskPool ){
    for( auto& pThread : mThreads ){
      pThread->terminate();
    }
    mThreads.clear();
    mTaskPool.reset();
  }
}

