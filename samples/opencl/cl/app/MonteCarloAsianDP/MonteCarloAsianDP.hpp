/* ============================================================

Copyright (c) 2009 Advanced Micro Devices, Inc.  All rights reserved.
 
Redistribution and use of this material is permitted under the following 
conditions:
 
Redistributions must retain the above copyright notice and all terms of this 
license.
 
In no event shall anyone redistributing or accessing or using this material 
commence or participate in any arbitration or legal action relating to this 
material against Advanced Micro Devices, Inc. or any copyright holders or 
contributors. The foregoing shall survive any expiration or termination of 
this license or any agreement or access or use related to this material. 

ANY BREACH OF ANY TERM OF THIS LICENSE SHALL RESULT IN THE IMMEDIATE REVOCATION 
OF ALL RIGHTS TO REDISTRIBUTE, ACCESS OR USE THIS MATERIAL.

THIS MATERIAL IS PROVIDED BY ADVANCED MICRO DEVICES, INC. AND ANY COPYRIGHT 
HOLDERS AND CONTRIBUTORS "AS IS" IN ITS CURRENT CONDITION AND WITHOUT ANY 
REPRESENTATIONS, GUARANTEE, OR WARRANTY OF ANY KIND OR IN ANY WAY RELATED TO 
SUPPORT, INDEMNITY, ERROR FREE OR UNINTERRUPTED OPERA TION, OR THAT IT IS FREE 
FROM DEFECTS OR VIRUSES.  ALL OBLIGATIONS ARE HEREBY DISCLAIMED - WHETHER 
EXPRESS, IMPLIED, OR STATUTORY - INCLUDING, BUT NOT LIMITED TO, ANY IMPLIED 
WARRANTIES OF TITLE, MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
ACCURACY, COMPLETENESS, OPERABILITY, QUALITY OF SERVICE, OR NON-INFRINGEMENT. 
IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. OR ANY COPYRIGHT HOLDERS OR 
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, PUNITIVE,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, REVENUE, DATA, OR PROFITS; OR 
BUSINESS INTERRUPTION) HOWEVER CAUSED OR BASED ON ANY THEORY OF LIABILITY 
ARISING IN ANY WAY RELATED TO THIS MATERIAL, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE. THE ENTIRE AND AGGREGATE LIABILITY OF ADVANCED MICRO DEVICES, 
INC. AND ANY COPYRIGHT HOLDERS AND CONTRIBUTORS SHALL NOT EXCEED TEN DOLLARS 
(US $10.00). ANYONE REDISTRIBUTING OR ACCESSING OR USING THIS MATERIAL ACCEPTS 
THIS ALLOCATION OF RISK AND AGREES TO RELEASE ADVANCED MICRO DEVICES, INC. AND 
ANY COPYRIGHT HOLDERS AND CONTRIBUTORS FROM ANY AND ALL LIABILITIES, 
OBLIGATIONS, CLAIMS, OR DEMANDS IN EXCESS OF TEN DOLLARS (US $10.00). THE 
FOREGOING ARE ESSENTIAL TERMS OF THIS LICENSE AND, IF ANY OF THESE TERMS ARE 
CONSTRUED AS UNENFORCEABLE, FAIL IN ESSENTIAL PURPOSE, OR BECOME VOID OR 
DETRIMENTAL TO ADVANCED MICRO DEVICES, INC. OR ANY COPYRIGHT HOLDERS OR 
CONTRIBUTORS FOR ANY REASON, THEN ALL RIGHTS TO REDISTRIBUTE, ACCESS OR USE 
THIS MATERIAL SHALL TERMINATE IMMEDIATELY. MOREOVER, THE FOREGOING SHALL 
SURVIVE ANY EXPIRATION OR TERMINATION OF THIS LICENSE OR ANY AGREEMENT OR 
ACCESS OR USE RELATED TO THIS MATERIAL.

NOTICE IS HEREBY PROVIDED, AND BY REDISTRIBUTING OR ACCESSING OR USING THIS 
MATERIAL SUCH NOTICE IS ACKNOWLEDGED, THAT THIS MATERIAL MAY BE SUBJECT TO 
RESTRICTIONS UNDER THE LAWS AND REGULATIONS OF THE UNITED STATES OR OTHER 
COUNTRIES, WHICH INCLUDE BUT ARE NOT LIMITED TO, U.S. EXPORT CONTROL LAWS SUCH 
AS THE EXPORT ADMINISTRATION REGULATIONS AND NATIONAL SECURITY CONTROLS AS 
DEFINED THEREUNDER, AS WELL AS STATE DEPARTMENT CONTROLS UNDER THE U.S. 
MUNITIONS LIST. THIS MATERIAL MAY NOT BE USED, RELEASED, TRANSFERRED, IMPORTED,
EXPORTED AND/OR RE-EXPORTED IN ANY MANNER PROHIBITED UNDER ANY APPLICABLE LAWS, 
INCLUDING U.S. EXPORT CONTROL LAWS REGARDING SPECIFICALLY DESIGNATED PERSONS, 
COUNTRIES AND NATIONALS OF COUNTRIES SUBJECT TO NATIONAL SECURITY CONTROLS. 
MOREOVER, THE FOREGOING SHALL SURVIVE ANY EXPIRATION OR TERMINATION OF ANY 
LICENSE OR AGREEMENT OR ACCESS OR USE RELATED TO THIS MATERIAL.

NOTICE REGARDING THE U.S. GOVERNMENT AND DOD AGENCIES: This material is 
provided with "RESTRICTED RIGHTS" and/or "LIMITED RIGHTS" as applicable to 
computer software and technical data, respectively. Use, duplication, 
distribution or disclosure by the U.S. Government and/or DOD agencies is 
subject to the full extent of restrictions in all applicable regulations, 
including those found at FAR52.227 and DFARS252.227 et seq. and any successor 
regulations thereof. Use of this material by the U.S. Government and/or DOD 
agencies is acknowledgment of the proprietary rights of any copyright holders 
and contributors, including those of Advanced Micro Devices, Inc., as well as 
the provisions of FAR52.227-14 through 23 regarding privately developed and/or 
commercial computer software.

This license forms the entire agreement regarding the subject matter hereof and 
supersedes all proposals and prior discussions and writings between the parties 
with respect thereto. This license does not affect any ownership, rights, title,
or interest in, or relating to, this material. No terms of this license can be 
modified or waived, and no breach of this license can be excused, unless done 
so in a writing signed by all affected parties. Each term of this license is 
separately enforceable. If any term of this license is determined to be or 
becomes unenforceable or illegal, such term shall be reformed to the minimum 
extent necessary in order for this license to remain in effect in accordance 
with its terms as modified by such reformation. This license shall be governed 
by and construed in accordance with the laws of the State of Texas without 
regard to rules on conflicts of law of any state or jurisdiction or the United 
Nations Convention on the International Sale of Goods. All disputes arising out 
of this license shall be subject to the jurisdiction of the federal and state 
courts in Austin, Texas, and all defenses are hereby waived concerning personal 
jurisdiction and venue of these courts.

============================================================ */


#ifndef MONTECARLOASIANDP_H_
#define MONTECARLOASIANDP_H_

#define GROUP_SIZE 256

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <SDKCommon.hpp>
#include <SDKApplication.hpp>
#include <SDKFile.hpp>

/**
 * MonteCarloAsian 
 * Class implements OpenCL  Monte Carlo Simution sample for Asian Option pricing using double
 * Derived from SDKSample base class
 */

class MonteCarloAsianDP : public SDKSample
{

    cl_int steps;                       /**< Steps for Asian Monte Carlo simution */
    cl_double initPrice;                 /**< Initial price */
    cl_double strikePrice;               /**< Stirke price */
    cl_double interest;                  /**< Interest rate */
    cl_double maturity;                  /**< maturity */

    cl_int noOfSum;                     /**< Number of excersize points */
    cl_int noOfTraj;                    /**< Number of samples */

    cl_double setupTime;                /**< time taken to setup OpenCL resources and building kernel */
    cl_double kernelTime;               /**< time taken to run kernel and read result back */

    cl_double *sigma;                    /**< Array of sigma values */
    cl_double *price;                    /**< Array of price values */
    cl_double *vega;                     /**< Array of vega values */

    cl_double *refPrice;                 /**< Array of reference price values */
    cl_double *refVega;                  /**< Array of reference vega values */

    cl_uint *randNum;                   /**< Array of random numbers */

    cl_double *priceVals;                /**< Array of price values for given samples */
    cl_double *priceDeriv;               /**< Array of price derivative values for given samples */

    cl_context context;                 /**< CL context */
    cl_device_id *devices;              /**< CL device list */

    cl_mem priceBuf;                    /**< CL memory buffer for sigma */
    cl_mem priceDerivBuf;               /**< CL memory buffer for price */
    cl_mem randBuf;                     /**< CL memroy buffer for random number */
    cl_command_queue commandQueue;      /**< CL command queue */
    cl_program program;                 /**< CL program  */
    cl_kernel kernel;                   /**< CL kernel */

    cl_int width;
    cl_int height;

    //size_t kernelWorkGroupSize;         /**< Group size returned by kernel */
    size_t blockSizeX;                  /**< Group-size in x-direction */ 
    size_t blockSizeY;                  /**< Group-size in y-direction */

    int iterations;                     /**< Number of iterations for kernel execution */

    cl_mem priceBufAsync;                    /**< CL memory buffer for sigma */
    cl_mem priceDerivBufAsync;               /**< CL memory buffer for price */
    cl_mem randBufAsync;                     /**< CL memroy buffer for random number */
    streamsdk::SDKDeviceInfo deviceInfo;     /**<Structure to store device related info */
    streamsdk::KernelWorkGroupInfo kernelInfo;/**< Structure to store kernel related info */
public:
    /** 
     * Constructor 
     * Initialize member variables
     * @param name name of sample (string)
     */
    MonteCarloAsianDP(std::string name)
        : SDKSample(name)
    {
        steps = 10;
        initPrice = 50.0;
        strikePrice = 55.0;
        interest = 0.06;
        maturity = 1.0;

        setupTime = 0;
        kernelTime = 0;
        blockSizeX = GROUP_SIZE;
        blockSizeY = 1;

        sigma = NULL;
        price = NULL;
        vega = NULL;
        refPrice = NULL;
        refVega = NULL;
        randNum = NULL;
        priceVals = NULL;
        priceDeriv = NULL;
        devices = NULL;
        iterations = 1;
    }

    /** 
     * Constructor 
     * Initialize member variables
     * @param name name of sample (const char*)
     */
    MonteCarloAsianDP(const char* name)
        : SDKSample(name)	
    {
        steps = 10;
        initPrice = 50.0;
        strikePrice = 55.0;
        interest = 0.06;
        maturity = 1.0;

        setupTime = 0;
        kernelTime = 0;
        blockSizeX = GROUP_SIZE;
        blockSizeY = 1;

        sigma = NULL;
        price = NULL;
        vega = NULL;
        refPrice = NULL;
        refVega = NULL;
        randNum = NULL;
        priceVals = NULL;
        priceDeriv = NULL;
        devices = NULL;

        iterations = 1;

    }

    /**
     * Destructor
     */
    ~MonteCarloAsianDP()
    {

            FREE(sigma);
            FREE(price);
            FREE(vega);
            FREE(refPrice);
            FREE(refVega);
        #ifdef _WIN32
            ALIGNED_FREE(randNum);
        #else
            FREE(randNum);
        #endif
            FREE(priceVals);
            FREE(priceDeriv);
            FREE(devices);



    }


    /**
     * Allocate and initialize host memory with appropriate values
     * @return SDK_SUCCESS on success and SDK_FAILURE on failure
     */
    int setupMonteCarloAsianDP();


    /**
     * Override from SDKSample, Generate binary image of given kernel 
     * and exit application
     * @return SDK_SUCCESS on success and SDK_FAILURE on failure
     */
    int genBinaryImage();

    /**
     * OpenCL related initialisations. 
     * Set up Context, Device list, Command Queue, Memory buffers
     * Build CL kernel program executable
     * @return SDK_SUCCESS on success and SDK_FAILURE on failure
     */
    int setupCL();

    /**
     * Set values for kernels' arguments, enqueue calls to the kernels
     * on to the command queue, wait till end of kernel execution.
     * Get kernel start and end time if timing is enabled
     * @return SDK_SUCCESS on success and SDK_FAILURE on failure
     */
    int runCLKernels();

    /**
     * Override from SDKSample. Print sample stats.
     */
    void printStats();

    /**
     * Override from SDKSample. Initialize 
     * command line parser, add custom options
     * @return SDK_SUCCESS on success and SDK_FAILURE on failure
     */
    int initialize();

    /**
     * Override from SDKSample, adjust width and height 
     * of execution domain, perform all sample setup
     * @return SDK_SUCCESS on success and SDK_FAILURE on failure
     */
    int setup();

    /**
     * Override from SDKSample
     * Run OpenCL Bitonic Sort
     * @return SDK_SUCCESS on success and SDK_FAILURE on failure
     */
    int run();

    /**
     * Override from SDKSample
    * Cleanup memory allocations
     * @return SDK_SUCCESS on success and SDK_FAILURE on failure
     */
    int cleanup();

    /**
     * Override from SDKSample
     * Verify against reference implementation
     * @return SDK_SUCCESS on success and SDK_FAILURE on failure
     */
    int verifyResults();

    private:

    /**
     * @brief Left shift
     * @param input input to be shifted
     * @param shift shifting count
     * @param output result after shifting input
     */
    void lshift128(unsigned int* input, unsigned int shift, unsigned int* output);

    /**
     * @brief Right shift
     * @param input input to be shifted
     * @param shift shifting count
     * @param output result after shifting input
     */
    void rshift128(unsigned int* input, unsigned int shift, unsigned int* output);


    /**
     * @brief Generates gaussian random numbers by using 
     *        Mersenenne Twister algo and box muller transformation
     * @param seedArray  seed
     * @param gaussianRand1 gaussian random number generatred
     * @param gaussianRand2 gaussian random number generarted 
     * @param nextRand  generated seed for next usage
     */
    void generateRand(unsigned int* seed,
                      double *gaussianRand1,
                      double *gaussianRand2,
                      unsigned int* nextRand);

    /**
     * @brief   calculates the  price and vega for all trajectories
     */
    void calOutputs(double strikePrice, double* meanDeriv1, 
                    double*  meanDeriv2, double* meanPrice1,
                    double* meanPrice2, double* pathDeriv1, 
                    double* pathDeriv2, double* priceVec1, double* priceVec2);

    /**
     * @brief   Reference implementation for Monte Carlo simuation for
     *          Asian Option pricing 
     */
    void cpuReferenceImpl();
};

#endif 


