#include "KConfig.h"
using namespace std;
#define BUF_SIZE 1024
int main()
{
        KConfig *cfg = new KConfig();
        char *value = (char*)malloc(BUF_SIZE * sizeof(char));
        FILE *pfile = fopen("D:\\C++ Project\\Configure\\conf.cfg","rw+");

        cfg->getValue(value,pfile,"DataBase","database");
        cfg->getValue(value,pfile,"Test","id");

        fclose(pfile);
        free(value);
        delete cfg;

        return 0;
}
