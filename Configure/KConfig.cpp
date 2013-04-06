#include "KConfig.h"

KConfig::KConfig()
{
        //ctor
}

KConfig::~KConfig()
{
        //dtor
}

char* KConfig::getValue(char* valueBuf, FILE *pfile,const char section[],const char key[])
{
        //<断言目标串有效(查找失败时函数返回NULL,终止查找),文件有效,检索section并断言它存在
        rewind(pfile);
        assert(valueBuf != NULL);
        assert(pfile != NULL && !feof(pfile));

        size_t j = 0;
        long pos;
        char ch;
        if((pos = isKeyExist(pfile,section,key)) != 0)  //<该section:key存在
        {
                printf("OK");
                fseek(pfile,pos,SEEK_SET);              //<因为在isKeyExist中进行了rewind()

                do
                {
                        ch = fgetc(pfile);
                }
                while(ch == '\t' || ch == ' ' || ch == '=');    //<跳过=号和一些空白,但不允许换行

                while(true)
                {
                        valueBuf[j] = ch;
                        if(valueBuf[j] == '\n' || valueBuf[j] == '\t' || valueBuf[j] == ' ' || valueBuf[j] == EOF)//<注意可能到文件尾)
                        {
                                break;
                        }
                        ++j;
                        ch = fgetc(pfile);
                }

                valueBuf[j] = '\0';
                printf("[%s]:%s = %s\n",section,key,valueBuf);
                rewind(pfile);
                return valueBuf;   //<由于域内键值唯一性,跨域key可重复,故找到后必须立即返回函数
        }
        rewind(pfile);  //<重置文件指针,因为一般读取配置都会一次性读取多个,而每次查找需要从头进行
        printf("[%s]:%s not found!\n",section,key);
        return NULL;
}

//<检索section:key是否存在,返回值为ftell(),需确保查询开始时文件指针在文件头
long KConfig::isKeyExist(FILE *pfile,const char section[], const char key[])
{
        long secPos;
        if( (secPos = _selectSection(pfile,section)) == 0) return 0;
        fseek(pfile,secPos,SEEK_SET);

        char ch;
        size_t keyCur = 0;

        while(!feof(pfile))
        {
                ch = fgetc(pfile);
                if(ch == '[')           //<遇到下个section,该域无此key,返回
                {
                        rewind(pfile);  //<重置文件指针
                        printf("[%s]:%s not found!\n",section,key);
                        return NULL;
                }
                if(ch == ';')           //<跳过注释行
                {
                        while((ch = fgetc(pfile)) != '\n');
                }

                if(key[keyCur] == ch)   //<key串匹配
                {
                        ++keyCur;
                }
                else
                {
                        keyCur = 0;
                }

                if(key[keyCur] == '\0') //<匹配成功
                {
                        printf("[%s]:%s found at %ld\n",section,key,ftell(pfile));
                        long pos = ftell(pfile);
                        rewind(pfile);
                        return pos;   //<由于域内键值唯一性,跨域key可重复,故找到后必须立即返回函数
                }
        }
        rewind(pfile);  //<重置文件指针,因为一般读取配置都会一次性读取多个,而每次查找需要从头进行
        printf("[%s]:%s not found!\n",section,key);
        return 0;       //<未找到
}

/*
        注:
        1.不允许section与key或value同行
        2.一个完整的section标签必须在一行内,函数内部直接使用MAX_BUF_SIZE
        3.支持单行注释value = name ;comment
*/


//<private:
long KConfig::_selectSection(FILE *pfile,const char section[])
{
        assert(pfile != NULL && !feof(pfile));
        char line[MAX_BUF_SIZE],secPart[MAX_BUF_SIZE];
        size_t secCur;
        bool isFound = false;

        //<按行搜索文件,找section
        while(fgets(line, MAX_BUF_SIZE, pfile) != NULL)
        {
                secCur = 0;
                //<提取该行中可能存在的第一个[section]到secPart中
                for(size_t lineCur = 0 ;line[lineCur] != '\0'; ++lineCur)       //<[database]和[d        ata base]是一样的
                {
                        if(line[lineCur] != ' ' && line[lineCur] != '\t')
                        {
                                secPart[secCur] = line[lineCur];
                                ++secCur;
                        }
                }

                if(secPart[0]!='[')     //<該行非section,或不合法
                {
                        continue;
                }
                else                    //<做strcmp,匹配section
                {
                        for(secCur = 0; section[secCur] != '\0'; ++secCur)
                        {
                                if(section[secCur] != secPart[secCur+1]) break;
                        }
                        if(secPart[secCur+1]==']')      //<匹配到完整的section
                        {
                                isFound = true;
                                break;
                        }
                }
        }


        if(!isFound)//<若未找到section,重置文件指针至其头,并返回空,找到则返回定位到Section的FILE指针
        {
                printf("Section:[%s] not found!\n",section);
                rewind(pfile);
                return 0;
        }
        else
        {       long pos = ftell(pfile);
                rewind(pfile);
                return pos;
        }
}

//<设置key值,分几种情况: (section是否存在)*(section:key是否存在)
long KConfig::setValue(const char *value,FILE *pfile, const char section[], const char key[])
{
        assert(value!=NULL && pfile!=NULL && !feof(pfile));
        long pos;
        char *pbuf = (char*)malloc(1024*sizeof(char));  //<用于暂存插入值后的内容
        char ch;
        size_t bufCur;

        if((pos = isKeyExist(pfile,section,key) ) != 0)//<该section:key存在,做更新
        {
                pbuf[0] = '=';
                bufCur = 1;
                while(value[bufCur-1] != '\0') //<将要写入的值先存于bufCur,后加入一个换行
                {
                        pbuf[bufCur] =  value[bufCur-1];
                        ++bufCur;
                }
                pbuf[bufCur++] = '\n';

                printf("FOUND!!!\n");
                fseek(pfile,pos,SEEK_SET);

                // 先定位到旧值的末尾再将后续数据并入pbuf
                do
                {
                        ch = fgetc(pfile);
                }
                while(ch == '\t' || ch == ' ' || ch == '=');    //<跳过=号和一些空白,但不允许换行

                while(true)
                {
                        ch = fgetc(pfile);
                        if(ch == '\n' || ch == '\t' || ch == ' ' || ch == EOF)//<注意可能到文件尾?)
                        {
                                break;
                        }
                }

                while((ch = fgetc(pfile)) != EOF)//<并入后续数据
                {
                       pbuf[bufCur++] = ch;
                }
                pbuf[bufCur] = '\0';

                //<定位到键值后,将缓冲区写入文件
                fseek(pfile,pos,SEEK_SET);
                for(bufCur = 0; pbuf[bufCur] != '\0' ; ++bufCur)
                {

                       fputc(pbuf[bufCur],pfile);
                }
                ftruncate(fileno(pfile),ftell(pfile));  //<截断文件,返回的位置被无效,需要在之前记录
        }
        else if( (pos = _selectSection(pfile,section)) != 0)//<该section存在,但key不存在,做插入
        {
                bufCur = 0;
                while(key[bufCur] != '\0') //<将要写入的值先存于bufCur,后加入一个换行
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

                fseek(pfile,pos,SEEK_SET);

                while((ch = fgetc(pfile)) != EOF)//<并入后续数据
                {
                       pbuf[bufCur++] = ch;
                }
                pbuf[bufCur] = '\0';

                //<定位到section后,将缓冲区写入文件
                fseek(pfile,pos,SEEK_SET);
                for(bufCur = 0; pbuf[bufCur] != '\0' ; ++bufCur)
                {
                       fputc(pbuf[bufCur],pfile);
                }
                //<文件内容增加,无需截断
        }
        else//<section不存在,需要在文件尾部插入section和键值对
        {
                //<先后将[section],key=name写入缓冲区
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

                //<定位到文件尾,插入新section和键值对
                bufCur = 0;
                fseek(pfile,0L,SEEK_END);
                do
                {
                        fputc(pbuf[bufCur++],pfile);
                }
                while( pbuf[bufCur] != '\0');
                //ftruncate(fileno(pfile),ftell(pfile));  //<截断文件
        }

        rewind(pfile);
        if(pbuf!=NULL)
        free(pbuf);
        pbuf = NULL;

        return pos;
}
