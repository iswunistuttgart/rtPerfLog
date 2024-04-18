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
 * @description: This file contains some test for rtPerfLog
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"
#ifndef WIN
#include <unistd.h>
#else
#include <windows.h>
#endif

#define TAGS(TAG) TAG(TAG_DEMO) TAG(TAG_DEMO2)

GENERATE_DEF(TAGS)

int main() {
    logger_tagDef_t *def = makeLoggerDef();
    logger_config_t conf;
#ifdef WIN
    conf.clockType = LCLOCK_WIN_QUERYPERFCOUNTER;
#else
    conf.clockType = LCLOCK_LINUX_REALTIME;
#endif
    conf.listCount = 1;
    conf.listSize = 100000;
    logger_init(conf);
    struct timespec start, end;

    for (int i = 0; i < 1000; i++) {
        logger_getTime(&start);
        logger_addLogEntry(TAG_DEMO_START, i, 0);
#ifdef WIN
        Sleep(100);
#else
        usleep(100);
#endif
        logger_getTime(&end);
        logger_addLogEntry(TAG_DEMO_END, i, 0);
        struct timespec diff = logger_elapsedTime(start, end);
        printf("Elapsed time is: %fms\n", logger_timespecToFloat_ms(diff));
    }
    logger_evaluate(evalListFull, evalListFullSize, def, TAG_COUNT, NULL, NULL);
    logger_evaluate(evalListFull, evalListFullSize, def, TAG_COUNT, "test_eval.csv", "test_eval.json");
    logger_writeToCSV("test.csv", def, TAG_COUNT);
    logger_reset();
    logger_evaluate(evalListFull, evalListFullSize, def, TAG_COUNT, NULL, NULL);
    logger_clear();
    return 0;
}