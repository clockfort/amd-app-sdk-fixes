//
// Copyright (c) 2008 Advanced Micro Devices, Inc. All rights reserved.
//


#ifndef SDKAPPLICATION_H_
#define SDKAPPLICATION_H_
#include <SDKCommon.hpp>
#include <SDKCommandArgs.hpp>

/**
 * GLOBAL DEFINED Macros
 */
#define CL_CONTEXT_OFFLINE_DEVICES_AMD        0x403F


/**
 * SDKApplication 
 * class is a basic abstract class for sample creation 
 */
class SDKApplication
{
protected:
    /**
     * initialize
     * Pure Virtual Function
     * Initialize the resources used by tests
     * @return 0 on success Positive if expected and Non-zero on failure
     */
    virtual int initialize() = 0;

    /**
     * run
     * Pure Virtual Function
     * Run the Sample
     * @return 0 on success Positive if expected and Non-zero on failure
     */
    virtual int run() = 0;

    /**
     * cleanup
     * Pure Virtual Function
     * Cleans up the resources used by tests
     * @return 0 on success Positive if expected and Non-zero on failure
     */
    virtual int cleanup() = 0;
};


/**
 * SDKSample
 * class implements various resources required by the test
 * Publically Inherited from SDKApplication
 * Initialize the resources used by tests
 */
class SDKSample : public SDKApplication
{
protected:
    streamsdk::SDKCommandArgs *sampleArgs;  /**< SDKCommandArgs class object to handle comd line options */
    streamsdk::SDKCommon * sampleCommon;    /**< SDKCommon class object */	
    streamsdk::sdkVersionStr sdkVerStr;     /**< SDK version string */
    bool version;                           /**< Cmd Line Option- if version */
    std::string name;                       /**< Name of the Sample */
    double totalTime;                       /**< Total Time taken by the Sample */
    bool quiet;                             /**< Cmd Line Option- if Quiet */
    bool verify;                            /**< Cmd Line Option- if verify */
    bool timing;                            /**< Cmd Line Option- if Timing */
    bool multiDevice;                       /**< Cmd Line Option- if MultiGPU */
    unsigned int deviceId;                  /**< Cmd Line Option- device number */
    bool enableDeviceId;                    /**< If deviceId used */
    unsigned int platformId;                /**< Cmd Line Option- platform number */
    bool enablePlatform;                    /**< If platformId Used */
    bool gpu;                               /**< If GPU used */
    bool amdPlatform;                       /**< If AMD Platform Used */
    std::string deviceType;                 /**< Cmd Line Option- set device type(cpu|gpu) */
    std::string dumpBinary;                 /**< Cmd Line Option- Dump Binary with name */
    std::string loadBinary;                 /**< Cmd Line Option- Load Binary with name */
    std::string flags;                      /**< Cmd Line Option- compiler flags */
    
protected:
    /**
     * setup
     * Pure Virtual Function
     * Setup the resources used by tests
     * @return 0 on success Positive if expected and Non-zero on failure
     */
    virtual int setup() = 0;

    /**
     * verifyResults
     * Pure Virtual Function
     * Verifies the results obtained with reference implementation
     * @return 0 on success Positive if expected and Non-zero on failure
     */
    virtual int verifyResults() = 0;	
    
    /**
     * genBinaryImage
     * Pure Virtual Function
     * Generate Binary of the kernel if option specified(offline compilation)
     * @return 0 on success Positive if expected and Non-zero on failure
     */
    virtual int genBinaryImage()= 0;

    /**
     * initialize
     * Pure Virtual Function
     * virtual Function from SDKApplication (Other virtual functions not defined)
     * Initialize the resources used by tests
     * @return 0 on success Positive if expected and Non-zero on failure
     */
    virtual int initialize();

    /**
     * printstats
     * Print the results from the test
     * @param stdStr Parameter
     * @param stats Statistic value of parameter
     * @param n number
     */
    virtual void printStats(std::string *stdStr, std::string * stats, int n);

    /**
     * Destructor
     * Destroy the resources used by tests
     */
    virtual ~SDKSample();

public:
    /**
     * Constructor
     * Initialize the resources used by tests
     * @param sampleName Name of the Sample
     * @param enableMultiDevice if MultiDevice is enabled. (optional param default to false)
     */
    SDKSample(std::string sampleName, bool enableMultiDevice = false);
    
    /**
     * Copy Constructor
     * Initialize the resources used by tests
     * @param sampleName Name of the Sample
     * @param enableMultiDevice if MultiDevice is enabled. (optional param default to false)
     */
    SDKSample(const char* sampleName, bool enableMultiDevice = false);
    
    /**
     * isDumpBinaryEnabled
     * Checks if dump Binary Option is enabled
     * @return true if DumpBinary Enabled else false
     */
    bool isDumpBinaryEnabled()
    {
        if(dumpBinary.size() == 0)
            return false;
        else
            return true;
    }

    /**
     * isLoadBinaryEnabled
     * Checks if the sample wants to load a prebuilt binary
     * @return true if LoadBinary Enabled else false
     */
    bool isLoadBinaryEnabled()
    {
        if(loadBinary.size() == 0)
            return false;
        else
            return true;
    }

    /**
     * isCompilerFlagsSpecified
     * Checks if any compiler flag is specified for the kernel
     * @return true if CompierFlag Enabled else false
     */
    bool isComplierFlagsSpecified()
    {
        if(flags.size() == 0)
            return false;
        else
            return true;
    }

    /**
     * isPlatformEnabled
     * Checks if platform option is used
     * @return true if PlatformId Enabled else false
     */
    bool isPlatformEnabled()
    {
        return enablePlatform;
    }

    /**
     * isDeviceEnabled
     * Checks if device option is used
     * @return true if DeviceId Enabled else false
     */
    bool isDeviceIdEnabled()
    {
        return enableDeviceId;
    }
    
    /**
     * isThereGPU
     * Checks if a GPU is Present
     * @return true if GPU Present else false
     */
    bool isThereGPU()
    {
        return gpu;
    }

    /**
     * isAmdPlatform
     * Checks if AMD Platform is used
     * @return true if AMD Platform Enabled else false
     */
    bool isAmdPlatform()
    {
        return amdPlatform;
    }

    /**
     * parseCommandLine
     * parses the command line options given by user
     * @param argc Number of elements in cmd line input
     * @param argv array of char* storing the CmdLine Options
     * @return 0 on success Positive if expected and Non-zero on failure
     */
    int parseCommandLine(int argc, char **argv);
    
    /**
     * validatePlatformAndDeviceOptions
     * Validates if the intended platform and device is used
     * @return 0 on success Positive if expected and Non-zero on failure
     */
    int validatePlatformAndDeviceOptions();


    /**
     * usage
     * Displays the various options available for any sample
     */
    void usage();

    /**
     * Returns SDK Version string
     * @return std::string
     */
    std::string getSdkVerStr()
    {
        char str[1024];
        std::string dbgStr("");
        std::string internal("");
        
        // Add -internal if revision is 0
        if(sdkVerStr.revision == 0)
            internal.append("-internal");

#ifdef _WIN32
#ifdef _DEBUG
        dbgStr.append("-dbg");
#endif
#else
#ifdef NDEBUG
        dbgStr.append("-dbg");
#endif
#endif

#if defined (_WIN32) && !defined(__MINGW32__)
        sprintf_s(str, 256, "AMD-APP-SDK-v%d.%d%s%s (%d.%d)",
            sdkVerStr.major,
            sdkVerStr.minor,
            internal.c_str(),
            dbgStr.c_str(),
            sdkVerStr.build,
            sdkVerStr.revision);
#else 
        sprintf(str, "AMD-APP-SDK-v%d.%d%s%s (%d.%d)",
            sdkVerStr.major,
            sdkVerStr.minor,
            internal.c_str(),
            dbgStr.c_str(),
            sdkVerStr.build,
            sdkVerStr.revision);
#endif

        return std::string(str);
    }

   
    /**
     * Returns correct version string(major.minor.build.revision)
     * version string contains major and minor values from SDK
     * build and revision values from OpenCL libraries
     * For example SDK version = AMD-APP-SDK-v2.5 (100.1)
     *             OpenCL version = OpenCL 1.1 AMD-APP-SDK (820.2)
     *             Final version = OpenCL 1.1 AMD-APP-SDK-v2.5 (820.2)
     * @return std::string 
     */
    std::string getExactVerStr(std::string clVerStr);
};

#endif


