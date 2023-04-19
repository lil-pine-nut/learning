/**
 * Copyright (c), 2012~2020 iot.10086.cn All Rights Reserved
 *
 * @file main.c
 * @brief
 */

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include <stdio.h>
#include <time.h>

#include "tm_data.h"
#include "data_types.h"
#include "tm_api.h"
#include "tm_user.h"

#include "tm_subdev.h"

/*****************************************************************************/
/* Local Definitions ( Constant and Macro )                                  */
/*****************************************************************************/
/**
 * 用户可在OneNET Studio创建新的产品，导入目录中model-schema.json定义的物模型后，
 * 直接使用SDK内的tm_user.[ch]进行调试，同时还可以从平台导出tm_user.[ch]与SDK
 * 内置的文件进行对比，熟悉物模型功能点的开发方式
 *
 * 本次测试使用的是自定义的物模型，非model-schema.json定义的物模型
 */

int my_subdev_props_get(const int8_t *product_id, const int8_t *dev_name, const int8_t *props_list, int8_t **props_data)
{
    printf("my_subdev_props_get: [%s]\n", props_data);
    return 0;
}

int my_subdev_props_set(const int8_t *product_id, const int8_t *dev_name, int8_t *props_data)
{

    printf("my_subdev_props_set: [%s]\n", props_data);
    return 0;
}

int my_subdev_service_invoke(const int8_t *product_id, const int8_t *dev_name, const int8_t *svc_id, int8_t *in_data, int8_t **out_data)
{
    printf("my_subdev_service_invoke: [%s]\n", in_data);
    return 0;
}

int my_subdev_topo(int8_t *topo_data)
{

    printf("my_subdev_topo:[%s]\n", topo_data);
    return 0;
}

int main(int arg, char **argv[])
{
    struct tm_downlink_tbl_t downlink_tbl = {0};

    // 使用 tm_user.h和tm_user.c，在产品物模型中，下载设备端SDK
    // downlink_tbl.prop_tbl = tm_prop_list;
    // downlink_tbl.prop_tbl_size = tm_prop_list_size;
    // downlink_tbl.svc_tbl = tm_svc_list;
    // downlink_tbl.svc_tbl_size = tm_svc_list_size;

    if (0 == tm_init(&downlink_tbl))
    {
        printf("tm init ok\n");
    }

    // tm_login 需要时间长一些
    if (0 == tm_login("ux9jchZp8s", "mqtt_test_2022_06_d1", "axnHhlFmJvxLM8ZCPFzmM5antrxOUuu/L5W9YzmXUm4=", 5000))
    {
        printf("tm api login ok\n");
    }
    else
    {
        printf("tm api login failed\n");
        return -1;
    }

    // 使用 tm_data 接口进行打包时可参考以下方式：
    void *post_json_data = tm_data_create();
    tm_data_set_float(post_json_data, "w10001", 22.2, 0);
    char *post_json = "{\"w10002\":{\"value\":22.2}}";

    char *test_data = cJSON_Print(post_json_data);
    printf("post_json: [%s]\n", test_data);
    // 注意：调用tm_post_property 传进的prop_data，如果是字符串需要进去 tm_post_property 修改 tm_send_request 的参数 as_raw 由 0 改为 1
    int ret = tm_post_property(post_json_data, 10000);
    printf("ret: %d\n", ret);

    struct tm_subdev_cbs subdev_cbs = {0};
    subdev_cbs.subdev_props_get = my_subdev_props_get;
    subdev_cbs.subdev_props_set = my_subdev_props_set;
    subdev_cbs.subdev_service_invoke = my_subdev_service_invoke;
    subdev_cbs.subdev_topo = my_subdev_topo;

    tm_subdev_init(subdev_cbs);

    // 将指定子设备绑定到当前网关设备
    int reuslt;
    reuslt = tm_subdev_add("Y8CLDa6OfI", "test_subdev_01", "dBTEGvV7bi/hf0Jq11i9Jd2gzmEpUXS8TnfxW2hfb9Y=", 10000);
    printf("tm_subdev_add  reuslt: %d\n", reuslt);

    reuslt = tm_subdev_add("Y8CLDa6OfI", "test_subdev_02", "zsqcmDrW68+J1lnYGz/XCBprBt9MyBPbWrJhHGDSbzQ=", 10000);
    printf("tm_subdev_add  reuslt: %d\n", reuslt);

    // 从平台获取当前网关绑定的子设备信息
    // reuslt = tm_subdev_topo_get(1000);
    // printf("tm_subdev_topo_get  reuslt: %d\n", reuslt);

    // // 获取平台为设备设置的期望属性值，会段错误，是因为没设置？
    // reuslt = tm_get_desired_props(100);
    // printf("tm_get_desired_props  reuslt: %d\n", reuslt);

    // 子设备登录平台
    reuslt = tm_subdev_login("Y8CLDa6OfI", "test_subdev_01", 10000);
    printf("tm_subdev_login  reuslt: %d\n", reuslt);

    reuslt = tm_subdev_login("Y8CLDa6OfI", "test_subdev_02", 10000);
    printf("tm_subdev_login  reuslt: %d\n", reuslt);

    // // 子设备上报数据到平台
    // reuslt = tm_subdev_post_data("Y8CLDa6OfI", "test_subdev_01", "{\"test_value\":{\"value\":1}, \"test_string\":{\"value\":\"hello, test_subdev_01\"}}", 0, 100);
    // printf("tm_subdev_post_data  reuslt: %d\n", reuslt);

    // reuslt = tm_subdev_post_data("Y8CLDa6OfI", "test_subdev_02", "{\"test_value\":{\"value\":1}, \"test_string\":{\"value\":\"hello, test_subdev_02\"}}", 0, 1);
    // printf("tm_subdev_post_data  reuslt: %d\n", reuslt);

    // // 批量子设备数据上报到平台
    char *data = NULL;
    char *result_data = NULL;
    char *prop_json = "{\"test_value\":{\"value\":1}, \"test_string\":{\"value\":\"hello, test_subdev_01\"}}";

    // // 打包设备的属性和事件数据，可用于子设备。(貌似只使用于子设备)
    result_data = tm_pack_device_data(data, "Y8CLDa6OfI", "test_subdev_01", prop_json, 0, 1);

    data = cJSON_Print(data);
    printf("11 data: [%s]\n", data);
    char *result_data2 = cJSON_Print(result_data);
    printf("11 result_data: [%s]\n", result_data2);

    prop_json = "{\"test_value\":{\"value\":1}, \"test_string\":{\"value\":\"hello, test_subdev_02\"}}";
    result_data = tm_pack_device_data(result_data, "Y8CLDa6OfI", "test_subdev_02", prop_json, 0, 1);

    data = cJSON_Print(data);
    printf("data: [%s]\n", data);
    result_data2 = cJSON_Print(result_data);
    printf("result_data: [%s]\n", result_data2);

    void *data2 = tm_pack_device_data(NULL, "Y8CLDa6OfI", "test_subdev_01", prop_json, "{\"test_value\":{\"value\":1}, \"test_string\":{\"value\":\"hello, test_subdev_01\"}}", 1);

    // 测试：1秒钟内发送100次请求，还是1秒回应一次请求，但只有52次回应
    time_t start_time = time(NULL);
    int num = 0;
    for (num = 0; num < 100; num++)
    {
        // 上报批量数据
        tm_post_pack_data(data2, 10);
        printf("tm_post_pack_data  reuslt");
    }
    printf("time(NULL) - start_time : %d\n", (time(NULL) - start_time));

    // 从平台获取当前网关绑定的子设备信息
    // reuslt = tm_subdev_topo_get(1000);
    // printf("tm_subdev_topo_get  reuslt: %d\n", reuslt);

    // reuslt = tm_subdev_delete("Y8CLDa6OfI", "test_subdev_01", "dBTEGvV7bi/hf0Jq11i9Jd2gzmEpUXS8TnfxW2hfb9Y=", 1);
    // printf("tm_subdev_add  reuslt: %d\n", reuslt);

    // reuslt = tm_subdev_delete("Y8CLDa6OfI", "test_subdev_02", "zsqcmDrW68+J1lnYGz/XCBprBt9MyBPbWrJhHGDSbzQ=", 1);
    // printf("tm_subdev_add  reuslt: %d\n", reuslt);

    // // 从平台获取当前网关绑定的子设备信息
    // // reuslt = tm_subdev_topo_get(1000);
    // // printf("tm_subdev_topo_get  reuslt: %d\n", reuslt);

    // // // 获取平台为设备设置的期望属性值，会段错误，是因为没设置？
    // // reuslt = tm_get_desired_props(100);
    // // printf("tm_get_desired_props  reuslt: %d\n", reuslt);

    // // 子设备登录平台
    // reuslt = tm_subdev_logout("Y8CLDa6OfI", "test_subdev_01", 1);
    // printf("tm_subdev_login  reuslt: %d\n", reuslt);

    // reuslt = tm_subdev_logout("Y8CLDa6OfI", "test_subdev_02", 1);
    // printf("tm_subdev_login  reuslt: %d\n", reuslt);

    // 从平台获取当前网关绑定的子设备信息
    // reuslt = tm_subdev_topo_get(1000);
    // printf("tm_subdev_topo_get  reuslt: %d\n", reuslt);
    // 需要这样进入循环....
    int i = 0;
    while (i < 10000000)
    {
        tm_step(200);
        // if(0 != tm_step(200))
        // {
        //     printf("step failed\n");
        //     break;
        // }

        usleep(50000); // 50ms
    }
    tm_logout(3000);
    return 0;
}