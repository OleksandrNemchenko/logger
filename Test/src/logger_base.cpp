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

    bool first_error;
    size_t logger_test_1_instances;

    class CLoggerTest : public Logger::CLoggerBase<false, std::string> {
    public:
        CLoggerTest()   { ++logger_test_1_instances; }
        ~CLoggerTest()  { --logger_test_1_instances; }

        bool OutData(std::size_t level, std::chrono::system_clock::time_point time, std::string &&data) override {
            ++_calls._out_strings;
            return true;
        }

        using CLoggerBase::AddTask;
        using CLoggerBase::EnableLevel;
        using CLoggerBase::DisableLevel;
        using CLoggerBase::AddToLog;
        using CLoggerBase::SetLevels;

        void ClearFlags() { _calls._all_flags = 0; }

        using TCalls = union {
            size_t _all_flags = 0;
            struct {
                unsigned _out_strings : 3;
            };
        };

        static TCalls _calls;

    };

    CLoggerTest::TCalls CLoggerTest::_calls;
    size_t logger_test_2_instances;

    class CLoggerTest2 : public Logger::CLoggerBase<true, std::string> {
    public:
        CLoggerTest2()  { ++logger_test_2_instances; }
        ~CLoggerTest2() { --logger_test_2_instances; }

        bool OutData(std::size_t level, std::chrono::system_clock::time_point time, std::string &&data) override {
            ++_calls._out_strings;
            return true;
        }

        using CLoggerBase::AddTask;
        using CLoggerBase::EnableLevel;
        using CLoggerBase::DisableLevel;
        using CLoggerBase::AddToLog;
        using CLoggerBase::SetLevels;

        void ClearFlags() { _calls._all_flags = 0; }

        using TCalls =  union {
            size_t _all_flags = 0;
            struct {
                unsigned _out_strings : 3;
            };
        };

        static TCalls _calls;

    };

    CLoggerTest2::TCalls CLoggerTest2::_calls;

    CLoggerTest test_log;
    Logger::CLoggerGroup<CLoggerTest, CLoggerTest2> log_grp;
    size_t errors;

    template<typename... T>
    void make_step( std::function<bool()> test, T &&... descr ) {
        test_log.ClearFlags();
        if( !test() ) {
            if( first_error ){
                std::cout << "ERROR" << std::endl;
                first_error = false;
            }
            std::cout << "[ERROR] ";
            (std::cout << ... << std::forward<T>(descr));
            std::cout << std::endl;
            ++errors;
        }
    };

}   // namespace

/*
size_t test_logger_task_group_base(void){
    Logger::CLoggerGroup<CLoggerTest, CLoggerTest2> log_grp;
    bool res = true;

    errors = 0;

    log_grp.InitLevel(1, true );
    log_grp.InitLevel(2, false );
    log_grp.Logger<0>().EnableLevel(3);
    log_grp.Logger<1>().DisableLevel(3);

    {
        auto log_tasks = log_grp.AddTask( true );

        log_grp.AddToLog(1, 'a');
        log_grp.AddToLog(2, 'a');
        log_grp.AddToLog(3, 'a');

    }

    if( log_grp.Logger<0>()._calls._out_strings != 2 || log_grp.Logger<1>()._calls._out_strings != 1 ){
        ++errors;
        std::cout << "[ERROR] Test 9 : Incorrect logging level initialization for CLoggerGroupTask" << std::endl;

    }

    log_grp.Logger<0>().ClearFlags();
    log_grp.Logger<1>().ClearFlags();

    {
        auto log_tasks = log_grp.AddTask( false );
        log_tasks.Failed();

        log_grp.AddToLog(1, 'a');
        log_grp.AddToLog(2, 'a');
        log_grp.AddToLog(3, 'a');

    }

    if( log_grp.Logger<0>()._calls._out_strings != 3 || log_grp.Logger<1>()._calls._out_strings != 3 ){
        ++errors;
        std::cout << "[ERROR] Test 10 : Incorrect logging level for failed CLoggerGroupTask" << std::endl;

    }

    return errors;
}

size_t test_logger_group_base(void){
    Logger::CLoggerGroup<CLoggerTest, CLoggerTest2> log_grp;
    bool res = true;

    log_grp.InitLevel(1, true );
    log_grp.InitLevel(2, false );
    log_grp.Logger<0>().EnableLevel(3);
    log_grp.Logger<1>().DisableLevel(3);

    auto levels_0 = log_grp.Logger<0>().GetLevels();
    auto levels_1 = log_grp.Logger<1>().GetLevels();

    if( !levels_0.count(1) || levels_0.count(2) || !levels_0.count(3) ||
        !levels_1.count(1) || levels_1.count(2) || levels_1.count(3) ) {
        std::cout << "[ERROR] Test 7 : Incorrect logging level initialization for CLoggerGroup" << std::endl;
        ++errors;
    }

    if( !log_grp.AddToLog( 1, 'a' ) || log_grp.AddToLog( 2, 'a' ) || log_grp.AddToLog( 3, 'a' ) ||
        log_grp.Logger<0>()._calls._out_strings != 2 || log_grp.Logger<1>()._calls._out_strings != 1 ){
        std::cout << "[ERROR] Test 8 : Incorrect AddToLog processing for CLoggerGroup" << std::endl;
        ++errors;
    }

    return errors;
}
*/

size_t test_logger_group_task(void){

    errors = 0;

    make_step([]() {
        auto task = log_grp.AddTask(Logger::TLevels{3});

        task.Task<0>().Succeeded();
        task.Task<1>().Failed();

        task.AddToLog( 1, "+" );
        if( CLoggerTest::_calls._out_strings != 0 && CLoggerTest2::_calls._out_strings != 1 )
            return false;

        return true;
    }, "Test test_logger_group_task.1 : Incorrect Succeeded / Failed");

    return errors;
}

size_t test_logger_task(void){

    errors = 0;

    logger_test_1_instances = 0;

    make_step([]() {
        test_log.SetLevels({1, 3});
        if( test_log.TaskOrToBeAdded(2) )
            return false;

        {
            test_log.ClearFlags();
            auto task = test_log.AddTask();
            test_log.AddToLog(1, "+");
            test_log.AddToLog(2, "-");
            test_log.AddToLog(3, "+");
            task.SetTaskResult( true );
            if( !test_log.TaskOrToBeAdded(2) )
                return false;
        }
        if( CLoggerTest::_calls._out_strings != 2 )
            return false;

        {
            test_log.ClearFlags();
            auto task = test_log.AddTask(Logger::TLevels{ 2 });
            test_log.AddToLog(1, "-");
            test_log.AddToLog(2, "+");
            test_log.AddToLog(3, "-");
            task.SetTaskResult( true );
        }
        if( CLoggerTest::_calls._out_strings != 1 )
            return false;

        {
            test_log.ClearFlags();
            auto task = test_log.AddTask( false );
            test_log.AddToLog(1, "+");
            test_log.AddToLog(2, "+");
            test_log.AddToLog(3, "+");
        }
        if( CLoggerTest::_calls._out_strings != 3 )
            return false;

        {
            test_log.ClearFlags();
            auto task = test_log.AddTask( true );
            task.DisableLevel(2);
            test_log.AddToLog(1, "+");
            test_log.AddToLog(2, "-");
            test_log.AddToLog(3, "+");
       }
        if( CLoggerTest::_calls._out_strings != 2 )
            return false;

        return true;
    }, "Test test_task.1 : Incorrect TaskOrToBeAdded, AddTask, SetLevels, InitLevel and AddToLog calls inside task");

    make_step([]() {
        bool local_res = true;
        auto task_step1 = [](){
            auto task = test_log.AddTask({2}, true);
            test_log.AddToLog(1, "-");
            test_log.AddToLog(2, "+");
            test_log.AddToLog(3, "-");
        };

        test_log.SetLevels({1, 2, 3});
        auto task_step2 = [&local_res](){
            if( !test_log.TaskOrToBeAdded(1) || test_log.TaskOrToBeAdded(4))
                local_res = false;
        };

        std::thread another1(task_step1);
        test_log.AddToLog(4, "-");
        another1.join();
        std::thread another2(task_step2);
        another2.join();
        return CLoggerTest::_calls._out_strings == 1 && local_res;

    }, "Test test_task.2 : Unable to process different logging level for different threads with successful finish");

    make_step([]() {
        test_log.SetLevels({1});
        {
            auto task1 = test_log.AddTask(true);
            test_log.AddToLog(1, "+");
            {
                auto task2 = test_log.AddTask(true);
                test_log.AddToLog(2, "+");
                test_log.AddToLog(1, "+");
                task2.Failed();
            }
            test_log.AddToLog(4, "-");
        }
        test_log.AddToLog(1, "+");
        test_log.AddToLog(4, "-");
        return CLoggerTest::_calls._out_strings == 4;
    }, "Test test_task.3 : Unable to process nested tasks");

    make_step([]() {
        test_log.SetLevels({1});
        auto task_step = [](){
            auto task1 = test_log.AddTask(true);
            test_log.AddToLog(1, "+");
            test_log.AddToLog(2, "-");
        };
        std::thread another(task_step);
        {
            auto task1 = test_log.AddTask(true);
            test_log.AddToLog(1, "+");
            {
                auto task2 = test_log.AddTask(true);
                test_log.AddToLog(2, "+");
                test_log.AddToLog(1, "+");
                task2.Failed();
            }
            test_log.AddToLog(4, "-");
        }
        test_log.AddToLog(1, "+");
        test_log.AddToLog(4, "-");
        another.join();
        return CLoggerTest::_calls._out_strings == 5;

    }, "Test test_task.4 : Unable to process nested tasks in different threads");

    make_step([](){
        return logger_test_1_instances == 0;
    }, "Test test_task.last : Incorrect logger_test_1_instances");

    return errors;

}

size_t test_logger_group(void){

    errors = 0;

    logger_test_1_instances = 0;
    logger_test_2_instances = 0;

    make_step([]() {
        log_grp.Logger<0>().ClearFlags();   log_grp.Logger<1>().ClearFlags();
        if( log_grp.SizeOf() != 2 )  return false;

        log_grp.Logger<0>().SetLevels({1}); log_grp.Logger<1>().SetLevels({2});
        if( log_grp.Logger<0>().Levels() != Logger::TLevels{1} || log_grp.Logger<1>().Levels() != Logger::TLevels{2} )
            return false;

        log_grp.SetLevels({3});
        if( log_grp.Logger<0>().Levels() != Logger::TLevels{3} || log_grp.Logger<1>().Levels() != Logger::TLevels{3} )
            return false;

        log_grp.InitLevel(1, true);
        if( log_grp.Logger<0>().Levels() != Logger::TLevels{1,3} || log_grp.Logger<1>().Levels() != Logger::TLevels{1,3} )
            return false;

        log_grp.EnableLevel(2);
        if( log_grp.Logger<0>().Levels() != Logger::TLevels{1,2,3} || log_grp.Logger<1>().Levels() != Logger::TLevels{1,2,3} )
            return false;

        log_grp.DisableLevel(2);
        if( log_grp.Logger<0>().Levels() != Logger::TLevels{1,3} || log_grp.Logger<1>().Levels() != Logger::TLevels{1,3} )
            return false;

        return true;
    }, "Test test_logger_group.1 : Incorrect logging levels initialization");

    make_step([]() {
        log_grp.Logger<0>().ClearFlags();   log_grp.Logger<1>().ClearFlags();
        log_grp.SetLevels({1});

        if( !log_grp.ForceAddToLog(1, "+") || !CLoggerTest::_calls._out_strings || !CLoggerTest2::_calls._out_strings )
            return false;

        log_grp.Logger<0>().ClearFlags(); log_grp.Logger<1>().ClearFlags();
        if( !log_grp.AddToLog(1, "+") || !CLoggerTest::_calls._out_strings || !CLoggerTest2::_calls._out_strings )
            return false;

        if( log_grp.AddToLog(2, "-") || CLoggerTest::_calls._out_strings != 1 || CLoggerTest2::_calls._out_strings != 1 )
            return false;

        log_grp.Logger<0>().ClearFlags(); log_grp.Logger<1>().ClearFlags();

        return true;
    }, "Test test_logger_group.2 : Incorrect AddToLog, ForceAddToLog output");

    make_step([](){
        return logger_test_1_instances == 0 && logger_test_2_instances == 0;
    }, "Test test_logger_group.last : Incorrect logger_test_x_instances");

    return errors;
}

size_t test_logger_base(void){

    errors = 0;

    logger_test_1_instances = 0;

    make_step([]() {
        test_log.ClearFlags();
        test_log.SetLevels({ 1 });
        if( !test_log.AddToLog(1, "+")      || !CLoggerTest::_calls._out_strings )
            return false;

        test_log.ClearFlags();
        if(  test_log.AddToLog(2, "-")      ||  CLoggerTest::_calls._out_strings )
            return false;

        test_log.ClearFlags();
        if( !test_log.ForceAddToLog(1, "+") || !CLoggerTest::_calls._out_strings )
            return false;

        test_log.ClearFlags();
        if( !test_log.ForceAddToLog(2, "+") || !CLoggerTest::_calls._out_strings )
            return false;
        test_log.ClearFlags();

        return true;
    }, "Test test_logger_base.1 : Incorrect AddToLog, ForceAddToLog calls without task");

    make_step([](){
        test_log.ClearFlags();

        test_log.SetLevels({ 1, 2, 4 });
        if( test_log.Levels() != Logger::TLevels {1, 2, 4} )
            return false;

        test_log.InitLevel( 3 );
        if( test_log.Levels() != Logger::TLevels {1, 2, 3, 4} )
            return false;

        test_log.InitLevel( 3, false );
        if( test_log.Levels() != Logger::TLevels {1, 2, 4} )
            return false;

        test_log.EnableLevel( 3 );
        if( test_log.Levels() != Logger::TLevels {1, 2, 3, 4} )
            return false;

        test_log.DisableLevel( 3 );
        if( test_log.Levels() != Logger::TLevels {1, 2, 4} )
            return false;

        if( !test_log.AddToLog(1, "+") || test_log.AddToLog(3, "-") || CLoggerTest::_calls._out_strings != 1)
            return false;
        test_log.ClearFlags();

        if( !test_log.TaskOrToBeAdded(1) || test_log.TaskOrToBeAdded(3) )
            return false;

        return true;
    }, "Test test_logger_base.2 : Incorrect SetLevels, InitLevel, EnableLevel, DisableLevel, TaskOrToBeAdded calls without task");

    make_step([](){
        return logger_test_1_instances == 0;
    }, "Test test_logger_base.last : Incorrect logger_test_1_instances");

    return errors;
}

size_t test_base(){
    size_t res = 0;

    std::cout << "START test_base... ";

    first_error = true;

    res += test_logger_base();
    res += test_logger_group();
    res += test_logger_task();
    res += test_logger_group_task();

    if( !res )
        std::cout << "OK" << std::endl;

    return res;
}