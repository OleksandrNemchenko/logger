//
// Created by Oleksandr Nemchenko on 11/5/19.
//

#ifndef _AVN_LOGGER_LOGGER_TXT_GROUP_TASK_H
#define _AVN_LOGGER_LOGGER_TXT_GROUP_TASK_H

#include <avn/logger/logger_group_task.h>

namespace Logger {

    template< typename... TLogger >
    class CLoggerTxtGroupTask : public CLoggerGroupTask<TLogger...> {
    };  // class CLoggerTxtGroup

}   // namespace Logger

#endif // _AVN_LOGGER_LOGGER_TXT_GROUP_TASK_H
