
#include <functional>
#include <iostream>
#include <string>
#include <thread>

#include <tests.h>
#include <avn/logger/logger_base.h>
#include <avn/logger/logger_task.h>
#include <avn/logger/logger_group.h>

using namespace std::string_literals;

namespace
{

class Logger : public ALogger::LoggerBase<std::string>
{
public:
    Logger() : LoggerBase(true)    {}
    size_t cntr = 0;

    void SetLogLevel(ALogger::TLevel level) noexcept    { _level = level; }
    bool operator<<(const std::string& str) noexcept    { return AddToLog(_level, str); }

private:
    ALogger::TLevel _level;
    bool OutputImpl(const TLogData& logData) noexcept override
    {
        (void) logData;
        ++cntr;
        return true;
    }
};

class Logger2 : public ALogger::LoggerBase<std::string>
{
public:
    Logger2() : LoggerBase(false)    {}
    size_t cntr = 0;

    void SetLogLevel(ALogger::TLevel level) noexcept    { _level = level; }
    bool operator<<(const std::string& str) noexcept    { return AddToLog(_level, str); }

private:
    ALogger::TLevel _level;
    bool OutputImpl(const TLogData& logData) noexcept override
    {
        (void) logData;
        ++cntr;
        return true;
    }
};

size_t _testLogger_base()
{
    size_t errors = 0;
    
    Logger base;

    base.SetLevels({1, 2, 4});
    base.DisableLevel(4);
    base.DisableLevel(3);
    base.DisableLevels({1, 3});
    base.EnableLevel(2);
    base.EnableLevels({1, 5});
    
    errors += ALogger::UnitTesting("base", "Invalid SetLevels/DisableLevel(s)/EnableLevel(s) calls", [&base]()
    {
        return base.Levels().count(1) && base.Levels().count(2) && !base.Levels().count(3) && !base.Levels().count(4) && base.Levels().count(5);
    } );

    base.cntr = 0;
    base.DisableLogger();
    base.SetLogLevel(0);    base << "test"s;
    base.SetLogLevel(1);    base << "test"s;
    errors += ALogger::UnitTesting("base", "Invalid DisableLogger/LoggerEnabled calls", [&base]()
    {
        return base.cntr == 0 && !base.IsLoggerEnabled();
    });

    base.cntr = 0;
    base.EnableLogger();
    base.SetLogLevel(0);    base << "test"s;
    base.SetLogLevel(1);    base << "test"s;
    errors += ALogger::UnitTesting("base", "Invalid EnableLogger/LoggerEnabled calls", [&base]()
    {
        return base.cntr == 1 && base.IsLoggerEnabled();
    });

    base.cntr = 0;
    base.SetLogLevel(0);    base << "test"s;
    base.SetLogLevel(1);    base << "test"s;
    errors += ALogger::UnitTesting("base", "Invalid output log levels usage", [&base]()
    {
        return base.cntr == 1;
    });

    base.cntr = 0;
    base.ForceOutput();
    base.SetLogLevel(1);    base << "test"s;
    base.SetLogLevel(10);   base << "test"s;
    base.UnforceOutput();
    base.SetLogLevel(10);   base << "test"s;
    errors += ALogger::UnitTesting("base", "Invalid ForceOutput/UnforceOutput calls processing", [&base]()
    {
        return base.cntr == 2;
    } );

    return errors;
}

size_t _testLogger_task_base()
{
    size_t errors = 0;
    
    Logger base;
    
    base.SetLevels({1, 2, 4});
    base.DisableLevel(4);
    base.DisableLevel(3);
    base.DisableLevels({1, 3});
    base.EnableLevel(2);
    base.EnableLevels({1, 5});

    {
        auto task = base.StartTask();
        base.SetLevels({});
        errors += ALogger::UnitTesting("task_base", "Invalid task level processing: clear logger levels after task initialization", [&base, &task]()
        {
            return !base.Levels().count(1) && !base.Levels().count(2) && !base.Levels().count(3) && !base.Levels().count(4) && !base.Levels().count(5) &&
                   task->Levels().count(1) && task->Levels().count(2) && !task->Levels().count(3) && !task->Levels().count(4) && task->Levels().count(5);
        } );

        task->SetLevels({});
        errors += ALogger::UnitTesting("task_base", "Invalid task level processing: clear task levels", [&base, &task]()
        {
            return !task->Levels().count(1) && !task->Levels().count(2) && !task->Levels().count(3) && !task->Levels().count(4) && !task->Levels().count(5);
        } );

        task->DisableLevel(4);
        task->DisableLevel(3);
        task->DisableLevels({1, 3});
        task->EnableLevel(2);
        task->EnableLevels({1, 5});
        errors += ALogger::UnitTesting("task_base", "Invalid task level processing: DisableLevel(s)/EnableLevel(s)", [&base, &task]()
        {
            return task->Levels().count(1) && task->Levels().count(2) && !task->Levels().count(3) && !task->Levels().count(4) && task->Levels().count(5);
        } );

        base.cntr = 0;
        base.SetLogLevel(0);    base << "test"s;
        base.SetLogLevel(1);    base << "test"s;
        task->Success();

        base.UnforceOutput(false);
        base.SetLogLevel(15);    base << "test"s;
        errors += ALogger::UnitTesting("task_base", "Invalid task outputs while task is not closed", [&base, &task]()
        {
            return base.cntr == 1;
        } );

        base.UnforceOutput();
        base.cntr = 0;
    }
    errors += ALogger::UnitTesting("task_base", "Invalid task outputs after task closing", [&base]()
    {
        return base.cntr == 1;
    } );

    {
        auto task = base.StartTask();

        base.DisableTasks();
        base.SetLogLevel(1);    base << "test"s;
        errors += ALogger::UnitTesting("task_base", "Invalid DisableTasks call processing", [&base]()
        {
            return base.cntr == 1;
        } );
        base.EnableTasks();
    }

    {
        auto task = base.StartTask();
        task->SetLevels({1});

        base.cntr = 0;
        base.SetLogLevel(0);    base << "test"s;
        base.SetLogLevel(1);    base << "test"s;

        task->Fail();
    }
    errors += ALogger::UnitTesting("task_base", "Invalid Fail call processing", [&base]()
    {
        return base.cntr == 2;
    } );

    return errors;
}

size_t _testLogger_group_base()
{
    size_t errors = 0;
    
    ALogger::LoggerGroup<Logger, Logger2> grp;
    errors += ALogger::UnitTesting("group_base", "Invalid SizeOf call processing ", [&grp]()
    {
        return grp.SizeOf() == 2;
    } );

    grp.SetLevels({1});
    grp.Logger<0>().SetLogLevel(1); grp.Logger<1>().SetLogLevel(1);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
    errors += ALogger::UnitTesting("group_base", "Invalid SetLevels call processing ", [&grp]()
    {
        return grp.Logger<0>().cntr == 1 && grp.Logger<1>().cntr == 1;
    } );

    grp.SetLevels({1, 2, 3, 4});
    grp.EnableLevels({5, 6});
    grp.EnableLevel(7);
    grp.DisableLevels({2, 3});
    grp.DisableLevel(1);

    grp.Logger<0>().cntr = 0;   grp.Logger<1>().cntr = 0;
    grp.Logger<0>().SetLogLevel(1); grp.Logger<1>().SetLogLevel(1);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
    grp.Logger<0>().SetLogLevel(2); grp.Logger<1>().SetLogLevel(2);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
    grp.Logger<0>().SetLogLevel(3); grp.Logger<1>().SetLogLevel(3);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
    grp.Logger<0>().SetLogLevel(4); grp.Logger<1>().SetLogLevel(4);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
    grp.Logger<0>().SetLogLevel(5); grp.Logger<1>().SetLogLevel(5);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
    grp.Logger<0>().SetLogLevel(6); grp.Logger<1>().SetLogLevel(6);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
    grp.Logger<0>().SetLogLevel(7); grp.Logger<1>().SetLogLevel(7);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
    errors += ALogger::UnitTesting("group_base", "Invalid SetLevels/EnableLevel(s)/DisableLevel(s) calls processing ", [&grp]()
    {
        return grp.Logger<0>().cntr == 4 && grp.Logger<1>().cntr == 4;
    } );

    grp.Logger<0>().cntr = 0;   grp.Logger<1>().cntr = 0;
    grp.ForceOutput();
    grp.Logger<0>().SetLogLevel(1); grp.Logger<1>().SetLogLevel(1);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
    grp.Logger<0>().SetLogLevel(2); grp.Logger<1>().SetLogLevel(2);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
    grp.Logger<0>().SetLogLevel(3); grp.Logger<1>().SetLogLevel(3);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
    grp.Logger<0>().SetLogLevel(4); grp.Logger<1>().SetLogLevel(4);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
    grp.Logger<0>().SetLogLevel(5); grp.Logger<1>().SetLogLevel(5);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
    grp.Logger<0>().SetLogLevel(6); grp.Logger<1>().SetLogLevel(6);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
    grp.Logger<0>().SetLogLevel(7); grp.Logger<1>().SetLogLevel(7);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
    errors += ALogger::UnitTesting("group_base", "Invalid ForceOutput call processing ", [&grp]()
    {
        return grp.Logger<0>().cntr == 7 && grp.Logger<1>().cntr == 7;
    } );

    grp.Logger<0>().cntr = 0;   grp.Logger<1>().cntr = 0;
    grp.UnforceOutput();
    grp.Logger<0>().SetLogLevel(1); grp.Logger<1>().SetLogLevel(1);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
    grp.Logger<0>().SetLogLevel(2); grp.Logger<1>().SetLogLevel(2);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
    grp.Logger<0>().SetLogLevel(3); grp.Logger<1>().SetLogLevel(3);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
    grp.Logger<0>().SetLogLevel(4); grp.Logger<1>().SetLogLevel(4);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
    grp.Logger<0>().SetLogLevel(5); grp.Logger<1>().SetLogLevel(5);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
    grp.Logger<0>().SetLogLevel(6); grp.Logger<1>().SetLogLevel(6);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
    grp.Logger<0>().SetLogLevel(7); grp.Logger<1>().SetLogLevel(7);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
    errors += ALogger::UnitTesting("group_base", "Invalid UnforceOutput call processing ", [&grp]()
    {
        return grp.Logger<0>().cntr == 4 && grp.Logger<1>().cntr == 4;
    } );

    return errors;
}

size_t _testLogger_group_task_base()
{
    size_t errors = 0;
    
    ALogger::LoggerGroup<Logger, Logger2> grp;
    grp.SetLevels({1, 2, 3, 4});

    {
        auto task = grp.StartTask();

        grp.Logger<0>().cntr = 0;       grp.Logger<1>().cntr = 0;
        grp.Logger<0>().SetLogLevel(0); grp.Logger<1>().SetLogLevel(0);
        grp.Logger<0>() << "test"s;     grp.Logger<1>() << "test"s;

        task->Success();
    }
    errors += ALogger::UnitTesting("group_task_base", "Invalid SetLogLevel call processing for disabled levels", [&grp]()
    {
        return grp.Logger<0>().cntr == 0 && grp.Logger<1>().cntr == 0;
    } );

    {
        auto task = grp.StartTask();

        grp.Logger<0>().cntr = 0;       grp.Logger<1>().cntr = 0;
        grp.Logger<0>().SetLogLevel(1); grp.Logger<1>().SetLogLevel(1);
        grp.Logger<0>() << "test"s;     grp.Logger<1>() << "test"s;

        task->Fail();
    }
    errors += ALogger::UnitTesting("group_task_base", "Invalid SetLogLevel call processing for enabled levels", [&grp]()
    {
        return grp.Logger<0>().cntr == 1 && grp.Logger<1>().cntr == 1;
    } );

    {
        auto task = grp.StartTask();
        task->SetLevels({1, 2, 3, 4});
        task->EnableLevels({5, 6});
        task->EnableLevel(7);
        task->DisableLevels({2, 3});
        task->DisableLevel(1);

        grp.Logger<0>().cntr = 0;   grp.Logger<1>().cntr = 0;
        grp.Logger<0>().SetLogLevel(1); grp.Logger<1>().SetLogLevel(1);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
        grp.Logger<0>().SetLogLevel(2); grp.Logger<1>().SetLogLevel(2);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
        grp.Logger<0>().SetLogLevel(3); grp.Logger<1>().SetLogLevel(3);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
        grp.Logger<0>().SetLogLevel(4); grp.Logger<1>().SetLogLevel(4);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
        grp.Logger<0>().SetLogLevel(5); grp.Logger<1>().SetLogLevel(5);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
        grp.Logger<0>().SetLogLevel(6); grp.Logger<1>().SetLogLevel(6);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
        grp.Logger<0>().SetLogLevel(7); grp.Logger<1>().SetLogLevel(7);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;

        task->Success();
    }
    errors += ALogger::UnitTesting("group_task_base", "Invalid SetLevels/EnableLevel(s)/DisableLevel(s) calls processing for succeeded task", [&grp]()
    {
        return grp.Logger<0>().cntr == 4 && grp.Logger<1>().cntr == 4;
    } );

    {
        auto task = grp.StartTask();
        task->SetLevels({1, 2, 3, 4});

        grp.Logger<0>().cntr = 0;   grp.Logger<1>().cntr = 0;
        grp.Logger<0>().SetLogLevel(1); grp.Logger<1>().SetLogLevel(1);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
        grp.Logger<0>().SetLogLevel(2); grp.Logger<1>().SetLogLevel(2);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
        grp.Logger<0>().SetLogLevel(3); grp.Logger<1>().SetLogLevel(3);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
        grp.Logger<0>().SetLogLevel(4); grp.Logger<1>().SetLogLevel(4);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
        grp.Logger<0>().SetLogLevel(5); grp.Logger<1>().SetLogLevel(5);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
        grp.Logger<0>().SetLogLevel(6); grp.Logger<1>().SetLogLevel(6);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;
        grp.Logger<0>().SetLogLevel(7); grp.Logger<1>().SetLogLevel(7);    grp.Logger<0>() << "test"s;  grp.Logger<1>() << "test"s;

        task->Fail();
    }
    errors += ALogger::UnitTesting("group_task_base", "Invalid SetLevels/EnableLevel(s)/DisableLevel(s) calls processing for failed task", [&grp]()
    {
        return grp.Logger<0>().cntr == 7 && grp.Logger<1>().cntr == 7;
    } );

    return errors;
}
}

namespace ALogger{
size_t test_base()
{
    size_t res = 0;

    std::cout << "[LOGGER] Base test cases" << std::endl;

    res += _testLogger_base();
    res += _testLogger_task_base();
    res += _testLogger_group_base();
    res += _testLogger_group_task_base();

    return res;
}
}