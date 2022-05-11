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
#include "logger.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define FOREACH_TAG(TAG) \
    TAG(TAG_DEMO)

enum TAG_ENUM
{
    FOREACH_TAG(GENERATE_ENUM)
        TAG_COUNT
};

static const char *TAG_STRING[] = {
    FOREACH_TAG(GENERATE_TAGSTRINGS)};

logger_tagDef_t *makeLoggerDef()
{
    logger_tagDef_t *def = malloc(sizeof(logger_tagDef_t[TAG_COUNT]));
    for (int i = 0; i < TAG_COUNT; i++)
    {
        def[i].tag = i;
        strcpy(def[i].info, TAG_STRING[i]);
    }
    return def;
}

void main()
{

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

    for (int i = 0; i < 1000; i++)
    {
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
        printf("Elapsed time is: %f\n", logger_timespecToFloat_ms(diff));
    }
    logger_tagPair_t list[1] = {{TAG_DEMO_START, TAG_DEMO_END}};
    logger_evaluate(list, 1, def, TAG_COUNT, NULL);
    logger_writeToCSV("test.csv", def, TAG_COUNT);
    logger_clear();
}