//
// Created by Oleksandr Nemchenko on 11/5/19.
//

#ifndef _AVN_LOGGER_LOGGER_TXT_GROUP_H
#define _AVN_LOGGER_LOGGER_TXT_GROUP_H

#include <avn/logger/logger_group.h>

namespace Logger {

    template< typename... TLogger >
    class CLoggerTxtGroup : public CLoggerGroup<TLogger...> {
    };  // class CLoggerGroup

}   // namespace Logger

#endif // _AVN_LOGGER_LOGGER_GROUP_H
