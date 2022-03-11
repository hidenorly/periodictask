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


#include "Task.hpp"


Task::Task() : ITask(), ITaskAdministration()
{

}

Task::~Task()
{

}

ITaskAdministration::ITaskAdministration(void) : mIsRunning(false), mStopRunning(false)
{

}

ITaskAdministration::~ITaskAdministration(void)
{

}

// -- Task
void ITaskAdministration::_execute(std::shared_ptr<TaskManager::TaskContext> pTaskContext)
{
  if( pTaskContext ){
    std::shared_ptr<ITask> pTask = std::dynamic_pointer_cast<ITask>( pTaskContext->getTask() );
    std::shared_ptr<ITask> pTaskThis = std::dynamic_pointer_cast<ITask>( shared_from_this() );

    if( pTask && pTask == pTaskThis ){
      execute();
      pTaskContext->callback();
    }
  }
}

void ITaskAdministration::execute(void)
{
  mStopRunning = false;
  mIsRunning = true;
    std::shared_ptr<ITask> pTask = std::dynamic_pointer_cast<ITask>( shared_from_this() );
    if( pTask ){
      pTask->onExecute();
      pTask->onComplete();
    }
  mIsRunning = false;
  mStopRunning = false;
}

void ITaskAdministration::executeThreadFunc(std::shared_ptr<TaskManager::TaskContext> pTaskContext)
{
  if( pTaskContext ){
    std::shared_ptr pTask = pTaskContext->getTask();
    if( pTask ){
      pTask->_execute( pTaskContext );
    }
  }
}

void ITaskAdministration::cancel(void)
{
  if( mIsRunning ) {
    mStopRunning = true;
  }
}
