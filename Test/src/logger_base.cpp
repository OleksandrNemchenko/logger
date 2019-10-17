
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
        using CLoggerBase::FinishTask;
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

    char make_item(){ return 'v'; };

    void test_without_task(void) {

        make_step([]()
            {
                return !test_log.AddToLog(1, make_item()) && !test_log._calls._out_strings;
            },
        "Incorrect TryToLog call");

        make_step([&]() {
            test_log.OnLevel(1);
            test_log.OnLevel(2);
            test_log.OffLevel(2);
            test_log.OffLevel(3);
            return test_log.AddToLog(1, make_item()) &&
                    !test_log.AddToLog(2, make_item()) &&
                    test_log._calls._out_strings == 1;
            },
            "Incorrect InitOutputLevel and TryToLog calls");

        make_step([]() {
                      test_log.SetLevels({1, 3});
                      test_log.AddTask();
                      test_log.AddToLog(1, make_item());
                      test_log.AddToLog(2, make_item());
                      test_log.AddToLog(3, make_item());
                      test_log.FinishTask(true);
                      return test_log._calls._out_strings == 2;
                  },
                  "Incorrect InitOutputLevel and TryToLog calls");
    }

    void test_task(void) {

        make_step([]() {
                      test_log.SetLevels({1, 3});
                      test_log.AddTask();
                      test_log.AddToLog(1, make_item());
                      test_log.AddToLog(2, make_item());
                      test_log.AddToLog(3, make_item());
                      test_log.FinishTask(true);
                      return test_log._calls._out_strings == 2;
                  },
                  "Incorrect InitOutputLevel and TryToLog calls");

        make_step([]() {
            auto task_step = [](){
                test_log.AddTask({2});
                test_log.AddToLog(1, make_item());
                test_log.AddToLog(2, make_item());
                test_log.AddToLog(3, make_item());
                test_log.FinishTask(false);
            };
            test_log.SetLevels({1, 2, 3});
            std::thread another(task_step);
            another.join();
            return test_log._calls._out_strings == 3;
        },
                "Unable to process different logging level for different threads with unsuccessful finish");
    }
}   // namespace

size_t test_logger_base(void){

////  Errors :
//  CLoggerTest test2 = test_log;

    std::cout << "START test_base" << std::endl;

    test_without_task();
    test_task();

    if(errors)
        std::cout << "UNSUCCESSFULLY finish test_base with " << errors << " errors" << std::endl;
    else
        std::cout << "SUCCESSFULLY finish test_base" << std::endl;

    return errors;
}