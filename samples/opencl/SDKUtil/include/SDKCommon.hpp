#ifndef SDKCOMMON_HPP_
#define SDKCOMMON_HPP_


/**
 * Header Files
 */
#include "SDKFile.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <ctime>
#include <cmath>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <malloc.h>

#include <CL/opencl.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <linux/limits.h>
#endif

#if defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
#define _aligned_malloc __mingw_aligned_malloc 
#define _aligned_free  __mingw_aligned_free 
#endif // __MINGW32__  and __MINGW64_VERSION_MAJOR


#ifndef _WIN32
#if defined(__INTEL_COMPILER)
#pragma warning(disable : 1125)
#endif
#endif


/**
 * Macro Definitions
 * SDK_SUCCESS
 * SDK_FAILURE
 * SDK_EXPECTED_FAILURE
 * CHECK_ALLOCATION
 * CHECK_ERROR
 * CHECK_OPRNCL_ERROR
 * OPENCL_EXPECTED_ERROR
 * CHECK_OPENVIDEO_ERROR
 * OPENVIDEO_EXPECTED_ERROR
 */
#define SDK_SUCCESS 0
#define SDK_FAILURE 1
#define SDK_EXPECTED_FAILURE 2

#define SDK_VERSION_MAJOR 2
#define SDK_VERSION_MINOR 7
#define SDK_VERSION_BUILD 5
#define SDK_VERSION_REVISION 8

#define CHECK_ALLOCATION(actual, msg) \
        if(actual == NULL) \
        { \
			streamsdk::SDKCommon::error(msg); \
            std::cout << "Location : " << __FILE__ << ":" << __LINE__<< std::endl; \
            return SDK_FAILURE; \
        }


#define CHECK_ERROR(actual, reference, msg) \
        if(actual != reference) \
        { \
		streamsdk::SDKCommon::error(msg); \
            std::cout << "Location : " << __FILE__ << ":" << __LINE__<< std::endl; \
            return SDK_FAILURE; \
        }

#define CHECK_OPENCL_ERROR(actual, msg) \
	if(streamsdk::SDKCommon::checkVal(actual, CL_SUCCESS, msg)) \
        { \
            std::cout << "Location : " << __FILE__ << ":" << __LINE__<< std::endl; \
            return SDK_FAILURE; \
        } 

#define OPENCL_EXPECTED_ERROR(msg) \
        { \
		streamsdk::SDKCommon::expectedError(msg); \
            return SDK_EXPECTED_FAILURE; \
        }

#define CHECK_OPENVIDEO_ERROR(actual, msg) \
	if(streamsdk::SDKCommon::checkVal(actual, CL_SUCCESS, msg)) \
        { \
            std::cout << "Location : " << __FILE__ << ":" << __LINE__<< std::endl; \
            return SDK_FAILURE; \
        } 

#define OPENVIDEO_EXPECTED_ERROR(msg) \
        { \
		streamsdk::SDKCommon::expectedError(msg); \
            return SDK_EXPECTED_FAILURE; \
        }

#define FREE(ptr) \
	{ \
    if(ptr != NULL) \
        { \
		    free(ptr); \
		    ptr = NULL; \
        } \
	} 

#ifdef _WIN32
#define ALIGNED_FREE(ptr) \
    { \
        if(ptr != NULL) \
        { \
            _aligned_free(ptr); \
            ptr = NULL; \
        } \
    }
#endif



/**
 * namespace streamsdk
 */
namespace streamsdk
{
	/**
	 * getOpencLErrorCodeStr
	 * global function to get corrosponding string for a error code
	 * @param input Templated the error code 
	 * @return const char* the string for the error code
	 */
	const char* getOpenCLErrorCodeStr(std::string input);

	template<typename T>
	const char* getOpenCLErrorCodeStr(T input);

	/**
	 * sdkVersionStr
	 * struct to form AMD APP SDK version
	 */
	struct sdkVersionStr
	{
		int major;		/**< SDK major release number */
		int minor;		/**< SDK minor release number */
		int build;		/**< SDK build number */
		int revision;	/**< SDK revision number */

		/**
		 * Constructor
		 */
		sdkVersionStr()
		{
			major = SDK_VERSION_MAJOR;
            minor = SDK_VERSION_MINOR;
            build = SDK_VERSION_BUILD;
            revision = SDK_VERSION_REVISION;
		}
	};




	/**
	 * bifData
	 * struct to generate/load binary functionality
	 */
	struct bifData
	{
		std::string kernelName;			/**< kernelName name of the kernel */
		std::string flagsFileName;		/**< flagFileName flags file for the kernel */
		std::string flagsStr;			/**< flagsStr flags string */
		std::string binaryName;			/**< binaryName name of the binary */

		/**
		 * Constructor
		 */
		bifData()
		{
			kernelName = std::string("");
			flagsFileName = std::string("");
			flagsStr = std::string("");
			binaryName = std::string("");
		}
	};

	/**
	 * buildProgramData
	 * struct to build the kernel
	 */
	struct buildProgramData
	{
		std::string kernelName;				/**< kernelName name of the kernel */
		std::string flagsFileName;			/**< flagFileName name of the file of flags */
		std::string flagsStr;				/**< flagsStr flags string */
		std::string binaryName;				/**< binaryName name of the binary */
		cl_device_id* devices;				/**< devices array of device to build kernel for */
		int deviceId;						/**< deviceId Id of the device to buid kernel for */

		/**
		 * Constructor
		 */
		buildProgramData()
		{
			kernelName = std::string("");
			flagsFileName = std::string("");
			flagsStr = std::string("");
			binaryName = std::string("");
		}
	};

	/**
	 * Timer
	 * struct to handle time measuring functionality
	 */
	struct Timer
	{
		std::string name;   /**< name name of time object*/
		long long _freq;	/**< _freq frequency*/
		long long _clocks;	/**< _clocks number of ticks at end*/
		long long _start;	/**< _start start point ticks*/
	};

	/**
	 * Table
	 * struct to create a table
	 */
	struct Table
	{
		int _numColumns;		/**< _numColumns  number of columns */
		int _numRows;			/**< _numRows  number of rows */
		int _columnWidth;		/**< _columnWidth  width of columns */
		std::string _delim;		/**< _delim  delimiter */
		std::string _dataItems; /**< _dataItems  string of data item */
	};

	/**
	 * KernelWorkGroupInfo
	 * class implements the functionality to query 
	 * various Kernel Work Group related parameters
	 */
	class KernelWorkGroupInfo
	{
	public:
		cl_ulong localMemoryUsed;			/**< localMemoryUsed amount of local memory used by kernel */
		size_t kernelWorkGroupSize;			/**< kernelWorkGroupSize Supported WorkGroup size as per OpenCL Runtime*/
		size_t compileWorkGroupSize[3];		/**< compileWorkGroupSize WorkGroup size as mentioned in kernel source */

		/**
		 * Constructor
		 */
		KernelWorkGroupInfo():
			localMemoryUsed(0),
			kernelWorkGroupSize(0)
			{				
				compileWorkGroupSize[0] = 0;
				compileWorkGroupSize[1] = 0;
				compileWorkGroupSize[2] = 0;
			}

		/**
		 * setKernelWorkGroupInfo
		 * Set all information for a given device id
		 * @param kernel kernel object
		 * @param deviceId deviceID of the kernel object
		 * @return 0 if success else nonzero
		 */
		int setKernelWorkGroupInfo(cl_kernel &kernel,cl_device_id &deviceId);
	private :

		/**
		 * checkVal
		 * Templated FunctionCheck whether any error occured
		 * @param input templated input
		 * @param reference templated input
		 * @param message string message
		 * @param isAPIerror bool optional variable
		 * @return 0 if success, else nonzero
		 */
		template<typename T>
		int checkVal(T input, T reference, std::string message, bool isAPIerror = true) const;
	};

	/**
	 * SDKDeviceInfo
	 * class implements the functionality to query 
	 * various Device related parameters
	 */
	class SDKDeviceInfo
	{
	public :
		cl_device_type dType;				/**< dType device type*/
		cl_uint venderId;					/**< vendorId VendorId of device*/
		cl_uint maxComputeUnits;			/**< maxComputeUnits maxComputeUnits of device*/
		cl_uint maxWorkItemDims;			/**< maxWorkItemDims maxWorkItemDimensions VendorId of device*/
		size_t* maxWorkItemSizes;			/**< maxWorkItemSizes maxWorkItemSizes of device*/
		size_t maxWorkGroupSize;			/**< maxWorkGroupSize max WorkGroup Size of device*/
		cl_uint preferredCharVecWidth;		/**< preferredCharVecWidth preferred Char VecWidth of device*/
		cl_uint preferredShortVecWidth;		/**< preferredShortVecWidth preferred Short VecWidth of device*/
		cl_uint preferredIntVecWidth;		/**< preferredIntVecWidth preferred Int VecWidth of device*/
		cl_uint preferredLongVecWidth;		/**< preferredLongVecWidth preferred Long VecWidth of device*/
		cl_uint preferredFloatVecWidth;		/**< preferredFloatVecWidth preferredFloatVecWidth of device*/
		cl_uint preferredDoubleVecWidth;	/**< preferredDoubleVecWidth preferred Double VecWidth of device*/
		cl_uint preferredHalfVecWidth;		/**< preferredHalfVecWidth preferred Half VecWidth of device*/
		cl_uint nativeCharVecWidth;		 	/**< nativeCharVecWidth native Char VecWidth of device*/
		cl_uint nativeShortVecWidth;		/**< nativeShortVecWidth nativeShortVecWidth of device*/
		cl_uint nativeIntVecWidth;			/**< nativeIntVecWidth nativeIntVecWidth of device*/
		cl_uint nativeLongVecWidth;			/**< nativeLongVecWidth nativeLongVecWidth of device*/
		cl_uint nativeFloatVecWidth;		/**< nativeFloatVecWidth native Float VecWidth of device*/
		cl_uint nativeDoubleVecWidth;		/**< nativeDoubleVecWidth native Double VecWidth of device*/
		cl_uint nativeHalfVecWidth;			/**< nativeHalfVecWidth native Half VecWidth of device*/
		cl_uint maxClockFrequency;			/**< maxClockFrequency max Clock Frequency of device*/
		cl_uint addressBits;				/**< addressBits address Bits of device*/
		cl_ulong maxMemAllocSize;			/**< maxMemAllocSize max Mem Alloc Size of device*/
		cl_bool imageSupport;				/**< imageSupport image Support of device*/
		cl_uint maxReadImageArgs;			/**< maxReadImageArgs max ReadImage Args of device*/
		cl_uint maxWriteImageArgs;			/**< maxWriteImageArgs max Write Image Args of device*/
		size_t image2dMaxWidth;				/**< image2dMaxWidth image 2dMax Width of device*/
		size_t image2dMaxHeight;			/**< image2dMaxHeight image 2dMax Height of device*/
		size_t image3dMaxWidth;				/**< image3dMaxWidth image3d MaxWidth of device*/ 
		size_t image3dMaxHeight;			/**< image3dMaxHeight image 3dMax Height of device*/
		size_t image3dMaxDepth;				/**< image3dMaxDepth image 3dMax Depth of device*/
		size_t maxSamplers;					/**< maxSamplers maxSamplers of device*/
		size_t maxParameterSize;			/**< maxParameterSize maxParameterSize of device*/
		cl_uint memBaseAddressAlign;		/**< memBaseAddressAlign memBase AddressAlign of device*/
		cl_uint minDataTypeAlignSize;		/**< minDataTypeAlignSize minDataType AlignSize of device*/
		cl_device_fp_config singleFpConfig;	/**< singleFpConfig singleFpConfig of device*/
		cl_device_fp_config doubleFpConfig; /**< doubleFpConfig doubleFpConfig of device*/
		cl_device_mem_cache_type globleMemCacheType; /**< globleMemCacheType globleMem CacheType of device*/
		cl_uint globalMemCachelineSize;		/**< globalMemCachelineSize globalMem Cacheline Size of device*/
		cl_ulong globalMemCacheSize;		/**< globalMemCacheSize globalMem CacheSize of device*/
		cl_ulong globalMemSize;				/**< globalMemSize globalMem Size of device*/
		cl_ulong maxConstBufSize;			/**< maxConstBufSize maxConst BufSize of device*/
		cl_uint maxConstArgs;				/**< maxConstArgs max ConstArgs of device*/
		cl_device_local_mem_type localMemType;/**< localMemType local MemType of device*/
		cl_ulong localMemSize;				/**< localMemSize localMem Size of device*/
		cl_bool errCorrectionSupport;		/**< errCorrectionSupport errCorrectionSupport of device*/
		cl_bool hostUnifiedMem;				/**< hostUnifiedMem hostUnifiedMem of device*/
		size_t timerResolution;				/**< timerResolution timerResolution of device*/
		cl_bool endianLittle;				/**< endianLittle endian Little of device*/
		cl_bool available;					/**< available available of device*/
		cl_bool compilerAvailable;			/**< compilerAvailable compilerAvailable of device*/
		cl_device_exec_capabilities execCapabilities;/**< execCapabilities exec Capabilities of device*/
		cl_command_queue_properties queueProperties;/**< queueProperties queueProperties of device*/
		cl_platform_id platform;			/**< platform platform of device*/
		char* name;							/**< name name of device*/
		char* vendorName;					/**< venderName vender Name of device*/
		char* driverVersion;				/**< driverVersion driver Version of device*/
		char* profileType;					/**< profileType profile Type of device*/
		char* deviceVersion;				/**< deviceVersion device Version of device*/
		char* openclCVersion;				/**< openclCVersion opencl C Version of device*/
		char* extensions;					/**< extensions extensions of device*/

		/**
		 * Constructor
		 */
		SDKDeviceInfo()
		{
			dType = CL_DEVICE_TYPE_GPU;
			venderId = 0;
			maxComputeUnits = 0;
			maxWorkItemDims = 0;
			maxWorkItemSizes = NULL;
			maxWorkGroupSize = 0;
			preferredCharVecWidth = 0;
			preferredShortVecWidth = 0;
			preferredIntVecWidth = 0;
			preferredLongVecWidth = 0;
			preferredFloatVecWidth = 0;
			preferredDoubleVecWidth = 0;
			preferredHalfVecWidth = 0;
			nativeCharVecWidth = 0;
			nativeShortVecWidth = 0;
			nativeIntVecWidth = 0;
			nativeLongVecWidth = 0;
			nativeFloatVecWidth = 0;
			nativeDoubleVecWidth = 0;
			nativeHalfVecWidth = 0;
			maxClockFrequency = 0;
			addressBits = 0;
			maxMemAllocSize = 0;
			imageSupport = CL_FALSE;
			maxReadImageArgs = 0;
			maxWriteImageArgs = 0;
			image2dMaxWidth = 0;
			image2dMaxHeight = 0;
			image3dMaxWidth = 0;
			image3dMaxHeight = 0;
			image3dMaxDepth = 0;
			maxSamplers = 0;
			maxParameterSize = 0;
			memBaseAddressAlign = 0;
			minDataTypeAlignSize = 0;
			singleFpConfig = CL_FP_ROUND_TO_NEAREST | CL_FP_INF_NAN;
			doubleFpConfig = CL_FP_FMA |
							 CL_FP_ROUND_TO_NEAREST |
							 CL_FP_ROUND_TO_ZERO |
							 CL_FP_ROUND_TO_INF |
							 CL_FP_INF_NAN |
							 CL_FP_DENORM;
			globleMemCacheType = CL_NONE;
			globalMemCachelineSize = CL_NONE;
			globalMemCacheSize = 0;
			globalMemSize = 0;
			maxConstBufSize = 0;
			maxConstArgs = 0;
			localMemType = CL_LOCAL;
			localMemSize = 0;
			errCorrectionSupport = CL_FALSE;
			hostUnifiedMem = CL_FALSE;
			timerResolution = 0;
			endianLittle = CL_FALSE;
			available = CL_FALSE;
			compilerAvailable = CL_FALSE;
			execCapabilities = CL_EXEC_KERNEL;
			queueProperties = 0;
			platform = 0;
			name = NULL;
			vendorName = NULL;
			driverVersion = NULL;
			profileType = NULL;
			deviceVersion = NULL;
			openclCVersion = NULL;
			extensions = NULL;
		};

		/**
		 * Destructor
		 */
		~SDKDeviceInfo()
		{
			delete maxWorkItemSizes;
			delete name;
			delete vendorName;
			delete driverVersion;
			delete profileType;
			delete deviceVersion;
			delete openclCVersion;
			delete extensions;
		};

		/**
		 * setKernelWorkGroupInfo
		 * Set all information for a given device id
		 * @param deviceId deviceID 
		 * @return 0 if success else nonzero
		 */
		int setDeviceInfo(cl_device_id deviceId);
	private :

		/**
		 * checkVal
		 * Templated FunctionCheck whether any error occured
		 */
		template<typename T>
		int checkVal(T input, T reference, std::string message, bool isAPIerror = true) const;
	};

	/**
	* SDKCommonCpp_Cl
	* class implemts  the builid programm step for Cpp_Cl samples
	*/
	//class SDKComonCpp_Cl
	//{
	//private:
	//	cl::Program program;                    /**< program object */
	//	cl::Context context;                    /**< Context */

	//public:
	//	/**
	//	 * buildOpenCLProgram of cpp_cl
	//	 * builds the opencl program
	//	 * @param program program object
	//	 * @param context cl_context object
	//	 * @param buildData buildProgramData Object
	//	 * @return SDK_SUCCESS if success else nonzero
	//	 */
	//	int buildOpenCLProgram(cl::Program &program, const cl::Context &context, const buildProgramData &buildData);

	//};

	/** 
	 * SDKCommon
	 * class implements the common steps which are done
	 * for most(if not all) samples
	 */
	class SDKCommon
	{
	private:
		//Timing 
		//Timer *_timers;
		//int _numTimers;
		std::vector<Timer*> _timers;      /**< _timers vector to Timer objects */
	    
	public:

		/**
		 * Constructor
		 */
		SDKCommon();
		
		/**
		 * Destructor
		 */
		~SDKCommon();
		
		/**
		 * getPath
		 * @return path of the current directory
		 */
		std::string getPath();

		/**
		 * error
		 * constant function, Prints error messages 
		 * @param errorMsg char* message
		 */
		static void error(const char* errorMsg);	
		
		/**
		 * error
		 * constant function, Prints error messages 
		 * @param errorMsg std::string message
		 */
		static void error(std::string errorMsg);

		/**
		 * expectedError
		 * constant function, Prints error messages 
		 * @param errorMsg char* message
		 */
		static void expectedError(const char* errorMsg);	
		
		/**
		 * expectedError
		 * constant function, Prints error messages 
		 * @param errorMsg string message
		 */
		static void expectedError(std::string errorMsg);
		
		/**
		 * filetoString
		 * converts the kernel file into a string 
		 * @param file string message
		 * @param str string message
		 * @return 0 on success Positive if expected and Non-zero on failure
		 */
		int fileToString(std::string &file, std::string &str);
		
		/**
		 * compare template version
		 * compare data to check error
		 * @param refData templated input
		 * @param data templated input
		 * @param length number of values to compare
		 * @param epsilon errorWindow
		 */
		bool compare(const float *refData, const float *data, 
						const int length, const float epsilon = 1e-6f); 
		bool compare(const double *refData, const double *data, 
						const int length, const double epsilon = 1e-6); 

		/**
		 * display devices
		 * displays the devices in a platform
		 * @param platform cl_platform_id
		 * @param deviceType deviceType
		 * @return 0 if success else nonzero
		 */
		int displayDevices(cl_platform_id platform, cl_device_type deviceType);

		/**
		 * displayPlatformAnddevices
		 * displays the devices in a platform
		 * @param platform cl_platform_id
		 * @param device array ofdeviceId
		 * @param deviceCount number of devices 
		 * @return 0 if success else nonzero
		 */
		int displayPlatformAndDevices(cl_platform_id platform, const cl_device_id* device, const int deviceCount);

		/**
		 * validateDeviceId
		 * validates whether intended device is used
		 * @param deviceId cl_device_id
		 * @param deviceCount device number
		 * @return 0 if success else nonzero
		 */
		int validateDeviceId(int deviceId, int deviceCount);

		/**
		 * generateBinaryImage
		 * geenrate Binary for a kernel
		 * @param binaryData bifdata object
		 * @return 0 if success else nonzero
		 */
		int generateBinaryImage(const bifData &binaryData);

		/**
		 * getPlatform
		 * selects intended platform
		 * @param platform cl_platform_id
		 * @param platformId platform Number
		 * @param platformIdEnabled if Platform option used
		 * @return 0 if success else nonzero
		 */
		int getPlatform(cl_platform_id &platform, int platformId, bool platformIdEnabled);

		/**
		 * getDevices
		 * selects intended device
		 * @param context cl_context object
		 * @param devices cl_device_id pointer to hold array of deviceIds
		 * @param deviceId device Number
		 * @param deviceIdEnabled if DeviceId option used
		 * @return 0 if success else nonzero
		 */
		int getDevices(cl_context &context, cl_device_id **devices, int deviceId, bool deviceIdEnabled);

		/**
		 * buildOpenCLProgram
		 * builds the opencl program
		 * @param program program object
		 * @param context cl_context object
		 * @param buildData buildProgramData Object
		 * @return 0 if success else nonzero
		 */
		int buildOpenCLProgram(cl_program &program, const cl_context& context, const buildProgramData &buildData);

		/**
		 * waitForEventAndRelease
		 * waits for a event to complete and release the event afterwards
		 * @param event cl_event object
		 * @return 0 if success else nonzero
		 */
		int waitForEventAndRelease(cl_event *event);

		/**
		 * printArray
		 * displays a array on std::out
		 */
		template<typename T> 
		void printArray(
				 const std::string header,
				 const T * data, 
				 const int width,
				 const int height) const;

		
		/**
		 * fillRandom
		 * fill array with random values
		 */
		template<typename T> 
		int fillRandom(
				 T * arrayPtr, 
				 const int width,
				 const int height,
				 const T rangeMin,
				 const T rangeMax,
				 unsigned int seed=123);	
	    
		/**
		 * fillPos
		 * fill the specified positions
		 */
		template<typename T> 
		int fillPos(
				 T * arrayPtr, 
				 const int width,
				 const int height);
	    
		/**
		 * fillConstant
		 * fill the array with constant value
		 */
		template<typename T> 
		int fillConstant(
				 T * arrayPtr, 
				 const int width,
				 const int height,
				 const T val);

	    
		/**
		 * roundToPowerOf2
		 * rounds to a power of 2
		 */
		template<typename T>
		T roundToPowerOf2(T val);

		/**
		 * isPowerOf2
		 * checks if input is a power of 2
		 */
		template<typename T>
		int isPowerOf2(T val);
	    
		/**
		 * checkVal
		 * Set default(isAPIerror) parameter to false 
		 * if checkVaul is used to check otherthan OpenCL API error code 
		 */
		template<typename T> 
		static int checkVal(
			T input, 
			T reference, 
			std::string message, bool isAPIerror = true);

		/**
		 * toString
		 * convert a T type to string
		 */
		template<typename T>
		std::string toString(T t, std::ios_base & (*r)(std::ios_base&)); 

		
		/**
		 * getLocalThreads
		 * get Local Threads number
		 */
		size_t getLocalThreads(size_t globalThreads, size_t maxWorkitemSize);

		/**
		 * Timer Functions
		 * CreateTimer 
		 * resetTimer
		 * startTimer
		 * stopTimer
		 * readTimer
		 */
		int createTimer();
		int resetTimer(int handle);
		int startTimer(int handle);
		int stopTimer(int handle);
		double readTimer(int handle);

		/**
		 * printTable
		 * displays a table of input/output 
		 * @param t Table type object
		 */
		void printTable(Table* t);
	}; 

} // namespace amd

#endif
