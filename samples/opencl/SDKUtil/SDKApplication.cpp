#include <SDKCommon.hpp>
#include <SDKApplication.hpp>

int 
SDKSample::initialize()
{
    sampleCommon = new streamsdk::SDKCommon();
    int defaultOptions = 10;

    if(multiDevice)
        defaultOptions = 9;

    
    streamsdk::Option *optionList = new streamsdk::Option[defaultOptions];
    CHECK_ALLOCATION(optionList, "Error. Failed to allocate memory (optionList)\n");

    optionList[0]._sVersion = "";
    optionList[0]._lVersion = "device";
    
    if(multiDevice)
        optionList[0]._description = "Execute the openCL kernel on a device [cpu|gpu|all]";
    else
        optionList[0]._description = "Execute the openCL kernel on a device [cpu|gpu]";

    optionList[0]._type = streamsdk::CA_ARG_STRING;
    optionList[0]._value = &deviceType;

    optionList[1]._sVersion = "q";
    optionList[1]._lVersion = "quiet";
    optionList[1]._description = "Quiet mode. Suppress all text output.";
    optionList[1]._type = streamsdk::CA_NO_ARGUMENT;
    optionList[1]._value = &quiet;

    optionList[2]._sVersion = "e";
    optionList[2]._lVersion = "verify";
    optionList[2]._description = "Verify results against reference implementation.";
    optionList[2]._type = streamsdk::CA_NO_ARGUMENT;
    optionList[2]._value = &verify;

    optionList[3]._sVersion = "t";
    optionList[3]._lVersion = "timing";
    optionList[3]._description = "Print timing.";
    optionList[3]._type = streamsdk::CA_NO_ARGUMENT;
    optionList[3]._value = &timing;

    optionList[4]._sVersion = "";
    optionList[4]._lVersion = "dump";
    optionList[4]._description = "Dump binary image for all devices";
    optionList[4]._type = streamsdk::CA_ARG_STRING;
    optionList[4]._value = &dumpBinary;

    optionList[5]._sVersion = "";
    optionList[5]._lVersion = "load";
    optionList[5]._description = "Load binary image and execute on device";
    optionList[5]._type = streamsdk::CA_ARG_STRING;
    optionList[5]._value = &loadBinary;

    optionList[6]._sVersion = "";
    optionList[6]._lVersion = "flags";
    optionList[6]._description = "Specify compiler flags to build kernel";
    optionList[6]._type = streamsdk::CA_ARG_STRING;
    optionList[6]._value = &flags;

    optionList[7]._sVersion = "p";
    optionList[7]._lVersion = "platformId";
    optionList[7]._description = "Select platformId to be used[0 to N-1 where N is number platforms available].";
    optionList[7]._type = streamsdk::CA_ARG_INT;
    optionList[7]._value = &platformId;

    optionList[8]._sVersion = "v";
    optionList[8]._lVersion = "version";
    optionList[8]._description = "AMD APP SDK version string.";
    optionList[8]._type = streamsdk::CA_NO_ARGUMENT;
    optionList[8]._value = &version;

    if(multiDevice == false)
    {
        optionList[9]._sVersion = "d";
        optionList[9]._lVersion = "deviceId";
        optionList[9]._description = "Select deviceId to be used[0 to N-1 where N is number devices available].";
        optionList[9]._type = streamsdk::CA_ARG_INT;
        optionList[9]._value = &deviceId;
    }

    sampleArgs = new streamsdk::SDKCommandArgs(defaultOptions, optionList);
    CHECK_ALLOCATION(sampleArgs, "Failed to allocate memory. (sampleArgs)\n");
                
    return SDK_SUCCESS;
}

void SDKSample::printStats(std::string *statsStr, std::string * stats, int n)
{
    if(timing)
    {
        streamsdk::Table sampleStats;

        sampleStats._numColumns = n;
        sampleStats._numRows = 1;
        sampleStats._columnWidth = 25;
        sampleStats._delim = '$';
        
        sampleStats._dataItems = "";
        for(int i=0; i < n; ++i)
        {
            sampleStats._dataItems.append( statsStr[i] + "$");
        }
        sampleStats._dataItems.append("$");

        for(int i=0; i < n; ++i)
        {
            sampleStats._dataItems.append( stats[i] + "$");
        }

        sampleCommon->printTable(&sampleStats);
    }
}

int SDKSample::parseCommandLine(int argc, char**argv)
{
    if(sampleArgs==NULL)
    {
        std::cout<<"Error. Command line parser not initialized.\n";
        return SDK_FAILURE;
    }
    else
    {
        if(!sampleArgs->parse(argv,argc))
        {
            usage();
            return SDK_FAILURE;
        }

        if(sampleArgs->isArgSet("h",true) != SDK_SUCCESS)
        {
            usage();
            return SDK_FAILURE;
        }

        // Print the sdk version and exit the application
        if(sampleArgs->isArgSet("v", true) || sampleArgs->isArgSet("version", false))
        {
            std::cout << "SDK version : " << getSdkVerStr().c_str() << std::endl;
            exit(0);
        }

        if(sampleArgs->isArgSet("p",true) || sampleArgs->isArgSet("platformId",false))
            enablePlatform = true;

        if(sampleArgs->isArgSet("d",true) || sampleArgs->isArgSet("deviceId",false))
            enableDeviceId = true;
    }

    /* check about the validity of the device type */

    if(multiDevice)
    {   
        if(!((deviceType.compare("cpu") == 0 ) 
              || (deviceType.compare("gpu") ==0) 
              || (deviceType.compare("all") ==0)))
        {
            std::cout << "Error. Invalid device options. "
                      << "only \"cpu\" or \"gpu\" or \"all\" supported\n";
            usage();
            return SDK_FAILURE;
        }
    }
    else
    {
        if(!((deviceType.compare("cpu") == 0 ) || (deviceType.compare("gpu") ==0)))
        {
            std::cout << "Error. Invalid device options. "
                      << "only \"cpu\" or \"gpu\" or \"all\" supported\n";
            usage();
            return SDK_FAILURE;
        }
    }

    if(dumpBinary.size() != 0 && loadBinary.size() != 0)
    {
        std::cout << "Error. --dump and --load options are mutually exclusive\n";
        usage();
        return SDK_FAILURE;
    }

    if(loadBinary.size() != 0 && flags.size() != 0)
    {
        std::cout << "Error. --flags and --load options are mutually exclusive\n";
        usage();
        return SDK_FAILURE;
    }

    if(validatePlatformAndDeviceOptions() != SDK_SUCCESS)
    {
        std::cout << "validatePlatfromAndDeviceOptions failed.\n ";
        return SDK_FAILURE;
    }

    return SDK_SUCCESS;
}

int SDKSample::validatePlatformAndDeviceOptions()
{
    cl_int status = CL_SUCCESS;
    cl_uint numPlatforms;
    cl_platform_id platform = NULL;
    status = clGetPlatformIDs(0, NULL, &numPlatforms);
    if(status != CL_SUCCESS)
    {
        std::cout<<"Error: clGetPlatformIDs failed. Error code : ";
        std::cout << streamsdk::getOpenCLErrorCodeStr(status) << std::endl;
        return SDK_FAILURE;
    }

    if (0 < numPlatforms) 
    {
        // Validate platformId
        if(platformId >= numPlatforms)
        {
            if(numPlatforms - 1 == 0)
                std::cout << "platformId should be 0" << std::endl;
            else
                std::cout << "platformId should be 0 to " << numPlatforms - 1 << std::endl;
            usage();
            return SDK_FAILURE;
        }

        // Get selected platform
        cl_platform_id* platforms = new cl_platform_id[numPlatforms];
        status = clGetPlatformIDs(numPlatforms, platforms, NULL);
        if(status != CL_SUCCESS)
        {
            std::cout<<"Error: clGetPlatformIDs failed. Error code : ";
            std::cout << streamsdk::getOpenCLErrorCodeStr(status) << std::endl;
            return SDK_FAILURE;
        }

        // Print all platforms
        for (unsigned i = 0; i < numPlatforms; ++i) 
        {
            char pbuf[100];
            status = clGetPlatformInfo(platforms[i],
                                       CL_PLATFORM_VENDOR,
                                       sizeof(pbuf),
                                       pbuf,
                                       NULL);

            if(status != CL_SUCCESS)
            {
                std::cout<<"Error: clGetPlatformInfo failed. Error code : ";
                std::cout << streamsdk::getOpenCLErrorCodeStr(status) << std::endl;
                return SDK_FAILURE;
            }

            std::cout << "Platform " << i << " : " << pbuf << std::endl;
        }

        // Get AMD platform
        for (unsigned i = 0; i < numPlatforms; ++i) 
        {
            char pbuf[100];
            status = clGetPlatformInfo(platforms[i],
                                       CL_PLATFORM_VENDOR,
                                       sizeof(pbuf),
                                       pbuf,
                                       NULL);

            if(status != CL_SUCCESS)
            {
                std::cout<<"Error: clGetPlatformInfo failed. Error code : ";
                std::cout << streamsdk::getOpenCLErrorCodeStr(status) << std::endl;
                return SDK_FAILURE;
            }

            platform = platforms[i];
            if (!strcmp(pbuf, "Advanced Micro Devices, Inc.")) 
            {
                break;
            }
        }

        if(isPlatformEnabled())
            platform = platforms[platformId];


        // Check for AMD platform
        char pbuf[100];
        status = clGetPlatformInfo(platform,
                                   CL_PLATFORM_VENDOR,
                                   sizeof(pbuf),
                                   pbuf,
                                   NULL);

        if(status != CL_SUCCESS)
        {
            std::cout<<"Error: clGetPlatformInfo failed. Error code : ";
            std::cout << streamsdk::getOpenCLErrorCodeStr(status) << std::endl;
            return SDK_FAILURE;
        }
        if (!strcmp(pbuf, "Advanced Micro Devices, Inc.")) 
            amdPlatform = true; 


        cl_device_type dType = CL_DEVICE_TYPE_GPU;
        if(deviceType.compare("cpu") == 0)
            dType = CL_DEVICE_TYPE_CPU;
        if(deviceType.compare("gpu") == 0)
            dType = CL_DEVICE_TYPE_GPU;
        else
            dType = CL_DEVICE_TYPE_ALL;

        // Check for GPU
        if(dType == CL_DEVICE_TYPE_GPU)
        {
            cl_context_properties cps[3] = 
            {
                CL_CONTEXT_PLATFORM, 
                (cl_context_properties)platform, 
                0
            };

            cl_context context = clCreateContextFromType(cps,
                                                        dType,
                                                        NULL,
                                                        NULL,
                                                        &status);

            if(status == CL_DEVICE_NOT_FOUND)
            {
                dType = CL_DEVICE_TYPE_CPU;
                gpu = false;
            }

            clReleaseContext(context);
        }

        // Get device count
        cl_uint deviceCount = 0;
        status = clGetDeviceIDs(platform, dType, 0, NULL, &deviceCount);
        if(status != CL_SUCCESS)
        {
            std::cout<<"Error: clGetDeviceIDs failed. Error code : ";
            std::cout << streamsdk::getOpenCLErrorCodeStr(status) << std::endl;
            return SDK_FAILURE;
        }

        // Validate deviceId
        if(deviceId >= deviceCount)
        {
            if(deviceCount - 1 == 0)
                std::cout << "deviceId should be 0" << std::endl;
            else
                std::cout << "deviceId should be 0 to " << deviceCount - 1 << std::endl;
            usage();
            return SDK_FAILURE;
        }

        delete[] platforms;
    }
    return SDK_SUCCESS;
}

void SDKSample::usage()
{
    if(sampleArgs==NULL)
        std::cout<<"Error. Command line parser not initialized.\n";
    else
    {
        std::cout<<"Usage\n";
        sampleArgs->help();
    }
}

std::string SDKSample::getExactVerStr(std::string clVerStr)
{
    std::string finalVerStr("");

    size_t vPos = clVerStr.find_first_of("v");
    
    /**
     * Use CL version string as it is if 'v' is found in 
     * CL version string
     */
    if(vPos == std::string::npos)
    {
        // Get the required string from CL version 
        size_t sPos = clVerStr.find_first_of(" ");
        sPos = clVerStr.find_first_of(" ", sPos + 1);
        finalVerStr = clVerStr.substr(0, sPos + 1);

        // Append required string from SDK version string
        std::string sdkStr = getSdkVerStr();
        size_t bPos = sdkStr.find_first_of("(");

        finalVerStr.append(sdkStr.substr(0, bPos + 1));

        // Append remaining string from CL version
        vPos = clVerStr.find_first_of("(");
        finalVerStr.append(clVerStr.substr(vPos + 1));
    }
    else
    {
        finalVerStr = clVerStr;
    }

    return finalVerStr;
}

SDKSample::SDKSample(std::string sampleName, bool enableMultiDevice)
{
    name = sampleName;
    sampleCommon = NULL;
    sampleArgs = NULL;
    quiet = 0;
    verify = 0;
    timing = 0;
    deviceType = "gpu";
    multiDevice = enableMultiDevice;
    deviceId = 0;
    platformId = 0;
    enablePlatform = false;
    enableDeviceId = false;
    gpu = true;
    amdPlatform = false;
}

SDKSample::SDKSample(const char* sampleName, bool enableMultiDevice)
{
    name = sampleName;
    sampleCommon = NULL;
    sampleArgs = NULL;
    quiet = 0;
    verify = 0;
    timing = 0;
    deviceType = "gpu";
    multiDevice = enableMultiDevice;
    deviceId = 0;
    platformId = 0;
    enablePlatform = false;
    enableDeviceId = false;
    gpu = true;
    amdPlatform = false;
}

SDKSample::~SDKSample()
{
    delete sampleCommon;
    delete sampleArgs;
}
