// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <functional>
#include <iostream>
#include <string>
#include <thread>

#include <tests.h>
#include <avn/logger/logger_base.h>
#include <avn/logger/logger_group.h>

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

    class CLoggerTest2 : public Logger::CLoggerBase<true, char> {
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

size_t test_logger_group_base(void){
    Logger::CLoggerGroup<char, CLoggerTest, CLoggerTest2> log_grp;
    size_t errors = 0;
    bool res = true;

    log_grp.InitLevel(1, true );
    log_grp.InitLevel(2, false );
    log_grp.Logger<0>().OnLevel(3);
    log_grp.Logger<1>().OffLevel(3);

    auto levels_0 = log_grp.Logger<0>().GetLevels();
    auto levels_1 = log_grp.Logger<1>().GetLevels();

    if( !levels_0.count(1) || levels_0.count(2) || !levels_0.count(3) ||
        !levels_1.count(1) || levels_1.count(2) || levels_1.count(3) ) {
        std::cout << "[ERROR] Incorrect logging level initialization for CLoggerGroup" << std::endl;
        ++errors;
    }

    if( !log_grp.AddToLog( 1, 'a' ) || log_grp.AddToLog( 2, 'a' ) || log_grp.AddToLog( 3, 'a' ) ||
        log_grp.Logger<0>()._calls._out_strings != 2 || log_grp.Logger<1>()._calls._out_strings != 1 ){
        std::cout << "[ERROR] Incorrect AddToLog processing for CLoggerGroup" << std::endl;
        ++errors;
    }

    return errors;
}

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

    errors += test_logger_group_base();

    if(errors)
        std::cout << "UNSUCCESSFULLY finish test_base with " << errors << " errors" << std::endl;
    else
        std::cout << "SUCCESSFULLY finish test_base" << std::endl;

    return errors;
}