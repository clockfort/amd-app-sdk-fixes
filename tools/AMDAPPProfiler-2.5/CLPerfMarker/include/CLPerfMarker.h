//=============================================================================
//
// Author: GPU Developer Tools
//         AMD, Inc.
//
// This file is the only header that must be included by an application that
// wishes to use CLPerfMarker. It defines all the available entrypoints.
//=============================================================================
// Copyright (c) 2010 Advanced Micro Devices, Inc.  All rights reserved.
//=============================================================================

#ifndef _CL_PERF_MARKER_H_
#define _CL_PERF_MARKER_H_

#ifdef __cplusplus
extern "C" {
#endif

#define AP_SUCCESS                             0
#define AP_UNINITIALIZED_PERF_MARKER          -1
#define AP_FINALIZED_PERF_MARKER              -2
#define AP_UNBALANCED_MARKER                  -3
#define AP_APP_PROFILER_NOT_DETECTED          -4
#define AP_NULL_MARKER_NAME                   -5
#define AP_INTERNAL_ERROR                     -6
#define AP_OUT_OF_MEMORY                      -7
#define AP_FAILED_TO_OPEN_OUTPUT_FILE         -8

#if defined(_WIN32) || defined(__CYGWIN__)
	#define AP_API_CALL __stdcall
#else
	#define AP_API_CALL
#endif

//------------------------------------------------------------------------------------
/// clPerfMarker helps users visualize the begin-end markers they defined in the application
/// in the Timeline. Markers created in different threads will be displayed in different
/// rows in the Timeline. User can create hierarchical markers by nesting clBeginPerfMarkerAMD
/// calls.
//------------------------------------------------------------------------------------

/// Initialize clPerfMarkerAMD.
/// \return status code
extern int AP_API_CALL clInitializePerfMarkerAMD();

/// Begin clPerfMarker block
/// Nested calls to clBeginPerfMarkerAMD will result in
/// markers showing in a hierarchical way.
/// \param szMarkerName Marker name
/// \param szGroupName Group name, Optional, Pass in NULL to use default group name
///        If group name is specified, additional sub-branch will be created under PerfMarker branch 
///        in Timeline and all markers that belongs to the group will be displayed in the group branch.
/// \return status code
extern int AP_API_CALL clBeginPerfMarkerAMD(const char* szMarkerName, const char* szGroupName);

/// End clPerfMarker block
/// \return status code
extern int AP_API_CALL clEndPerfMarkerAMD();

/// Finaliza clPerfMarker, Save collected data in specified output file.
/// Failed to call the function will result in no clPerfMarker file is generated.
/// \return status code
extern int AP_API_CALL clFinalizePerfMarkerAMD();

#ifdef __cplusplus
}
#endif

#endif
