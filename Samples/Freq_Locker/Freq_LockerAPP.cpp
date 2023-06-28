//===========================================================================
// Copyright (C) 2022 Intel Corporation
//
//
//
// SPDX-License-Identifier: MIT
//--------------------------------------------------------------------------

/**
 * @author  daniel.enriquez.montanez@intel.com  kevin.ma@intel.com
 * @file  Freq_LockerAPP.cpp
 * @brief This file contains the 'main' function. Program execution begins and ends there.
 *
 */

#define _CRTDBG_MAP_ALLOC

#include <crtdbg.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <vector>

#include <iostream>
#include <string>

#include <windows.h>
#include <stdio.h>
#include "igcl_api.h"
#include "GenericIGCLApp.h"

std::string DecodeRetCode(ctl_result_t Res);
void CtlFrequencyTest(ctl_device_adapter_handle_t hDAhandle);

const char *printType(ctl_data_type_t Type)
{
    switch (Type)
    {
        case ctl_data_type_t::CTL_DATA_TYPE_INT8:
        {
            return "CTL_DATA_TYPE_INT8";
        }
        break;
        case ctl_data_type_t::CTL_DATA_TYPE_UINT8:
        {
            return "CTL_DATA_TYPE_UINT8";
        }
        break;
        case ctl_data_type_t::CTL_DATA_TYPE_INT16:
        {
            return "CTL_DATA_TYPE_INT16";
        }
        break;
        case ctl_data_type_t::CTL_DATA_TYPE_UINT16:
        {
            return "CTL_DATA_TYPE_UINT16";
        }
        break;
        case ctl_data_type_t::CTL_DATA_TYPE_INT32:
        {
            return "CTL_DATA_TYPE_INT32";
        }
        break;
        case ctl_data_type_t::CTL_DATA_TYPE_UINT32:
        {
            return "CTL_DATA_TYPE_UINT32";
        }
        break;
        case ctl_data_type_t::CTL_DATA_TYPE_INT64:
        {
            return "CTL_DATA_TYPE_INT64";
        }
        break;
        case ctl_data_type_t::CTL_DATA_TYPE_UINT64:
        {
            return "CTL_DATA_TYPE_UINT64";
        }
        break;
        case ctl_data_type_t::CTL_DATA_TYPE_FLOAT:
        {
            return "CTL_DATA_TYPE_FLOAT";
        }
        break;
        case ctl_data_type_t::CTL_DATA_TYPE_DOUBLE:
        {
            return "CTL_DATA_TYPE_DOUBLE";
        }
        break;
        case ctl_data_type_t::CTL_DATA_TYPE_STRING_ASCII:
        {
            return "CTL_DATA_TYPE_STRING_ASCII";
        }
        break;
        case ctl_data_type_t::CTL_DATA_TYPE_STRING_UTF16:
        {
            return "CTL_DATA_TYPE_STRING_UTF16";
        }
        break;
        case ctl_data_type_t::CTL_DATA_TYPE_STRING_UTF132:
        {
            return "CTL_DATA_TYPE_STRING_UTF132";
        }
        break;
        default:
            return "Unknown units";
    }
    return "Unknown units";
}

const char *printUnits(ctl_units_t Units)
{
    switch (Units)
    {
        case ctl_units_t::CTL_UNITS_FREQUENCY_MHZ:
        {
            return "Frequency in MHz";
        }
        break;
        case ctl_units_t::CTL_UNITS_OPERATIONS_GTS:
        {
            return "GigaOperations per Second";
        }
        break;
        case ctl_units_t::CTL_UNITS_OPERATIONS_MTS:
        {
            return "MegaOperations per Second";
        }
        break;
        case ctl_units_t::CTL_UNITS_VOLTAGE_VOLTS:
        {
            return "Voltage in Volts";
        }
        break;
        case ctl_units_t::CTL_UNITS_POWER_WATTS:
        {
            return "Power in Watts";
        }
        break;
        case ctl_units_t::CTL_UNITS_TEMPERATURE_CELSIUS:
        {
            return "Temperature in Celsius";
        }
        break;
        case ctl_units_t::CTL_UNITS_ENERGY_JOULES:
        {
            return "Energy in Joules";
        }
        break;
        case ctl_units_t::CTL_UNITS_TIME_SECONDS:
        {
            return "Time in Seconds";
        }
        break;
        case ctl_units_t::CTL_UNITS_MEMORY_BYTES:
        {
            return "Memory in Bytes";
        }
        break;
        case ctl_units_t::CTL_UNITS_ANGULAR_SPEED_RPM:
        {
            return "Angular Speed in RPM";
        }
        break;
        default:
            return "Unknown units";
    }
    return "Unknown units";
}

/***************************************************************
 * @brief
 * keep GPU freq at max
 * @param
 * @return
 ***************************************************************/
void CtlFrequencyLocker(ctl_device_adapter_handle_t hDAhandle)
{

    uint32_t FrequencyHandlerCount = 0;
    ctl_result_t res               = ctlEnumFrequencyDomains(hDAhandle, &FrequencyHandlerCount, nullptr);
    if ((res != CTL_RESULT_SUCCESS) || FrequencyHandlerCount == 0)
    {
        PRINT_LOGS("\nTemperature component not supported. Error: %s", DecodeRetCode(res).c_str());
        return;
    }
    else
    {
        PRINT_LOGS("\nNumber of Frequency Handles [%u]", FrequencyHandlerCount);
    }

    ctl_freq_handle_t *pFrequencyHandle = new ctl_freq_handle_t[FrequencyHandlerCount];

    res = ctlEnumFrequencyDomains(hDAhandle, &FrequencyHandlerCount, pFrequencyHandle);

    if (res != CTL_RESULT_SUCCESS)
    {
        PRINT_LOGS("\nError: %s for Frequency handle.", DecodeRetCode(res).c_str());
        goto cleanUp;
    }

    for (uint32_t i = 0; i < FrequencyHandlerCount; i++)
    {
        PRINT_LOGS("\n\nFor Frequency Handle: [%u]", i);

        PRINT_LOGS("\n\n[Frequency] Properties:");

        ctl_freq_properties_t freqProperties = { 0 };
        freqProperties.Size                  = sizeof(ctl_freq_properties_t);
        res                                  = ctlFrequencyGetProperties(pFrequencyHandle[i], &freqProperties);
        if (res)
        {
            PRINT_LOGS("\n from Frequency get properties. %s", DecodeRetCode(res).c_str());
        }
        else
        {
            PRINT_LOGS("\n[Frequency] Min [%f]] Mhz", freqProperties.min);
            PRINT_LOGS("\n[Frequency] Max [%f]] Mhz", freqProperties.max);
            PRINT_LOGS("\n[Frequency] Can Control Frequency? [%u]]", (uint32_t)freqProperties.canControl);
            PRINT_LOGS("\n[Frequency] Frequency Domain [%s]]", ((freqProperties.type == CTL_FREQ_DOMAIN_GPU) ? "GPU" : "MEMORY"));
        }

        PRINT_LOGS("\n\n[Frequency] State:");

        ctl_freq_state_t freqState = { 0 };
        freqState.Size             = sizeof(ctl_freq_state_t);
        res                        = ctlFrequencyGetState(pFrequencyHandle[i], &freqState);
        if (res)
        {
            PRINT_LOGS("\n %s from Frequency get state.", DecodeRetCode(res).c_str());
        }
        else
        {
            PRINT_LOGS("\n[Frequency] Actual Frequency [%f] Mhz", freqState.actual);
            PRINT_LOGS("\n[Frequency] Efficient Frequency [%f] Mhz", freqState.efficient);
            PRINT_LOGS("\n[Frequency] Requested Frequency [%f] Mhz", freqState.request);
            PRINT_LOGS("\n[Frequency] Max clock Frequency for a target TDP [%f] Mhz", freqState.tdp);
            PRINT_LOGS("\n[Frequency] Throttle Reasons [%u]", freqState.throttleReasons);
            PRINT_LOGS("\n[Frequency] Voltage [%f] Volts", freqState.currentVoltage);
        }

        PRINT_LOGS("\n\n[Frequency] Frequency range:");

        ctl_freq_range_t freqRange = { 0 };
        freqRange.Size             = sizeof(ctl_freq_range_t);
        res                        = ctlFrequencyGetRange(pFrequencyHandle[i], &freqRange);

        if (res)
        {
            PRINT_LOGS("\n %s from Frequency get range.", DecodeRetCode(res).c_str());
        }
        else
        {
            PRINT_LOGS("\n[Frequency] Range Max [%f] Mhz", freqRange.max);
            PRINT_LOGS("\n[Frequency] Range Min [%f] Mhz", freqRange.min);
        }

        PRINT_LOGS("\n\n[Frequency] Set Frequency to max:");

        freqRange.min = freqRange.max;

        res = ctlFrequencySetRange(pFrequencyHandle[i], &freqRange);

        if (res)
        {
            PRINT_LOGS("\n %s from Frequency set to max.", DecodeRetCode(res).c_str());
        }
        else
        {

            PRINT_LOGS("\n\n[Frequency] Success for Set Frequency to max: [%f] MHz", freqRange.max);
        }
    }

cleanUp:
    delete[] pFrequencyHandle;
    pFrequencyHandle = nullptr;
}

/***************************************************************
 * @brief Main Function
 *
 * place_holder_for_Detailed_desc
 * @param
 * @return
 ***************************************************************/
int main()
{
    ctl_result_t Result                                       = CTL_RESULT_SUCCESS;
    ctl_device_adapter_handle_t *hDevices                     = nullptr;
    ctl_display_output_handle_t *hDisplayOutput               = nullptr;
    ctl_device_adapter_properties_t StDeviceAdapterProperties = { 0 };
    // Get a handle to the DLL module.
    uint32_t Adapter_count = 0;
    uint32_t Display_count = 0;
    uint32_t Index         = 0;
    uint32_t Display_index = 0;

    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    ctl_init_args_t CtlInitArgs;
    ctl_api_handle_t hAPIHandle;
    CtlInitArgs.AppVersion = CTL_MAKE_VERSION(CTL_IMPL_MAJOR_VERSION, CTL_IMPL_MINOR_VERSION);
    CtlInitArgs.flags      = CTL_INIT_FLAG_USE_LEVEL_ZERO;
    CtlInitArgs.Size       = sizeof(CtlInitArgs);
    CtlInitArgs.Version    = 0;
    ZeroMemory(&CtlInitArgs.ApplicationUID, sizeof(ctl_application_id_t));
    Result = ctlInit(&CtlInitArgs, &hAPIHandle);

    if (CTL_RESULT_SUCCESS == Result)
    {
        // Initialization successful
        // Get the list of Intel Adapters

        Result = ctlEnumerateDevices(hAPIHandle, &Adapter_count, hDevices);

        if (CTL_RESULT_SUCCESS == Result)
        {
            hDevices = (ctl_device_adapter_handle_t *)malloc(sizeof(ctl_device_adapter_handle_t) * Adapter_count);
            if (hDevices == NULL)
            {
                return ERROR;
            }
            Result = ctlEnumerateDevices(hAPIHandle, &Adapter_count, hDevices);
        }
        if (CTL_RESULT_SUCCESS != Result)
        {
            printf("ctlEnumerateDevices returned failure code: 0x%X\n", Result);
            goto free_exit;
        }

        for (Index = 0; Index < Adapter_count; Index++)
        {
            if (NULL != hDevices[Index])
            {
                LUID AdapterID;
                StDeviceAdapterProperties.Size           = sizeof(ctl_device_adapter_properties_t);
                StDeviceAdapterProperties.pDeviceID      = malloc(sizeof(LUID));
                StDeviceAdapterProperties.device_id_size = sizeof(LUID);
                StDeviceAdapterProperties.Version        = 2;

                if (NULL == StDeviceAdapterProperties.pDeviceID)
                {
                    return ERROR;
                }

                Result = ctlGetDeviceProperties(hDevices[Index], &StDeviceAdapterProperties);

                if (Result != CTL_RESULT_SUCCESS)
                {
                    printf("ctlGetDeviceProperties returned failure code: 0x%X\n", Result);
                    break;
                }

                if (CTL_DEVICE_TYPE_GRAPHICS != StDeviceAdapterProperties.device_type)
                {
                    printf("This is not a Graphics device \n");

                    if (NULL != StDeviceAdapterProperties.pDeviceID)
                    {
                        free(StDeviceAdapterProperties.pDeviceID);
                    }
                    continue;
                }

                if (NULL != StDeviceAdapterProperties.pDeviceID)
                {
                    AdapterID = *(reinterpret_cast<LUID *>(StDeviceAdapterProperties.pDeviceID));
                    PRINT_LOGS("\nAdapter ID %lu \n", AdapterID.LowPart);
                }

                if (0x8086 == StDeviceAdapterProperties.pci_vendor_id)
                {
                    PRINT_LOGS("\nIntel Adapter Name %s", StDeviceAdapterProperties.name);
                    PRINT_LOGS("\nVendor id  0x%X", StDeviceAdapterProperties.pci_vendor_id);
                    PRINT_LOGS("\nDevice id 0x%X", StDeviceAdapterProperties.pci_device_id);
                    PRINT_LOGS("\nSubSys id 0x%X", StDeviceAdapterProperties.pci_subsys_id);
                    PRINT_LOGS("\nSubSys Vendor id 0x%X", StDeviceAdapterProperties.pci_subsys_vendor_id);
                    PRINT_LOGS("\nRev id 0x%X", StDeviceAdapterProperties.rev_id);
                }

                // Per Component Tests
                CtlFrequencyLocker(hDevices[Index]);

                if (NULL != StDeviceAdapterProperties.pDeviceID)
                {
                    free(StDeviceAdapterProperties.pDeviceID);
                }
            }
        }
    }

free_exit:

    ctlClose(hAPIHandle);

    if (hDisplayOutput != nullptr)
    {
        free(hDisplayOutput);
        hDisplayOutput = nullptr;
    }
    hDisplayOutput = nullptr;

    if (hDevices != nullptr)
    {
        free(hDevices);
        hDevices = nullptr;
    }

    return 0;
}

// Decoding the return code for the most common error codes.
std::string DecodeRetCode(ctl_result_t Res)
{
    switch (Res)
    {
        case CTL_RESULT_SUCCESS:
        {
            return std::string("[CTL_RESULT_SUCCESS]");
        }
        case CTL_RESULT_ERROR_CORE_OVERCLOCK_NOT_SUPPORTED:
        {
            return std::string("[CTL_RESULT_ERROR_CORE_OVERCLOCK_NOT_SUPPORTED]");
        }
        case CTL_RESULT_ERROR_CORE_OVERCLOCK_VOLTAGE_OUTSIDE_RANGE:
        {
            return std::string("[CTL_RESULT_ERROR_CORE_OVERCLOCK_VOLTAGE_OUTSIDE_RANGE]");
        }
        case CTL_RESULT_ERROR_CORE_OVERCLOCK_FREQUENCY_OUTSIDE_RANGE:
        {
            return std::string("[CTL_RESULT_ERROR_CORE_OVERCLOCK_FREQUENCY_OUTSIDE_RANGE]");
        }
        case CTL_RESULT_ERROR_CORE_OVERCLOCK_POWER_OUTSIDE_RANGE:
        {
            return std::string("[CTL_RESULT_ERROR_CORE_OVERCLOCK_POWER_OUTSIDE_RANGE]");
        }
        case CTL_RESULT_ERROR_CORE_OVERCLOCK_TEMPERATURE_OUTSIDE_RANGE:
        {
            return std::string("[CTL_RESULT_ERROR_CORE_OVERCLOCK_TEMPERATURE_OUTSIDE_RANGE]");
        }
        case CTL_RESULT_ERROR_GENERIC_START:
        {
            return std::string("[CTL_RESULT_ERROR_GENERIC_START]");
        }
        case CTL_RESULT_ERROR_CORE_OVERCLOCK_RESET_REQUIRED:
        {
            return std::string("[CTL_RESULT_ERROR_CORE_OVERCLOCK_RESET_REQUIRED]");
        }
        case CTL_RESULT_ERROR_CORE_OVERCLOCK_IN_VOLTAGE_LOCKED_MODE:
        {
            return std::string("[CTL_RESULT_ERROR_CORE_OVERCLOCK_IN_VOLTAGE_LOCKED_MODE");
        }
        case CTL_RESULT_ERROR_CORE_OVERCLOCK_WAIVER_NOT_SET:
        {
            return std::string("[CTL_RESULT_ERROR_CORE_OVERCLOCK_WAIVER_NOT_SET]");
        }
        case CTL_RESULT_ERROR_NOT_INITIALIZED:
        {
            return std::string("[CTL_RESULT_ERROR_NOT_INITIALIZED]");
        }
        case CTL_RESULT_ERROR_ALREADY_INITIALIZED:
        {
            return std::string("[CTL_RESULT_ERROR_ALREADY_INITIALIZED]");
        }
        case CTL_RESULT_ERROR_DEVICE_LOST:
        {
            return std::string("[CTL_RESULT_ERROR_DEVICE_LOST]");
        }
        case CTL_RESULT_ERROR_INSUFFICIENT_PERMISSIONS:
        {
            return std::string("[CTL_RESULT_ERROR_INSUFFICIENT_PERMISSIONS]");
        }
        case CTL_RESULT_ERROR_NOT_AVAILABLE:
        {
            return std::string("[CTL_RESULT_ERROR_NOT_AVAILABLE]");
        }
        case CTL_RESULT_ERROR_UNINITIALIZED:
        {
            return std::string("[CTL_RESULT_ERROR_UNINITIALIZED]");
        }
        case CTL_RESULT_ERROR_UNSUPPORTED_VERSION:
        {
            return std::string("[CTL_RESULT_ERROR_UNSUPPORTED_VERSION]");
        }
        case CTL_RESULT_ERROR_UNSUPPORTED_FEATURE:
        {
            return std::string("[CTL_RESULT_ERROR_UNSUPPORTED_FEATURE]");
        }
        case CTL_RESULT_ERROR_INVALID_ARGUMENT:
        {
            return std::string("[CTL_RESULT_ERROR_INVALID_ARGUMENT]");
        }
        case CTL_RESULT_ERROR_INVALID_NULL_HANDLE:
        {
            return std::string("[CTL_RESULT_ERROR_INVALID_NULL_HANDLE]");
        }
        case CTL_RESULT_ERROR_INVALID_NULL_POINTER:
        {
            return std::string("[CTL_RESULT_ERROR_INVALID_NULL_POINTER]");
        }
        case CTL_RESULT_ERROR_INVALID_SIZE:
        {
            return std::string("[CTL_RESULT_ERROR_INVALID_SIZE]");
        }
        case CTL_RESULT_ERROR_UNSUPPORTED_SIZE:
        {
            return std::string("[CTL_RESULT_ERROR_UNSUPPORTED_SIZE]");
        }
        case CTL_RESULT_ERROR_NOT_IMPLEMENTED:
        {
            return std::string("[CTL_RESULT_ERROR_NOT_IMPLEMENTED]");
        }
        case CTL_RESULT_ERROR_ZE_LOADER:
        {
            return std::string("[CTL_RESULT_ERROR_ZE_LOADER]");
        }
        case CTL_RESULT_ERROR_INVALID_OPERATION_TYPE:
        {
            return std::string("[CTL_RESULT_ERROR_INVALID_OPERATION_TYPE]");
        }
        case CTL_RESULT_ERROR_UNKNOWN:
        {
            return std::string("[CTL_RESULT_ERROR_UNKNOWN]");
        }
        default:
            return std::string("[Unknown Error]");
    }
}
