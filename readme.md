# 使用creader来读取.ini文件

## 简述

creader（Configration Reader）用来读取.ini文件类型的配置信息, 使用C语言

单文件（creader.c/creader.h）即可使用，全部通过字符串保存

字符串的空间是动态申请，所以参数越少，堆占用空间越少

提供字符串转化函数，可以将字符串转化成整数或者浮点数

## TODO

暂时不支持键值对的设置，也不支持配置导出到文件（因为这个项目叫creader...）

## 使用

使用可以参考 `main.c`

这里简要介绍一下函数的功能

- `creader_t *creader_create(int sections_max, int params_max, int line_max);`
    - 创建并初始化 `creader_t`
    - `sections_max`，`params_max`，`line_max` 分别指明了节的最大数量，参数键值对的最大数量， ini文件最长行的长度（函数内最小约束为32）
    这里只是创建指针的空间，并不会占用太多的空间

- `int creader_load(creader_t *cr, const char *filename);`
    - 将文件 filename 加载到结构体中， 失败返回-1，成功返回0
    - 读取到的节名，键值和值名都会忽略两端的空白字符（' ' '\t' '\n'）
- `void creader_destroy(creader_t *cr);`
    - 将结构体cr占用的堆空间释放
- `void creader_dump(creader_t *cr);`
    - 打印结构体cr读取到的信息
- `char *creader_get(creader_t *cr, const char *section, const char *name);`
    - 从结构体cr读取键值对，section如果为NULL，将从全部参数中读取，否则将从相应节获取
    - 读取时，从后往前读取，如果找到键名，返回值，否则返回NULL
- `int str2long(const char *str, long *number);`
    - 将字符串中第一处连续的数字读取出来，组成long型的整数
    - (因为懒得判断数字大小，所以读出来的数需要根据你的预设来进行强制类型转换，如果你不接受long型的话)
    - 读取成功返回停止时字符串的下标（>0），否则返回-1
- `int str2longarray(const char *str, long *number, int n);`
    - 将字符串中读取n片连续的数字，组成long型的整数数组
    - 返回成功读取到的数量
- `int str2double(const char *str, double *number);`
    - 作用同 `str2long`
- `int str2doublearray(const char *str, double *number, int n);`
    - 作用同 `str2longarray`

## 测试

获取本工程，然后执行 `make` 可以编译出测试程序，然后 `./bin/creader` 即可运行

或者直接使用 `make run` 命令

注意修改 `test.ini` 文件的位置

## license

MIT
