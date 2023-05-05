---
title: include/logger.h

---

# include/logger.h



## Classes

|                | Name           |
| -------------- | -------------- |
| struct | **[logger_logEntry_t](Classes/structlogger__logEntry__t.md)**  |
| struct | **[logger_tagDef_t](Classes/structlogger__tagDef__t.md)**  |
| struct | **[logger_tagPair_t](Classes/structlogger__tagPair__t.md)**  |
| struct | **[logger_config_t](Classes/structlogger__config__t.md)**  |

## Types

|                | Name           |
| -------------- | -------------- |
| enum| **[logger_clockType_t](Files/logger_8h.md#enum-logger-clocktype-t)** { LCLOCK_RDTSCP, LCLOCK_LINUX_REALTIME, LCLOCK_LINUX_TIMEOFDAY, LCLOCK_LINUX_PTP} |
| typedef int | **[logger_logTag_t](Files/logger_8h.md#typedef-logger-logtag-t)**  |

## Functions

|                | Name           |
| -------------- | -------------- |
| int | **[logger_addLogEntry](Files/logger_8h.md#function-logger-addlogentry)**(logger_logTag_t tag, long id, int listNumber) |
| int | **[logger_addLogEntryCustTime](Files/logger_8h.md#function-logger-addlogentrycusttime)**(logger_logTag_t tag, long id, int listNumber, struct timespec time) |
| int | **[logger_init](Files/logger_8h.md#function-logger-init)**([logger_config_t](Classes/structlogger__config__t.md) conf) |
| int | **[logger_writeToCSV](Files/logger_8h.md#function-logger-writetocsv)**(const char * fileName, [logger_tagDef_t](Classes/structlogger__tagDef__t.md) * logDef, int logDefCount) |
| int | **[logger_writeListToCSV](Files/logger_8h.md#function-logger-writelisttocsv)**(const char * fileName, int * listIds, int listIdsCount, [logger_tagDef_t](Classes/structlogger__tagDef__t.md) * logDef, int logDefCount) |
| int | **[logger_evaluate](Files/logger_8h.md#function-logger-evaluate)**([logger_tagPair_t](Classes/structlogger__tagPair__t.md) * pairList, int pairListCount, [logger_tagDef_t](Classes/structlogger__tagDef__t.md) * logDef, int logDefCount, const char * csv_filename) |
| int | **[logger_evaluate_diff](Files/logger_8h.md#function-logger-evaluate-diff)**([logger_tagPair_t](Classes/structlogger__tagPair__t.md) * pairList, int pairListCount, [logger_tagDef_t](Classes/structlogger__tagDef__t.md) * logDef, int logDefCount, const char * csv_filename) |
| int * | **[logger_getErrorCount](Files/logger_8h.md#function-logger-geterrorcount)**() |
| void | **[logger_clear](Files/logger_8h.md#function-logger-clear)**() |
| void | **[logger_getTime](Files/logger_8h.md#function-logger-gettime)**(struct timespec * time) |
| struct timespec | **[logger_elapsedTime](Files/logger_8h.md#function-logger-elapsedtime)**(struct timespec start, struct timespec end) |
| int | **[logger_cmpTime](Files/logger_8h.md#function-logger-cmptime)**(struct timespec first, struct timespec second) |
| float | **[logger_timespecToFloat_ms](Files/logger_8h.md#function-logger-timespectofloat-ms)**(struct timespec time) |

## Defines

|                | Name           |
| -------------- | -------------- |
|  | **[TAG](Files/logger_8h.md#define-tag)**(X)  |
|  | **[GENERATE_ENUM](Files/logger_8h.md#define-generate-enum)**(ENUM)  |
|  | **[GENERATE_STRING](Files/logger_8h.md#define-generate-string)**(STRING)  |
|  | **[GENERATE_TAGSTRINGS](Files/logger_8h.md#define-generate-tagstrings)**(TAG)  |
|  | **[LOGGER_TAG_INFO_MAXLEN](Files/logger_8h.md#define-logger-tag-info-maxlen)**  |

## Types Documentation

### enum logger_clockType_t

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| LCLOCK_RDTSCP | |   |
| LCLOCK_LINUX_REALTIME | |   |
| LCLOCK_LINUX_TIMEOFDAY | |   |
| LCLOCK_LINUX_PTP | |   |




It's a list of all the different ways we can measure time. 


### typedef logger_logTag_t

```cpp
typedef int logger_logTag_t;
```



## Functions Documentation

### function logger_addLogEntry

```cpp
int logger_addLogEntry(
    logger_logTag_t tag,
    long id,
    int listNumber
)
```


**Parameters**: 

  * **tag** The tag is a number that identifies the type of log entry. For example, you might have a tag for a "start" event, a "stop" event, etc. The tag is of type `logger_logTag_t`. 
  * **id** This is the id of the log entry. This is a unique id for each log entry. It is used to identify multiple runs of the same tag. 
  * **listNumber** The number of the list to add the entry to.


**Return**: 0=success;-1=list not found;-2=list overflow 



> This function adds a log entry to the log entry list with the current time. 
> 
> 


### function logger_addLogEntryCustTime

```cpp
int logger_addLogEntryCustTime(
    logger_logTag_t tag,
    long id,
    int listNumber,
    struct timespec time
)
```


**Parameters**: 

  * **tag** The tag is a number that identifies the type of log entry. For example, you might have a tag for a "start" event, a "stop" event, etc. The tag is of type `logger_logTag_t`. 
  * **id** This is the id of the log entry. This is a unique id for each log entry. It is used to identify multiple runs of the same tag. 
  * **listNumber** The number of the list to add the entry to. 
  * **time** The time of the event.


**Return**: 0=success;-1=list not found;-2=list overflow 



> This function adds a log entry to the specified log list with a custom time stamp 
> 
> 


### function logger_init

```cpp
int logger_init(
    logger_config_t conf
)
```


**Parameters**: 

  * **conf** the configuration of the logger


**Return**: 0=success;else=error 

Initialize the logger. Use the conf to configure the logger.It allocates memory for the logger.


### function logger_writeToCSV

```cpp
int logger_writeToCSV(
    const char * fileName,
    logger_tagDef_t * logDef,
    int logDefCount
)
```


**Parameters**: 

  * **fileName** The name of the file to write to. 
  * **logDef** This is a pointer to an array of [logger_tagDef_t](Classes/structlogger__tagDef__t.md) structures. 
  * **logDefCount** The number of tags in the logDef array.


**Return**: 0=success;-2=file error; 



> Write the content of the log lists to a CSV file 
> 
> 


### function logger_writeListToCSV

```cpp
int logger_writeListToCSV(
    const char * fileName,
    int * listIds,
    int listIdsCount,
    logger_tagDef_t * logDef,
    int logDefCount
)
```


**Parameters**: 

  * **fileName** The name of the file to write to. 
  * **exportList** Array of list numbers that should be exported. If NULL, all lists will be exported. 
  * **exportListCount** The number of lists to export in exportList. 
  * **logDef** This is a pointer to an array of [logger_tagDef_t](Classes/structlogger__tagDef__t.md) structures. 
  * **logDefCount** The number of log definitions in logDef


**Return**: 0=success;-1=list not found;-2=file error; 

It writes the log entries of a specific log list to a CSV file


### function logger_evaluate

```cpp
int logger_evaluate(
    logger_tagPair_t * pairList,
    int pairListCount,
    logger_tagDef_t * logDef,
    int logDefCount,
    const char * csv_filename,
    const char * json_filename
)
```


**Parameters**: 

  * **pairList** A list of tag pairs to evaluate. 
  * **pairListCount** The number of pairs of tags to evaluate. 
  * **logDef** This is a list of all the tag meta definitiosn that you want to evaluate. 
  * **logDefCount** The number of tag definitions. 
  * **csv_filename** The name of the file to write the results to in CSV format.
  * **json_filename** The name of the file to write the results to in JSON format. If **csv_filename** and **json_filename** are NULL the results will be printed to the console.


**Return**: The return value is the status of the function. 0=Sucess;-2=Could not open file 

It takes a list of tag pairs and a list of tag definitions and prints out the min, max, mean and median of the time difference between the tags


### function logger_evaluate_diff

```cpp
int logger_evaluate_diff(
    logger_tagPair_t * pairList,
    int pairListCount,
    logger_tagDef_t * logDef,
    int logDefCount,
    const char * csv_filename
)
```


**Parameters**: 

  * **pairList** A list of tag pairs to evaluate. 
  * **pairListCount** The number of pairs of tags to evaluate 
  * **logDef** This is a list of all the tag meta definition that you want to evaluate. 
  * **logDefCount** The number of tag definitions in the logDef array. 
  * **csv_filename** The name of the file to write the results to. If NULL, the results will be printed to the console.


It takes a list of tag pairs and a list of tag definitions and prints the time difference between each pair of tags


@returnThe return value is the status of the function. 0=Sucess;-2=Could not open file 


### function logger_getErrorCount

```cpp
int * logger_getErrorCount()
```


**Return**: A pointer to the variable _logger_errorCount. 



> Returns the count of errors while trying to wirte to the log list. 
> 
> 

### function logger_reset

```cpp
void logger_reset()
```


Resets the logger without freeing the memory.

### function logger_clear

```cpp
void logger_clear()
```


It frees the memory allocated by the logger_init() function 


### function logger_getTime

```cpp
void logger_getTime(
    struct timespec * time
)
```


**Parameters**: 

  * **time** Reference where the time is written to. 


It gets the time according to the configuration. 


### function logger_elapsedTime

```cpp
struct timespec logger_elapsedTime(
    struct timespec start,
    struct timespec end
)
```


**Parameters**: 

  * **start** The start time of the function 
  * **end** The end time of the process.


**Return**: The difference between the two times. 



> This function takes two timespecs as arguments and returns the difference 
> 
> 

between them


### function logger_cmpTime

```cpp
int logger_cmpTime(
    struct timespec first,
    struct timespec second
)
```


**Return**: 0 when equal; -1 when first<second and 1 when first>second 

Compare first and second value 


### function logger_timespecToFloat_ms

```cpp
float logger_timespecToFloat_ms(
    struct timespec time
)
```


**Parameters**: 

  * **time** The time to convert


**Return**: A float value that is the time in milliseconds. 

Converts a timespec to a milisecond float. 




## Macros Documentation

### define TAG

```cpp
#define TAG(
    X
)
X##_START
```


### define GENERATE_ENUM

```cpp
#define GENERATE_ENUM(
    ENUM
)
ENUM##_START, ENUM##_END,
```


### define GENERATE_STRING

```cpp
#define GENERATE_STRING(
    STRING
)
#STRING
```


### define GENERATE_TAGSTRINGS

```cpp
#define GENERATE_TAGSTRINGS(
    TAG
)
GENERATE_STRING(TAG##_START), GENERATE_STRING(TAG##_END),
```


### define LOGGER_TAG_INFO_MAXLEN

```cpp
#define LOGGER_TAG_INFO_MAXLEN 25
```


## Source code

```cpp
#ifndef RTPERFLOGGER_H
#define RTPERFLOGGER_H

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define WIN 1
#include "time.h"
#else // Unix
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

    typedef struct
    {
        logger_logTag_t tag;
        unsigned long id;
        struct timespec time_stamp;
    } logger_logEntry_t;

    typedef struct
    {
        logger_logTag_t tag;
        char info[LOGGER_TAG_INFO_MAXLEN];
    } logger_tagDef_t;

    typedef struct
    {
        logger_logTag_t tag_start;
        logger_logTag_t tag_end;
    } logger_tagPair_t;

    typedef enum
    {
        LCLOCK_RDTSCP,
#if defined(WIN)
        LCLOCK_WIN_AFILEDATE,
        LCLOCK_WIN_QUERYPERFCOUNTER,
#else
    LCLOCK_LINUX_REALTIME,
    LCLOCK_LINUX_TIMEOFDAY,
    LCLOCK_LINUX_PTP
#endif
    } logger_clockType_t;

    typedef struct
    {
        logger_clockType_t clockType;
        int listCount;
        int listSize;
    } logger_config_t;

    // Realtime safe functions with very small performance impact
    //------------------------------------------------------------------------------------------------------------------
    int logger_addLogEntry(logger_logTag_t tag, long id, int listNumber);
    int logger_addLogEntryCustTime(logger_logTag_t tag, long id, int listNumber, struct timespec time);

    // Non real time safe functions. The are used to setup the logger and save the results. You must call them after using
    //------------------------------------------------------------------------------------------------------------------
    int logger_init(logger_config_t conf);
    int logger_writeToCSV(const char *fileName, logger_tagDef_t *logDef, int logDefCount);
    int logger_writeListToCSV(const char *fileName, int *listIds, int listIdsCount, logger_tagDef_t *logDef, int logDefCount);

    int logger_evaluate(logger_tagPair_t *pairList, int pairListCount, logger_tagDef_t *logDef, int logDefCount, const char *csv_filename);

    int logger_evaluate_diff(logger_tagPair_t *pairList, int pairListCount, logger_tagDef_t *logDef, int logDefCount, const char *csv_filename);

    int *logger_getErrorCount();

    void logger_clear();
    void logger_getTime(struct timespec *time);

    struct timespec logger_elapsedTime(struct timespec start, struct timespec end);

    // Some helper functions

    int logger_cmpTime(struct timespec first, struct timespec second);

    float logger_timespecToFloat_ms(struct timespec time);

#ifdef __cplusplus
}
#endif

#endif // RTPERFLOGGER_H
```


-------------------------------

Updated on 2022-05-06 at 10:58:04 +0000
