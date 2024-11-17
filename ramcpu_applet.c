/*
ramcpu_applet.c

A simple panel application to display memory and cpu usage
in the xfce4 pannel using "Generic Monitor"

There are 3 options to display the usage:

n: two Numbers from 0 to 9 (from 0% to 100%)
d: two columns of four Dots
c: two Characters from A to Z
s: speedometer _\|/_ (ugly don't use)

To avoid the dynamic resizing with n and c, use the monospace font

Example: 

user:~$ compiled_app n
user:~$ 37

Here the cpu usage is between 30% and 39% and
the memory usage is between 70% and 79%

user:~$ compiled_app c
user:~$ BK

Here the cpu usage is between 3.8% and 7.7% and
the memory usage is between 38.5% and 42.3%
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <regex.h>

const wchar_t DOTS[5][5] = {{0x2800, 0x2880, 0x28A0, 0x28B0, 0x28B8},
                            {0x2840, 0x28C0, 0x28E0, 0x28F0, 0x28F8},
                            {0x2844, 0x28C4, 0x28E4, 0x28F4, 0x28FC},
                            {0x2846, 0x28C6, 0x28E6, 0x28F6, 0x28FE},
                            {0x2847, 0x28C7, 0x28E7, 0x28F7, 0x28FF}};
//⠀⢀⢠⢰⢸
//⡀⣀⣠⣰⣸
//⡄⣄⣤⣴⣼
//⡆⣆⣦⣶⣾
//⡇⣇⣧⣷⣿

const wchar_t speedometer[] = {0x2015, 0xFF3C, 0x29F9, 0x23D0, 0x29F8, 0xFF0F, 0x2015};
char* cpufile = "/proc/stat";
char* ramfile = "/proc/meminfo";

void extract_group(char* line, regmatch_t* groups, int nbGroups, long* results);
float get_ram_usage();
void get_cpu_snapshot(long* nonIdle, long* idle); // tuple of two long
float get_cpu_usage();

void extract_group(char* line, regmatch_t* groups, int nbGroups, long* results) {
    char copy[strlen(line)+1];
    char temp[nbGroups][128];
    for (int i=0; i<nbGroups; i++) {
        strcpy(copy, line);
        copy[groups[i+1].rm_eo] = 0; // first group is the whole match
        strcpy(temp[i], copy + groups[i+1].rm_so);
        results[i] = strtol(temp[i], NULL, 10);
    }
}


float get_ram_usage() {
    FILE *input;
    input = fopen("/proc/meminfo", "r");
    if (input == NULL)
        exit(EXIT_FAILURE);
    // regex compilation
    regex_t pattern1, pattern2;
    if (regcomp(&pattern1, "MemAvailable:\\s+([0-9]+)", REG_EXTENDED) != 0 ||
        regcomp(&pattern2, "MemTotal:\\s+([0-9]+)", REG_EXTENDED) != 0)
        exit(EXIT_FAILURE);
    // read the file
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    regmatch_t groupArr[2]; // full match + 1 group
    bool patt1Found, patt2Found = false;
    long tempResults[1]; // 1 group expected
    long total, available;
    while ((read = getline(&line, &len, input)) != -1) {
        if (regexec(&pattern1, line, 2, groupArr, 0) == 0) {
            extract_group(line, groupArr, 1, tempResults);
            available = tempResults[0];
            patt1Found = true;
        }
        if (regexec(&pattern2, line, 2, groupArr, 0) == 0) {
            extract_group(line, groupArr, 1, tempResults);
            total = tempResults[0];
            patt2Found = true;
        }
        if (patt1Found==true && patt2Found==true) {
           break;
        }
    }
    regfree(&pattern1);
    regfree(&pattern2);
    fclose(input);

    float usage;
    usage = ((float)total - (float)available) / (float)total;
    return usage;
}

void get_cpu_snapshot(long* nonIdle, long* idle) {
    FILE *input;
    input = fopen(cpufile, "r");
    if (input == NULL)
        exit(EXIT_FAILURE);
    // regex compilation
    regex_t pattern;
    if (regcomp(&pattern, "^cpu\\s+([0-9]+) +([0-9]+) +([0-9]+) +([0-9]+) +([0-9]+) +([0-9]+) +([0-9]+) +([0-9]+) +([0-9]+) +([0-9]+)", REG_EXTENDED) != 0)
        exit(1);
    // read the file
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    int maxGroups = 11; // full match + 10 groups
    long resArr[maxGroups-1];
    regmatch_t groupArr[maxGroups];
    long cpuLoad[2];
    while ((read = getline(&line, &len, input)) != -1) {
        if (regexec(&pattern, line, maxGroups, groupArr, 0) == 0) {
            extract_group(line, groupArr, maxGroups-1, resArr);
            *nonIdle = resArr[0]+resArr[1]+resArr[2]+resArr[5]+resArr[6]+resArr[7];
            *idle = resArr[3]+resArr[4];
        }
    }
    regfree(&pattern);
    fclose(input);
}

float get_cpu_usage() {
    time_t starting;
    starting = time(NULL);
    long idle0, nonIdle0;
    long idle1, nonIdle1;
    get_cpu_snapshot(&nonIdle0, &idle0);
    usleep(100*1000); // 100ms converted into µs
    get_cpu_snapshot(&nonIdle1, &idle1);
    long total0 = idle0 + nonIdle0;
    long total1 = idle1 + nonIdle1;
    long totalDiff = total1 - total0;
    long idleDiff = idle1 - idle0;
    float xxx = ((float)totalDiff - (float)idleDiff) / (float)totalDiff;
    return xxx;
}

void display(char option, float cpu, float ram) {
    option = tolower(option);
    if (option=='c') { // make it scale from "A" to "Z"
        int ru26 = 65 + (int)(ram*26);
        int cu26 = 65 + (int)(cpu*26);
        printf("%c%c\n", cu26, ru26);
    } else if (option=='d') {
        int ruD = ram*5;
        int cuD = cpu*5;
        wprintf(L"%lc\n", DOTS[cuD][ruD]);
    } else if (option=='s') {
        int c = cpu*7;
        int r = cpu*7;
        if (cpu < 3) wprintf(L"%lc  ", speedometer[c]);
        if (cpu == 3) wprintf(L" %lc ", speedometer[c]);
        if (cpu > 3) wprintf(L"  %lc", speedometer[c]);
        //wprintf(L"%lc\n", speedometer[c]
    } else { // default if numbers
        int c = cpu*10;
        int r = ram*10;
        printf("%d%d\n", c, r);
    }
}


int main(int argc, char* argv[]) {
    float ram = get_ram_usage();
    float cpu = get_cpu_usage();
    if (ram>1) ram=1; // just in case
    if (cpu>1) cpu=1;
    setlocale(LC_CTYPE, ""); // for special characters
    if (argc < 2) {
        display('n', cpu, ram);
    } else if (argv[1][0]=='d') {
        display('d', cpu, ram);
    } else if (argv[1][0]=='c') {
        display('c', cpu, ram);
    } else if (argv[1][0]=='s') {
        display('s', cpu, ram);
    } else {
        display('n', cpu, ram);
    }
    return 0;
}
