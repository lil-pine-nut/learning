#include <stdio.h>
#include <cJSON.h>
#include <iostream>
using namespace std;
int main()
{
    // 遍历获取所有key
    const char *json_str = "{\"A\":22,\"B\":\"1111111111\",\"C\":12345,\"D\":123.456}";
    cJSON *item = cJSON_Parse(json_str);
    cJSON *node = NULL;
    node = item->child;
    while (node != NULL)
    {
        printf("%s\n", node->string);
        // if(string(node->string) == "D")
        {
            printf("cJSON_IsNumber: %d\n", cJSON_IsNumber(node));

            if ((node->valuedouble) - ((int)node->valuedouble) > 0)
                printf("valuedouble: %lf\n", node->valuedouble);
            else if (node->valueint != 0)
                printf("valueint: %d\n", node->valueint);
        }
        node = node->next;
    }
    return 0;
}