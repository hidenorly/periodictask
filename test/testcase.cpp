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

#include <gtest/gtest.h>
#include "testcase.hpp"
#include "TaskManager.hpp"
#include "PeriodicTask.hpp"
#include "ThreadPool.hpp"
#include "LambdaTask.hpp"
#include "Timer.hpp"
#include <iostream>
#include <chrono>

TestCase_TaskManager::TestCase_TaskManager()
{
}

TestCase_TaskManager::~TestCase_TaskManager()
{
}

void TestCase_TaskManager::SetUp()
{
}

void TestCase_TaskManager::TearDown()
{
}

class MyTask : public Task
{
protected:
  int mId;
public:
  MyTask( int nId ) : mId(nId){};
  virtual void onExecute(void)
  {
    std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
    std::cout << "MyTask(" << std::to_string( mId ) << ") is running..." << std::endl;
    for(int i=0; i<1000; i++){
      [[maybe_unused]] volatile int j = i * i;
      if ( mStopRunning ) break;
      std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
    std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>( endTime - startTime );

    std::cout << "MyTask(" << std::to_string( mId ) << ") is finished (" << std::to_string( duration.count() ) << "msec)." << std::endl;
  }
};

#if 1
TEST_F(TestCase_TaskManager, testTaskManager)
{
  std::cout << "Hello, World!" << std::endl;

  std::shared_ptr<TaskManager> pTaskMan = std::make_shared<TaskManager>();
  int i = 0;
  pTaskMan->addTask( std::make_shared<MyTask>( i++ ) );
  pTaskMan->addTask( std::make_shared<MyTask>( i++ ) );
  pTaskMan->addTask( std::make_shared<MyTask>( i++ ) );
  pTaskMan->addTask( std::make_shared<MyTask>( i++ ) );
  pTaskMan->addTask( std::make_shared<MyTask>( i++ ) );
  pTaskMan->addTask( std::make_shared<MyTask>( i++ ) );

  std::cout << "executeAllTasks()" << std::endl;
  pTaskMan->executeAllTasks();

  std::this_thread::sleep_for(std::chrono::microseconds(1000*1000*5)); // 5sec

  std::cout << "stopAllTasks()" << std::endl;
  pTaskMan->stopAllTasks();

  std::cout << "finalize()" << std::endl;
  pTaskMan->finalize();
}

TEST_F(TestCase_TaskManager, testPeridocTask)
{
  std::shared_ptr<PeriodicTask> pPeriodicTask = std::make_shared<PeriodicTask>(1000);
  int i = 200;
  pPeriodicTask->addTask( std::make_shared<MyTask>( i++ ) );
  pPeriodicTask->addTask( std::make_shared<MyTask>( i++ ) );
  pPeriodicTask->addTask( std::make_shared<MyTask>( i++ ) );

  std::shared_ptr<TaskManager> pTaskMan = std::make_shared<TaskManager>();
  pTaskMan->addTask( pPeriodicTask );

  std::cout << "executeAllTasks()" << std::endl;
  pTaskMan->executeAllTasks();

  std::this_thread::sleep_for(std::chrono::microseconds(1000*1000*5)); // 5sec
  std::cout << "stopAllTasks()" << std::endl;
  pTaskMan->stopAllTasks();

  std::cout << "finalize()" << std::endl;
  pTaskMan->finalize();
}
#endif


TEST_F(TestCase_TaskManager, testThreadPool)
{
  std::cout << "instantiate ThreadPool" << std::endl;
  std::shared_ptr<ThreadPool> pThreadPool = std::make_shared<ThreadPool>( );

  std::cout << "adding tasks" << std::endl;
  int i = 100;
  pThreadPool->addTask( std::make_shared<MyTask>( i++ ) );
  pThreadPool->addTask( std::make_shared<MyTask>( i++ ) );
  pThreadPool->addTask( std::make_shared<MyTask>( i++ ) );
  pThreadPool->addTask( std::make_shared<MyTask>( i++ ) );
  pThreadPool->addTask( std::make_shared<MyTask>( i++ ) );

  std::cout << "execute thread pool" << std::endl;
  pThreadPool->execute();

  std::cout << "wait the execution" << std::endl;
  std::this_thread::sleep_for(std::chrono::microseconds(1000*1000*2)); // 2sec

  std::cout << "adding tasks" << std::endl;
  pThreadPool->addTask( std::make_shared<MyTask>( i++ ) );
  pThreadPool->addTask( std::make_shared<MyTask>( i++ ) );

  std::cout << "wait the execution" << std::endl;
  std::this_thread::sleep_for(std::chrono::microseconds(1000*1000*3)); // 3sec

  std::cout << "terminate()" << std::endl;
  pThreadPool->terminate();
}


TEST_F(TestCase_TaskManager, testPeridocTaskManager)
{
  std::shared_ptr<PeriodicTaskManager> pTaskMan = std::make_shared<PeriodicTaskManager>();
  int i = 300;
  pTaskMan->scheduleRepeat( std::make_shared<MyTask>( i++ ), 1000 );
  pTaskMan->scheduleRepeat( std::make_shared<MyTask>( i++ ), 1000 );
  pTaskMan->scheduleRepeat( std::make_shared<MyTask>( i++ ), 200 );

  std::cout << "execute()" << std::endl;
  pTaskMan->execute();

  std::this_thread::sleep_for(std::chrono::microseconds(1000*1000*5)); // 3sec

  std::cout << "terminate()" << std::endl;
  pTaskMan->terminate();
}


TEST_F(TestCase_TaskManager, testPeridocTaskManagerCancel)
{
  std::shared_ptr<PeriodicTaskManager> pTaskMan = std::make_shared<PeriodicTaskManager>();
  int i = 400;

  std::shared_ptr<MyTask> pTask1000_1 = std::make_shared<MyTask>( i++ );
  std::shared_ptr<MyTask> pTask1000_2 = std::make_shared<MyTask>( i++ );
  std::shared_ptr<MyTask> pTask200_1 = std::make_shared<MyTask>( i++ );

  pTaskMan->scheduleRepeat( pTask1000_1, 1000 );
  pTaskMan->scheduleRepeat( pTask1000_2, 1000 );
  pTaskMan->scheduleRepeat( pTask200_1, 200 );

  std::cout << "execute()" << std::endl;
  pTaskMan->execute();

  std::this_thread::sleep_for(std::chrono::microseconds(1000*1000*2)); // 2sec

  pTaskMan->cancelScheduleRepeat( pTask200_1 );

  std::this_thread::sleep_for(std::chrono::microseconds(1000*1000*3)); // 3sec

  std::cout << "terminate()" << std::endl;
  pTaskMan->terminate();
}


TEST_F(TestCase_TaskManager, testLambdaTask)
{
  std::cout << "instantiate ThreadPool" << std::endl;
  std::shared_ptr<ThreadPool> pThreadPool = std::make_shared<ThreadPool>( );

  std::cout << "adding tasks" << std::endl;

  TASK_LAMBDA task = [](std::shared_ptr<Task> pTask){
    std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
    std::cout << "lambdaTask is running..." << std::endl;
    for(int i=0; i<1000; i++){
      [[maybe_unused]] volatile int j = i * i;
      std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
    std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>( endTime - startTime );

    std::cout << "lamndatask is finished (" << std::to_string( duration.count() ) << "msec)." << std::endl;
  };

  pThreadPool->addTask( std::make_shared<LambdaTask>( task ) );
  pThreadPool->addTask( std::make_shared<LambdaTask>( task ) );
  pThreadPool->addTask( std::make_shared<LambdaTask>( task ) );
  pThreadPool->addTask( std::make_shared<LambdaTask>( task ) );
  pThreadPool->addTask( std::make_shared<LambdaTask>( task ) );

  std::cout << "execute thread pool" << std::endl;
  pThreadPool->execute();

  std::cout << "wait the execution" << std::endl;
  std::this_thread::sleep_for(std::chrono::microseconds(1000*1000*2)); // 2sec

  std::cout << "adding tasks" << std::endl;
  pThreadPool->addTask( std::make_shared<LambdaTask>( task ) );
  pThreadPool->addTask( std::make_shared<LambdaTask>( task ) );

  std::cout << "wait the execution" << std::endl;
  std::this_thread::sleep_for(std::chrono::microseconds(1000*1000*3)); // 3sec

  std::cout << "terminate()" << std::endl;
  pThreadPool->terminate();
}

TEST_F(TestCase_TaskManager, testTimer)
{
  class MyTimer : public Timer
  {
    public:
      MyTimer(int nDelayMsec, bool bRepeat = false):Timer(nDelayMsec, bRepeat){};
      virtual ~MyTimer(){};
      virtual void onExecute(void){
        std::cout << "MyTimer (" << std::to_string( reinterpret_cast<uint64_t>( shared_from_this().get() ) ) << "):" << std::to_string( mDelayMsec ) << " msec" << std::endl;
      }
  };

  std::vector<std::shared_ptr<MyTimer>> timers;
  timers.push_back( std::make_shared<MyTimer>( 100, true ) );
  timers.push_back( std::make_shared<MyTimer>( 100, true ) );
  timers.push_back( std::make_shared<MyTimer>( 300, true ) );
  timers.push_back( std::make_shared<MyTimer>( 133, false ) );

  for( auto& pTimer : timers ){
    pTimer->schedule();
  }

  std::cout << "wait the execution" << std::endl;
  std::this_thread::sleep_for(std::chrono::microseconds(1000*1000*1)); // 1sec

  for( auto& pTimer : timers ){
    pTimer->cancelSchedule();
  }

  timers.clear();
}


TEST_F(TestCase_TaskManager, testLambdaTimer)
{
  TASK_LAMBDA task = [](std::shared_ptr<Task> pTask){
    std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
    std::cout << "lambdaTask is running..." << std::endl;
    for(int i=0; i<100; i++){
      [[maybe_unused]] volatile int j = i * i;
      std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
    std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>( endTime - startTime );

    std::cout << "lamndatask is finished (" << std::to_string( duration.count() ) << "msec)." << std::endl;
  };

  std::vector<std::shared_ptr<LambdaTimer>> timers;
  timers.push_back( std::make_shared<LambdaTimer>( task, 100, true ) );
  timers.push_back( std::make_shared<LambdaTimer>( task, 100, true ) );
  timers.push_back( std::make_shared<LambdaTimer>( task, 300, true ) );
  timers.push_back( std::make_shared<LambdaTimer>( task, 133, false ) );

  for( auto& pTimer : timers ){
    pTimer->schedule();
  }

  std::cout << "wait the execution" << std::endl;
  std::this_thread::sleep_for(std::chrono::microseconds(1000*1000*1)); // 1sec

  for( auto& pTimer : timers ){
    pTimer->cancelSchedule();
  }

  timers.clear();
}


int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
