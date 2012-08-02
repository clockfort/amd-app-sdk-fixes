/* ============================================================

Copyright (c) 2011 Advanced Micro Devices, Inc.  All rights reserved.

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
SUPPORT, INDEMNITY, ERROR FREE OR UNINTERRUPTED OPERATION, OR THAT IT IS FREE
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

// OVEncode.h
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the OVENCODE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// OVENCODE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

// Open Encode Change define Win32
#define _WIN32 1
// end Open Encode Note/Change

#ifndef __OVENCODE_H__
#define __OVENCODE_H__

#ifndef OPENVIDEOAPI
#define OPENVIDEOAPI __stdcall
#endif // OPENVIDEOAPI


#include "OVEncodeTypes.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

    int OPENVIDEOAPI fnOVEncode(void);

    /* 
     * This function is used by the application to query the available encoder devices. 
     * The ovencode_device_info contains a unique device_id and the size of the 
     * encode_cap structure for each available device. The encode_cap size is the
     * size of the encode_cap structure that the application should provide in
     * the OVEncodeGetDeviceCap call.
     */
    OVresult OPENVIDEOAPI OVEncodeGetDeviceInfo (
        unsigned int            *num_device,
        ovencode_device_info    *device_info);

    /*
     * This function is used by application to query the encoder capability that includes
     * codec information and format that the device can support.
     */
    OVresult OPENVIDEOAPI OVEncodeGetDeviceCap (
        OPContextHandle             platform_context,
        unsigned int                device_id,
        unsigned int                encode_cap_list_size,
        unsigned int                *num_encode_cap,
        OVE_ENCODE_CAPS             *encode_cap_list);

    /*
     * This function is used by the application to create the encode handle from the 
     * platform memory handle. The encode handle can be used in the OVEncodePicture 
     * function as the output encode buffer. The application can create multiple 
     * output buffers to queue up the decode job. 
     */
    ove_handle OPENVIDEOAPI OVCreateOVEHandleFromOPHandle (
        OPMemHandle                 platform_memhandle);

    /* 
     * This function is used by the application to release the encode handle. 
     * After release, the handle is invalid and should not be used for encode picture. 
     */
    OVresult OPENVIDEOAPI OVReleaseOVEHandle(
        ove_handle                  encode_handle);

    /* 
     * This function is used by the application to acquire the memory objects that 
     * have been created from OpenCL. These objects need to be acquired before they 
     * can be used by the decode function. 
     */

    OVresult OPENVIDEOAPI OVEncodeAcquireObject (
        ove_session                 session,
        unsigned int                num_handle,
        ove_handle                 *encode_handle,
        unsigned int                num_event_in_wait_list,
        OPEventHandle              *event_wait_list,
        OPEventHandle              *event);

    /*
     * This function is used by the application to release the memory objects that
     * have been created from OpenCL. The objects need to be released before they
     * can be used by OpenCL.
     */
    OVresult OPENVIDEOAPI OVEncodeReleaseObject (
       ove_session                  session,
       unsigned int                 num_handle,
       ove_handle                  *encode_handle,
       unsigned int                 num_event_in_wait_list,
       OPEventHandle               *event_wait_list,
       OPEventHandle               *event);


	ove_event OPENVIDEOAPI OVCreateOVEEventFromOPEventHandle (
        OPEventHandle               platform_eventhandle);

    /* 
     * This function is used by the application to release the encode event handle. 
     * After release, the event handle is invalid and should not be used. 
     */
    OVresult OPENVIDEOAPI OVEncodeReleaseOVEEventHandle (
        ove_event                   ove_ev);


    /*
     * This function is used by the application to create the encode session for
     * each encoding stream. After the session creation, the encoder is ready to
     * accept the encode picture job from the application. For multiple streams
     * encoding, the application can create multiple sessions within the same
     * platform context and the application is responsible to manage the input and
     * output buffers for each corresponding session.
     */
    ove_session OPENVIDEOAPI OVEncodeCreateSession (
        OPContextHandle             platform_context,
        unsigned int                device_id,
        OVE_ENCODE_MODE             encode_mode,
        OVE_PROFILE_LEVEL           encode_profile,
        OVE_PICTURE_FORMAT	        encode_format,
        unsigned int                encode_width,
        unsigned int                encode_height,
        OVE_ENCODE_TASK_PRIORITY    encode_task_priority);

    /*
     * This function is used by the application to destroy the encode session. Destroying a
     * session will release all associated hardware resources.  No further decoding work
     * can be performed with the session after it is destroyed.
     */
    OVresult OPENVIDEOAPI OVEncodeDestroySession (
        ove_session                 session);

	// Retrieve one configuration data structure
	OVresult OPENVIDEOAPI OVEncodeGetPictureControlConfig (
        ove_session                 session,
        OVE_CONFIG_PICTURE_CONTROL *pPictureControlConfig);

	// Get current rate control configuration
	OVresult OPENVIDEOAPI OVEncodeGetRateControlConfig (
        ove_session                 session,
        OVE_CONFIG_RATE_CONTROL	   *pRateControlConfig);

	// Get current motion estimation configuration
	OVresult OPENVIDEOAPI OVEncodeGetMotionEstimationConfig (
        ove_session                 session,
        OVE_CONFIG_MOTION_ESTIMATION *pMotionEstimationConfig);

	// Get current RDO configuration
	OVresult OPENVIDEOAPI OVEncodeGetRDOControlConfig (
        ove_session             session,
        OVE_CONFIG_RDO          *pRDOConfig);

	OVresult OPENVIDEOAPI OVEncodeSendConfig (
        ove_session             session,
        unsigned int            num_of_config_buffers,
        OVE_CONFIG              *pConfigBuffers);

	// Fully encode a single picture
	OVresult OPENVIDEOAPI OVEncodeTask (
        ove_session             session,
        unsigned int            number_of_encode_task_input_buffers,
        OVE_INPUT_DESCRIPTION   *encode_task_input_buffers_list,
        void                    *picture_parameter,
        unsigned int            *pTaskID,
        unsigned int            num_event_in_wait_list,
        ove_event               *event_wait_list,
        ove_event               *event);

	// Query outputs
	OVresult OPENVIDEOAPI OVEncodeQueryTaskDescription (
        ove_session             session,
        unsigned int            num_of_task_description_request,
        unsigned int            *num_of_task_description_return,
        OVE_OUTPUT_DESCRIPTION  *task_description_list);

	// Reclaim the resource of the output ring up to the specified task.
	OVresult OPENVIDEOAPI OVEncodeReleaseTask (
        ove_session             session,
        unsigned int            taskID);


#ifdef __cplusplus
};
#endif //  __cplusplus

#endif // __OVENCODE_H__