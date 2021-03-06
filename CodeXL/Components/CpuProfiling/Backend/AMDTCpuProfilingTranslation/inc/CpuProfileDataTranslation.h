//==================================================================================
// Copyright (c) 2013-2016 , Advanced Micro Devices, Inc.  All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file CpuProfileDataTranslation.h
/// \brief CPU profile data translation interface.
/// \note This API is only valid for sampling profile data generated by
///       the \ref CpuProfileControl.h.
///
//==================================================================================

/** These APIs are used to aggregate the raw profile data from ".prd" or
    ".caperf" file and the processed data is written into a db file(cxlcpudb).

    Processing of raw profile data is required to aggregate and attribute
    the samples to processes/load-modules/functions/source statements and
    instructions. Raw profile data files have a ".prd" extension and
    should be opened with \ref fnOpenProfile. A profile data file must be
    opened to generate a reader handle, which will be used in all other
    interactions.

    The processing (data-translation) of raw profile data is performed by
    fnCpuProfileDataTranslate API. This api writes the processed CPU profile
    data into a db file. The profile data stored in db can be querried
    using cxlProfileDataReader class.

    Once the data translation is completed, fnCloseProfile should be called
    to free the file handles. This will dispose off the reader handle.
*/

#ifndef _CPUPROFILEDATATRANSLATION_H_
#define _CPUPROFILEDATATRANSLATION_H_

#include <AMDTOSWrappers/Include/osFilePath.h>
#include "CpuProfilingTranslationDLLBuild.h"


struct CP_TRANS_API CpuProfilerProgress
{
    CpuProfilerProgress(const gtString& profilerName, const gtString& progress, int value) :
        m_profilerName(profilerName), m_progress(progress), m_value(value) {};
    ~CpuProfilerProgress() = default;

    const gtString& profileName() const { return m_profilerName; };
    void setProfileName(const gtString& profilerName) { m_profilerName = profilerName; }

    const gtString& progress() const { return m_progress; };
    void setProgress(const gtString& progress) { m_progress = progress; }

    int value() const { return m_value; };
    void setValue(int value) { m_value = value; }

    bool aborted() const { return m_aborted; };
    void setAborted(bool aborted) { m_aborted = aborted; }

    bool increment() const { return m_increment; };
    void setIncrement(bool increment) { m_increment = increment; }

    gtString m_profilerName;
    gtString m_progress;
    int m_value = 0;
    bool m_aborted = false;
    bool m_increment = false;
};

typedef void(*PfnProgressBarCallback)(CpuProfilerProgress& progressEvent);

/****************************************************************************/

/** \typedef ReaderHandle
    \brief A handle to identify which profiles were opened.
    \ingroup datafiles

    The handle is necessary for all functions related to retrieving data from
    the opened profiles.  The two functions that will generate a new valid
    ReaderHandle are \ref fnOpenProfile and \ref fnOpenAggregatedProfile.
*/
typedef void ReaderHandle;

/** This function will open a raw profile data file.

    This function supports the following profile input:
    1. CodeXL profile data with ".prd" file extension
    2. CodeXL profile data with ".caperf" file extension (Linux only)

    \ingroup datafiles
    @param[in] pPath The profile data file or directory of interest
    @param[out] pReaderHandle The handle to use for retrieving the data
    \return The success of opening the profile
    \retval S_OK Success
    \retval E_INVALIDARG either pPath or pReaderHandle is not a valid
    pointer
    \retval ERROR_FILE_NOT_FOUND The file wasn't found
    \retval E_ACCESSDENIED The file wasn't able to be opened
    \retval E_HANDLE The reader handle is already open
    \retval E_OUTOFMEMORY no more memory is available
    \retval E_UNEXPECTED an unexpected error occurred
*/
CP_TRANS_API HRESULT fnOpenProfile(
    /*in*/ const wchar_t* pPath,
    /*out*/ ReaderHandle** pReaderHandle);

/** Releases all resources appropriately.  Note that after this call, the
    reader handle will not be valid.
    \ingroup datafiles
    @param[in,out] pReaderHandle The handle to close
    \return The success of closing the profile
    \retval S_OK Success
    \retval E_INVALIDARG pReaderHandle was not an open handle
    \retval E_ACCESSDENIED There is data currently being aggregated, and that
    function must be canceled first.
    \retval E_UNEXPECTED an unexpected error occurred
*/
CP_TRANS_API HRESULT fnCloseProfile(
    /*in*/ ReaderHandle** pReaderHandle);

/** This processes the raw cpu-profile data and writes the processes/aggregated
    data into a db file. This also takes care of processing the call-stack samples and
    CLR/JAVA JIT files.

    \ingroup datasets
    @param[in] pReaderHandle The open reader handle
    @param[in] pFileName The file name to write to
    @param[out] pPercentComplete an optional way to track the progress of the writing
    \return The success of removing the data set
    \retval S_OK Success
    \retval E_PENDING The profile data was not yet aggregated
    \retval E_INVALIDARG pReaderHandle was not an open handle, or an invalid
    data set name was provided
    \retval E_ACCESSDENIED the access to the path was denied
    \retval E_UNEXPECTED an unexpected error occurred
*/
CP_TRANS_API HRESULT fnCpuProfileDataTranslate(
    /*in*/ ReaderHandle* pReaderHandle,
    /*in*/ const wchar_t* pFileName,
    /*in*/ PfnProgressBarCallback pfnProgressBarCallback,
    /*in*/ const wchar_t* pSearchPath = NULL,
    /*in*/ const wchar_t* pServerList = NULL,
    /*in*/ const wchar_t* pCachePath = NULL);

CP_TRANS_API HRESULT fnMigrateEBPToDB(
    /*in*/ const osFilePath& ebpFilePath);

#endif  // _CPUPROFILEDATATRANSLATION_H_