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
 * @description: This file contains the API defintion for the logger.
 */

#ifndef RTPERFLOGGER_H
#define RTPERFLOGGER_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64) && !defined(__CYGWIN__)
#define WIN 1
#include "time.h"
#endif
#ifdef __linux__
#include "sys/time.h"
#endif

// Helper defines to generate TAGs
#define TAG(X) X##_START
#define GENERATE_ENUM(ENUM) ENUM##_START, ENUM##_END,
#define GENERATE_STRING(STRING) #STRING
#define GENERATE_TAGSTRINGS(TAG) GENERATE_STRING(TAG##_START), GENERATE_STRING(TAG##_END),

#define LOGGER_TAG_INFO_MAXLEN 25

// Type define for a Tag.
typedef int logger_logTag_t;

/**
 * A log entry consists of a tag, an id, and a timestamp.
 * @property {logger_logTag_t} tag - This is a tag to differentiate between
 * different log points.
 * @property {unsigned long} id - This is a unique id for each log entry. It is
 * used to identify multiple runs of the same tag.
 * @property time_stamp - The timestamp of the log entry.
 */
typedef struct {
    logger_logTag_t tag;
    unsigned long id;
    struct timespec time_stamp;
} logger_logEntry_t;

/**
 * `logger_tagDef_t` is a structure that contains meta information for a `logger_logTag_t`.
 * @property {logger_logTag_t} tag - The tag to be used for the log.
 * @property {char} info - This is the string that will be printed while exporting. info is a
 * `char` array of length `LOGGER_TAG_INFO_MAXLEN`.
 */
typedef struct {
    logger_logTag_t tag;
    char info[LOGGER_TAG_INFO_MAXLEN];
} logger_tagDef_t;

/**
 * `logger_tagPair_t` is a struct that contains two `logger_logTag_t`s. It is
 * used to make calculations between a tag pair.
 * @property {logger_logTag_t} tag_start - The starting tag
 * the log.
 * @property {logger_logTag_t} tag_end - The ending tag.
 */
typedef struct {
    logger_logTag_t tag_start;
    logger_logTag_t tag_end;
} logger_tagPair_t;

/**
 * It's a list of all the different ways we can measure time.
 */
typedef enum {
#if defined(__amd64__) || defined(_M_AMD64) || defined(_M_X64) || defined(_M_IX86)
    LCLOCK_RDTSCP,
#endif
#if defined(WIN)
    LCLOCK_WIN_AFILEDATE,
    LCLOCK_WIN_QUERYPERFCOUNTER,
#endif
#ifdef __linux__
    LCLOCK_LINUX_REALTIME,
    LCLOCK_LINUX_TIMEOFDAY
#endif
} logger_clockType_t;

/**
 * `logger_config_t` is a struct to configure the logger while initialization.
 * @property {logger_clockType_t} clockType - The type of clock to use for the
 * logger.
 * @property {int} listCount - The number of list. You can use different list e.g. for each thread in a multithreaded
 * environment.
 * @property {int} listSize - The size of each list. This is the maximum count of tags per list.
 */
typedef struct {
    logger_clockType_t clockType;
    int listCount;
    int listSize;
} logger_config_t;

// Realtime safe functions with very small performance impact
//------------------------------------------------------------------------------------------------------------------
/**
 * > This function adds a log entry to the log entry list with the current time.
 *
 * @param tag The tag is a number that identifies the type of log entry.  For
 * example, you might have a tag for a "start" event, a "stop" event, etc.
 * The tag is of type `logger_logTag_t`.
 * @param id This is the id of the log entry. This is a unique id for each log entry. It is
 * used to identify multiple runs of the same tag.
 * @param listNumber The number of the list to add the entry to.
 *
 * @return 0=success;-1=list not found;-2=list overflow
 */
int logger_addLogEntry(logger_logTag_t tag, long id, int listNumber);
/**
 * > This function adds a log entry to the specified log list with a custom timestamp
 *
 * @param tag The tag is a number that identifies the type of log entry.  For
 * example, you might have a tag for a "start" event, a "stop" event, etc.
 * The tag is of type `logger_logTag_t`.
 * @param id This is the id of the log entry. This is a unique id for each log entry. It is
 * used to identify multiple runs of the same tag.
 * @param listNumber The number of the list to add the entry to.
 * @param time The time of the event.
 *
 * @return 0=success;-1=list not found;-2=list overflow
 */
int logger_addLogEntryCustTime(logger_logTag_t tag, long id, int listNumber, struct timespec time);

// Non real-time safe functions. They are used to set up the logger and save the results. You must call them after using
//------------------------------------------------------------------------------------------------------------------
/**
 * Initialize the logger. Use the conf to configure the logger.It allocates memory for the logger.
 *
 * @param conf the configuration of the logger
 *
 * @return 0=success;else=error
 */
int logger_init(logger_config_t conf);
/**
 * > Write the content of the log lists to a CSV file
 *
 * @param fileName The name of the file to write to.
 * @param logDef This is a pointer to an array of logger_tagDef_t structures.
 * @param logDefCount The number of tags in the logDef array.
 *
 * @return 0=success;-2=file error;
 */
int logger_writeToCSV(const char *fileName, logger_tagDef_t *logDef, int logDefCount);
/**
 * It writes the log entries of a specific log list to a CSV file
 *
 * @param fileName The name of the file to write to.
 * @param exportList Array of list numbers that should be exported. If NULL,
 * all lists will be exported.
 * @param exportListCount The number of lists to export in exportList.
 * @param logDef This is a pointer to an array of logger_tagDef_t structures.
 * @param logDefCount The number of log definitions in logDef
 *
 * @return 0=success;-1=list not found;-2=file error;
 */
int logger_writeListToCSV(const char *fileName, int *listIds, int listIdsCount, logger_tagDef_t *logDef,
                          int logDefCount);

/**
 * It takes a list of tag pairs and a list of tag definitions and exports out the
 * min, max, mean and median of the time difference between the tags
 *
 * @param pairList A list of tag pairs to evaluate.
 * @param pairListCount The number of pairs of tags to evaluate.
 * @param logDef This is a list of all the tag meta definitions that you want to evaluate.
 * @param logDefCount The number of tag definitions.
 * @param csv_filename The name of the file to write the results to. If NULL, the
 * results will be printed to the console.
 *
 * @return The return value is the status of the function. 0=Sucess;-2=Could not open file
 */
int logger_evaluate(logger_tagPair_t *pairList, int pairListCount, logger_tagDef_t *logDef, int logDefCount,
                    const char *csv_filename);

/**
 * It takes a list of tag pairs and a list of tag definitions and exports the
 * time difference between each pair of tags
 *
 * @param pairList  A list of tag pairs to evaluate.
 * @param pairListCount The number of pairs of tags to evaluate
 * @param logDef This is a list of all the tag meta definitions you want to evaluate.
 * @param logDefCount The number of tag definitions in the logDef array.
 * @param csv_filename The name of the file to write the results to. If NULL,
 * the results will be printed to the console.
 *
 * @returnThe return value is the status of the function. 0=Sucess;-2=Could not open file
 */
int logger_evaluate_diff(logger_tagPair_t *pairList, int pairListCount, logger_tagDef_t *logDef, int logDefCount,
                         const char *csv_filename);

/**
 * > Returns the count of errors while trying to wirte to the log list.
 *
 * @return A pointer to the variable _logger_errorCount.
 */
int *logger_getErrorCount();

/**
 * It frees the memory allocated by the logger_init() function
 */
void logger_clear();
/**
 * It gets the time according to the configuration.
 * @param time Reference where the time is written to.
 */
void logger_getTime(struct timespec *time);

/**
 * > This function takes two timespecs as arguments and returns the difference
 * between them
 *
 * @param start The start time of the function
 * @param end The end time of the process.
 *
 * @return The difference between the two times.
 */
struct timespec logger_elapsedTime(struct timespec start, struct timespec end);

// Some helper functions

/**
 * Compare first and second value
 * @return 0 when equal; -1 when first<second and 1 when first>second
 */
int logger_cmpTime(struct timespec first, struct timespec second);

/**
 * Converts a timespec to a millisecond float.
 * @param time The time to convert
 *
 * @return A float value that is the time in milliseconds.
 */
float logger_timespecToFloat_ms(struct timespec time);

#ifdef __cplusplus
}
#endif

#endif  // RTPERFLOGGER_H