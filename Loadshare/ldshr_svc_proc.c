
#include <stdlib.h>
#include <math.h>
#include "ldshr.h"

extern double get_max(int n, int mean, int seed);

double result;
double *get_load_1_svc(void* arg, struct svc_req *req)
{
    double loads[3];
    getloadavg(loads, 1);
    result = loads[0];
    return &result;
}

double * get_max_gpu_1_svc(exp_param *param, struct svc_req *req)
{
    result = get_max(param->N, param->M, param->S);
    printf("%f", result);
    return &result;
}

list * upd_list_1_svc(list *lst, struct svc_req *req)
{
    list *ptr = lst;
    while(ptr != NULL) {
        ptr->value = sqrt(ptr->value) * 10;
        ptr = ptr->next;
    }
    return lst;
}
