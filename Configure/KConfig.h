#ifndef KCONFIG_H
#define KCONFIG_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

class KConfig
{

#define MAX_BUF_SIZE 128

public:
        KConfig();
        virtual ~KConfig();

        /*@fun  getValue        允许section内部的空格,tab,但=与value间不允许\n
         *@param buffer         用于存放value的缓冲区
         *@param pfile          已打开的文件指针
         *@param section        key所在的域
         *@param key            键
         *@return               找到则返回值
        */
        char *getValue(char* valueBuf, FILE *pfile,const char section[],const char key[]);

        //<检索section:key是否存在,返回值为ftell()
        long isKeyExist(FILE *pfile,const char section[], const char key[]);

        //<设置key值,分几种情况: (section是否存在)*(section:key是否存在)
        long setValue(const char *value,FILE *pfile, const char section[], const char key[]);

private:
        /*@fun   _selectSection 选择指定section,定位文件指针至此section,返回處理過的文件指針
         *@param pfile          已打开的文件指针
         *@param section        section名称
         *@return               找到section时返回该section的文件位移(fseek),
         *                      找不到时返回0
        */
        long _selectSection(FILE *pfile,const char section[]);

private:
        FILE *m_file;            //<配置文件指针(记得在析构中释放)

};


//reset,重置文件指針至文件頭?
//還要考慮屏蔽註釋
//目前還區分大小寫
//<要對文件中的空白符進行預處理(全部去掉,不處理單個\n,但如果有連續多個\n則合併成1個)

/*
        注:
        1.不允许section与key或value同行
        2.一个完整的section标签必须在一行内,函数内部直接使用MAX_BUF_SIZE
        3.支持单行注释value = name ;comment
*/

#endif // KCONFIG_H
