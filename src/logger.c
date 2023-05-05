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
 * @description: This file contains the logger implementation.
 */
#include "logger.h"

#include <errno.h>
#include <fcntl.h>
#include <float.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "loggerMem.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <windows.h>
typedef uint64_t __uint64_t;
typedef uint32_t __uint32_t;
typedef int64_t __int64_t;

static long qpcFreq;
static long ticks2nano;
#endif
#ifdef __linux__

#include <malloc.h>
#include <sys/mman.h>

#include "limits.h"
#include "time.h"
#endif

static __uint32_t u = 0;

#ifdef __linux__
#if defined(__amd64__)

//! This Queries the rdtscp but the current cpu frequency is required to calculate the time.
//! Do NOT use this function for productiv systems!
static inline __uint64_t _rdtscp(__uint32_t *aux) {
    __uint64_t rax, rdx;
    asm volatile("rdtscp\n" : "=a"(rax), "=d"(rdx), "=c"(aux) : :);
    return (rdx << 32) + rax;
}
#endif
#endif

#ifdef WIN
int clock_getAsFileTime(struct timespec *spec)  // C-file part
{
    __int64_t wintime;
    GetSystemTimeAsFileTime((FILETIME *)&wintime);
    wintime -= 116444736000000000;               // 1jan1601 to 1jan1970
    (*spec).tv_sec = wintime / 10000000;         // seconds
    (*spec).tv_nsec = wintime % 10000000 * 100;  // nano-seconds
    return 0;
}

#define exp7 10000000            // 1E+7
#define exp9 1000000000          // 1E+9
#define w2ux 116444736000000000  // 1.jan1601 to 1.jan1970

void unix_time(struct timespec *spec) {
    __int64 wintime;
    GetSystemTimeAsFileTime((FILETIME *)&wintime);
    wintime -= w2ux;
    spec->tv_sec = wintime / exp7;
    spec->tv_nsec = wintime % exp7 * 100;
}

int clock_getAsQueryPerfCount(struct timespec *spec) {
    __int64_t curticks;
    QueryPerformanceCounter((LARGE_INTEGER *)&curticks);

    spec->tv_sec = (curticks / qpcFreq);
    spec->tv_nsec = (curticks % qpcFreq) * ticks2nano;
    if (!(spec->tv_nsec < exp9)) {
        spec->tv_sec++;
        spec->tv_nsec -= exp9;
    }
    return 0;
}

//! This Queries the rdtscp but the current cpu frequency is required to calculate the time.
//! Do NOT use this function for productiv systems!
void _clock_getAsRdtscp(struct timespec *spec) {
    long currFreq = 2000000000;  // only fix for test system.
    __int64_t time = __rdtsc();
    spec->tv_sec = time / currFreq;
    spec->tv_nsec = time % currFreq;
}
#endif

void _getTime(struct timespec *time, logger_clockType_t type) {
#ifdef WIN
    if (type == LCLOCK_WIN_AFILEDATE) {
        clock_getAsFileTime(time);
    } else if (type == LCLOCK_WIN_QUERYPERFCOUNTER) {
        clock_getAsQueryPerfCount(time);
    } else if (type == LCLOCK_RDTSCP) {
        _clock_getAsRdtscp(time);
    }
#endif  // WIN
#ifdef __linux__
    if (type == LCLOCK_LINUX_REALTIME) {
        clock_gettime(CLOCK_MONOTONIC, time);
    } else if (type == LCLOCK_LINUX_TIMEOFDAY) {
        struct timeval tv;
        gettimeofday(&tv, NULL);

        TIMEVAL_TO_TIMESPEC(&tv, time);
    }
#if defined(__amd64__)
    else if (type == LCLOCK_RDTSCP) {
        time->tv_nsec = _rdtscp(&u);
        time->tv_sec = 0;
    }
#endif
#endif
}

void logger_getTime(struct timespec *time) { _getTime(time, _logger_config.clockType); }

int logger_init(logger_config_t conf) {
#ifdef WIN
    QueryPerformanceFrequency((LARGE_INTEGER *)&qpcFreq);
    ticks2nano = exp9 / qpcFreq;
#endif
    _logger_config = conf;

    _logger_nextEntry = (int *)calloc(conf.listCount, sizeof(int));
    _logger_logEntryList = (logger_logEntry_t **)malloc(sizeof(logger_logEntry_t *) * conf.listCount);
    _logger_logEntryList[0] = (logger_logEntry_t *)malloc(sizeof(logger_logEntry_t) * conf.listSize * conf.listCount);
    _logger_errorCount = (int *)calloc(conf.listCount, sizeof(int));
#ifndef WIN
    int ret = mlock(_logger_nextEntry, sizeof(int) * conf.listCount);
    ret += mlock(_logger_nextEntry, sizeof(logger_logEntry_t *) * conf.listCount);
    ret += mlock(_logger_logEntryList[0], sizeof(logger_logEntry_t) * conf.listSize * conf.listCount);
    ret += mlock(_logger_errorCount, sizeof(int) * conf.listCount);
#endif

    for (int i = 1; i < conf.listCount; i++) {
        _logger_nextEntry[i] = 0;
        _logger_errorCount[i] = 0;
        _logger_logEntryList[i] = _logger_logEntryList[0] + i * conf.listSize;
    }
    // TODO error
    return 0;
}

void logger_reset() {
    for (int i = 1; i < _logger_config.listCount; i++) {
        _logger_nextEntry[i] = 0;
        _logger_errorCount[i] = 0;
    }
}

int logger_addLogEntry(logger_logTag_t tag, long id, int listNumber) {
    if (listNumber >= _logger_config.listCount) {
        _logger_errorCount[listNumber]++;
        return -1;
    }
    if (_logger_nextEntry[listNumber] >= _logger_config.listSize) {
        _logger_errorCount[listNumber]++;
        return -2;
    }
    logger_logEntry_t *entr = &_logger_logEntryList[listNumber][_logger_nextEntry[listNumber]];
    _getTime(&(entr->time_stamp), _logger_config.clockType);
    entr->id = id;
    entr->tag = tag;
    _logger_nextEntry[listNumber]++;

    return 0;
}

int logger_addLogEntryCustTime(logger_logTag_t tag, long id, int listNumber, struct timespec time) {
    if (listNumber >= _logger_config.listCount) {
        _logger_errorCount[listNumber]++;
        return -1;
    }
    if (_logger_nextEntry[listNumber] >= _logger_config.listSize) {
        _logger_errorCount[listNumber]++;
        return -2;
    } else {
        logger_logEntry_t *entr = &_logger_logEntryList[listNumber][_logger_nextEntry[listNumber]];
        entr->time_stamp = time;
        entr->id = id;
        entr->tag = tag;
        _logger_nextEntry[listNumber]++;
    }
    return 0;
}

unsigned long _log10(unsigned long v) {
    return (v >= 10000000000000000000u)  ? 19
           : (v >= 1000000000000000000u) ? 18
           : (v >= 100000000000000000u)  ? 17
           : (v >= 10000000000000000u)   ? 16
           : (v >= 1000000000000000u)    ? 15
           : (v >= 100000000000000u)     ? 14
           : (v >= 10000000000000u)      ? 13
           : (v >= 1000000000000u)       ? 12
           : (v >= 100000000000u)        ? 11
           : (v >= 10000000000u)         ? 10
           : (v >= 1000000000u)          ? 9
           : (v >= 100000000u)           ? 8
           : (v >= 10000000u)            ? 7
           : (v >= 1000000u)             ? 6
           : (v >= 100000u)              ? 5
           : (v >= 10000u)               ? 4
           : (v >= 1000u)                ? 3
           : (v >= 100u)                 ? 2
           : (v >= 10u)                  ? 1u
                                         : 0u;
}

int logger_writeToCSV(const char *fileName, logger_tagDef_t *logDef, int logDefCount) {
    return logger_writeListToCSV(fileName, NULL, -1, logDef, logDefCount);
}

int __compare(void const *lhs, void const *rhs) {
    double left = *(double *)lhs;
    double right = *(double *)rhs;
    if (left > right)
        return 1;
    else if (left < right)
        return -1;
    else
        return 0;
}

int logger_evaluate(logger_tagPair_t *pairList, int pairListCount, logger_tagDef_t *logDef, int logDefCount,
                    const char *csv_filename, const char *json_filename) {
#define DIFFSIZE 1000
    FILE *pCsvFile = NULL;
    FILE *pJsonFile = NULL;
    if (csv_filename != NULL) {
        pCsvFile = fopen(csv_filename, "w");
        if (!pCsvFile) {
            printf("[Error] Could not open files: %s\n", strerror(errno));
            return -2;
        }
        fprintf(pCsvFile, "\n");
        fprintf(pCsvFile, "TAGS;COUNT;MIN;MAX;AVG;MEDIAN\n");
    }
    if (json_filename != NULL) {
        pJsonFile = fopen(json_filename, "w");
        if (!pJsonFile) {
            printf("[Error] Could not open files: %s\n", strerror(errno));
            return -2;
        }
        fprintf(pJsonFile, "\n");
        fprintf(pJsonFile, "{\"data\":[\n");
    }
    for (int c = 0; c < pairListCount; c++) {
        logger_logTag_t tags = pairList[c].tag_start;
        logger_logTag_t tage = pairList[c].tag_end;
        double max = 0.0;
        double mean = 0.0;
        double min = FLT_MAX;
        size_t count = 0;
        unsigned int median_list_size = 1000;
        double *median_list = (double *)malloc(median_list_size * sizeof(double));

        for (int j = 0; j < _logger_config.listCount; j++) {
            logger_logEntry_t *it1_list = _logger_logEntryList[j];
            for (int i = 0; i < _logger_nextEntry[j]; i++) {
                logger_logEntry_t it1_entry = it1_list[i];
                bool it2_isBreak = false;
                if (it1_entry.tag == tags) {
                    for (int k = 0; k < _logger_config.listCount; k++) {
                        logger_logEntry_t *it2_list = _logger_logEntryList[k];
                        for (int l = 0; l < _logger_nextEntry[k]; l++) {
                            logger_logEntry_t it2_entry = it2_list[l];
                            if (it2_entry.tag == tage && it1_entry.id == it2_entry.id) {
                                struct timespec diff = logger_elapsedTime(it1_entry.time_stamp, it2_entry.time_stamp);
                                double diff_ms = logger_timespecToFloat_ms(diff);
                                // min
                                if (diff_ms < min) {
                                    min = diff_ms;
                                }
                                // max
                                if (diff_ms > max) {
                                    max = diff_ms;
                                }
                                // mean
                                mean += diff_ms;
                                // median
                                median_list[count] = diff_ms;

                                // loop control and counter
                                count += 1;
                                if (count >= median_list_size) {
                                    median_list_size *= 2;
                                    median_list = realloc(median_list, median_list_size * sizeof(double));
                                }
                                it2_isBreak = true;
                                break;
                            }
                        }
                        if (it2_isBreak) break;
                    }
                }
            }
        }

        // Evaluate median
        qsort(median_list, count, sizeof(double), __compare);
        size_t mid = count / 2U;
        double median = (count % 2 != 0) ? median_list[mid] : (median_list[mid] + median_list[mid - 1]) / 2.0;
        free(median_list);

        char infos[LOGGER_TAG_INFO_MAXLEN] = "";
        char infoe[LOGGER_TAG_INFO_MAXLEN] = "";
        for (int k = 0; k < logDefCount; k++) {
            if (tags == logDef[k].tag) {
                strncpy(infos, logDef[k].info, LOGGER_TAG_INFO_MAXLEN);
            }
            if (tage == logDef[k].tag) {
                strncpy(infoe, logDef[k].info, LOGGER_TAG_INFO_MAXLEN);
            }
        }
        if (csv_filename == NULL && json_filename == NULL) {
            printf("%s-%s | Count:%lu Min:%.5fms Max:%.5fms Mean:%.5fms Median:%.5fms\n", infos, infoe, count, min, max,
                   mean / count, median);
        }
        if (csv_filename != NULL) {
            fprintf(pCsvFile, "%s-%s;%lu;%.10f;%.10f;%.10f;%.10f\n", infos, infoe, count, min, max, mean / count,
                    median);
        }
        if (json_filename != NULL) {
            fprintf(pJsonFile, "\t{\n");
            fprintf(pJsonFile, "\t\t\"name\":\"%s-%s\",\n", infos, infoe);
            fprintf(pJsonFile, "\t\t\"count\":%lu,\n", count);
            fprintf(pJsonFile, "\t\t\"min\":%.10f,\n", min);
            fprintf(pJsonFile, "\t\t\"max\":%.10f,\n", max);
            fprintf(pJsonFile, "\t\t\"mean\":%.10f,\n", mean / count);
            fprintf(pJsonFile, "\t\t\"median\":%.10f\n", median);
            fprintf(pJsonFile, "\t}");
            if (c < (pairListCount - 1)) fprintf(pJsonFile, ",");
            fprintf(pJsonFile, "\n");
        }
    }
    if (csv_filename != NULL) fclose(pCsvFile);
    if (json_filename != NULL) {
        fprintf(pJsonFile, "]}");
        fclose(pJsonFile);
    }
    return 0;
}

int logger_evaluate_diff(logger_tagPair_t *pairList, int pairListCount, logger_tagDef_t *logDef, int logDefCount,
                         const char *csv_filename) {
    FILE *pFile = NULL;
    if (csv_filename != NULL) {
        pFile = fopen(csv_filename, "w");
        if (!pFile) {
            printf("[Error] Could not open files: %s\n", strerror(errno));
            return -2;
        }
        fprintf(pFile, "\n");
        fprintf(pFile, "TAGS;DIFF\n");
    }
    for (int c = 0; c < pairListCount; c++) {
        logger_logTag_t tags = pairList[c].tag_start;
        logger_logTag_t tage = pairList[c].tag_end;

        for (int j = 0; j < _logger_config.listCount; j++) {
            logger_logEntry_t *it1_list = _logger_logEntryList[j];
            for (int i = 0; i < _logger_nextEntry[j]; i++) {
                logger_logEntry_t it1_entry = it1_list[i];
                bool it2_isBreak = false;
                if (it1_entry.tag == tags) {
                    for (int k = 0; k < _logger_config.listCount; k++) {
                        logger_logEntry_t *it2_list = _logger_logEntryList[k];
                        for (int l = 0; l < _logger_nextEntry[k]; l++) {
                            logger_logEntry_t it2_entry = it2_list[l];
                            if (it2_entry.tag == tage && it1_entry.id == it2_entry.id) {
                                struct timespec diff = logger_elapsedTime(it1_entry.time_stamp, it2_entry.time_stamp);
                                double diff_ms = logger_timespecToFloat_ms(diff);
                                char infos[LOGGER_TAG_INFO_MAXLEN] = "";
                                char infoe[LOGGER_TAG_INFO_MAXLEN] = "";
                                for (int k = 0; k < logDefCount; k++) {
                                    if (tags == logDef[k].tag) {
                                        strncpy(infos, logDef[k].info, LOGGER_TAG_INFO_MAXLEN);
                                    }
                                    if (tage == logDef[k].tag) {
                                        strncpy(infoe, logDef[k].info, LOGGER_TAG_INFO_MAXLEN);
                                    }
                                }
                                if (csv_filename == NULL) {
                                    printf("%s-%s: %.12f\n", infos, infoe, diff_ms);
                                } else {
                                    fprintf(pFile, "%s;%s;%.12f\n", infos, infoe, diff_ms);
                                }
                                it2_isBreak = true;
                                break;
                            }
                        }
                        if (it2_isBreak) break;
                    }
                }
            }
        }
    }
    if (csv_filename != NULL) fclose(pFile);
    return 0;
}

int logger_writeListToCSV(const char *fileName, int *exportList, int exportListCount, logger_tagDef_t *logDef,
                          int logDefCount) {
    if (_logger_config.listCount == 0) {
        printf("[Error] No List allocated\n");
        return -1;
    }

    int exportOnlySpecificLists = 0;
    if (exportList != NULL && exportListCount > 0) {
        exportOnlySpecificLists = 1;
    }

    FILE *pFile = fopen(fileName, "w");
    if (!pFile) {
        printf("[Error] Could not open files\n");
        return -2;
    }
    fprintf(pFile, "\n");
    long startTime = INT_MAX;
    for (int j = 0; j < _logger_config.listCount; j++) {
        if (exportOnlySpecificLists) {
            // Check if current is in exportList
            int isInList = 0;
            for (int k = 0; k < exportListCount; k++) {
                if (exportList[k] == j) {
                    isInList = 1;
                }
            }
            if (!isInList) {
                continue;
            }
        }
        if (_logger_logEntryList[j][0].time_stamp.tv_sec > 0 &&
            _logger_logEntryList[j][0].time_stamp.tv_sec < startTime) {
            startTime = (long)_logger_logEntryList[j][0].time_stamp.tv_sec;
        }
    }

    for (int j = 0; j < _logger_config.listCount; j++) {
        if (exportOnlySpecificLists) {
            // Check if current is in exportList
            int isInList = 0;
            for (int k = 0; k < exportListCount; k++) {
                if (exportList[k] == j) {
                    isInList = 1;
                }
            }
            if (!isInList) {
                continue;
            }
        }
        for (int i = 0; i < _logger_nextEntry[j]; i++) {
            logger_logEntry_t *lEntr = &_logger_logEntryList[j][i];
            int stellen = _log10(lEntr->time_stamp.tv_nsec);
            int restZeros = 8 - stellen;
            char zeroString[9] = "";
            for (int j = 0; j < restZeros; j++) {
                strcat(zeroString, "0");
            }
            char info[LOGGER_TAG_INFO_MAXLEN] = "";
            for (int k = 0; k < logDefCount; k++) {
                if (lEntr->tag == logDef[k].tag) {
                    strncpy(info, logDef[k].info, LOGGER_TAG_INFO_MAXLEN);
                    break;
                }
            }
            fprintf(pFile, "%s,%lu,%d.%s%ld\n", info, lEntr->id, (int)(lEntr->time_stamp.tv_sec - startTime),
                    zeroString, lEntr->time_stamp.tv_nsec);
        }
    }

    fclose(pFile);
    return 0;
}

int *logger_getErrorCount() { return _logger_errorCount; }

struct timespec logger_elapsedTime(struct timespec start, struct timespec end) {
    struct timespec temp;
    if ((end.tv_nsec - start.tv_nsec) < 0) {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1l;
        temp.tv_nsec = 1000000000l + end.tv_nsec - start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}

int logger_cmpTime(struct timespec first, struct timespec second) {
    if (first.tv_sec == second.tv_sec) {
        if (first.tv_nsec == second.tv_nsec) {
            return 0;
        } else {
            if (first.tv_nsec < second.tv_nsec) {
                return -1;
            } else {
                return 1;
            }
        }
    } else {
        if (first.tv_sec < second.tv_sec) {
            return -1;
        } else {
            return 1;
        }
    }
}
float logger_timespecToFloat_ms(struct timespec time) {
    return (float)time.tv_sec * 1000.0f + (float)time.tv_nsec / 1000000.0f;
}

void logger_clear() {
#ifndef WIN
    munlock(_logger_nextEntry, sizeof(int) * _logger_config.listCount);
    munlock(_logger_logEntryList[0], sizeof(logger_logEntry_t) * _logger_config.listSize * _logger_config.listCount);
    munlock(_logger_logEntryList, sizeof(logger_logEntry_t *) * _logger_config.listCount);
#endif
    free(_logger_nextEntry);
    free(_logger_logEntryList[0]);
    free(_logger_logEntryList);
}