/**
 * @copyright: (c) 2019-2022, Institute for Control Engineering of Machine Tools and Manufacturing Units,
 *             University of Stuttgart
 *             All rights reserved. Licensed under the Apache License, Version 2.0 (the "License");
 *             you may not use this file except in compliance with the License.
 *             You may obtain a copy of the License at
 *                  http://www.apache.org/licenses/LICENSE-2.0
 *             Unless required by applicable law or agreed to in writing, software
 *             distributed under the License is distributed on an "AS IS" BASIS,
 *             WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *             See the License for the specific language governing permissions and
 *             limitations under the License.
 * @author: Marc Fischer <marc.fischer@isw.uni-stuttgart.de>
 * @description: This file some static memory definition.
 */

#ifndef LOGGERMEM_H
#define LOGGERMEM_H
#include "logger.h"
// To store the logger results, static variables are used, so that an mem initialization must be called only once and
// not per compilation unit.
static logger_logEntry_t *_logger_logEntryList;
static int *_logger_nextEntry;
static logger_config_t _logger_config;
static int *_logger_errorCount;
#endif  // LOGGERMEM_H