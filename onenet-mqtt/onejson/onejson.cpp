#include "cJSON.h"
#include <stdio.h>
#include <stdarg.h>

void *onejson_create_data()
{
    return (void *)cJSON_CreateObject();
}

void *onejson_create_array()
{
    return (void *)cJSON_CreateArray();
}

void *onejson_create_struct()
{
    return (void *)cJSON_CreateObject();
}

void onejson_delete_data(void *data)
{
    cJSON_Delete((cJSON *)data);
}

static void set_value(cJSON *data, const char *name, cJSON *value)
{
    if (cJSON_IsArray(data))
    {
        if (value)
        {
            cJSON_AddItemToArray(data, value);
        }
    }
    else
    {
        if (value)
        {
            cJSON_AddItemToObject(data, name, value);
        }
        else
        {
            cJSON_AddObjectToObject(data, name);
        }
    }
}

int osl_sprintf(char *str, const char *format, ...)
{
    va_list ap;
    int ret = 0;

    va_start(ap, format);
    ret = vsprintf(str, format, ap);
    va_end(ap);

    return ret;
}

static double float_to_double(float val)
{
    char tmp_val1[16] = {0};
    double tmp_val2 = 0;

    osl_sprintf(tmp_val1, "%lg", val);
    sscanf(tmp_val1, "%lf", &tmp_val2);
    return tmp_val2;
}

void onejson_set_double(void *data, const char *name, double val)
{
    set_value((cJSON *)data, name, cJSON_CreateNumber(val));
}

void onejson_set_int32(void *data, const char *name, int val)
{
    set_value((cJSON *)data, name, cJSON_CreateNumber(val));
}

void onejson_set_int64(void *data, const char *name, long long val)
{
    set_value((cJSON *)data, name, cJSON_CreateNumber(val));
}

void onejson_set_float(void *data, const char *name, float val)
{
    set_value((cJSON *)data, name, cJSON_CreateNumber(float_to_double(val)));
}

void onejson_set_string(void *data, const char *name, const char *val)
{
    set_value((cJSON *)data, name, cJSON_CreateString(val));
}

void onejson_set_bool(void *data, const char *name, bool val)
{
    set_value((cJSON *)data, name, cJSON_CreateBool(val));
}

void onejson_set_struct(void *data, const char *name, const char *val)
{
    set_value((cJSON *)data, name, (cJSON *)val);
}

void onejson_set_array(void *data, const char *name, const char *val)
{
    set_value((cJSON *)data, name, (cJSON *)val);
}

void onejson_set_data(void *data, const char *name, const char *val)
{
    set_value((cJSON *)data, name, (cJSON *)val);
}

void onejson_set_enum(void *data, const char *name, int val)
{
    set_value((cJSON *)data, name, cJSON_CreateNumber(val));
}

void onejson_set_bitmap(void *data, const char *name, unsigned int val)
{
    set_value((cJSON *)data, name, cJSON_CreateNumber(val));
}

static void set_value_with_timestamp(cJSON *data, const char *name, cJSON *value, long long ts_in_ms)
{
    cJSON *sub = NULL;
    cJSON *target = cJSON_GetObjectItem(data, name);
    cJSON *value_item, *time_item = NULL;

    if (target)
    {
        if (ts_in_ms)
        {
            time_item = cJSON_GetObjectItem(target, "time");
            if (time_item)
            {
                cJSON_SetIntValue(time_item, ts_in_ms);
            }
            else
            {
                cJSON_AddItemToObject(target, "time", cJSON_CreateNumber(ts_in_ms));
            }
        }

        value_item = cJSON_GetObjectItem(target, "value");
        if (cJSON_Array != value_item->type)
        {
            cJSON *item = cJSON_DetachItemFromObject(target, "value");
            cJSON *array = cJSON_CreateArray();
            cJSON_AddItemToArray(array, item);
            cJSON_AddItemToArray(array, value);
            cJSON_AddItemToObject(target, "value", array);
        }
        else
        {
            cJSON_AddItemToArray(value_item, value);
        }
    }
    else
    {
        sub = cJSON_CreateObject();
        cJSON_AddItemToObject(sub, "value", value);
        if (ts_in_ms)
        {
            cJSON_AddItemToObject(sub, "time", cJSON_CreateNumber(ts_in_ms));
        }

        if (cJSON_IsArray(data))
        {
            cJSON_AddItemToArray(data, sub);
        }
        else
        {
            cJSON_AddItemToObject(data, name, sub);
        }
    }
}

void onejson_set_double(void *data, const char *name, double val, unsigned long long timestamp)
{
    set_value_with_timestamp((cJSON *)data, name, cJSON_CreateNumber(val), timestamp);
}

void onejson_set_int32(void *data, const char *name, int val, unsigned long long timestamp)
{
    set_value_with_timestamp((cJSON *)data, name, cJSON_CreateNumber(val), timestamp);
}

void onejson_set_int64(void *data, const char *name, long long val, unsigned long long timestamp)
{
    set_value_with_timestamp((cJSON *)data, name, cJSON_CreateNumber(val), timestamp);
}

void onejson_set_float(void *data, const char *name, float val, unsigned long long timestamp)
{
    set_value_with_timestamp((cJSON *)data, name, cJSON_CreateNumber(float_to_double(val)), timestamp);
}

void onejson_set_string(void *data, const char *name, const char *val, unsigned long long timestamp)
{
    set_value_with_timestamp((cJSON *)data, name, cJSON_CreateString(val), timestamp);
}

void onejson_set_bool(void *data, const char *name, bool val, unsigned long long timestamp)
{
    set_value_with_timestamp((cJSON *)data, name, cJSON_CreateBool(val), timestamp);
}

void onejson_set_struct(void *data, const char *name, const char *val, unsigned long long timestamp)
{
    set_value_with_timestamp((cJSON *)data, name, (cJSON *)val, timestamp);
}

void onejson_set_array(void *data, const char *name, const char *val, unsigned long long timestamp)
{
    set_value_with_timestamp((cJSON *)data, name, (cJSON *)val, timestamp);
}

void onejson_set_data(void *data, const char *name, const char *val, unsigned long long timestamp)
{
    set_value_with_timestamp((cJSON *)data, name, (cJSON *)val, timestamp);
}

void onejson_set_enum(void *data, const char *name, int val, unsigned long long timestamp)
{
    set_value_with_timestamp((cJSON *)data, name, cJSON_CreateNumber(val), timestamp);
}

void onejson_set_bitmap(void *data, const char *name, unsigned int val, unsigned long long timestamp)
{
    set_value_with_timestamp((cJSON *)data, name, cJSON_CreateNumber(val), timestamp);
}

void onejson_array_set_bool(void *array, bool val)
{
    set_value((cJSON *)array, NULL, cJSON_CreateBool(val));
}

void onejson_array_set_enum(void *array, int val)
{
    set_value((cJSON *)array, NULL, cJSON_CreateNumber(val));
}

void onejson_array_set_int32(void *array, int val)
{
    set_value((cJSON *)array, NULL, cJSON_CreateNumber(val));
}

void onejson_array_set_int64(void *array, long long val)
{
    set_value((cJSON *)array, NULL, cJSON_CreateNumber(val));
}

void onejson_array_set_float(void *array, float val)
{
    set_value((cJSON *)array, NULL, cJSON_CreateNumber(float_to_double(val)));
}

void onejson_array_set_double(void *array, double val)
{
    set_value((cJSON *)array, NULL, cJSON_CreateNumber(val));
}

void onejson_array_set_string(void *array, const char *val)
{
    return set_value((cJSON *)array, NULL, cJSON_CreateString(val));
}

void onejson_array_set_bitmap(void *array, unsigned int val)
{
    set_value((cJSON *)array, NULL, cJSON_CreateNumber(val));
}

void onejson_array_set_data(void *array, void *val)
{
    return set_value((cJSON *)array, NULL, (cJSON *)val);
}

void onejson_struct_set_bool(void *data, const char *name, bool val)
{
    set_value((cJSON *)data, name, cJSON_CreateBool(val));
}

void onejson_struct_set_enum(void *data, const char *name, int val)
{
    set_value((cJSON *)data, name, cJSON_CreateNumber(val));
}

void onejson_struct_set_int32(void *data, const char *name, int val)
{
    set_value((cJSON *)data, name, cJSON_CreateNumber(val));
}

void onejson_struct_set_int64(void *data, const char *name, long long val)
{
    set_value((cJSON *)data, name, cJSON_CreateNumber(val));
}

void onejson_struct_set_float(void *data, const char *name, float val)
{
    set_value((cJSON *)data, name, cJSON_CreateNumber(float_to_double(val)));
}

void onejson_struct_set_double(void *data, const char *name, double val)
{
    set_value((cJSON *)data, name, cJSON_CreateNumber(val));
}

void onejson_struct_set_string(void *data, const char *name, const char *val)
{
    set_value((cJSON *)data, name, cJSON_CreateString(val));
}

void onejson_struct_set_bitmap(void *data, const char *name, unsigned int val)
{
    set_value((cJSON *)data, name, cJSON_CreateNumber(val));
}

void onejson_struct_set_data(void *data, const char *name, void *val)
{
    set_value((cJSON *)data, name, (cJSON *)val);
}

bool parse_bool(void *data, bool *val)
{
    cJSON *obj = (cJSON *)data;

    if ((NULL == data) || (NULL == val))
        return false;

    if (cJSON_True == obj->type)
    {
        *val = 1;
    }
    else
    {
        *val = 0;
    }
    return true;
}

static cJSON *get_value_item(cJSON *obj)
{
    if (cJSON_IsObject(obj))
    {
        return cJSON_GetObjectItem(obj, "value");
    }
    else
    {
        return obj;
    }
}

bool parse_number(void *data, double *val)
{
    cJSON *obj = (cJSON *)data;

    if ((NULL == data) || (NULL == val))
        return false;

    *val = get_value_item(obj)->valuedouble;

    return true;
}

bool parse_string(void *data, char **val)
{
    cJSON *obj = (cJSON *)data;

    if ((NULL == data) || (NULL == val))
        return false;

    *val = get_value_item(obj)->valuestring;

    return true;
}

bool onejson_get_bool(void *data, bool *val)
{
    return parse_bool(data, val);
}

bool onejson_get_enum(void *data, int *val)
{
    double num = 0;

    if (parse_number(data, &num))
    {
        *val = (int)num;
        return true;
    }
    return false;
}

bool onejson_get_int32(void *data, int *val)
{
    double num = 0;

    if (parse_number(data, &num))
    {
        *val = (int)num;
        return true;
    }
    return false;
}

bool onejson_get_int64(void *data, long long *val)
{
    double num = 0;

    if (parse_number(data, &num))
    {
        *val = (long long)num;
        return true;
    }
    return false;
}

bool onejson_get_float(void *data, float *val)
{
    double num = 0;

    if (parse_number(data, &num))
    {
        *val = (float)num;
        return true;
    }
    return false;
}

bool onejson_get_double(void *data, double *val)
{
    return parse_number(data, val);
}

bool onejson_get_bitmap(void *data, unsigned int *val)
{
    double num = 0;

    if (parse_number(data, &num))
    {
        *val = (unsigned int)num;
        return true;
    }
    return false;
}

bool onejson_get_string(void *data, char **val)
{
    return parse_string(data, val);
}

void *get_data_by_name(void *data, const char *name)
{
    return (void *)cJSON_GetObjectItem((cJSON *)data, name);
}

bool onejson_struct_get_bool(void *data, const char *name, bool *val)
{
    void *get_data = get_data_by_name(data, name);

    if (get_data)
    {
        return onejson_get_bool(get_data, val);
    }
    else
    {
        return false;
    }
}

bool onejson_struct_get_enum(void *data, const char *name, int *val)
{
    void *get_data = get_data_by_name(data, name);

    if (get_data)
    {
        return onejson_get_enum(get_data, val);
    }
    else
    {
        return false;
    }
}

bool onejson_struct_get_int32(void *data, const char *name, int *val)
{
    void *get_data = get_data_by_name(data, name);

    if (get_data)
    {
        return onejson_get_int32(get_data, val);
    }
    else
    {
        return false;
    }
}

bool onejson_struct_get_int64(void *data, const char *name, long long *val)
{
    void *get_data = get_data_by_name(data, name);

    if (get_data)
    {
        return onejson_get_int64(get_data, val);
    }
    else
    {
        return false;
    }
}

bool onejson_struct_get_float(void *data, const char *name, float *val)
{
    void *get_data = get_data_by_name(data, name);

    if (get_data)
    {
        return onejson_get_float(get_data, val);
    }
    else
    {
        return false;
    }
}

bool onejson_struct_get_double(void *data, const char *name, double *val)
{
    void *get_data = get_data_by_name(data, name);

    if (get_data)
    {
        return onejson_get_double(get_data, val);
    }
    else
    {
        return false;
    }
}

bool onejson_struct_get_string(void *data, const char *name, char **val)
{
    void *get_data = get_data_by_name(data, name);

    if (get_data)
    {
        return onejson_get_string(get_data, val);
    }
    else
    {
        return false;
    }
}

bool onejson_struct_get_bitmap(void *data, const char *name, unsigned int *val)
{
    void *get_data = get_data_by_name(data, name);

    if (get_data)
    {
        return onejson_get_bitmap(get_data, val);
    }
    else
    {
        return false;
    }
}

int onejson_get_array_size(void *array)
{
    return cJSON_GetArraySize((cJSON *)array);
}

void *onejson_array_get_element(void *array, unsigned int index)
{
    return (void *)cJSON_GetArrayItem((cJSON *)array, index);
}

char *onejson_cJSON2char(void *data)
{
    // cJSON_PrintUnformatted()与cJSON_Print()类似，只是打印输出不带格式，而只是一个字符串数据
    return cJSON_PrintUnformatted((cJSON *)data);
}

char *onejson_cJSON2formatchar(void *data)
{
    return cJSON_Print((cJSON *)data);
}