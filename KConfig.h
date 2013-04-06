#ifndef KCONFIG_H
#define KCONFIG_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h> //<��Ҫ�õ�ftruncate

class KConfig
{

#define MAX_BUF_SIZE 128

public:
        KConfig();
        KConfig(const char *cfgFile,const char *mode);    //<���������ļ�
        virtual ~KConfig();

        /*@fun  getValue        ����section�ڲ��Ŀո�,tab,��=��value�䲻����\n
         *@param buffer         ���ڴ��value�Ļ�����
         *@param pfile          �Ѵ򿪵��ļ�ָ��
         *@param section        key���ڵ���
         *@param key            ��
         *@return               �ҵ��򷵻�ֵ
        */
        char *getValue(char* valueBuf, const char section[],const char key[]);

        //<����section:key�Ƿ����,����ֵΪftell()
        long isKeyExist(const char section[], const char key[]);

        //<����keyֵ,�ּ������: (section�Ƿ����)*(section:key�Ƿ����)
        long setValue(const char *value, const char section[], const char key[]);

private:
        /*@fun   _selectSection ѡ��ָ��section,��λ�ļ�ָ������section,����̎���^���ļ�ָ�
         *@param pfile          �Ѵ򿪵��ļ�ָ��
         *@param section        section����
         *@return               �ҵ�sectionʱ���ظ�section���ļ�λ��(fseek),
         *                      �Ҳ���ʱ����0
        */
        long _selectSection(const char section[]);

private:
        FILE *m_pfile;            //<�����ļ�ָ��(�ǵ����������ͷ�)
};


//reset,�����ļ�ָ����ļ��^?
//߀Ҫ���]�����]�
//Ŀǰ߀�^�ִ�С��
//<Ҫ���ļ��еĿհ׷��M���A̎��(ȫ��ȥ��,��̎��΂�\n,��������B�m����\n�t�ρ��1��)

/*
        ע:
        1.������section��key��valueͬ��
        2.һ��������section��ǩ������һ����,�����ڲ�ֱ��ʹ��MAX_BUF_SIZE
        3.֧�ֵ���ע��value = name ;comment
*/

#endif // KCONFIG_H
