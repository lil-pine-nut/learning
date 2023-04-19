/**
 * @file onejson.h
 * @author 参照 onenet_studio_sdk 的 tm_data.h
 * @brief
 * @version 0.1
 * @date 2022-06-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __ONE__JSON__H__
#define __ONE__JSON__H__

void *onejson_create_data();
void *onejson_create_array();
void *onejson_create_struct();
void onejson_delete_data(void *data);

// 不加timestamp， 形成 {"xxx":xxx}
void onejson_set_double(void *data, const char *name, double val);
void onejson_set_int32(void *data, const char *name, int val);
void onejson_set_int64(void *data, const char *name, long long val);
void onejson_set_float(void *data, const char *name, float val);
void onejson_set_string(void *data, const char *name, const char *val);
void onejson_set_bool(void *data, const char *name, bool val);
void onejson_set_struct(void *data, const char *name, const char *val);
void onejson_set_array(void *data, const char *name, const char *val);
void onejson_set_data(void *data, const char *name, const char *val);
void onejson_set_enum(void *data, const char *name, int val);
void onejson_set_bitmap(void *data, const char *name, unsigned int val);

// 加timestamp， 形成 {"xxx":{"value":xxx}
void onejson_set_double(void *data, const char *name, double val, unsigned long long timestamp);
void onejson_set_int32(void *data, const char *name, int val, unsigned long long timestamp);
void onejson_set_int64(void *data, const char *name, long long val, unsigned long long timestamp);
void onejson_set_float(void *data, const char *name, float val, unsigned long long timestamp);
void onejson_set_string(void *data, const char *name, const char *val, unsigned long long timestamp);
void onejson_set_bool(void *data, const char *name, bool val, unsigned long long timestamp);
void onejson_set_struct(void *data, const char *name, const char *val, unsigned long long timestamp);
void onejson_set_array(void *data, const char *name, const char *val, unsigned long long timestamp);
void onejson_set_data(void *data, const char *name, const char *val, unsigned long long timestamp);
void onejson_set_enum(void *data, const char *name, int val, unsigned long long timestamp);
void onejson_set_bitmap(void *data, const char *name, unsigned int val, unsigned long long timestamp);

void onejson_array_set_bool(void *array, bool val);
void onejson_array_set_enum(void *array, int val);
void onejson_array_set_int32(void *array, int val);
void onejson_array_set_int64(void *array, long long val);
void onejson_array_set_float(void *array, float val);
void onejson_array_set_double(void *array, double val);
void onejson_array_set_string(void *array, const char *val);
void onejson_array_set_bitmap(void *array, unsigned int val);
void onejson_array_set_data(void *array, void *val);

void onejson_struct_set_bool(void *data, const char *name, bool val);
void onejson_struct_set_enum(void *data, const char *name, int val);
void onejson_struct_set_int32(void *data, const char *name, int val);
void onejson_struct_set_int64(void *data, const char *name, long long val);
void onejson_struct_set_float(void *data, const char *name, float val);
void onejson_struct_set_double(void *data, const char *name, double val);
void onejson_struct_set_string(void *data, const char *name, const char *val);
void onejson_struct_set_bitmap(void *data, const char *name, unsigned int val);
void onejson_struct_set_data(void *data, const char *name, void *val);

bool onejson_get_bool(void *data, bool *val);
bool onejson_get_enum(void *data, int *val);
bool onejson_get_int32(void *data, int *val);
bool onejson_get_int64(void *data, long long *val);
bool onejson_get_float(void *data, float *val);
bool onejson_get_double(void *data, double *val);
bool onejson_get_bitmap(void *data, unsigned int *val);
bool onejson_get_string(void *data, char **val);

bool onejson_struct_get_bool(void *data, const char *name, bool *val);
bool onejson_struct_get_enum(void *data, const char *name, int *val);
bool onejson_struct_get_int32(void *data, const char *name, int *val);
bool onejson_struct_get_int64(void *data, const char *name, long long *val);
bool onejson_struct_get_float(void *data, const char *name, float *val);
bool onejson_struct_get_double(void *data, const char *name, double *val);
bool onejson_struct_get_string(void *data, const char *name, char **val);
bool onejson_struct_get_bitmap(void *data, const char *name, unsigned int *val);

int onejson_get_array_size(void *array);
void *onejson_array_get_element(void *array, unsigned int index);

char *onejson_cJSON2char(void *data);
char *onejson_cJSON2formatchar(void *data);

#endif