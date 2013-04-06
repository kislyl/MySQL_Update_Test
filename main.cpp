#include <stdio.h>
#include "winsock2.h"
#include "mysql.h"
#include <stdio.h>
#include <time.h>

#include <stdlib.h>
#include "KConfig.h"

int main()
{

    //��ȡ�����ļ�
    KConfig *cfg = new KConfig("config.txt","r");
    char host[16];
    char user[16];
    char pwd[8];
    char database[16];
    char port[8];
    char itemId[8];

    cfg->getValue(host,"database","host");
    cfg->getValue(user,"database","user");
    cfg->getValue(pwd,"database","password");
    cfg->getValue(port,"database","port");
    cfg->getValue(database,"database","database");
    cfg->getValue(itemId,"database","id");

    //unsigned long id = 35787530;

    MYSQL *conn = mysql_init(NULL);
    if(!conn){printf("error");}
    if(!mysql_real_connect(conn,host,user,pwd,database,atoi(port),NULL,0))
    {
        return -1;
    }
    printf("Connecting mysql database succeed.");

    //<�����
    unsigned long rNum;
    srand(unsigned(time(NULL)));
    int top;
    char strValue[16] = ""; //<��������ַ�����ʽ

    char update[128] = "";   //<sql���
    char query[128] = "";

    MYSQL_RES* res;
    MYSQL_ROW row = NULL;

    FILE *logFile = fopen("file.dat","w");
    char errorLog[64];

    while(true)
    {
        //<generate random number
        top = (rand()%10+1)* 1000000000;    //<��֤���λ,�м����top����ȥ��
        rNum = rand()%99999999+top;     //<overflow possible?
        printf("%lu\n",rNum);

        sprintf(strValue,"%lu",rNum);
        sprintf(update,"update cq_item set chk_sum=%s where id=%lu ",strValue,(long unsigned int)atoi(itemId));
        sprintf(query,"select id,chk_sum from cq_item where id=%lu limit 1",(long unsigned int)atoi(itemId));
        printf("%s%s",query,update);

        //cout<<update;
        mysql_query(conn,update);
        Sleep(100);
        //<select
        mysql_query(conn,query);

        //<��ѯ������
        res = mysql_store_result(conn);
        while (NULL != (row = mysql_fetch_row(res)))
        {
            for (unsigned int i = 0; i < 2; ++i)
            {
                //logFile<<row[i]<<"\t";
                if((unsigned long)atol(row[1]) != rNum)
                    sprintf(errorLog,"Error found!\terror value:\t%lu\n",(unsigned long)row[1]);
                    fwrite(errorLog,sizeof(errorLog),1,logFile);    //<����3��ʾд�����
            }
        }
        mysql_free_result(res); //<�ͷ�mysql_store_result,�����ڴ汩����
    }
    mysql_close(conn);
}
