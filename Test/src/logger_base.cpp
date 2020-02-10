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

    bool _firstError;
    size_t _loggerTest1Instances;

    class ALoggerTest : public ALogger::ALoggerBase<false, std::string> {
    public:
        ALoggerTest()   { ++_loggerTest1Instances; }
        ~ALoggerTest()  { --_loggerTest1Instances; }

        bool outData(std::size_t level, std::chrono::system_clock::time_point time, std::string&& data) override
        {
            ++_calls._outStrings;
            return true;
        }

        using ALoggerBase::addTask;
        using ALoggerBase::enableLevel;
        using ALoggerBase::disableLevel;
        using ALoggerBase::addToLog;
        using ALoggerBase::setlevels;

        void ClearFlags() { _calls._allFlags = 0; }

        using TCalls = union {
            size_t _allFlags = 0;
            struct {
                unsigned _outStrings : 3;
            };
        };

        static TCalls _calls;

    };

    ALoggerTest::TCalls ALoggerTest::_calls;
    size_t logger_test_2_instances;

    class ALoggerTest2 : public ALogger::ALoggerBase<true, std::string> {
    public:
        ALoggerTest2()  { ++logger_test_2_instances; }
        ~ALoggerTest2() { --logger_test_2_instances; }

        bool outData(std::size_t level, std::chrono::system_clock::time_point time, std::string&& data) override
        {
            ++_calls._outStrings;
            return true;
        }

        using ALoggerBase::addTask;
        using ALoggerBase::enableLevel;
        using ALoggerBase::disableLevel;
        using ALoggerBase::addToLog;
        using ALoggerBase::setlevels;

        void ClearFlags() { _calls._allFlags = 0; }

        using TCalls = union {
            size_t _allFlags = 0;
            struct {
                unsigned _outStrings : 3;
            };
        };

        static TCalls _calls;

    };

    ALoggerTest2::TCalls ALoggerTest2::_calls;

    ALoggerTest _testLog;
    ALogger::ALoggerGroup<ALoggerTest, ALoggerTest2> _logGrp;
    size_t _errors;

    template<typename... T>
    void makeStep(std::function<bool()> test, T&&... descr)
    {
        _testLog.ClearFlags();
        if (!test()) {
            if (_firstError) {
                std::cout << "ERROR" << std::endl;
                _firstError = false;
            }
            std::cout << "[ERROR] ";
            (std::cout << ... << std::forward<T>(descr));
            std::cout << std::endl;
            ++_errors;
        }
    };

}   // namespace

/*
size_t _testLogger_task_group_base(void){
    ALogger::ALoggerGroup<ALoggerTest, ALoggerTest2> _logGrp;
    bool res = true;

    _errors = 0;

    _logGrp.initLevel(1, true);
    _logGrp.initLevel(2, false);
    _logGrp.ALogger<0>().enableLevel(3);
    _logGrp.ALogger<1>().disableLevel(3);

    {
        auto log_tasks = _logGrp.addTask(true);

        _logGrp.addToLog(1, 'a');
        _logGrp.addToLog(2, 'a');
        _logGrp.addToLog(3, 'a');

    }

    if (_logGrp.ALogger<0>()._calls._outStrings != 2 || _logGrp.ALogger<1>()._calls._outStrings != 1){
        ++_errors;
        std::cout << "[ERROR] Test 9 : Incorrect logging level initialization for ALoggerGroupTask" << std::endl;

    }

    _logGrp.ALogger<0>().ClearFlags();
    _logGrp.ALogger<1>().ClearFlags();

    {
        auto log_tasks = _logGrp.addTask(false);
        log_tasks.failed();

        _logGrp.addToLog(1, 'a');
        _logGrp.addToLog(2, 'a');
        _logGrp.addToLog(3, 'a');

    }

    if (_logGrp.ALogger<0>()._calls._outStrings != 3 || _logGrp.ALogger<1>()._calls._outStrings != 3){
        ++_errors;
        std::cout << "[ERROR] Test 10 : Incorrect logging level for failed ALoggerGroupTask" << std::endl;

    }

    return _errors;
}

size_t _testLogger_group_base(void){
    ALogger::ALoggerGroup<ALoggerTest, ALoggerTest2> _logGrp;
    bool res = true;

    _logGrp.initLevel(1, true);
    _logGrp.initLevel(2, false);
    _logGrp.ALogger<0>().enableLevel(3);
    _logGrp.ALogger<1>().disableLevel(3);

    auto levels_0 = _logGrp.ALogger<0>().Getlevels();
    auto levels_1 = _logGrp.ALogger<1>().Getlevels();

    if (!levels_0.count(1) || levels_0.count(2) || !levels_0.count(3) ||
        !levels_1.count(1) || levels_1.count(2) || levels_1.count(3)) {
        std::cout << "[ERROR] Test 7 : Incorrect logging level initialization for ALoggerGroup" << std::endl;
        ++_errors;
    }

    if (!_logGrp.addToLog(1, 'a') || _logGrp.addToLog(2, 'a') || _logGrp.addToLog(3, 'a') ||
        _logGrp.ALogger<0>()._calls._outStrings != 2 || _logGrp.ALogger<1>()._calls._outStrings != 1){
        std::cout << "[ERROR] Test 8 : Incorrect addToLog processing for ALoggerGroup" << std::endl;
        ++_errors;
    }

    return _errors;
}
*/

size_t _testLogger_group_task(void)
{
    _errors = 0;

    makeStep([]()
    {
        auto task = _logGrp.addTask(ALogger::TLevels{3});

        task.task<0>().succeeded();
        task.task<1>().failed();

        task.addToLog(1, "+");
        if (ALoggerTest::_calls._outStrings != 0 && ALoggerTest2::_calls._outStrings != 1)
            return false;

        return true;
    }, "Test _testLogger_group_task.1 : Incorrect succeeded / failed");

    return _errors;
}

size_t _testLogger_task(void)
{
    _errors = 0;

    _loggerTest1Instances = 0;

    makeStep([]()
    {
        _testLog.setlevels({1, 3});
        if (_testLog.taskOrToBeAdded(2))
            return false;

        {
            _testLog.ClearFlags();
            auto task = _testLog.addTask();
            _testLog.addToLog(1, "+");
            _testLog.addToLog(2, "-");
            _testLog.addToLog(3, "+");
            task.setTaskResult(true);
            if (!_testLog.taskOrToBeAdded(2))
                return false;
        }
        if (ALoggerTest::_calls._outStrings != 2)
            return false;

        {
            _testLog.ClearFlags();
            auto task = _testLog.addTask(ALogger::TLevels{ 2 });
            _testLog.addToLog(1, "-");
            _testLog.addToLog(2, "+");
            _testLog.addToLog(3, "-");
            task.setTaskResult(true);
        }
        if (ALoggerTest::_calls._outStrings != 1)
            return false;

        {
            _testLog.ClearFlags();
            auto task = _testLog.addTask(false);
            _testLog.addToLog(1, "+");
            _testLog.addToLog(2, "+");
            _testLog.addToLog(3, "+");
        }
        if (ALoggerTest::_calls._outStrings != 3)
            return false;

        {
            _testLog.ClearFlags();
            auto task = _testLog.addTask(true);
            task.disableLevel(2);
            _testLog.addToLog(1, "+");
            _testLog.addToLog(2, "-");
            _testLog.addToLog(3, "+");
       }
        if (ALoggerTest::_calls._outStrings != 2)
            return false;

        return true;
    }, "Test test_task.1 : Incorrect taskOrToBeAdded, addTask, setlevels, initLevel and addToLog calls inside task");

    makeStep([]()
    {
        bool local_res = true;
        auto task_step1 = [](){
            auto task = _testLog.addTask({2}, true);
            _testLog.addToLog(1, "-");
            _testLog.addToLog(2, "+");
            _testLog.addToLog(3, "-");
        };

        _testLog.setlevels({1, 2, 3});
        auto task_step2 = [&local_res](){
            if (!_testLog.taskOrToBeAdded(1) || _testLog.taskOrToBeAdded(4))
                local_res = false;
        };

        std::thread another1(task_step1);
        _testLog.addToLog(4, "-");
        another1.join();
        std::thread another2(task_step2);
        another2.join();
        return ALoggerTest::_calls._outStrings == 1 && local_res;

    }, "Test test_task.2 : Unable to process different logging level for different threads with successful finish");

    makeStep([]()
    {
        _testLog.setlevels({1});
        {
            auto task1 = _testLog.addTask(true);
            _testLog.addToLog(1, "+");
            {
                auto task2 = _testLog.addTask(true);
                _testLog.addToLog(2, "+");
                _testLog.addToLog(1, "+");
                task2.failed();
            }
            _testLog.addToLog(4, "-");
        }
        _testLog.addToLog(1, "+");
        _testLog.addToLog(4, "-");
        return ALoggerTest::_calls._outStrings == 4;
    }, "Test test_task.3 : Unable to process nested tasks");

    makeStep([]()
    {
        _testLog.setlevels({1});
        auto task_step = [](){
            auto task1 = _testLog.addTask(true);
            _testLog.addToLog(1, "+");
            _testLog.addToLog(2, "-");
        };
        std::thread another(task_step);
        {
            auto task1 = _testLog.addTask(true);
            _testLog.addToLog(1, "+");
            {
                auto task2 = _testLog.addTask(true);
                _testLog.addToLog(2, "+");
                _testLog.addToLog(1, "+");
                task2.failed();
            }
            _testLog.addToLog(4, "-");
        }
        _testLog.addToLog(1, "+");
        _testLog.addToLog(4, "-");
        another.join();
        return ALoggerTest::_calls._outStrings == 5;

    }, "Test test_task.4 : Unable to process nested tasks in different threads");

    makeStep([]()
    {
        return _loggerTest1Instances == 0;
    }, "Test test_task.last : Incorrect _loggerTest1Instances");

    return _errors;

}

size_t _testLogger_group()
{
    _errors = 0;

    _loggerTest1Instances = 0;
    logger_test_2_instances = 0;

    makeStep([]()
    {
        _logGrp.logger<0>().ClearFlags();   _logGrp.logger<1>().ClearFlags();
        if (_logGrp.sizeOf() != 2)  return false;

        _logGrp.logger<0>().setlevels({1}); _logGrp.logger<1>().setlevels({2});
        if (_logGrp.logger<0>().levels() != ALogger::TLevels{1} || _logGrp.logger<1>().levels() != ALogger::TLevels{2})
            return false;

        _logGrp.setlevels({3});
        if (_logGrp.logger<0>().levels() != ALogger::TLevels{3} || _logGrp.logger<1>().levels() != ALogger::TLevels{3})
            return false;

        _logGrp.initLevel(1, true);
        if (_logGrp.logger<0>().levels() != ALogger::TLevels{1,3} || _logGrp.logger<1>().levels() != ALogger::TLevels{1,3})
            return false;

        _logGrp.enableLevel(2);
        if (_logGrp.logger<0>().levels() != ALogger::TLevels{1,2,3} || _logGrp.logger<1>().levels() != ALogger::TLevels{1,2,3})
            return false;

        _logGrp.disableLevel(2);
        if (_logGrp.logger<0>().levels() != ALogger::TLevels{1,3} || _logGrp.logger<1>().levels() != ALogger::TLevels{1,3})
            return false;

        return true;
    }, "Test _testLogger_group.1 : Incorrect logging levels initialization");

    makeStep([]()
    {
        _logGrp.logger<0>().ClearFlags();   _logGrp.logger<1>().ClearFlags();
        _logGrp.setlevels({1});

        if (!_logGrp.forceAddToLog(1, "+") || !ALoggerTest::_calls._outStrings || !ALoggerTest2::_calls._outStrings)
            return false;

        _logGrp.logger<0>().ClearFlags(); _logGrp.logger<1>().ClearFlags();
        if (!_logGrp.addToLog(1, "+") || !ALoggerTest::_calls._outStrings || !ALoggerTest2::_calls._outStrings)
            return false;

        if (_logGrp.addToLog(2, "-") || ALoggerTest::_calls._outStrings != 1 || ALoggerTest2::_calls._outStrings != 1)
            return false;

        _logGrp.logger<0>().ClearFlags(); _logGrp.logger<1>().ClearFlags();

        return true;
    }, "Test _testLogger_group.2 : Incorrect addToLog, forceAddToLog output");

    makeStep([]()
    {
        return _loggerTest1Instances == 0 && logger_test_2_instances == 0;
    }, "Test _testLogger_group.last : Incorrect logger_test_x_instances");

    return _errors;
}

size_t _testLogger_base()
{
    _errors = 0;

    _loggerTest1Instances = 0;

    makeStep([]()
    {
        _testLog.ClearFlags();
        _testLog.setlevels({ 1 });
        if (!_testLog.addToLog(1, "+")      || !ALoggerTest::_calls._outStrings)
            return false;

        _testLog.ClearFlags();
        if (  _testLog.addToLog(2, "-")      ||  ALoggerTest::_calls._outStrings)
            return false;

        _testLog.ClearFlags();
        if (!_testLog.forceAddToLog(1, "+") || !ALoggerTest::_calls._outStrings)
            return false;

        _testLog.ClearFlags();
        if (!_testLog.forceAddToLog(2, "+") || !ALoggerTest::_calls._outStrings)
            return false;
        _testLog.ClearFlags();

        return true;
    }, "Test _testLogger_base.1 : Incorrect addToLog, forceAddToLog calls without task");

    makeStep([]()
    {
        _testLog.ClearFlags();

        _testLog.setlevels({ 1, 2, 4 });
        if (_testLog.levels() != ALogger::TLevels {1, 2, 4})
            return false;

        _testLog.initLevel(3);
        if (_testLog.levels() != ALogger::TLevels {1, 2, 3, 4})
            return false;

        _testLog.initLevel(3, false);
        if (_testLog.levels() != ALogger::TLevels {1, 2, 4})
            return false;

        _testLog.enableLevel(3);
        if (_testLog.levels() != ALogger::TLevels {1, 2, 3, 4})
            return false;

        _testLog.disableLevel(3);
        if (_testLog.levels() != ALogger::TLevels {1, 2, 4})
            return false;

        if (!_testLog.addToLog(1, "+") || _testLog.addToLog(3, "-") || ALoggerTest::_calls._outStrings != 1)
            return false;
        _testLog.ClearFlags();

        if (!_testLog.taskOrToBeAdded(1) || _testLog.taskOrToBeAdded(3))
            return false;

        return true;
    }, "Test _testLogger_base.2 : Incorrect setlevels, initLevel, enableLevel, disableLevel, taskOrToBeAdded calls without task");

    makeStep([](){
        return _loggerTest1Instances == 0;
    }, "Test _testLogger_base.last : Incorrect _loggerTest1Instances");

    return _errors;
}

size_t test_base()
{
    size_t res = 0;

    std::cout << "START test_base... ";

    _firstError = true;

    res += _testLogger_base();
    res += _testLogger_group();
    res += _testLogger_task();
    res += _testLogger_group_task();

    if (!res)
        std::cout << "OK" << std::endl;

    return res;
}