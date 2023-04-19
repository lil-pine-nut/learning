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
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>
#include "cJSON.h"
using namespace std;

#define LOG_WARN(str) cout << time(NULL) << " - " << str << endl
#define LOG_ERROR(str) cout << time(NULL) << " - " << str << endl

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
    cerr << __FUNCTION__ << ", " << topo_data << endl;
    return 0;
}

struct SubDevInfo
{
    string DeviceName;
    string ProductID;
    string SecKey;
};

vector<string> split_string(const string &str, char p)
{
    vector<string> strvec;

    string::size_type pos1, pos2;
    pos2 = str.find(p);
    pos1 = 0;
    while (string::npos != pos2)
    {
        strvec.push_back(str.substr(pos1, pos2 - pos1));

        pos1 = pos2 + 1;
        pos2 = str.find(p, pos1);
    }
    strvec.push_back(str.substr(pos1));

    return strvec;
}

bool ReadDevices(const char *decives_file, vector<SubDevInfo> &SubDevInfo_vec)
{
    FILE *file = fopen(decives_file, "rb");
    if (!file)
    {
        ostringstream oss;
        oss << "open " << decives_file << " Failed!" << endl;
        LOG_ERROR(oss.str().c_str());
        return false;
    }
    char buff[4096];
    bool continue_first = true;
    SubDevInfo tmp_onenet;
    while ((fgets(buff, sizeof(buff), file)) != NULL)
    {
        if (continue_first)
        {
            continue_first = false;
            continue;
        }
        vector<string> split_vec = split_string(buff, ',');
        if (split_vec.size() < 3)
        {
            ostringstream oss;
            oss << "split " << buff << " size < 5 !" << endl;
            LOG_WARN(oss.str().c_str());
            continue;
        }
        tmp_onenet.DeviceName = split_vec[0];
        tmp_onenet.ProductID = split_vec[1];
        tmp_onenet.SecKey = split_vec[2];
        SubDevInfo_vec.push_back(tmp_onenet);
    }
    fclose(file);

    return true;
}

int main(int arg, char **argv[])
{
    struct tm_downlink_tbl_t downlink_tbl = {0};

    // 使用 tm_user.h和tm_user.c，在产品物模型中，下载设备端SDK
    downlink_tbl.prop_tbl = tm_prop_list;
    downlink_tbl.prop_tbl_size = tm_prop_list_size;
    downlink_tbl.svc_tbl = tm_svc_list;
    downlink_tbl.svc_tbl_size = tm_svc_list_size;

    if (0 == tm_init(&downlink_tbl))
    {
        printf("tm init ok\n");
    }

    // tm_login 需要时间长一些
    if (0 == tm_login((int8_t *)"c5qbVecedL", (int8_t *)"gateway_02", (int8_t *)"2S1GIPiD2e3CiaiPFGiDBcAzF1MoUxunHIPNwH0enI0=", 5000))
    {
        printf("tm api login ok\n");
    }
    else
    {
        printf("tm api login failed\n");
        return -1;
    }

    // 测试post数据....
    // char* test_data = cJSON_PrintUnformatted((cJSON *)post_json_data);
    // printf("post_json: [%s]\n", test_data);
    // free(test_data);
    // for (size_t i = 0; i < 50; i++)
    // {
    //     void *post_json_data = tm_data_create();
    //     tm_data_set_string(post_json_data, (int8_t*)"Word", (int8_t*)"hello word...", (time(NULL) - 3600 )*1000);
    //     tm_post_property(post_json_data, 5);
    // }

    // struct tm_subdev_cbs subdev_cbs = {0};
    // subdev_cbs.subdev_props_get = my_subdev_props_get;
    // subdev_cbs.subdev_props_set = my_subdev_props_set;
    // subdev_cbs.subdev_service_invoke = my_subdev_service_invoke;
    // subdev_cbs.subdev_topo = my_subdev_topo;

    // tm_subdev_init(subdev_cbs);

    // 将指定子设备绑定到当前网关设备
    int reuslt;

    vector<SubDevInfo> SubDevInfo_vec;
    if (!ReadDevices("/data4/lws/learning/onenet-mqtt/demo_onenet_studio_sdk/device-export.csv", SubDevInfo_vec))
    {
        cerr << "ReadDevices Failed" << endl;
        return -1;
    }
    cerr << "SubDevInfo_vec.size() = " << SubDevInfo_vec.size() << endl;
    time_t start = time(NULL);
    // // 将指定子设备绑定到当前网关设备
    // for (size_t i = 0; i < SubDevInfo_vec.size(); i++)
    // {
    //     reuslt = tm_subdev_add((int8_t*)SubDevInfo_vec[i].ProductID.c_str(), (int8_t*)SubDevInfo_vec[i].DeviceName.c_str(), (int8_t*)SubDevInfo_vec[i].SecKey.c_str(), 5);
    // }

    // // 子设备登录平台
    // for (size_t i = 0; i < SubDevInfo_vec.size(); i++)
    // {
    //     reuslt = tm_subdev_login((int8_t*)SubDevInfo_vec[i].ProductID.c_str(), (int8_t*)SubDevInfo_vec[i].DeviceName.c_str(), 5);
    // }

    // 子设备登出平台
    // for (size_t i = 0; i < SubDevInfo_vec.size(); i++)
    // {
    //     reuslt = tm_subdev_logout((int8_t*)SubDevInfo_vec[i].ProductID.c_str(), (int8_t*)SubDevInfo_vec[i].DeviceName.c_str(), 1000);
    // }

    // // 子设备解绑网关
    // for (size_t i = 0; i < SubDevInfo_vec.size(); i++)
    // {
    //     reuslt = tm_subdev_delete((int8_t*)SubDevInfo_vec[i].ProductID.c_str(), (int8_t*)SubDevInfo_vec[i].DeviceName.c_str(), (int8_t*)SubDevInfo_vec[i].SecKey.c_str(), 1000);
    // }

    // cerr << "Handle Finish.... Cost:" << time(NULL) - start << endl;

    // 从平台获取当前网关绑定的子设备信息
    // reuslt = tm_subdev_topo_get(1000);
    // printf("tm_subdev_topo_get  reuslt: %d\n", reuslt);

    // // 获取平台为设备设置的期望属性值，会段错误，是因为没设置？
    // reuslt = tm_get_desired_props(100);
    // printf("tm_get_desired_props  reuslt: %d\n", reuslt);

    // 子设备登录平台

    // // 子设备上报数据到平台
    // reuslt = tm_subdev_post_data("Y8CLDa6OfI", "test_subdev_01", "{\"test_value\":{\"value\":1}, \"test_string\":{\"value\":\"hello, test_subdev_01\"}}", 0, 100);
    // printf("tm_subdev_post_data  reuslt: %d\n", reuslt);

    // reuslt = tm_subdev_post_data("Y8CLDa6OfI", "test_subdev_02", "{\"test_value\":{\"value\":1}, \"test_string\":{\"value\":\"hello, test_subdev_02\"}}", 0, 100);
    // printf("tm_subdev_post_data  reuslt: %d\n", reuslt);

    // // 批量子设备数据上报到平台
    // char *data = NULL;
    // char *result_data = NULL;
    // char *prop_json = "{\"test_value\":{\"value\":1}, \"test_string\":{\"value\":\"hello, test_subdev_01\"}}";

    // // 打包设备的属性和事件数据，可用于子设备。(貌似只使用于子设备)
    // result_data = tm_pack_device_data(data, "Y8CLDa6OfI", "test_subdev_01", prop_json, 0, 1);

    // data = cJSON_Print(data);
    // printf("11 data: [%s]\n", data);
    // char *result_data2 = cJSON_Print(result_data);
    // printf("11 result_data: [%s]\n", result_data2);

    // prop_json = "{\"test_value\":{\"value\":1}, \"test_string\":{\"value\":\"hello, test_subdev_02\"}}";
    // result_data = tm_pack_device_data(result_data, "Y8CLDa6OfI", "test_subdev_02", prop_json, 0, 1);

    // data = cJSON_Print(data);
    // printf("data: [%s]\n", data);
    // result_data2 = cJSON_Print(result_data);
    // printf("result_data: [%s]\n", result_data2);

    // // 上报批量数据
    // reuslt = tm_post_pack_data(result_data, 100);
    // printf("tm_post_pack_data  reuslt: %d\n", reuslt);

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
    while (i < 10000)
    {
        if (0 != tm_step(200))
        {
            printf("step failed\n");
            break;
        }

        usleep(50000); // 50ms
    }
    tm_logout(3000);
    return 0;
}