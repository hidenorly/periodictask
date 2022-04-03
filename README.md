# C++20 based asynchronous task utility

This is library to be easier to handle asynchronous task, periodic task and the lambda.

* If you need to run tasks concurrently, please use ```ThreadPool``` and the ```Task```.
  * As default, the concurrency is based on the platform's maximum concurrency.
  * If necessary to limit to smaller number, you can specify the maximum number of threads by constructor argument.

* If you need to run task periodically, you can use ```PeriodicTaskManager``` to run the Task at your specified period periodically.

* If you want to use lambda, you can use ```LambdaTask```. This helps to use your lambda for the above managers.

* If you want to use so called Timer simply, you can use ```Timer```. This helps to use your simple timer use without any noticing the above managers.

* If you want to use Timer as lambda manner, you can use ```LambdaTimer```. 

* Please refer to testcase.cpp to know how to use them.


## how to build libasynctask.so(.dylib)

```
$ make -j 10
```

## how to test the built libasynctask.so(.dylib)

```
$ make -j 10 test
$ ./bin/asynctasktest
[==========] Running 8 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 8 tests from TestCase_TaskManager
[ RUN      ] TestCase_TaskManager.testTaskManager
..snip..
[       OK ] TestCase_TaskManager.testLambdaTimer (1005 ms)
[----------] 8 tests from TestCase_TaskManager (32244 ms total)

[----------] Global test environment tear-down
[==========] 8 tests from 1 test suite ran. (32244 ms total)
[  PASSED  ] 8 tests.
```

## structure

```
├── LICENSE
├── Makefile
├── README.md : this document
├── bin : built test case
│  └── asynctasktest
├── include : header files
│  ├── LambdaTask.hpp
│  ├── PeriodicTask.hpp
│  ├── Task.hpp
│  ├── TaskManager.hpp
│  ├── ThreadPool.hpp
│  └── Timer.hpp
├── lib
│  └── libasynctask.dylib : built artifact
├── out : built intermediated output
├── src
│  ├── LambdaTask.cpp
│  ├── PeriodicTask.cpp
│  ├── Task.cpp
│  ├── TaskManager.cpp
│  ├── ThreadPool.cpp
│  └── Timer.cpp
└── test
    ├── testcase.cpp
    └── testcase.hpp
```


## Confirmed environment

* MacOS Monterey 12.3
* clang++ 13.1.6

```Apple clang version 13.1.6 (clang-1316.0.21.2)
Target: arm64-apple-darwin21.4.0```
