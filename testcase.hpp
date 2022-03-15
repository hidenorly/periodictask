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

#ifndef __TESTCASE_TASKMAN_HPP__
#define __TESTCASE_TASKMAN_HPP__

#include <gtest/gtest.h>

class TestCase_TaskManager : public ::testing::Test
{
protected:
  TestCase_TaskManager();
  virtual ~TestCase_TaskManager();
  virtual void SetUp();
  virtual void TearDown();

  void testTaskManager(void);
  void testPeridocTask(void);
  void testThreadPool(void);
  void testPeridocTaskManager(void);
  void testPeridocTaskManagerCancel(void);
  void testLambdaTask(void);
};

#endif /* __TESTCASE_TASKMAN_HPP__ */
