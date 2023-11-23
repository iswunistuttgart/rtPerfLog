# rtPerfLog

rtPerfLog is a library for performance evaluation in real-time and non-real-time applications. rtPerfLog is written in C and can be used directly in C/C++ applications. Moreover, you can build language-specific wrappers quickly.

Supported OS:
 - Linux (Tested with Ubuntu 18.04+)
 - Windows (Tested with WIN10)
Supported Platforms:
 - x86-64
 - ARM (Tested with ARM v8.2)

rtPerfLog was used successfully in the following publications for performance evaluation:
  - M. Fischer, O. Riedel, A. Lechler and A. Verl, (2021) **Arithmetic Coding for Floating-Point Numbers,** 2021 IEEE Conference on Dependable and Secure Computing (DSC), pp. 01-08, doi: [10.1109/DSC49826.2021.9346236](https://ieeexplore.ieee.org/document/9346236/)
  - Tasci, T., Fischer, M., Lechler, A., Verl, A. (2021) **Predictable and Real-Time Message-Based Communication in the Context of Control Technology.** In: Weißgraeber, P., Heieck, F., Ackermann, C. (eds) Advances in Automotive Production Technology – Theory and Application. ARENA2036. Springer Vieweg, Berlin, Heidelberg. [https://doi.org/10.1007/978-3-662-62962-8_31](https://link.springer.com/chapter/10.1007/978-3-662-62962-8_31) 
  - Schmidt, A., Schellroth, F., Fischer, M. et al. (2021) **Reinforcement learning methods based on GPU accelerated industrial control hardware**. Neural Comput & Applic 33, 12191–12207 [https://doi.org/10.1007/s00521-021-05848-4](https://link.springer.com/article/10.1007/s00521-021-05848-4) 
  - M. Fischer, O. Riedel and A. Lechler, **Arithmetic Coding for Floating-Points and Elementary Mathematical Functions,** 2021 5th International Conference on System Reliability and Safety (ICSRS), 2021, pp. 270-275, doi: [10.1109/ICSRS53853.2021.9660663](https://ieeexplore.ieee.org/document/9660663)

## Project structure

 * docs: Generated documentation of the `logger.h`
 * include: Contains the API header.
 * src: Contains the source code.
 * test: Some tests


## How to build

rtPerfLog uses CMake as a build tool. You can build CMake projects with the command line or use your IDE with CMake support.

### Build with Powershell under Windows

Execute the following commands in the root dir.

```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"  -DCMAKE_INSTALL_PREFIX:PATH=<your target path>
cmake --build . --target install
```

* If you do not specify the `-DCMAKE_INSTALL_PREFIX:PATH=`, the install path is "C:/Program Files (x86)/rtperfloglib"
* `-G` specefies the buld tool. All VS versions should work but only tested under VS16


### Build with command line under Linux

Execute the following commands in the root dir.

```cmd
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX:PATH=<your target path>
cmake --build . --target install
```

## How to use

An example project is given in the test directory.

### Manual include

Depending on your build system, you have to add the library and the header.
For CMake the following lines are required:
```cmake
include_directories(<INSTALL_PATH>/include/rtperflog)
link_directories(<INSTALL_PATH>/lib)
target_link_libraries(<YOURTARGET> rtperflog)
```

### Include by find_package

```cmake
find_package(rtperflog REQUIRED PATHS <INSTALL_PATH>)
target_link_libraries(<YOURTARGET> rtperflog::rtperflog)
```


## Examples

### Preprocessor defines tagging

Tags are required for logging. The generation of tags with the preprocessor commands is explained in the following:

```c
#include "logger.h"
//define your tags
#define TAGS(TAG) \
    TAG(TAG_DEMO) \
    TAG(TAG_DEMO2)

// Generate the enum. For each tag a start and end tag is defined
// Therefore, the Preprocessor will expand in this example too. The TAG_COUNT can be used to identify the number of tags quickly.
// 
// enum TAG_ENUM {
//    TAG_DEMO_START,
//    TAG_DEMO_END,
//    TAG_DEMO2_START,
//    TAG_DEMO2_END,
//    TAG_COUNT
//};
// 
enum TAG_ENUM {
    TAGS(GENERATE_ENUM)
    TAG_COUNT
};

//Similar to the enum generation, a  string array is generated, which is used to export the timestamp by name to the CSV.
static const char *TAG_STRING[] = {
    TAGS(GENERATE_TAGSTRINGS)
};

//It also generates a evaluation list of all defined tags.
logger_tagPair_t evalListFull[] = {TAGS(GENERATE_EVALLIST)};
int evalListFullSize = sizeof(evalListFull) / sizeof(enum TAG_ENUM) / 2;

//This function generates the logger_tagDef_t struct by the preprocessor commands.
logger_tagDef_t* makeLoggerDef(){
  logger_tagDef_t* def = malloc(sizeof(logger_tagDef_t[TAG_COUNT]));
    for(int i=0;i<TAG_COUNT;i++){
        def[i].tag = i;
        strcpy(def[i].info,TAG_STRING[i]);
    }
  return def;
}

//The definitions shown above can be simplified by
GENERATE_DEF(TAGS)


//Now you can use the tags to log timestamps by tags and you are able to export the timestamps by the generated tag string names. Only the inital configuration of the logger is missing.
int main(){
  logger_tagDef_t* def = makeLoggerDef();
  //Before using the logger the first time the config must be initialized.
  logger_config_t conf;
  #ifdef WIN
    conf.clockType = LCLOCK_WIN_QUERYPERFCOUNTER; //Recommend for Win
  #else
    conf.clockType = LCLOCK_LINUX_REALTIME; //Recommend for Linux
  #endif
  conf.listCount = 1; //When using different threads, multiple lists are required, to avoid locks.
  conf.listSize = 100000;
  //The maximum list count and size depends on the allowed heap size.
  logger_init(conf); //This initializes the logger with the given config

  for(int i=0;i<100;i++){
    logger_addLogEntry(TAG_DEMO_START,i,0); //TAG,id,list. As we defined one list, we use the first list (0). The ids are required to find two corresponding iterations.
    sleep(1);
    logger_addLogEntry(TAG_DEMO_STOP,i,0);
  }

  logger_writeToCSV("test.csv",def,TAG_COUNT);
  /** The expoted csv file looks like this:
  TAG_DEMO_START,0,0.871261200
  TAG_DEMO_END,0,0.972029400
  TAG_DEMO_START,1,0.972606000
  TAG_DEMO_END,1,1.072890900
  TAG_DEMO_START,2,1.072965400
  TAG_DEMO_END,2,1.173906600
  TAG_DEMO_START,3,1.173985400
  TAG_DEMO_END,3,1.274874900
  **/
  logger_clear();

}
```

### Evaluation
You can use `logger_evaluate` and `logger_evaluate_diff` to evalute the logging results.

```c
#include "logger.h"
//define your tags
#define TAGS(TAG) \
    TAG(TAG_DEMO) \
    TAG(TAG_DEMO2)

GENERATE_DEF(TAGS)

int main(){
  logger_tagDef_t *tagdef = makeLoggerDef();
  logger_config_t conf;
  #ifdef WIN
      conf.clockType = LCLOCK_WIN_QUERYPERFCOUNTER;
  #else
      conf.clockType = LCLOCK_LINUX_REALTIME;
  #endif
    conf.listCount = 1;
    conf.listSize = 100000;
    logger_init(conf);
    for (int i = 0; i < 1000; i++)
    {
        logger_addLogEntry(TAG_DEMO_START, i, 0);
#ifdef WIN
        Sleep(100);
#else
        usleep(100);
#endif
        logger_addLogEntry(TAG_DEMO_END, i, 0);
    }
  logger_tagPair_t evalList[1] = { { TAG_DEMO_START, TAG_DEMO_END }};

  // Prints the evaluation to stdout
  logger_evaluate(evalList, 1, tagdef, TAG_COUNT, NULL, NULL);
  // This prints something like this:
  // TAG_DEMO_START-TAG_DEMO_END | Count:1000 Min:0.10653 Max:3.68175 Mean:0.17097 Median:0.15712

  // Prints the evaluation with the default list to stdout
  logger_evaluate(evalListFull, evalListFullSize, tagdef, TAG_COUNT, NULL, NULL);

  // Prints the evaluation to stdout
  logger_evaluate_diff(evalList, 1, tagdef, TAG_COUNT, NULL);
  // This prints something like this:
  // TAG_DEMO_START-TAG_DEMO_END: 0.164780005813
  // TAG_DEMO_START-TAG_DEMO_END: 0.155450999737
  // TAG_DEMO_START-TAG_DEMO_END: 0.154724001884
  // TAG_DEMO_START-TAG_DEMO_END: 0.154458999634
  // ...
}
```

## API

A more detailed API documentation can be found in [logger](docs/logger.md)

Non-real-time safe functions. They are used to set up the logger and save the results.

* `int logger_init(logger_config_t conf)`
  * Initilzes the logger. This function must be called first. You must use the `logger_config_t` struct for configuration. This function allocates memory for the log entries and pins the memory.
* `int logger_writeToCSV(const char* fileName,logger_tagDef_t* logDef,int logDefCount)`
  * Writes all logged timestamps to one csv file. The `logger_tagDef_t` struct defines the tag mapping.
* `int logger_writeListsToCSV(const char* fileName,int* exportList,int exportListCount,logger_tagDef_t* logDef,int logDefCount)`
  * Writes the logged timestamps of specific lists to one csv file. The lists to export can be defined in the export list array. The `logger_tagDef_t` struct defines the tag mapping.
* `int* logger_getErrorCount()`
  * Returns the count of errors while trying to wirte to the log list.
* ` int logger_evaluate(logger_tagPair_t *pairList, int pairListCount, logger_tagDef_t *logDef, int logDefCount, const char *csv_filename, const char *json_filename);`
  * It takes a list of tag pairs and a list of tag definitions and prints out the min, max, mean and median of the time difference between the tags
* `int logger_evaluate_diff(logger_tagPair_t *pairList, int pairListCount, logger_tagDef_t *logDef, int logDefCount, const char *csv_filename);`
  * It takes a list of tag pairs and a list of tag definitions and export the time difference between each pair of tags
* `void logger_reset()`
  * Resets the logger list.
* `void logger_clear()`
  * Clears the logger. Frees all memory.

Real-time safe functions. Time calculations for directly printing to the screen. **Never use printf() in the real-time part of your application except for debugging**

* `struct timespec logger_elapsedTime(struct timespec start,struct timespec end)`
  * Calculates the elapsed time between two time stamps
* `float logger_timespecToFloat_ms(struct timespec time)`
  * Converts a timespec struct to an float in millisecond format

Real-time safe functions with minimal performance impact

* `int logger_addLogEntry(logger_logTag_t tag, long id, int listNumber)`
  * Adds a new log entry. You must spezify the id and tag.
* `int logger_addLogEntryCustTime(logger_logTag_t tag, long id, int listNumber, struct timespec time)`


