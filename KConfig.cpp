#include "KConfig.h"

KConfig::KConfig()
{
}

KConfig::KConfig(const char *cfgFile, const char *mode)
{
    m_pfile = fopen(cfgFile,mode);
}

KConfig::~KConfig()
{
    if(m_pfile != NULL)
    {
        fclose(m_pfile);   //<��������ֻ�������Źر��ļ�,������
        delete m_pfile;
        m_pfile = NULL;
    }
}

char* KConfig::getValue(char* valueBuf, const char section[],const char key[])
{
        //<����Ŀ�괮��Ч(����ʧ��ʱ��������NULL,��ֹ����),�ļ���Ч,����section������������
        rewind(m_pfile);
        assert(valueBuf != NULL);
        assert(m_pfile != NULL && !feof(m_pfile));

        size_t j = 0;
        long pos;
        char ch;
        if((pos = isKeyExist(section,key)) != 0)  //<��section:key����
        {
                printf("OK");
                fseek(m_pfile,pos,SEEK_SET);              //<��Ϊ��isKeyExist�н�����rewind()

                do
                {
                        ch = fgetc(m_pfile);
                }
                while(ch == '\t' || ch == ' ' || ch == '=');    //<����=�ź�һЩ�հ�,����������

                while(true)
                {
                        valueBuf[j] = ch;
                        if(valueBuf[j] == '\n' || valueBuf[j] == '\t' || valueBuf[j] == ' ' || valueBuf[j] == EOF)//<ע����ܵ��ļ�β)
                        {
                                break;
                        }
                        ++j;
                        ch = fgetc(m_pfile);
                }

                valueBuf[j] = '\0';
                printf("[%s]:%s = %s\n",section,key,valueBuf);
                rewind(m_pfile);
                return valueBuf;   //<�������ڼ�ֵΨһ��,����key���ظ�,���ҵ�������������غ���
        }
        rewind(m_pfile);  //<�����ļ�ָ��,��Ϊһ���ȡ���ö���һ���Զ�ȡ���,��ÿ�β�����Ҫ��ͷ����
        printf("[%s]:%s not found!\n",section,key);
        return NULL;
}

//<����section:key�Ƿ����,����ֵΪftell(),��ȷ����ѯ��ʼʱ�ļ�ָ�����ļ�ͷ
long KConfig::isKeyExist(const char section[], const char key[])
{
        long secPos;
        if( (secPos = _selectSection(section)) == 0) return 0;
        fseek(m_pfile,secPos,SEEK_SET);

        char ch;
        size_t keyCur = 0;

        while(!feof(m_pfile))
        {
                ch = fgetc(m_pfile);
                if(ch == '[')           //<�����¸�section,�����޴�key,����
                {
                        rewind(m_pfile);  //<�����ļ�ָ��
                        printf("[%s]:%s not found!\n",section,key);
                        return -1;
                }
                if(ch == ';')           //<����ע����
                {
                        while((ch = fgetc(m_pfile)) != '\n');
                }

                if(key[keyCur] == ch)   //<key��ƥ��
                {
                        ++keyCur;
                }
                else
                {
                        keyCur = 0;
                }

                if(key[keyCur] == '\0') //<ƥ��ɹ�
                {
                        printf("[%s]:%s found at %ld\n",section,key,ftell(m_pfile));
                        long pos = ftell(m_pfile);
                        rewind(m_pfile);
                        return pos;   //<�������ڼ�ֵΨһ��,����key���ظ�,���ҵ�������������غ���
                }
        }
        rewind(m_pfile);  //<�����ļ�ָ��,��Ϊһ���ȡ���ö���һ���Զ�ȡ���,��ÿ�β�����Ҫ��ͷ����
        printf("[%s]:%s not found!\n",section,key);
        return 0;       //<δ�ҵ�
}

/*
        ע:
        1.������section��key��valueͬ��
        2.һ��������section��ǩ������һ����,�����ڲ�ֱ��ʹ��MAX_BUF_SIZE
        3.֧�ֵ���ע��value = name ;comment
*/


//<private:
long KConfig::_selectSection(const char section[])
{
        assert(m_pfile != NULL && !feof(m_pfile));
        char line[MAX_BUF_SIZE],secPart[MAX_BUF_SIZE];
        size_t secCur;
        bool isFound = false;

        //<���������ļ�,��section
        while(fgets(line, MAX_BUF_SIZE, m_pfile) != NULL)
        {
                secCur = 0;
                //<��ȡ�����п��ܴ��ڵĵ�һ��[section]��secPart��
                for(size_t lineCur = 0 ;line[lineCur] != '\0'; ++lineCur)       //<[database]��[d        ata base]��һ����
                {
                        if(line[lineCur] != ' ' && line[lineCur] != '\t')
                        {
                                secPart[secCur] = line[lineCur];
                                ++secCur;
                        }
                }

                if(secPart[0]!='[')     //<ԓ�з�section,�򲻺Ϸ�
                {
                        continue;
                }
                else                    //<��strcmp,ƥ��section
                {
                        for(secCur = 0; section[secCur] != '\0'; ++secCur)
                        {
                                if(section[secCur] != secPart[secCur+1]) break;
                        }
                        if(secPart[secCur+1]==']')      //<ƥ�䵽������section
                        {
                                isFound = true;
                                break;
                        }
                }
        }


        if(!isFound)//<��δ�ҵ�section,�����ļ�ָ������ͷ,�����ؿ�,�ҵ��򷵻ض�λ��Section��FILEָ��
        {
                printf("Section:[%s] not found!\n",section);
                rewind(m_pfile);
                return 0;
        }
        else
        {       long pos = ftell(m_pfile);
                rewind(m_pfile);
                return pos;
        }
}

//<����keyֵ,�ּ������: (section�Ƿ����)*(section:key�Ƿ����)
long KConfig::setValue(const char *value, const char section[], const char key[])
{
        assert(value!=NULL && m_pfile!=NULL && !feof(m_pfile));
        long pos;
        char *pbuf = (char*)malloc(1024*sizeof(char));  //<�����ݴ����ֵ�������
        char ch;
        size_t bufCur;

        if((pos = isKeyExist(section,key) ) != 0)//<��section:key����,������
        {
                pbuf[0] = '=';
                bufCur = 1;
                while(value[bufCur-1] != '\0') //<��Ҫд���ֵ�ȴ���bufCur,�����һ������
                {
                        pbuf[bufCur] =  value[bufCur-1];
                        ++bufCur;
                }
                pbuf[bufCur++] = '\n';

                printf("FOUND!!!\n");
                fseek(m_pfile,pos,SEEK_SET);

                // �ȶ�λ����ֵ��ĩβ�ٽ��������ݲ���pbuf
                do
                {
                        ch = fgetc(m_pfile);
                }
                while(ch == '\t' || ch == ' ' || ch == '=');    //<����=�ź�һЩ�հ�,����������

                while(true)
                {
                        ch = fgetc(m_pfile);
                        if(ch == '\n' || ch == '\t' || ch == ' ' || ch == EOF)//<ע����ܵ��ļ�β?)
                        {
                                break;
                        }
                }

                while((ch = fgetc(m_pfile)) != EOF)//<�����������
                {
                       pbuf[bufCur++] = ch;
                }
                pbuf[bufCur] = '\0';

                //<��λ����ֵ��,��������д���ļ�
                fseek(m_pfile,pos,SEEK_SET);
                for(bufCur = 0; pbuf[bufCur] != '\0' ; ++bufCur)
                {

                       fputc(pbuf[bufCur],m_pfile);
                }
                ftruncate(fileno(m_pfile),ftell(m_pfile));  //<�ض��ļ�,���ص�λ�ñ���Ч,��Ҫ��֮ǰ��¼
        }
        else if( (pos = _selectSection(section)) != 0)//<��section����,��key������,������
        {
                bufCur = 0;
                while(key[bufCur] != '\0') //<��Ҫд���ֵ�ȴ���bufCur,�����һ������
                {
                        pbuf[bufCur] =  key[bufCur];
                        ++bufCur;
                }
                pbuf[bufCur++] = '=';

                for(int i = 0; value[i] != '\0' ;++i)
                {
                        pbuf[bufCur++] = value[i];
                }
                pbuf[bufCur++] = '\n';

                fseek(m_pfile,pos,SEEK_SET);

                while((ch = fgetc(m_pfile)) != EOF)//<�����������
                {
                       pbuf[bufCur++] = ch;
                }
                pbuf[bufCur] = '\0';

                //<��λ��section��,��������д���ļ�
                fseek(m_pfile,pos,SEEK_SET);
                for(bufCur = 0; pbuf[bufCur] != '\0' ; ++bufCur)
                {
                       fputc(pbuf[bufCur],m_pfile);
                }
                //<�ļ���������,����ض�
        }
        else//<section������,��Ҫ���ļ�β������section�ͼ�ֵ��
        {
                //<�Ⱥ�[section],key=nameд�뻺����
                pbuf[0] = '\n',pbuf[1] = '[';
                bufCur = 2;
                while(section[bufCur-2] != '\0')
                {
                        pbuf[bufCur] = section[bufCur-2];
                        ++bufCur;
                }
                pbuf[bufCur++] = ']';
                pbuf[bufCur++] = '\n';

                int i;
                for(i = 0; key[i] != '\0' ;++i)
                {
                        pbuf[bufCur++] = key[i];
                }
                pbuf[bufCur++] = '=';

                for(i = 0; value[i] != '\0' ;++i)
                {
                        pbuf[bufCur++] = value[i];
                }
                pbuf[bufCur++] = '\n';
                pbuf[bufCur++] = '\0';

                //<��λ���ļ�β,������section�ͼ�ֵ��
                bufCur = 0;
                fseek(m_pfile,0L,SEEK_END);
                do
                {
                        fputc(pbuf[bufCur++],m_pfile);
                }
                while( pbuf[bufCur] != '\0');
                ftruncate(fileno(m_pfile),ftell(m_pfile));  //<�ض��ļ�
        }

        rewind(m_pfile);
        if(pbuf!=NULL)
        free(pbuf);
        pbuf = NULL;

        return pos;
}
