/*
Copyright (c) Helio Perroni Filho <xperroni@gmail.com>

This file is part of Cight.

Cight is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Cight is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Cight. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CIGHT_SETTINGS_HPP
#define CIGHT_SETTINGS_HPP

// Uncomment this flag to enable various GUI diagnostic windows.
#define DIAGNOSTICS

// Time parameter for cv::waitKey() calls, in milliseconds. Define as an empty
// macro to enable indefinite wait for user confirmation.
#define WAIT_KEY_MS 200
//#define WAIT_KEY_MS

#ifdef DIAGNOSTICS
#include <iostream>
#define LOG(message) std::cerr << message << std::endl;
#else
#define LOG(message)
#endif

#endif
