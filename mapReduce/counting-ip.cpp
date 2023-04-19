#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
// #include<boost/functional/hash.hpp>
#include <string>
#include <ext/hash_map>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/time.h>
#include <unistd.h>
typedef unsigned long long guint64;
guint64 GetCurrentMicroTime()
{
    struct timeval dwStart;
    gettimeofday(&dwStart, NULL);
    guint64 dwTime = 1000000 * dwStart.tv_sec + dwStart.tv_usec;
    return dwTime;
}

using namespace __gnu_cxx;

namespace __gnu_cxx
{
    template <>
    struct hash<std::string> //定义计算std::string的hashcode的类
    {
        std::size_t operator()(const std::string &s) const
        {
            // boost::hash<std::string> string_hash;
            return __stl_hash_string(s.c_str());
        }
    };
}

int cmp(const pair<std::string, int> &x, const pair<std::string, int> &y)
{
    return x.second > y.second;
}

void create_original_file()
{
    FILE *original_file_pointer = fopen("./original_file", "w");
    char str_ip[16];
    int rand_number;
    char str_random[5];
    srand(0);
    for (int i = 0; i < 1000000; i++)
    {
        strcpy(str_ip, "192.168.");
        rand_number = (int)(255 * (rand() / (RAND_MAX + 1.0)));
        sprintf(str_random, "%d", rand_number);
        strcat(str_ip, str_random);
        strcat(str_ip, ".");
        rand_number = (int)(255 * (rand() / (RAND_MAX + 1.0)));
        sprintf(str_random, "%d", rand_number);
        strcat(str_ip, str_random);

        if (i < 999999)
            fprintf(original_file_pointer, "%s\n", str_ip);
        else
            fprintf(original_file_pointer, "%s", str_ip);
    }
    fclose(original_file_pointer);
}

void divide_file()
{
    // boost::hash<std::string> ip_hash;
    std::size_t ip_hash_code;
    FILE *small_files[1000];
    for (int i = 0; i < 1000; i++)
    {
        char th[50] = "./smallfiles/small_file_";
        sprintf(th + 24, "%d", i);
        small_files[i] = fopen(th, "w");
    }
    FILE *original_file_pointer = fopen("./original_file", "r");
    char str_ip[16];
    std::string string_ip;
    for (int i = 0; i < 1000000; i++)
    {
        fscanf(original_file_pointer, "%s", str_ip);
        string_ip = str_ip;
        ip_hash_code = __stl_hash_string(string_ip.c_str());
        fprintf(small_files[ip_hash_code % 1000], "%s\n", str_ip);
    }
    fclose(original_file_pointer);
    for (int i = 0; i < 1000; i++)
    {
        fclose(small_files[i]);
    }
}

struct ip_counter_struct
{
    std::string struct_string_ip;
    int struct_counter;
};

// 把ip_counter_struct struct_map[]进行排序
void sift_down(ip_counter_struct struct_map[], int position, int length)
{
    int j = 2 * position + 1;
    while (j < length)
    {
        if (((j + 1) < length) && (struct_map[j + 1].struct_counter < struct_map[j].struct_counter))
            j = j + 1;
        if (struct_map[position].struct_counter > struct_map[j].struct_counter)
        {
            std::swap(struct_map[position], struct_map[j]);
            position = j;
            j = 2 * position + 1;
        }
        else
            break;
    }
}

void create_heap(ip_counter_struct struct_map[], int length)
{
    int i = (length - 2) / 2;
    while (i > 0)
    {
        sift_down(struct_map, i, length);
        i = (i - 1) / 2;
        std::cout << "i=" << i << std::endl;
    }
    sift_down(struct_map, 0, length);
}

void sortMapByValue(hash_map<std::string, int> &tMap, std::vector<std::pair<std::string, int>> &tVector)
{
    for (hash_map<std::string, int>::iterator curr = tMap.begin(); curr != tMap.end(); curr++)
        tVector.push_back(make_pair(curr->first, curr->second));
    if (!tVector.empty())
        sort(tVector.begin(), tVector.end(), cmp);
}

// 通过 sift_down 堆排序
void statistic()
{
    ip_counter_struct ip_counter_s[3];
    for (int i = 0; i < 3; i++)
    {
        ip_counter_s[i].struct_counter = 0;
    }

    char str_ip[16] = "";
    std::string string_ip;
    for (int i = 0; i < 1000; i++)
    {
        hash_map<std::string, int> ip_counter;
        char th[30] = "./smallfiles/small_file_";
        char statistic_th[35] = "./statistic/statistic_file_";
        sprintf(th + 24, "%d", i);
        sprintf(statistic_th + 27, "%d", i);
        FILE *small_file_pointer = fopen(th, "r");
        FILE *statistic_file_pointer = fopen(statistic_th, "w");
        for (; fscanf(small_file_pointer, "%s", str_ip) != EOF;)
        {
            string_ip = str_ip;
            ip_counter[string_ip] = ip_counter[string_ip] + 1;
        }
        fclose(small_file_pointer);
        hash_map<std::string, int>::iterator hash_map_it = ip_counter.begin();
        int number;
        for (number = 0; number < 3; number++)
        {

            if (hash_map_it != ip_counter.end())
            {
                ip_counter_s[number].struct_string_ip = hash_map_it->first;
                ip_counter_s[number].struct_counter = hash_map_it->second;
                ++hash_map_it;
            }
            else
                break;
        }
        switch (number)
        {
        case 0:
            break;
        case 1:
            fprintf(statistic_file_pointer, "%s ", ip_counter_s[0].struct_string_ip.c_str());
            fprintf(statistic_file_pointer, "%d", ip_counter_s[0].struct_counter);
            break;
        case 2:
            fprintf(statistic_file_pointer, "%s ", ip_counter_s[0].struct_string_ip.c_str());
            fprintf(statistic_file_pointer, "%d\n", ip_counter_s[0].struct_counter);
            fprintf(statistic_file_pointer, "%s ", ip_counter_s[1].struct_string_ip.c_str());
            fprintf(statistic_file_pointer, "%d\n", ip_counter_s[1].struct_counter);
            break;
        case 3:
            sift_down(ip_counter_s, 0, 3);
            // create_heap(ip_counter_s,3);
            while (hash_map_it != ip_counter.end())
            {
                if (hash_map_it->second > ip_counter_s[0].struct_counter)
                {
                    ip_counter_s[0].struct_string_ip = hash_map_it->first;
                    ip_counter_s[0].struct_counter = hash_map_it->second;
                    sift_down(ip_counter_s, 0, 3);
                }
                ++hash_map_it;
            }
            for (int i = 0; i < 3; i++)
            {
                fprintf(statistic_file_pointer, "%s ", ip_counter_s[i].struct_string_ip.c_str());
                fprintf(statistic_file_pointer, "%d\n", ip_counter_s[i].struct_counter);
            }
            break;
        defualt:
            break;
        }
        fclose(statistic_file_pointer);
    }
}

// 通过 转化为vector后用sort 冒泡排序
void statistic2()
{
    ip_counter_struct ip_counter_s[3];
    for (int i = 0; i < 3; i++)
    {
        ip_counter_s[i].struct_counter = 0;
    }

    char str_ip[16] = "";
    std::string string_ip;
    for (int i = 0; i < 1000; i++)
    {
        hash_map<std::string, int> ip_counter;
        char th[30] = "./smallfiles/small_file_";
        char statistic_th[35] = "./statistic/statistic_file_";
        sprintf(th + 24, "%d", i);
        sprintf(statistic_th + 27, "%d", i);
        FILE *small_file_pointer = fopen(th, "r");
        FILE *statistic_file_pointer = fopen(statistic_th, "w");
        for (; fscanf(small_file_pointer, "%s", str_ip) != EOF;)
        {
            string_ip = str_ip;
            ip_counter[string_ip] = ip_counter[string_ip] + 1;
        }
        fclose(small_file_pointer);
        int number;
        std::vector<std::pair<std::string, int>> tVector;
        sortMapByValue(ip_counter, tVector);
        for (number = 0; number < 3; number++)
        {
            if (number < tVector.size())
            {
                ip_counter_s[number].struct_string_ip = tVector[number].first;
                ip_counter_s[number].struct_counter = tVector[number].second;
            }
            else
                break;
        }
        switch (number)
        {
        case 0:
            break;
        case 1:
            fprintf(statistic_file_pointer, "%s ", ip_counter_s[0].struct_string_ip.c_str());
            fprintf(statistic_file_pointer, "%d", ip_counter_s[0].struct_counter);
            break;
        case 2:
            fprintf(statistic_file_pointer, "%s ", ip_counter_s[0].struct_string_ip.c_str());
            fprintf(statistic_file_pointer, "%d\n", ip_counter_s[0].struct_counter);
            fprintf(statistic_file_pointer, "%s ", ip_counter_s[1].struct_string_ip.c_str());
            fprintf(statistic_file_pointer, "%d\n", ip_counter_s[1].struct_counter);
            break;
        case 3:
            for (int i = 0; i < 3; i++)
            {
                fprintf(statistic_file_pointer, "%s ", ip_counter_s[i].struct_string_ip.c_str());
                fprintf(statistic_file_pointer, "%d\n", ip_counter_s[i].struct_counter);
            }
            break;
        defualt:
            break;
        }
        fclose(statistic_file_pointer);
    }
}

void get_total_top_k()
{
    ip_counter_struct ip_counter_s[3];
    for (int i = 0; i < 3; i++)
    {
        ip_counter_s[i].struct_counter = 0;
    }
    int number = 0;
    char str_ip[16] = "";
    int counter = 0;
    for (int i = 0; i < 1000; i++)
    {
        char statistic_th[35] = "./statistic/statistic_file_";
        sprintf(statistic_th + 27, "%d", i);
        FILE *statistic_file_pointer = fopen(statistic_th, "r");
        while (fscanf(statistic_file_pointer, "%s", str_ip) != EOF)
        {
            fscanf(statistic_file_pointer, "%d", &counter);
            if (number < 2)
            {
                ip_counter_s[number].struct_string_ip = str_ip;
                ip_counter_s[number].struct_counter = counter;
                number++;
            }
            else if (number == 2)
            {
                ip_counter_s[number].struct_string_ip = str_ip;
                ip_counter_s[number].struct_counter = counter;
                number++;

                // create_heap(ip_counter_s,3);
                sift_down(ip_counter_s, 0, 3);
            }
            else
            {
                if (counter > ip_counter_s[0].struct_counter)
                {
                    ip_counter_s[0].struct_string_ip = str_ip;
                    ip_counter_s[0].struct_counter = counter;
                    sift_down(ip_counter_s, 0, 3);
                }
                number++;
            }
        }
        fclose(statistic_file_pointer);
    }
    for (int i = 0; i < 3; i++)
        std::cout << ip_counter_s[i].struct_string_ip << " " << ip_counter_s[i].struct_counter << std::endl;
}

int main()
{
    if (access("./smallfiles/", NULL) != 0)
    {
        if (mkdir("./smallfiles/", 0755) == -1)
        {
            printf("mkdir   error\n");
            return 0;
        }
    }

    if (access("./statistic/", NULL) != 0)
    {
        if (mkdir("./statistic/", 0755) == -1)
        {
            printf("mkdir   error\n");
            return 0;
        }
    }

    create_original_file();
    std::cout << "create_original_file finish" << std::endl;
    divide_file();
    std::cout << "divide_file finish" << std::endl;
    guint64 start = GetCurrentMicroTime();
    statistic();
    std::cerr << "statistic cost: " << GetCurrentMicroTime() - start << std::endl;
    std::cout << "statistic finish" << std::endl;
    get_total_top_k();
    std::cout << "get_total_top_k finish" << std::endl;
    start = GetCurrentMicroTime();
    statistic2();
    std::cerr << "statistic2 cost: " << GetCurrentMicroTime() - start << std::endl;
    std::cout << "statistic finish" << std::endl;
    get_total_top_k();
    std::cout << "get_total_top_k finish" << std::endl;
    return 0;
}