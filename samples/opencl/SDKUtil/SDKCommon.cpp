#include <SDKCommon.hpp>



namespace streamsdk
{
SDKCommon::SDKCommon()
{
    
}

SDKCommon::~SDKCommon()
{
    while(!_timers.empty())
    {
        Timer *temp = _timers.back();
        _timers.pop_back();
        delete temp;
    }
}

/* Returns the path of executable being generated */
std::string
SDKCommon::getPath()
{
#ifdef _WIN32
    char buffer[MAX_PATH];
#ifdef UNICODE
    if(!GetModuleFileName(NULL, (LPWCH)buffer, sizeof(buffer)))
        throw std::string("GetModuleFileName() failed!");
#else
    if(!GetModuleFileName(NULL, buffer, sizeof(buffer)))
        throw std::string("GetModuleFileName() failed!");
#endif
    std::string str(buffer);
    /* '\' == 92 */
    int last = (int)str.find_last_of((char)92);
#else
    char buffer[PATH_MAX + 1];
    ssize_t len;
    if((len = readlink("/proc/self/exe",buffer, sizeof(buffer) - 1)) == -1)
        throw std::string("readlink() failed!");
    buffer[len] = '\0';
    std::string str(buffer);
    /* '/' == 47 */
    int last = (int)str.find_last_of((char)47);
#endif
    return str.substr(0, last + 1);
}

/*
 * Prints no more than 256 elements of the given array.
 * Prints full array if length is less than 256.
 * Prints Array name followed by elements.
 */
template<typename T> 
void SDKCommon::printArray(
    std::string header, 
    const T * data, 
    const int width,
    const int height) const
{
    std::cout<<"\n"<<header<<"\n";
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            std::cout<<data[i*width+j]<<" ";
        }
        std::cout<<"\n";
    }
    std::cout<<"\n";
}

int SDKCommon::waitForEventAndRelease(cl_event *event)
{
    cl_int status = CL_SUCCESS;
    cl_int eventStatus = CL_QUEUED;
	while(eventStatus != CL_COMPLETE)
    {
        status = clGetEventInfo(
                        *event, 
                        CL_EVENT_COMMAND_EXECUTION_STATUS, 
                        sizeof(cl_int),
                        &eventStatus,
                        NULL);
		CHECK_OPENCL_ERROR(status, "clGetEventEventInfo Failed with Error Code:");
	}

    status = clReleaseEvent(*event);
    CHECK_OPENCL_ERROR(status, "clReleaseEvent Failed with Error Code:");

    return SDK_SUCCESS;
}

template<typename T> 
int SDKCommon::fillRandom(
         T * arrayPtr, 
         const int width,
         const int height,
         const T rangeMin,
         const T rangeMax,
         unsigned int seed)
{
    if(!arrayPtr)
    {
        error("Cannot fill array. NULL pointer.");
        return SDK_FAILURE;
    }

    if(!seed)
        seed = (unsigned int)time(NULL);

    srand(seed);
    double range = double(rangeMax - rangeMin) + 1.0; 

    /* random initialisation of input */
    for(int i = 0; i < height; i++)
        for(int j = 0; j < width; j++)
        {
            int index = i*width + j;
            arrayPtr[index] = rangeMin + T(range*rand()/(RAND_MAX + 1.0)); 
        }

    return SDK_SUCCESS;
}

template<typename T> 
int SDKCommon::fillPos(
         T * arrayPtr, 
         const int width,
         const int height)
{
    if(!arrayPtr)
    {
        error("Cannot fill array. NULL pointer.");
        return SDK_FAILURE;
    }

    /* initialisation of input with positions*/
    for(T i = 0; i < height; i++)
        for(T j = 0; j < width; j++)
        {
            T index = i*width + j;
            arrayPtr[index] = index;
        }

    return SDK_SUCCESS;
}

template<typename T> 
int SDKCommon::fillConstant(
         T * arrayPtr, 
         const int width,
         const int height,
         const T val)
{
    if(!arrayPtr)
    {
        error("Cannot fill array. NULL pointer.");
        return SDK_FAILURE;
    }

    /* initialisation of input with constant value*/
    for(int i = 0; i < height; i++)
        for(int j = 0; j < width; j++)
        {
            int index = i*width + j;
            arrayPtr[index] = val;
        }

    return SDK_SUCCESS;
}

template<typename T>
T SDKCommon::roundToPowerOf2(T val)
{
    int bytes = sizeof(T);

    val--;
    for(int i = 0; i < bytes; i++)
        val |= val >> (1<<i);  
    val++;

    return val;
}

template<typename T>
int SDKCommon::isPowerOf2(T val)
{
    long long _val = val;
    if((_val & (-_val))-_val == 0 && _val != 0)
        return SDK_SUCCESS;
    else
        return SDK_FAILURE;
}
const char* 
getOpenCLErrorCodeStr(std::string input)
{
    return "unknown error code"; 
}

template<typename T>
const char* 
getOpenCLErrorCodeStr(T input)
{
    int errorCode = (int)input;
    switch(errorCode)
    {
        case CL_DEVICE_NOT_FOUND:
            return "CL_DEVICE_NOT_FOUND";
        case CL_DEVICE_NOT_AVAILABLE:
            return "CL_DEVICE_NOT_AVAILABLE";               
        case CL_COMPILER_NOT_AVAILABLE:
            return "CL_COMPILER_NOT_AVAILABLE";           
        case CL_MEM_OBJECT_ALLOCATION_FAILURE:
            return "CL_MEM_OBJECT_ALLOCATION_FAILURE";      
        case CL_OUT_OF_RESOURCES:
            return "CL_OUT_OF_RESOURCES";                    
        case CL_OUT_OF_HOST_MEMORY:
            return "CL_OUT_OF_HOST_MEMORY";                 
        case CL_PROFILING_INFO_NOT_AVAILABLE:
            return "CL_PROFILING_INFO_NOT_AVAILABLE";        
        case CL_MEM_COPY_OVERLAP:
            return "CL_MEM_COPY_OVERLAP";                    
        case CL_IMAGE_FORMAT_MISMATCH:
            return "CL_IMAGE_FORMAT_MISMATCH";               
        case CL_IMAGE_FORMAT_NOT_SUPPORTED:
            return "CL_IMAGE_FORMAT_NOT_SUPPORTED";         
        case CL_BUILD_PROGRAM_FAILURE:
            return "CL_BUILD_PROGRAM_FAILURE";              
        case CL_MAP_FAILURE:
            return "CL_MAP_FAILURE";                         
        case CL_MISALIGNED_SUB_BUFFER_OFFSET:
            return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
        case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
            return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
        case CL_INVALID_VALUE:
            return "CL_INVALID_VALUE";                      
        case CL_INVALID_DEVICE_TYPE:
            return "CL_INVALID_DEVICE_TYPE";               
        case CL_INVALID_PLATFORM:
            return "CL_INVALID_PLATFORM";                   
        case CL_INVALID_DEVICE:
            return "CL_INVALID_DEVICE";                    
        case CL_INVALID_CONTEXT:
            return "CL_INVALID_CONTEXT";                    
        case CL_INVALID_QUEUE_PROPERTIES:
            return "CL_INVALID_QUEUE_PROPERTIES";           
        case CL_INVALID_COMMAND_QUEUE:
            return "CL_INVALID_COMMAND_QUEUE";              
        case CL_INVALID_HOST_PTR:
            return "CL_INVALID_HOST_PTR";                   
        case CL_INVALID_MEM_OBJECT:
            return "CL_INVALID_MEM_OBJECT";                  
        case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
            return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";    
        case CL_INVALID_IMAGE_SIZE:
             return "CL_INVALID_IMAGE_SIZE";                 
        case CL_INVALID_SAMPLER:
            return "CL_INVALID_SAMPLER";                    
        case CL_INVALID_BINARY:
            return "CL_INVALID_BINARY";                     
        case CL_INVALID_BUILD_OPTIONS:
            return "CL_INVALID_BUILD_OPTIONS";              
        case CL_INVALID_PROGRAM:
            return "CL_INVALID_PROGRAM";                    
        case CL_INVALID_PROGRAM_EXECUTABLE:
            return "CL_INVALID_PROGRAM_EXECUTABLE";          
        case CL_INVALID_KERNEL_NAME:
            return "CL_INVALID_KERNEL_NAME";                
        case CL_INVALID_KERNEL_DEFINITION:
            return "CL_INVALID_KERNEL_DEFINITION";          
        case CL_INVALID_KERNEL:
            return "CL_INVALID_KERNEL";                     
        case CL_INVALID_ARG_INDEX:
            return "CL_INVALID_ARG_INDEX";                   
        case CL_INVALID_ARG_VALUE:
            return "CL_INVALID_ARG_VALUE";                   
        case CL_INVALID_ARG_SIZE:
            return "CL_INVALID_ARG_SIZE";                    
        case CL_INVALID_KERNEL_ARGS:
            return "CL_INVALID_KERNEL_ARGS";                
        case CL_INVALID_WORK_DIMENSION:
            return "CL_INVALID_WORK_DIMENSION";              
        case CL_INVALID_WORK_GROUP_SIZE:
            return "CL_INVALID_WORK_GROUP_SIZE";             
        case CL_INVALID_WORK_ITEM_SIZE:
            return "CL_INVALID_WORK_ITEM_SIZE";             
        case CL_INVALID_GLOBAL_OFFSET:
            return "CL_INVALID_GLOBAL_OFFSET";              
        case CL_INVALID_EVENT_WAIT_LIST:
            return "CL_INVALID_EVENT_WAIT_LIST";             
        case CL_INVALID_EVENT:
            return "CL_INVALID_EVENT";                      
        case CL_INVALID_OPERATION:
            return "CL_INVALID_OPERATION";                 
        case CL_INVALID_GL_OBJECT:
            return "CL_INVALID_GL_OBJECT";                  
        case CL_INVALID_BUFFER_SIZE:
            return "CL_INVALID_BUFFER_SIZE";                 
        case CL_INVALID_MIP_LEVEL:
            return "CL_INVALID_MIP_LEVEL";                   
        case CL_INVALID_GLOBAL_WORK_SIZE:
            return "CL_INVALID_GLOBAL_WORK_SIZE";            
        case CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR:
            return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
        case CL_PLATFORM_NOT_FOUND_KHR:
            return "CL_PLATFORM_NOT_FOUND_KHR";
        //case CL_INVALID_PROPERTY_EXT:
        //    return "CL_INVALID_PROPERTY_EXT";
        case CL_DEVICE_PARTITION_FAILED_EXT:
            return "CL_DEVICE_PARTITION_FAILED_EXT";
        case CL_INVALID_PARTITION_COUNT_EXT:
            return "CL_INVALID_PARTITION_COUNT_EXT"; 
        default:
            return "unknown error code";
    }

    return "unknown error code";
}


template<typename T>
int SDKDeviceInfo::checkVal(
    T input, 
    T reference, 
    std::string message,
    bool isAPIerror) const
{
    if(input==reference)
    {
        return 0;
    }
    else
    {
        if(isAPIerror)
        {
            std::cout<<"Error: "<< message << " Error code : ";
            std::cout << getOpenCLErrorCodeStr(input) << std::endl;
        }
        else
			std::cout << message;   
        return 1;
    }
}


template<typename T>
int KernelWorkGroupInfo::checkVal(
    T input, 
    T reference, 
    std::string message,
    bool isAPIerror) const
{
    if(input==reference)
    {
        return 0;
    }
    else
    {
        if(isAPIerror)
        {
            std::cout<<"Error: "<< message << " Error code : ";
            std::cout << getOpenCLErrorCodeStr(input) << std::endl;
        }
        else
			std::cout << message;   
        return 1;
    }
}

int KernelWorkGroupInfo::setKernelWorkGroupInfo(cl_kernel &kernel,cl_device_id &device)
{
	cl_int status = CL_SUCCESS;

	//Get Kernel Work Group Info
	status = clGetKernelWorkGroupInfo(kernel,
									  device,
									  CL_KERNEL_WORK_GROUP_SIZE,
									  sizeof(size_t),
									  &kernelWorkGroupSize,
									  NULL);
	if(checkVal(status, CL_SUCCESS, "clGetKernelWorkGroupInfo failed(CL_KERNEL_WORK_GROUP_SIZE)"))
			return SDK_FAILURE;
	
	status = clGetKernelWorkGroupInfo(kernel,
									  device,
									  CL_KERNEL_LOCAL_MEM_SIZE,
									  sizeof(cl_ulong),
									  &localMemoryUsed,
									  NULL);
	if(checkVal(status, CL_SUCCESS, "clGetKernelWorkGroupInfo failed(CL_KERNEL_LOCAL_MEM_SIZE)"))
			return SDK_FAILURE;

	status = clGetKernelWorkGroupInfo(kernel,
									  device,
									  CL_KERNEL_COMPILE_WORK_GROUP_SIZE,
									  sizeof(size_t) * 3,
									  compileWorkGroupSize,
									  NULL);
	if(checkVal(status, CL_SUCCESS, "clGetKernelWorkGroupInfo failed(CL_KERNEL_COMPILE_WORK_GROUP_SIZE)"))
			return SDK_FAILURE;

	return SDK_SUCCESS;
}


// Set all information for a given device id
int 
SDKDeviceInfo::setDeviceInfo(cl_device_id deviceId)
{
    cl_int status = CL_SUCCESS;


    //Get device type
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_TYPE, 
                    sizeof(cl_device_type), 
                    &dType, 
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_TYPE) failed");

    //Get vender ID
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_VENDOR_ID, 
                    sizeof(cl_uint), 
                    &venderId, 
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_VENDOR_ID) failed");

    //Get max compute units
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_MAX_COMPUTE_UNITS, 
                    sizeof(cl_uint), 
                    &maxComputeUnits, 
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_MAX_COMPUTE_UNITS) failed");

    //Get max work item dimensions
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
                    sizeof(cl_uint),
                    &maxWorkItemDims,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS) failed");

    //Get max work item sizes
    delete maxWorkItemSizes;
    maxWorkItemSizes = new size_t[maxWorkItemDims];
    CHECK_ALLOCATION(maxWorkItemSizes, "Failed to allocate memory(maxWorkItemSizes)");

    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_MAX_WORK_ITEM_SIZES,
                    maxWorkItemDims * sizeof(size_t),
                    maxWorkItemSizes,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS) failed");

    // Maximum work group size
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_MAX_WORK_GROUP_SIZE,
                    sizeof(size_t),
                    &maxWorkGroupSize,
                    NULL);
   CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_MAX_WORK_GROUP_SIZE) failed");

    // Preferred vector sizes of all data types
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR,
                    sizeof(cl_uint),
                    &preferredCharVecWidth,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR) failed");

    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT,
                    sizeof(cl_uint),
                    &preferredShortVecWidth,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT) failed");

    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT,
                    sizeof(cl_uint),
                    &preferredIntVecWidth,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT) failed");

    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG,
                    sizeof(cl_uint),
                    &preferredLongVecWidth,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG) failed");

    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT,
                    sizeof(cl_uint),
                    &preferredFloatVecWidth,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT) failed");

    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE,
                    sizeof(cl_uint),
                    &preferredDoubleVecWidth,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE) failed");

    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF,
                    sizeof(cl_uint),
                    &preferredHalfVecWidth,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF) failed");

    // Clock frequency
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_MAX_CLOCK_FREQUENCY,
                    sizeof(cl_uint),
                    &maxClockFrequency,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_MAX_CLOCK_FREQUENCY) failed");

    // Address bits
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_ADDRESS_BITS,
                    sizeof(cl_uint),
                    &addressBits,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_ADDRESS_BITS) failed");

    // Maximum memory alloc size
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_MAX_MEM_ALLOC_SIZE,
                    sizeof(cl_ulong),
                    &maxMemAllocSize,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_MAX_MEM_ALLOC_SIZE) failed");

    // Image support
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_IMAGE_SUPPORT,
                    sizeof(cl_bool),
                    &imageSupport,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_IMAGE_SUPPORT) failed");

    // Maximum read image arguments
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_MAX_READ_IMAGE_ARGS,
                    sizeof(cl_uint),
                    &maxReadImageArgs,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_MAX_READ_IMAGE_ARGS) failed");

    // Maximum write image arguments
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_MAX_WRITE_IMAGE_ARGS,
                    sizeof(cl_uint),
                    &maxWriteImageArgs,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_MAX_WRITE_IMAGE_ARGS) failed");

    // 2D image and 3D dimensions
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_IMAGE2D_MAX_WIDTH,
                    sizeof(size_t),
                    &image2dMaxWidth,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_IMAGE2D_MAX_WIDTH) failed");

    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_IMAGE2D_MAX_HEIGHT,
                    sizeof(size_t),
                    &image2dMaxHeight,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_IMAGE2D_MAX_HEIGHT) failed");

    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_IMAGE3D_MAX_WIDTH,
                    sizeof(size_t),
                    &image3dMaxWidth,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_IMAGE3D_MAX_WIDTH) failed");

    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_IMAGE3D_MAX_HEIGHT,
                    sizeof(size_t),
                    &image3dMaxHeight,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_IMAGE3D_MAX_HEIGHT) failed");

    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_IMAGE3D_MAX_DEPTH,
                    sizeof(size_t),
                    &image3dMaxDepth,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_IMAGE3D_MAX_DEPTH) failed");

    // Maximum samplers
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_MAX_SAMPLERS,
                    sizeof(cl_uint),
                    &maxSamplers,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_MAX_SAMPLERS) failed");

    // Maximum parameter size
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_MAX_PARAMETER_SIZE,
                    sizeof(size_t),
                    &maxParameterSize,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_MAX_PARAMETER_SIZE) failed");

    // Memory base address align
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_MEM_BASE_ADDR_ALIGN,
                    sizeof(cl_uint),
                    &memBaseAddressAlign,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_MEM_BASE_ADDR_ALIGN) failed");

    // Minimum data type align size
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE,
                    sizeof(cl_uint),
                    &minDataTypeAlignSize,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE) failed");

    // Single precision floating point configuration
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_SINGLE_FP_CONFIG,
                    sizeof(cl_device_fp_config),
                    &singleFpConfig,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_SINGLE_FP_CONFIG) failed");

    // Double precision floating point configuration
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_DOUBLE_FP_CONFIG,
                    sizeof(cl_device_fp_config),
                    &doubleFpConfig,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_DOUBLE_FP_CONFIG) failed");

    // Global memory cache type
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_GLOBAL_MEM_CACHE_TYPE,
                    sizeof(cl_device_mem_cache_type),
                    &globleMemCacheType,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_GLOBAL_MEM_CACHE_TYPE) failed");

    // Global memory cache line size
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE,
                    sizeof(cl_uint),
                    &globalMemCachelineSize,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE) failed");

    // Global memory cache size
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_GLOBAL_MEM_CACHE_SIZE,
                    sizeof(cl_ulong),
                    &globalMemCacheSize,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_GLOBAL_MEM_CACHE_SIZE) failed");

    // Global memory size
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_GLOBAL_MEM_SIZE,
                    sizeof(cl_ulong),
                    &globalMemSize,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_GLOBAL_MEM_SIZE) failed");

    // Maximum constant buffer size
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE,
                    sizeof(cl_ulong),
                    &maxConstBufSize,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE) failed");

    // Maximum constant arguments
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_MAX_CONSTANT_ARGS,
                    sizeof(cl_uint),
                    &maxConstArgs,
                    NULL);
   CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_MAX_CONSTANT_ARGS) failed");

    // Local memory type
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_LOCAL_MEM_TYPE,
                    sizeof(cl_device_local_mem_type),
                    &localMemType,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_LOCAL_MEM_TYPE) failed");

    // Local memory size
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_LOCAL_MEM_SIZE,
                    sizeof(cl_ulong),
                    &localMemSize,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_LOCAL_MEM_SIZE) failed");

    // Error correction support
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_ERROR_CORRECTION_SUPPORT,
                    sizeof(cl_bool),
                    &errCorrectionSupport,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_ERROR_CORRECTION_SUPPORT) failed");

    // Profiling timer resolution
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_PROFILING_TIMER_RESOLUTION,
                    sizeof(size_t),
                    &timerResolution,
                    NULL);
   CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_PROFILING_TIMER_RESOLUTION) failed");

    // Endian little
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_ENDIAN_LITTLE,
                    sizeof(cl_bool),
                    &endianLittle,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_ENDIAN_LITTLE) failed");

    // Device available
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_AVAILABLE,
                    sizeof(cl_bool),
                    &available,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_AVAILABLE) failed");

    // Device compiler available
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_COMPILER_AVAILABLE,
                    sizeof(cl_bool),
                    &compilerAvailable,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_COMPILER_AVAILABLE) failed");

    // Device execution capabilities
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_EXECUTION_CAPABILITIES,
                    sizeof(cl_device_exec_capabilities),
                    &execCapabilities,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_EXECUTION_CAPABILITIES) failed");

    // Device queue properities
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_QUEUE_PROPERTIES,
                    sizeof(cl_command_queue_properties),
                    &queueProperties,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_QUEUE_PROPERTIES) failed");

    // Platform
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_PLATFORM,
                    sizeof(cl_platform_id),
                    &platform,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_PLATFORM) failed");

    // Device name
    size_t tempSize = 0;
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_NAME,
                    0,
                    NULL,
                    &tempSize);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_NAME) failed");

    delete name;
    name = new char[tempSize];
    CHECK_ALLOCATION(name, "Failed to allocate memory(name)");

    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_NAME,
                    sizeof(char) * tempSize,
                    name,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_NAME) failed");

    // Vender name
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_VENDOR,
                    0,
                    NULL,
                    &tempSize);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_VENDOR) failed");

    delete vendorName;
    vendorName = new char[tempSize];
    CHECK_ALLOCATION(vendorName, "Failed to allocate memory(venderName)");

    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_VENDOR,
                    sizeof(char) * tempSize,
                    vendorName,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_VENDOR) failed");

    // Driver name
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DRIVER_VERSION,
                    0,
                    NULL,
                    &tempSize);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DRIVER_VERSION) failed");

    delete driverVersion;
    driverVersion = new char[tempSize];
    CHECK_ALLOCATION(driverVersion, "Failed to allocate memory(driverVersion)");

    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DRIVER_VERSION,
                    sizeof(char) * tempSize,
                    driverVersion,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DRIVER_VERSION) failed");

    // Device profile
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_PROFILE,
                    0,
                    NULL,
                    &tempSize);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_PROFILE) failed");

    delete profileType;
    profileType = new char[tempSize];
    CHECK_ALLOCATION(profileType, "Failed to allocate memory(profileType)");

    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_PROFILE,
                    sizeof(char) * tempSize,
                    profileType,
                    NULL);
   CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_PROFILE) failed");

    // Device version
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_VERSION,
                    0,
                    NULL,
                    &tempSize);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_VERSION) failed");

    delete deviceVersion;
    deviceVersion = new char[tempSize];
    CHECK_ALLOCATION(deviceVersion, "Failed to allocate memory(deviceVersion)");

    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_VERSION,
                    sizeof(char) * tempSize,
                    deviceVersion,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_VERSION) failed");

    // Device extensions
    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_EXTENSIONS,
                    0,
                    NULL,
                    &tempSize);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_EXTENSIONS) failed");

    delete extensions;
    extensions = new char[tempSize];
    CHECK_ALLOCATION(extensions, "Failed to allocate memory(extensions)");

    status = clGetDeviceInfo(
                    deviceId, 
                    CL_DEVICE_EXTENSIONS,
                    sizeof(char) * tempSize,
                    extensions,
                    NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_EXTENSIONS) failed");

    // Device parameters of OpenCL 1.1 Specification
#ifdef CL_VERSION_1_1
    std::string deviceVerStr(deviceVersion);
    size_t vStart = deviceVerStr.find(" ", 0);
    size_t vEnd = deviceVerStr.find(" ", vStart + 1);
    std::string vStrVal = deviceVerStr.substr(vStart + 1, vEnd - vStart - 1);
    if(vStrVal.compare("1.0") > 0)
    {
        // Native vector sizes of all data types
        status = clGetDeviceInfo(
                        deviceId, 
                        CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR,
                        sizeof(cl_uint),
                        &nativeCharVecWidth,
                        NULL);
        CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR) failed");

        status = clGetDeviceInfo(
                        deviceId, 
                        CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT,
                        sizeof(cl_uint),
                        &nativeShortVecWidth,
                        NULL);
        CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT) failed");

        status = clGetDeviceInfo(
                        deviceId, 
                        CL_DEVICE_NATIVE_VECTOR_WIDTH_INT,
                        sizeof(cl_uint),
                        &nativeIntVecWidth,
                        NULL);
        CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_NATIVE_VECTOR_WIDTH_INT) failed");

        status = clGetDeviceInfo(
                        deviceId, 
                        CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG,
                        sizeof(cl_uint),
                        &nativeLongVecWidth,
                        NULL);
        CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG) failed");

        status = clGetDeviceInfo(
                        deviceId, 
                        CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT,
                        sizeof(cl_uint),
                        &nativeFloatVecWidth,
                        NULL);
        CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT) failed");

        status = clGetDeviceInfo(
                        deviceId, 
                        CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE,
                        sizeof(cl_uint),
                        &nativeDoubleVecWidth,
                        NULL);
        CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE) failed");

        status = clGetDeviceInfo(
                        deviceId, 
                        CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF,
                        sizeof(cl_uint),
                        &nativeHalfVecWidth,
                        NULL);
        CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF) failed");

        // Host unified memory
        status = clGetDeviceInfo(
                        deviceId, 
                        CL_DEVICE_HOST_UNIFIED_MEMORY,
                        sizeof(cl_bool),
                        &hostUnifiedMem,
                        NULL);
        CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_HOST_UNIFIED_MEMORY) failed");

        // Device OpenCL C version
        status = clGetDeviceInfo(
                        deviceId, 
                        CL_DEVICE_OPENCL_C_VERSION,
                        0,
                        NULL,
                        &tempSize);
        CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_OPENCL_C_VERSION) failed");

        delete openclCVersion;
        openclCVersion = new char[tempSize];
        CHECK_ALLOCATION(openclCVersion, "Failed to allocate memory(openclCVersion)");

        status = clGetDeviceInfo(
                        deviceId, 
                        CL_DEVICE_OPENCL_C_VERSION,
                        sizeof(char) * tempSize,
                        openclCVersion,
                        NULL);
        CHECK_OPENCL_ERROR(status, "clGetDeviceIDs(CL_DEVICE_OPENCL_C_VERSION) failed");
    }
#endif

    return SDK_SUCCESS;
}

template<typename T>
int SDKCommon::checkVal(
    T input, 
    T reference, 
    std::string message,
    bool isAPIerror)
{
    if(input==reference)
    {
        return SDK_SUCCESS;
    }
    else
    {
        if(isAPIerror)
        {
            std::cout<<"Error: "<< message << " Error code : ";
            std::cout << getOpenCLErrorCodeStr(input) << std::endl;
        }
        else
            error(message);   
        return SDK_FAILURE;
    }
}

template<typename T>
std::string SDKCommon::toString(T t, std::ios_base &(*r)(std::ios_base&))
{
  std::ostringstream output;
  output << r << t;
  return output.str();
}

/*
 * Displays the platform name,  device ids and device names for given platform
 */
int SDKCommon::displayDevices(cl_platform_id platform, cl_device_type deviceType)
{
    cl_int status;

    // Get platform name
    char platformVendor[1024];
    status = clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, sizeof(platformVendor), platformVendor, NULL);
    CHECK_OPENCL_ERROR(status, "clGetPlatformInfo failed");
    
    std::cout << "\nSelected Platform Vendor : " << platformVendor << std::endl;

    // Get number of devices available 
    cl_uint deviceCount = 0;
    status = clGetDeviceIDs(platform, deviceType, 0, NULL, &deviceCount);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs failed");

    cl_device_id* deviceIds = (cl_device_id*)malloc(sizeof(cl_device_id) * deviceCount);
    CHECK_ALLOCATION(deviceIds, "Failed to allocate memory(deviceIds)");

    // Get device ids
    status = clGetDeviceIDs(platform, deviceType, deviceCount, deviceIds, NULL);
    CHECK_OPENCL_ERROR(status, "clGetDeviceIDs failed");

    // Print device index and device names
    for(cl_uint i = 0; i < deviceCount; ++i)
    {
        char deviceName[1024];
        status = clGetDeviceInfo(deviceIds[i], CL_DEVICE_NAME, sizeof(deviceName), deviceName, NULL);
        CHECK_OPENCL_ERROR(status, "clGetDeviceInfo failed");
        
        std::cout << "Device " << i << " : " << deviceName <<" Device ID is "<<deviceIds[i]<< std::endl;
    }

    free(deviceIds);
    
    return SDK_SUCCESS;
}

int 
SDKCommon::displayPlatformAndDevices(
cl_platform_id platform, const cl_device_id* devices, const int deviceCount)
{
    cl_int status;

    // Get platform name
    char platformVendor[1024];
    status = clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, sizeof(platformVendor), platformVendor, NULL);
    CHECK_OPENCL_ERROR(status, "clGetPlatformInfo failed");
    
    std::cout << "\nSelected Platform Vendor : " << platformVendor << std::endl;

    // Print device index and device names
    for(cl_int i = 0; i < deviceCount; ++i)
    {
        char deviceName[1024];
        status = clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(deviceName), deviceName, NULL);
        CHECK_OPENCL_ERROR(status, "clGetDeviceInfo failed");
        
        std::cout << "Device " << i << " : " << deviceName << std::endl;
    }

    return SDK_SUCCESS;
}


int 
SDKCommon::validateDeviceId(int deviceId, int deviceCount)
{
    // Validate deviceIndex
    if(deviceId >= (int)deviceCount)
    {
        std::cout << "DeviceId should be < " << deviceCount << std::endl;
        return SDK_FAILURE;
    }
    return SDK_SUCCESS;
}

int
SDKCommon::generateBinaryImage(const bifData &binaryData)
{
    cl_int status = CL_SUCCESS;

    /*
     * Have a look at the available platforms and pick either
     * the AMD one if available or a reasonable default.
     */
    cl_uint numPlatforms;
    cl_platform_id platform = NULL;
    status = clGetPlatformIDs(0, NULL, &numPlatforms);
    CHECK_OPENCL_ERROR(status, "clGetPlatformIDs failed.");

    if (0 < numPlatforms) 
    {
        cl_platform_id* platforms = new cl_platform_id[numPlatforms];
        status = clGetPlatformIDs(numPlatforms, platforms, NULL);
        CHECK_OPENCL_ERROR(status, "clGetPlatformIDs failed.");

        char platformName[100];
        for (unsigned i = 0; i < numPlatforms; ++i) 
        {
            status = clGetPlatformInfo(
                        platforms[i],
                        CL_PLATFORM_VENDOR,
                        sizeof(platformName),
                        platformName,
                        NULL);
			CHECK_OPENCL_ERROR(status, "clGetPlatformInfo failed.");

            platform = platforms[i];
            if (!strcmp(platformName, "Advanced Micro Devices, Inc.")) 
                break;
        }
        std::cout << "Platform found : " << platformName << "\n";
        delete[] platforms;
    }

    if(NULL == platform)
    {
		std::cout << "NULL platform found so Exiting Application.";
        return SDK_FAILURE;
    }

    /*
     * If we could find our platform, use it. Otherwise use just available platform.
     */
    cl_context_properties cps[5] = 
    {
        CL_CONTEXT_PLATFORM, 
        (cl_context_properties)platform, 
        CL_CONTEXT_OFFLINE_DEVICES_AMD,
        (cl_context_properties)1,
        0
    };

    cl_context context = clCreateContextFromType(
                            cps,
                            CL_DEVICE_TYPE_ALL,
                            NULL,
                            NULL,
                            &status);
    CHECK_OPENCL_ERROR(status, "clCreateContextFromType failed.");

    /* create a CL program using the kernel source */
    SDKFile kernelFile;
    std::string kernelPath = getPath();
    kernelPath.append(binaryData.kernelName.c_str());
    if(!kernelFile.open(kernelPath.c_str()))
    {
        std::cout << "Failed to load kernel file : " << kernelPath << std::endl;
        return SDK_FAILURE;
    }
    const char * source = kernelFile.source().c_str();
    size_t sourceSize[] = {strlen(source)};
    cl_program program = clCreateProgramWithSource(
                            context,
                            1,
                            &source,
                            sourceSize,
                            &status);
    CHECK_OPENCL_ERROR(status, "clCreateProgramWithSource failed.");

    std::string flagsStr = std::string(binaryData.flagsStr.c_str());

    // Get additional options
    if(binaryData.flagsFileName.size() != 0)
    {
        streamsdk::SDKFile flagsFile;
        std::string flagsPath = getPath();
        flagsPath.append(binaryData.flagsFileName.c_str());
        if(!flagsFile.open(flagsPath.c_str()))
        {
            std::cout << "Failed to load flags file: " << flagsPath << std::endl;
            return SDK_FAILURE;
        }
        flagsFile.replaceNewlineWithSpaces();
        const char * flags = flagsFile.source().c_str();
        flagsStr.append(flags);
    }

    if(flagsStr.size() != 0)
        std::cout << "Build Options are : " << flagsStr.c_str() << std::endl;

    /* create a cl program executable for all the devices specified */
    status = clBuildProgram(
                program,
                0,
                NULL,
                flagsStr.c_str(),
                NULL,
                NULL);
    /* This function is intentionally left without a error check
      as it may not pass if kernels rely on specific properties of devices
      In such cases, binaries for eligible devices are geenrated and dumped
      even wen this function will return an error */

    //CHECK_OPENCL_ERROR(status, "clBuildProgram failed.");

    size_t numDevices;
    status = clGetProgramInfo(
                program, 
                CL_PROGRAM_NUM_DEVICES,
                sizeof(numDevices),
                &numDevices,
                NULL );
    CHECK_OPENCL_ERROR(status, "clGetProgramInfo(CL_PROGRAM_NUM_DEVICES) failed.");

    std::cout << "Number of devices found : " << numDevices << "\n\n";
    cl_device_id *devices = (cl_device_id *)malloc( sizeof(cl_device_id) * numDevices );
    CHECK_ALLOCATION(devices, "Failed to allocate host memory.(devices)");

    /* grab the handles to all of the devices in the program. */
    status = clGetProgramInfo(
                program, 
                CL_PROGRAM_DEVICES, 
                sizeof(cl_device_id) * numDevices,
                devices,
                NULL );
    CHECK_OPENCL_ERROR(status, "clGetProgramInfo(CL_PROGRAM_DEVICES) failed.");

    /* figure out the sizes of each of the binaries. */
    size_t *binarySizes = (size_t*)malloc( sizeof(size_t) * numDevices );
   CHECK_ALLOCATION(binarySizes, "Failed to allocate host memory.(binarySizes)");
    
    status = clGetProgramInfo(
                program, 
                CL_PROGRAM_BINARY_SIZES,
                sizeof(size_t) * numDevices, 
                binarySizes, 
                NULL);
    CHECK_OPENCL_ERROR(status, "clGetProgramInfo(CL_PROGRAM_BINARY_SIZES) failed.");

    size_t i = 0;
    /* copy over all of the generated binaries. */
    char **binaries = (char **)malloc( sizeof(char *) * numDevices );
    CHECK_ALLOCATION(binaries, "Failed to allocate host memory.(binaries)");

    for(i = 0; i < numDevices; i++)
    {
        if(binarySizes[i] != 0)
        {
            binaries[i] = (char *)malloc( sizeof(char) * binarySizes[i]);
            CHECK_ALLOCATION(binaries[i], "Failed to allocate host memory.(binaries[i])");
        }
        else
        {
            binaries[i] = NULL;
        }
    }
    status = clGetProgramInfo(
                program, 
                CL_PROGRAM_BINARIES,
                sizeof(char *) * numDevices, 
                binaries, 
                NULL);
    CHECK_OPENCL_ERROR(status, "clGetProgramInfo(CL_PROGRAM_BINARIES) failed.");

    /* dump out each binary into its own separate file. */
    for(i = 0; i < numDevices; i++)
    {
        char fileName[100];
        sprintf(fileName, "%s.%d", binaryData.binaryName.c_str(), (int)i);
        char deviceName[1024];
        status = clGetDeviceInfo(
                    devices[i], 
                    CL_DEVICE_NAME, 
                    sizeof(deviceName),
                    deviceName, 
                    NULL);
        CHECK_OPENCL_ERROR(status, "clGetDeviceInfo(CL_DEVICE_NAME) failed.");

        if(binarySizes[i] != 0)
        {
            printf( "%s binary kernel: %s\n", deviceName, fileName);
            streamsdk::SDKFile BinaryFile;
            if(BinaryFile.writeBinaryToFile(fileName, 
                                             binaries[i], 
                                             binarySizes[i]))
            {
                std::cout << "Failed to load kernel file : " << fileName << std::endl;
                return SDK_FAILURE;
            }
        }
        else
        {
            printf(
                "%s binary kernel(%s) : %s\n", 
                deviceName, 
                fileName,
                "Skipping as there is no binary data to write!");
        }
    }

    // Release all resouces and memory
    for(i = 0; i < numDevices; i++)
    {
        if(binaries[i] != NULL)
        {
            free(binaries[i]);
            binaries[i] = NULL;
        }
    }

    if(binaries != NULL)
    {
        free(binaries);
        binaries = NULL;
    }

    if(binarySizes != NULL)
    {
        free(binarySizes);
        binarySizes = NULL;
    }

    if(devices != NULL)
    {
        free(devices);
        devices = NULL;
    }

    status = clReleaseProgram(program);
    CHECK_OPENCL_ERROR(status, "clReleaseProgram failed.");

    status = clReleaseContext(context);
    CHECK_OPENCL_ERROR(status, "clReleaseContext failed.");

    return SDK_SUCCESS;
}

int SDKCommon::getDevices(cl_context &context, cl_device_id **devices, cl_int deviceId, bool deviceIdEnabled)
{
	/* First, get the size of device list data */
    size_t deviceListSize = 0;
	int status = 0;
	status = clGetContextInfo(
                 context, 
                 CL_CONTEXT_DEVICES, 
                 0, 
                 NULL, 
                 &deviceListSize);
    CHECK_OPENCL_ERROR(status, "clGetContextInfo failed.");

    int deviceCount = (int)(deviceListSize / sizeof(cl_device_id));
    if(validateDeviceId(deviceId, deviceCount))
    {
		std::cout << "Invalid Device Selected";
        return SDK_FAILURE;
    }

    /**
	 * Now allocate memory for device list based on the size we got earlier
	 * Note that this memory is allocated to a pointer which is a argument
	 * so it must not be deleted inside this function. The Sample implementer
	 * has to delete the devices pointer in the host code at clean up
	 */

    (*devices) = (cl_device_id *)malloc(deviceListSize);
    CHECK_ALLOCATION((*devices), "Failed to allocate memory (devices).");

    /* Now, get the device list data */
    status = clGetContextInfo(context, 
                 CL_CONTEXT_DEVICES, 
                 deviceListSize, 
                 (*devices), 
                 NULL);
    CHECK_OPENCL_ERROR(status, "clGetGetContextInfo failed.");

	return SDK_SUCCESS;
}

int
SDKCommon::getPlatform(cl_platform_id &platform, int platformId, bool platformIdEnabled)
{
    cl_uint numPlatforms;
    cl_int status = clGetPlatformIDs(0, NULL, &numPlatforms);
    CHECK_OPENCL_ERROR(status, "clGetPlatformIDs failed.");

    if (0 < numPlatforms) 
    {
        cl_platform_id* platforms = new cl_platform_id[numPlatforms];
        status = clGetPlatformIDs(numPlatforms, platforms, NULL);
        CHECK_OPENCL_ERROR(status, "clGetPlatformIDs failed.");

        if(platformIdEnabled)
        {
            platform = platforms[platformId];
        }
        else
        {
            char platformName[100];
            for (unsigned i = 0; i < numPlatforms; ++i) 
            {
                status = clGetPlatformInfo(platforms[i],
                                           CL_PLATFORM_VENDOR,
                                           sizeof(platformName),
                                           platformName,
                                           NULL);
				CHECK_OPENCL_ERROR(status, "clGetPlatformInfo failed.");

                platform = platforms[i];
                if (!strcmp(platformName, "Advanced Micro Devices, Inc.")) 
                {
                    break;
                }
            }
            std::cout << "Platform found : " << platformName << "\n";
        }
        delete[] platforms;
    }

    if(NULL == platform)
    {
        error("NULL platform found so Exiting Application.");
        return SDK_FAILURE;
    }

    return SDK_SUCCESS;
}


int
SDKCommon::buildOpenCLProgram(cl_program &program, const cl_context& context, const buildProgramData &buildData)
{
    cl_int status = CL_SUCCESS;
    SDKFile kernelFile;
    std::string kernelPath = getPath();
    if(buildData.binaryName.size() != 0)
    {
        kernelPath.append(buildData.binaryName.c_str());
        if(kernelFile.readBinaryFromFile(kernelPath.c_str()))
        {
            std::cout << "Failed to load kernel file : " << kernelPath << std::endl;
            return SDK_FAILURE;
        }

        const char * binary = kernelFile.source().c_str();
        size_t binarySize = kernelFile.source().size();
        program = clCreateProgramWithBinary(context,
                                            1,
                                            &buildData.devices[buildData.deviceId], 
                                            (const size_t *)&binarySize,
                                            (const unsigned char**)&binary,
                                            NULL,
                                            &status);
        CHECK_OPENCL_ERROR(status, "clCreateProgramWithBinary failed.");
    }
    else
    {
        kernelPath.append(buildData.kernelName.c_str());
        if(!kernelFile.open(kernelPath.c_str()))//bool
        {
            std::cout << "Failed to load kernel file: " << kernelPath << std::endl;
            return SDK_FAILURE;
        }
        const char * source = kernelFile.source().c_str();
        size_t sourceSize[] = {strlen(source)};
        program = clCreateProgramWithSource(context,
                                            1,
                                            &source,
                                            sourceSize,
                                            &status);
        CHECK_OPENCL_ERROR(status, "clCreateProgramWithSource failed.");
    }

    std::string flagsStr = std::string(buildData.flagsStr.c_str());

    // Get additional options
    if(buildData.flagsFileName.size() != 0)
    {
        streamsdk::SDKFile flagsFile;
        std::string flagsPath = getPath();
        flagsPath.append(buildData.flagsFileName.c_str());
        if(!flagsFile.open(flagsPath.c_str()))
        {
            std::cout << "Failed to load flags file: " << flagsPath << std::endl;
            return SDK_FAILURE;
        }
        flagsFile.replaceNewlineWithSpaces();
        const char * flags = flagsFile.source().c_str();
        flagsStr.append(flags);
    }

    if(flagsStr.size() != 0)
        std::cout << "Build Options are : " << flagsStr.c_str() << std::endl;

    /* create a cl program executable for all the devices specified */
    status = clBuildProgram(program, 1, &buildData.devices[buildData.deviceId], flagsStr.c_str(), NULL, NULL);
    if(status != CL_SUCCESS)
    {
        if(status == CL_BUILD_PROGRAM_FAILURE)
        {
            cl_int logStatus;
            char *buildLog = NULL;
            size_t buildLogSize = 0;
            logStatus = clGetProgramBuildInfo (
                            program, 
                            buildData.devices[buildData.deviceId], 
                            CL_PROGRAM_BUILD_LOG, 
                            buildLogSize, 
                            buildLog, 
                            &buildLogSize);
            CHECK_OPENCL_ERROR(logStatus, "clGetProgramBuildInfo failed.");

            buildLog = (char*)malloc(buildLogSize);
            CHECK_ALLOCATION(buildLog, "Failed to allocate host memory. (buildLog)");

            memset(buildLog, 0, buildLogSize);

            logStatus = clGetProgramBuildInfo (
                            program, 
                            buildData.devices[buildData.deviceId], 
                            CL_PROGRAM_BUILD_LOG, 
                            buildLogSize, 
                            buildLog, 
                            NULL);
            if(checkVal(logStatus, CL_SUCCESS, "clGetProgramBuildInfo failed."))
            {
                free(buildLog);
                return SDK_FAILURE;
            }

            std::cout << " \n\t\t\tBUILD LOG\n";
            std::cout << " ************************************************\n";
            std::cout << buildLog << std::endl;
            std::cout << " ************************************************\n";
            free(buildLog);
        }

        CHECK_OPENCL_ERROR(status, "clBuildProgram failed.");
    }
    return SDK_SUCCESS;
}

bool
SDKCommon::compare(const float *refData, const float *data, 
                        const int length, const float epsilon)
{
    float error = 0.0f;
    float ref = 0.0f;

    for(int i = 1; i < length; ++i) 
    {
        float diff = refData[i] - data[i];
        error += diff * diff;
        ref += refData[i] * refData[i];
    }

    float normRef =::sqrtf((float) ref);
    if (::fabs((float) ref) < 1e-7f) {
        return false;
    }
    float normError = ::sqrtf((float) error);
    error = normError / normRef;

    return error < epsilon;
}

bool
SDKCommon::compare(const double *refData, const double *data, 
                        const int length, const double epsilon)
{
    double error = 0.0;
    double ref = 0.0;

    for(int i = 1; i < length; ++i) 
    {
        double diff = refData[i] - data[i];
        error += diff * diff;
        ref += refData[i] * refData[i];
    }

    double normRef =::sqrt((double) ref);
    if (::fabs((double) ref) < 1e-7) {
        return false;
    }
    double normError = ::sqrt((double) error);
    error = normError / normRef;

    return error < epsilon;
}

size_t
SDKCommon::getLocalThreads(const size_t globalThreads,
                           const size_t maxWorkItemSize)
{
    if(maxWorkItemSize < globalThreads)
    {
        if(globalThreads%maxWorkItemSize == 0)
            return maxWorkItemSize;
        else
        {
            for(size_t i=maxWorkItemSize-1; i > 0; --i)
            {
                if(globalThreads%i == 0)
                    return i;
            }
        }
    }
    else
    {
        return globalThreads;
    }

    return SDK_SUCCESS;
}

int SDKCommon::createTimer()
{
    Timer* newTimer = new Timer;
    newTimer->_start = 0;
    newTimer->_clocks = 0;

#ifdef _WIN32
    QueryPerformanceFrequency((LARGE_INTEGER*)&newTimer->_freq);
#else
    newTimer->_freq = (long long)1.0E3;
#endif
    
    /* Push back the address of new Timer instance created */
    _timers.push_back(newTimer);

    /*if(_numTimers == 1)
    {
        _timers = newTimer; 
    }
    else
    {
        Timer *save = _timers;

        _timers = new Timer[_numTimers];
        memcpy(_timers,save,sizeof(Timer)*(_numTimers-1));
        _timers[_numTimers-1] = *newTimer;
        delete newTimer;
        newTimer = 0;

        if(_numTimers <= 2 )
        {
            delete save; 
        }
        else
        {
            delete[] save;
        }
        save = 0;
    }*/

    return (int)(_timers.size() - 1);
}

int SDKCommon::resetTimer(int handle)
{
    if(handle >= (int)_timers.size())
    {
        error("Cannot reset timer. Invalid handle.");
        return -1;
    }

    (_timers[handle]->_start) = 0;
    (_timers[handle]->_clocks) = 0;
    return SDK_SUCCESS;
}

int SDKCommon::startTimer(int handle)
{
    if(handle >= (int)_timers.size())
    {
        error("Cannot reset timer. Invalid handle.");
        return SDK_FAILURE;
    }

#ifdef _WIN32
    QueryPerformanceCounter((LARGE_INTEGER*)&(_timers[handle]->_start));	
#else
    struct timeval s;
    gettimeofday(&s, 0);
    _timers[handle]->_start = (long long)s.tv_sec * (long long)1.0E3 + (long long)s.tv_usec / (long long)1.0E3;
#endif

    return SDK_SUCCESS;
}

int SDKCommon::stopTimer(int handle)
{
    long long n=0;

    if(handle >= (int)_timers.size())
    {
        error("Cannot reset timer. Invalid handle.");
        return SDK_FAILURE;
    }

#ifdef _WIN32
    QueryPerformanceCounter((LARGE_INTEGER*)&(n));	
#else
    struct timeval s;
    gettimeofday(&s, 0);
    n = (long long)s.tv_sec * (long long)1.0E3+ (long long)s.tv_usec / (long long)1.0E3;
#endif

    n -= _timers[handle]->_start;
    _timers[handle]->_start = 0;
    _timers[handle]->_clocks += n;

    return SDK_SUCCESS;
}

double SDKCommon::readTimer(int handle)
{
    if(handle >= (int)_timers.size())
    {
        error("Cannot read timer. Invalid handle.");
        return SDK_FAILURE;
    }

    double reading = double(_timers[handle]->_clocks);
    reading = double(reading / _timers[handle]->_freq);

    return reading;
}

void SDKCommon::printTable(Table *t)
{
    if(t == NULL)
    {
        error("Cannot print table, NULL pointer.");
        return;
    }

    int count = 0;
    // Skip delimiters at beginning.
    std::string::size_type curIndex = t->_dataItems.find_first_not_of(t->_delim, 0);
    // Find first "non-delimiter".
    std::string::size_type nextIndex = 
        t->_dataItems.find_first_of(t->_delim, curIndex);

    while (std::string::npos != nextIndex || std::string::npos != curIndex)
    {
        // Found a token, add it to the vector.
        // tokens.push_back(str.substr(curIndex, nextIndex - curIndex));
        std::cout<<std::setw(t->_columnWidth)<<std::left
                 <<t->_dataItems.substr(curIndex, nextIndex - curIndex);				 
        // Skip delimiters.  Note the "not_of"
        curIndex = t->_dataItems.find_first_not_of(t->_delim, nextIndex);
        // Find next "non-delimiter"
        nextIndex = t->_dataItems.find_first_of(t->_delim, curIndex);
        
        count++;

        if(count%t->_numColumns==0)
            std::cout<<"\n";
    }	
}

int 
SDKCommon::fileToString(std::string &fileName, std::string &str)
{
    size_t      size;
    char*       buf;

    // Open file stream
    std::fstream f(fileName.c_str(), (std::fstream::in | std::fstream::binary));

    // Check if we have opened file stream
    if (f.is_open()) 
    {
        size_t  sizeFile;

        // Find the stream size
        f.seekg(0, std::fstream::end);
        size = sizeFile = (size_t)f.tellg();
        f.seekg(0, std::fstream::beg);

        buf = new char[size + 1];
        if (!buf) {
            f.close();
            return  SDK_FAILURE;
        }

        // Read file
        f.read(buf, sizeFile);
        f.close();
        str[size] = '\0';

        str = buf;

        return SDK_SUCCESS;
    }
    else
    {
        error("Converting file to string. Cannot open file.");
        str = "";	
        return SDK_FAILURE;
    }
}

void 
SDKCommon::error(const char* errorMsg)
{
    std::cout<<"Error: "<<errorMsg<<std::endl;
}

void 
SDKCommon::error(std::string errorMsg)
{
    std::cout<<"Error: "<<errorMsg<<std::endl;
}

void 
SDKCommon::expectedError(const char* errorMsg)
{
    std::cout<<"Expected Error: "<<errorMsg<<std::endl;
}

void 
SDKCommon::expectedError(std::string errorMsg)
{
    std::cout<<"Expected Error: "<<errorMsg<<std::endl;
}

/////////////////////////////////////////////////////////////////
// Template Instantiations 
/////////////////////////////////////////////////////////////////
template 
void SDKCommon::printArray<short>(const std::string, 
        const short*, int, int)const;
template 
void SDKCommon::printArray<unsigned char>(const std::string, 
        const unsigned char *, int, int)const;
template 
void SDKCommon::printArray<unsigned int>(const std::string, 
        const unsigned int *, int, int)const;
template 
void SDKCommon::printArray<int>(const std::string, 
        const int *, int, int)const;
template 
void SDKCommon::printArray<long>(const std::string, 
        const long*, int, int)const;
template 
void SDKCommon::printArray<float>(const std::string, 
        const float*, int, int)const;
template 
void SDKCommon::printArray<double>(const std::string, 
        const double*, int, int)const;

template 
int SDKCommon::fillRandom<unsigned char>(unsigned char* arrayPtr, 
        const int width, const int height, 
        unsigned char rangeMin, unsigned char rangeMax, unsigned int seed);	
template 
int SDKCommon::fillRandom<unsigned int>(unsigned int* arrayPtr, 
        const int width, const int height, 
        unsigned int rangeMin, unsigned int rangeMax, unsigned int seed);	
template 
int SDKCommon::fillRandom<int>(int* arrayPtr, 
        const int width, const int height, 
        int rangeMin, int rangeMax, unsigned int seed);	
template 
int SDKCommon::fillRandom<long>(long* arrayPtr, 
        const int width, const int height, 
        long rangeMin, long rangeMax, unsigned int seed);	
template 
int SDKCommon::fillRandom<float>(float* arrayPtr, 
        const int width, const int height, 
        float rangeMin, float rangeMax, unsigned int seed);	
template 
int SDKCommon::fillRandom<double>(double* arrayPtr, 
        const int width, const int height, 
        double rangeMin, double rangeMax, unsigned int seed);	

template 
short SDKCommon::roundToPowerOf2<short>(short val);
template 
unsigned int SDKCommon::roundToPowerOf2<unsigned int>(unsigned int val);
template 
int SDKCommon::roundToPowerOf2<int>(int val);
template 
long SDKCommon::roundToPowerOf2<long>(long val);

template
int SDKCommon::isPowerOf2<short>(short val);
template
int SDKCommon::isPowerOf2<unsigned int>(unsigned int val);
template
int SDKCommon::isPowerOf2<int>(int val);
template
int SDKCommon::isPowerOf2<long>(long val);

template<> 
int SDKCommon::fillPos<short>(short * arrayPtr, const int width, const int height);
template<> 
int SDKCommon::fillPos<unsigned int>(unsigned int * arrayPtr, const int width, const int height);
template<> 
int SDKCommon::fillPos<int>(int * arrayPtr, const int width, const int height);
template<> 
int SDKCommon::fillPos<long>(long * arrayPtr, const int width, const int height);

template<> 
int SDKCommon::fillConstant<short>(short * arrayPtr, 
        const int width, const int height, 
        const short val);
template<> 
int SDKCommon::fillConstant(unsigned int * arrayPtr, 
        const int width, const int height, 
        const unsigned int val);
template<> 
int SDKCommon::fillConstant(int * arrayPtr, 
        const int width, const int height, 
        const int val);
template<> 
int SDKCommon::fillConstant(long * arrayPtr, 
        const int width, const int height, 
        const long val);
template<> 
int SDKCommon::fillConstant(long * arrayPtr, 
        const int width, const int height, 
        const long val);
template<> 
int SDKCommon::fillConstant(long * arrayPtr, 
        const int width, const int height, 
        const long val);


template
const char* getOpenCLErrorCodeStr<int>(int input);

template
int SDKCommon::checkVal<char>(char input, char reference, std::string message, bool isAPIerror);
template
int SDKCommon::checkVal<bool>(bool input, bool reference, std::string message, bool isAPIerror);
template
int SDKCommon::checkVal<std::string>(std::string input, std::string reference, std::string message, bool isAPIerror);
template
int SDKCommon::checkVal<short>(short input, short reference, std::string message, bool isAPIerror);
template
int SDKCommon::checkVal<unsigned int>(unsigned int  input, unsigned int  reference, std::string message, bool isAPIerror);
template
int SDKCommon::checkVal<int>(int input, int reference, std::string message, bool isAPIerror);
template
int SDKCommon::checkVal<long>(long input, long reference, std::string message, bool isAPIerror);


template
std::string SDKCommon::toString<char>(char t, std::ios_base &(*r)(std::ios_base&));
template
std::string SDKCommon::toString<short>(short t, std::ios_base &(*r)(std::ios_base&));
template
std::string SDKCommon::toString<unsigned int>(unsigned int t, std::ios_base &(*r)(std::ios_base&));
template
std::string SDKCommon::toString<int>(int t, std::ios_base &(*r)(std::ios_base&));
template
std::string SDKCommon::toString<long>(long t, std::ios_base &(*r)(std::ios_base&));
template
std::string SDKCommon::toString<float>(float t, std::ios_base &(*r)(std::ios_base&));
template
std::string SDKCommon::toString<double>(double t, std::ios_base &(*r)(std::ios_base&));
}


