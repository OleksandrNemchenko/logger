//
// Created by Oleksandr Nemchenko on 11/5/19.
//

/*! \file data_types.h
 * \brief Logger data types that are used by library.
 */

#ifndef _AVN_LOGGER_BASE_DATA_TYPES_H
#define _AVN_LOGGER_BASE_DATA_TYPES_H

#include <cstddef>
#include <set>

namespace Logger {

    /** Levels that are used by logger */
    using TLevels = std::set<std::size_t>;

}   // namespace Logger

#endif //_AVN_LOGGER_BASE_DATA_TYPES_H
