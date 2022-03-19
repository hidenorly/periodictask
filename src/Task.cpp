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


Task::Task() : ITask(), mIsRunning(false), mStopRunning(false)
{

}

Task::~Task()
{

}

// -- Task
void Task::_execute(std::shared_ptr<Task::ITaskNotifier> pNotifier)
{
  execute();
  if( pNotifier ){
    pNotifier->onTaskCompletion( std::dynamic_pointer_cast<ITask>( shared_from_this() ) );
  }
}

void Task::execute(void)
{
  mStopRunning = false;
  mIsRunning = true;
    onExecute();
    onComplete();
  mIsRunning = false;
  mStopRunning = false;
}

void Task::executeThreadFunc(std::shared_ptr<ITask> pTask, std::shared_ptr<Task::ITaskNotifier> pNotifier)
{
  std::shared_ptr<Task> pTaskAdmin = std::dynamic_pointer_cast<Task>( pTask );
  if( pTaskAdmin ){
    pTaskAdmin->_execute( pNotifier );
  }
}

void Task::cancel(void)
{
  mStopRunning = true;
}
