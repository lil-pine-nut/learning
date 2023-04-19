/**
 * Copyright (c), 2012~2020 iot.10086.cn All Rights Reserved
 *
 * @file tm_user.c
 * @date 2020/05/14
 * @brief
 */

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "tm_data.h"
#include "tm_api.h"
#include "tm_user.h"
#include <stdio.h>

/*****************************************************************************/
/* Local Definitions ( Constant and Macro )                                  */
/*****************************************************************************/

/*****************************************************************************/
/* Structures, Enum and Typedefs                                             */
/*****************************************************************************/
/*************************** Property Func List ******************************/
struct tm_prop_tbl_t tm_prop_list[] = {
    TM_PROPERTY_RW($sys_excel_int32),
    TM_PROPERTY_RW(w10001)};
uint16_t tm_prop_list_size = ARRAY_SIZE(tm_prop_list);
/****************************** Auto Generated *******************************/

/***************************** Service Func List *******************************/
struct tm_svc_tbl_t tm_svc_list[] = {
    TM_SERVICE($sys_excel_service)};
uint16_t tm_svc_list_size = ARRAY_SIZE(tm_svc_list);
/****************************** Auto Generated *******************************/

/*****************************************************************************/
/* Local Function Prototype                                                  */
/*****************************************************************************/

/*****************************************************************************/
/* Local Variables                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/

/*****************************************************************************/
/* Function Implementation                                                   */
/*****************************************************************************/
/**************************** Property Func Read *****************************/
int32_t tm_prop_$sys_excel_int32_rd_cb(void *data)
{
    fprintf(stderr, "%s\n", __FUNCTION__);
    int32_t val = 0;

    /** 根据业务逻辑获取功能点值，设置到val */

    tm_data_struct_set_int32(data, "$sys_excel_int32", val);

    return 0;
}

int32_t tm_prop_w10001_rd_cb(void *data)
{
    fprintf(stderr, "%s\n", __FUNCTION__);
    float64_t val = 0;

    /** 根据业务逻辑获取功能点值，设置到val */

    tm_data_struct_set_double(data, "w10001", val);

    return 0;
}

/****************************** Auto Generated *******************************/

/**************************** Property Func Write ****************************/
int32_t tm_prop_$sys_excel_int32_wr_cb(void *data)
{
    fprintf(stderr, "%s\n", __FUNCTION__);
    int32_t val = 0;

    tm_data_get_int32(data, &val);

    /** 根据变量val的值，填入下发控制逻辑 */

    return 0;
}

int32_t tm_prop_w10001_wr_cb(void *data)
{
    fprintf(stderr, "%s\n", __FUNCTION__);
    float64_t val = 0;

    tm_data_get_double(data, &val);

    /** 根据变量val的值，填入下发控制逻辑 */

    return 0;
}

/****************************** Auto Generated *******************************/

/**************************** Service Func Invoke ****************************/
int32_t tm_svc_$sys_excel_service_cb(void *in_data, void *out_data)
{
    fprintf(stderr, "%s\n", __FUNCTION__);
    struct svc_$sys_excel_service_in_t in_param = {0};
    struct svc_$sys_excel_service_out_t out_param = {0};
    int32_t ret = 0;

    tm_data_struct_get_enum(in_data, "input", &(in_param.input));

    /** 根据输入参数in_param，生成输出参数out_param */

    tm_data_struct_set_enum(out_data, "output", out_param.output);

    return ret;
}

/****************************** Auto Generated *******************************/

/**************************** Property Func Notify ***************************/
int32_t tm_prop_$sys_excel_int32_notify(void *data, int32_t val, uint64_t timestamp, uint32_t timeout_ms)
{
    fprintf(stderr, "%s\n", __FUNCTION__);
    void *resource = NULL;
    int32_t ret = 0;

    if (NULL == data)
    {
        resource = tm_data_create();
    }
    else
    {
        resource = data;
    }

    tm_data_set_int32(resource, "$sys_excel_int32", val, timestamp);

    if (NULL == data)
    {
        ret = tm_post_property(resource, timeout_ms);
    }

    return ret;
}

int32_t tm_prop_w10001_notify(void *data, float64_t val, uint64_t timestamp, uint32_t timeout_ms)
{
    fprintf(stderr, "%s\n", __FUNCTION__);
    void *resource = NULL;
    int32_t ret = 0;

    if (NULL == data)
    {
        resource = tm_data_create();
    }
    else
    {
        resource = data;
    }

    tm_data_set_double(resource, "w10001", val, timestamp);

    if (NULL == data)
    {
        ret = tm_post_property(resource, timeout_ms);
    }

    return ret;
}

/****************************** Auto Generated *******************************/

/***************************** Event Func Notify *****************************/
/****************************** Auto Generated *******************************/
