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

TEST_F(TestCase_TaskManager, testTaskManager)
{
  class MyTask : public Task
  {
  protected:
    int mId;
  public:
    MyTask( int nId ) : mId(nId){};
    virtual void onExecute(void)
    {
      std::cout << "MyTask(" << std::to_string( mId ) << ") is running..." << std::endl;
      for(int i=0; i<1000; i++){
        [[maybe_unused]] volatile int j = i * i;
        if ( mStopRunning ) break;
        std::this_thread::sleep_for(std::chrono::microseconds(100));
      }
      std::cout << "MyTask(" << std::to_string( mId ) << ") is finished." << std::endl;
    }
  };

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

  std::this_thread::sleep_for(std::chrono::microseconds(1000*1000*10)); // 5sec

  std::cout << "stopAllTasks()" << std::endl;
  pTaskMan->stopAllTasks();

  std::cout << "finalize()" << std::endl;
  pTaskMan->finalize();
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
