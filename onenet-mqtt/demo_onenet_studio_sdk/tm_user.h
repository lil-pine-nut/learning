/**
 * Copyright (c), 2012~2020 iot.10086.cn All Rights Reserved,
 * 此tm_user.h和tm_user.c，在产品物模型中，下载设备端SDK。
 *
 * @file tm_user.h
 * @date 2020/05/14
 * @brief
 */

#ifndef __TM_USER_H__
#define __TM_USER_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "data_types.h"
#include "tm_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************/
/* External Definition ( Constant and Macro )                                */
/*****************************************************************************/

/*****************************************************************************/
/* External Structures, Enum and Typedefs                                    */
/*****************************************************************************/
/****************************** Structure type *******************************/
#pragma pack(1)

    struct svc_$sys_excel_service_in_t
    {
        int64_t input;
    };

    struct svc_$sys_excel_service_out_t
    {
        int32_t output;
    };
#pragma pack()
    /****************************** Auto Generated *******************************/

    /*****************************************************************************/
    /* External Variables and Functions                                          */
    /*****************************************************************************/
    /*************************** Property Func List ******************************/
    extern struct tm_prop_tbl_t tm_prop_list[];
    extern uint16_t tm_prop_list_size;
    /****************************** Auto Generated *******************************/

    /**************************** Service Func List ******************************/
    extern struct tm_svc_tbl_t tm_svc_list[];
    extern uint16_t tm_svc_list_size;
    /****************************** Auto Generated *******************************/

    /**************************** Property Func Read ****************************/
    int32_t tm_prop_$sys_excel_int32_rd_cb(void *data);
    int32_t tm_prop_w10001_rd_cb(void *data);
    /****************************** Auto Generated *******************************/

    /**************************** Property Func Write ****************************/
    int32_t tm_prop_$sys_excel_int32_wr_cb(void *data);
    int32_t tm_prop_w10001_wr_cb(void *data);
    /****************************** Auto Generated *******************************/

    /**************************** Service Func Invoke ****************************/
    int32_t tm_svc_$sys_excel_service_cb(void *in_data, void *out_data);
    /****************************** Auto Generated *******************************/

    /**************************** Property Func Notify ***************************/
    int32_t tm_prop_$sys_excel_int32_notify(void *data, int32_t val, uint64_t timestamp, uint32_t timeout_ms);
    int32_t tm_prop_w10001_notify(void *data, float64_t val, uint64_t timestamp, uint32_t timeout_ms);
    /****************************** Auto Generated *******************************/

    /***************************** Event Func Notify *****************************/
    /****************************** Auto Generated *******************************/

#ifdef __cplusplus
}
#endif

#endif
