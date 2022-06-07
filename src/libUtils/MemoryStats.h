/*
 * Copyright (C) 2021 Zilliqa
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef ZILLIQA_SRC_LIBUTILS_MEMORYSTATS_H_
#define ZILLIQA_SRC_LIBUTILS_MEMORYSTATS_H_

#include <string>
void DisplayVirtualMemoryStats();
int64_t DisplayPhysicalMemoryStats(const std::string& str,
                                   int64_t startMem = 0);

int64_t display_PhysicalMemoryStats(const std::string& str,
                                   int64_t startMem = 0);

template<typename ...Ts> struct make_void   // NOLINT
{
    using type = void;
};

#endif  // ZILLIQA_SRC_LIBUTILS_MEMORYSTATS_H_
