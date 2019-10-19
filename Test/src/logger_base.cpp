// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <functional>
#include <iostream>
#include <string>
#include <thread>

#include <tests.h>
#include <avn/logger/logger_base.h>

namespace {
class CLoggerTest : public Logger::CLoggerBase<false, char> {
    public:
        bool OutStrings(std::size_t level, std::chrono::system_clock::time_point time, char &&data) override {
            ++_calls._out_strings;
            return true;
        }

        using CLoggerBase::AddTask;
        using CLoggerBase::OnLevel;
        using CLoggerBase::OffLevel;
        using CLoggerBase::AddToLog;
        using CLoggerBase::SetLevels;

        void ClearFlags() { _calls._all_flags = 0; }

        union {
            size_t _all_flags = 0;
            struct {
                unsigned _out_strings : 3;
            };
        } _calls;

    };

    CLoggerTest test_log;
    size_t errors = 0;

    template<typename... T>
    void make_step(std::function<bool()> test, T &&... descr) {
        test_log.ClearFlags();
        if (!test()) {
            std::cout << "[ERROR] ";
            (std::cout << ... << std::forward<T>(descr));
            std::cout << std::endl;
            ++errors;
        }
    };

}   // namespace

size_t test_logger_base(void){

    std::cout << "START test_base" << std::endl;

    make_step([]()
              {
                  return !test_log.AddToLog(1, '+') && !test_log._calls._out_strings;
              },
              "Test 1 : Incorrect AddToLog call");

    make_step([&]() {
                  test_log.OnLevel(1);
                  test_log.OnLevel(2);
                  test_log.OffLevel(2);
                  test_log.OffLevel(3);
                  return test_log.AddToLog(1, '+') &&
                         !test_log.AddToLog(2, '-') &&
                         test_log._calls._out_strings == 1;
              },
              "Test 2 : Incorrect OnLevel / OffLevel and AddToLog calls");

    make_step([]() {
                  test_log.SetLevels({1, 3});
                  {
                      auto task = test_log.AddTask();
                      test_log.AddToLog(1, '+');
                      test_log.AddToLog(2, '-');
                      test_log.AddToLog(3, '+');
                      task.SetTaskResult( true );
                  }
                  return test_log._calls._out_strings == 2;
              },
              "Test 3 : Incorrect SetLevels and AddToLog calls inside task");

    make_step([]() {
                  auto task_step = [](){
                      auto task = test_log.AddTask({2}, true);
                      test_log.AddToLog(1, '-');
                      test_log.AddToLog(2, '+');
                      test_log.AddToLog(3, '-');
                  };
                  test_log.SetLevels({1, 2, 3});
                  std::thread another(task_step);
                  test_log.AddToLog(4, '-');
                  another.join();
                  return test_log._calls._out_strings == 1;
              },
              "Test 4 : Unable to process different logging level for different threads with successful finish");

    make_step([]() {
                  test_log.SetLevels({1});
                  {
                      auto task1 = test_log.AddTask(true);
                      test_log.AddToLog(1, '+');
                      {
                          auto task2 = test_log.AddTask(true);
                          test_log.AddToLog(2, '+');
                          test_log.AddToLog(1, '+');
                          task2.SetFail();
                      }
                      test_log.AddToLog(4, '-');
                  }
                  test_log.AddToLog(1, '+');
                  test_log.AddToLog(4, '-');
                  return test_log._calls._out_strings == 4;
              },
              "Test 5 : Unable to process nested tasks");

    make_step([]() {
                  test_log.SetLevels({1});
                  auto task_step = [](){
                      auto task1 = test_log.AddTask(true);
                      test_log.AddToLog(1, '+');
                      test_log.AddToLog(2, '-');
                  };
                  std::thread another(task_step);
                  {
                      auto task1 = test_log.AddTask(true);
                      test_log.AddToLog(1, '+');
                      {
                          auto task2 = test_log.AddTask(true);
                          test_log.AddToLog(2, '+');
                          test_log.AddToLog(1, '+');
                          task2.SetFail();
                      }
                      test_log.AddToLog(4, '-');
                  }
                  test_log.AddToLog(1, '+');
                  test_log.AddToLog(4, '-');
                  another.join();
                  return test_log._calls._out_strings == 5;
              },
              "Test 6 : Unable to process nested tasks in different threads");

    if(errors)
        std::cout << "UNSUCCESSFULLY finish test_base with " << errors << " errors" << std::endl;
    else
        std::cout << "SUCCESSFULLY finish test_base" << std::endl;

    return errors;
}