#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "onejson.h"
using namespace std;

string itoa(int num, const char *format = "%06u")
{
    char buff[12] = {0};
    sprintf(buff, format, num);
    return buff;
}

int main()
{

    void *data = onejson_create_data();
    // 组成 onenet中的 struct
    void *struct_data = onejson_create_struct();
    onejson_set_int32(struct_data, "test1", 1888);
    onejson_set_string(struct_data, "test2", "nihao?");
    onejson_set_data(data, "test", (char *)struct_data, 0);

    // 读取struct
    int int_val = 0;
    char *char_vale = NULL;
    onejson_struct_get_int32(struct_data, "test1", &int_val);
    onejson_struct_get_string(struct_data, "test2", &char_vale);
    cerr << int_val << endl;
    cerr << char_vale << endl;

    // 生成array
    int element_cnt = 3;
    void *array_data = onejson_create_array();
    for (int i = 0; i < element_cnt; i++)
    {
        onejson_set_string(array_data, NULL, itoa(i).c_str());
    }
    onejson_set_array(data, "array_test", (char *)array_data, 0);

    // 读取array
    int array_size = onejson_get_array_size(array_data);
    cerr << "array_size = " << array_size << endl;
    char *val = NULL;
    void *element = NULL;
    for (int i = 0; i < array_size; i++)
    {
        element = onejson_array_get_element(array_data, i);
        onejson_get_string(element, &val);
        cerr << val << endl;
    }

    char *test_data = onejson_cJSON2char(data);
    printf("%s\n", test_data);
    free(test_data);
    onejson_delete_data(data);
}