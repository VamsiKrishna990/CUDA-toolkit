
#include <stdlib.h>
#include <math.h>
#include "myldshr.h"

extern double mygetmax(int n, int mean, int seed);

double result;

double *my_get_one_svc(void* arg, struct svc_req *req)
{
    double myld[3];
    getloadavg(myld, 1);
    result = myld[0];
    return &result;
}

double * mygetmaxonesbygpu(MyEP *param, struct svc_req *req)
{
    result = mygetmax(param->NV, param->MV, param->SV);
    return &result;
}

list * myupdlist1svc(list *lst, struct svc_req *req)
{
    int val = 10;
    list *p = lst;
    while(ptr != NULL) {
        p->value = sqrt(ptr->value) * val;
        p = ptr->next;
    }
    return lst;
}
