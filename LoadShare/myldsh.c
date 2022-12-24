
#include "myldshr.h"

#define MCCNT   5
#define MAXADDLEN 32

struct MaInF {
    char name[32];
    char address[32];
};
typedef struct MaInF MaInF;
MaInF mcinf[MCCNT];
void my_load_mcf()
{
    FILE * fp = fopen("mycmf", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    for(int i=0; i<MCCNT; i++)
        memset(&mcinf[i], 0, sizeof(MaInF));
    ssize_t reads; 
    char * line = NULL;
    size_t len = 0;
    int lc = 0;
    while ((reads = getline(&line, &len, fp)) != -1 && lc < MCCNT) {
        if(reads == 0)
            continue;
        int index = 0, sd;
        sd = 0;
        for(index=0; index<reads; index++) {
            if(line[index] == ' ') {
                memcpy(mcinf[lc].name, line, index);
                break;
            }
        }
        sd = index + 1;
        for(index=sd; index<reads; index++) {
            if(line[index] == ' ' || line[index] == '\n') {
                memcpy(mcinf[lc].address, line+sd, index-sd);
                break;
            }
        }
        lc++;
    }
    fclose(fp);
}
CLIENT* getAvailableCient()
{
    CLIENT *mc = NULL;
    CLIENT *getAvailableCient = NULL;
    double mylo = __DBL_MAX__;
    double* tl;
    int machine_idx = 0;
    int i = 0;
    while(i<MCCNT){
        if(strlen(mcinf[i].address) == 0)
            continue;
        mc = clnt_create(mcinf[i].address, PRINTER, PRINTER_V1, "tcp");
        if (mc == NULL) 
            continue;
        tl = get_load_1(NULL, mc);
        if(tl == NULL)
            continue;
        if(mylo > *tl) {
            mylo = *tl;
            getAvailableCient = mc;
            machine_idx = i;
        }
        i++;
    }
    return getAvailableCient;
}
MyList *createlinkedlist(int val)
{
    MyList *list;

    list = (MyList*)malloc(sizeof(list));
    if (lst == NULL)
        return NULL;
    list->value = val;
    list->next = NULL;
    return list;
}
int main(int argc, char *argv[])
{
    if (argc < 2) {
        return 1;
    }
    my_load_mcf();
    CLIENT *cl = getAvailableCient();
    if (cl == NULL) {
        return 1;
    }
    if(strcmp(argv[1], "-max") == 0) {
        if(argc != 5) {
            return 1;            
        }
        MyEP param;
        param.NV = atoi(argv[2]);
        param.MV = atoi(argv[3]);
        param.SV = atoi(argv[4]);
        double *mv = get_max_gpu_1(&param, cl);
        if(mv == NULL) {
            return 1;          
        }
        return 0;
    }
    else if(strcmp(argv[1], "-upd") == 0) {
        if(argc < 3) {
            return 1;            
        }
        int retv = atoi(argv[2]);
        int pc = argc - 2;
        MyList *list1, *newlist;
        l = new = createlinkedlist(retv);
        for(int i=1; i<pc; i++) {
            retv = atoi(argv[2+i]);
            newlist->next = createlinkedlist(retv);
            newlist = newlist->next;
        }   
        MyList* ret = upd_list_1(list1, cl);
        if(ret == NULL) {
            return 1;          
        }
        while (ret != NULL) {
            ret = ret->next;
        }
        return 0;
    }
    else {
        return 1;
    }
}
