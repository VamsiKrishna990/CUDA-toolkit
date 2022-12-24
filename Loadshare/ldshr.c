/*
  llist printer RPC client
  by Kalyan Velury
*/

#include "ldshr.h"

#define MACHINE_COUNT   5
#define MAX_ADDRESS_LEN 32

struct MachinInfo {
    char name[32];
    char address[32];
};
typedef struct MachinInfo MachinInfo;

MachinInfo machine_info[MACHINE_COUNT];

void load_machinefile()
{
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    int line_count = 0;

    // initialize address list
    for(int i=0; i<MACHINE_COUNT; i++)
        memset(&machine_info[i], 0, sizeof(MachinInfo));

    fp = fopen("machinefile", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    ssize_t reads; 
    // read addresses
    while ((reads = getline(&line, &len, fp)) != -1 && line_count < MACHINE_COUNT) {
        if(reads == 0)
            continue;
        int i = 0, startIdx;
        startIdx = 0;
        for(i=0; i<reads; i++) {
            if(line[i] == ' ') {
                memcpy(machine_info[line_count].name, line, i);
                break;
            }
        }
        startIdx = i + 1;
        for(i=startIdx; i<reads; i++) {
            if(line[i] == ' ' || line[i] == '\n') {
                memcpy(machine_info[line_count].address, line+startIdx, i-startIdx);
                break;
            }
        }

        line_count++;
    }

    fclose(fp);
}

list *make_list(int value)
{
    list *lst;

    lst = (list*)malloc(sizeof(list));
    if (lst == NULL)
        return NULL;
    lst->value = value;
    lst->next = NULL;
    return lst;
}

void print_usage() 
{
    printf("---------- Usage -----------\n");
    printf("ldshr -max N M S\n");
    printf("ldshr -upd value_1 value_2 [value_n]\n");
}

CLIENT* available_client()
{
    CLIENT *cl = NULL;
    CLIENT *available_client = NULL;
    double load = __DBL_MAX__;
    double* temp_load;
    int machine_idx = 0;
    for(int i=0; i<MACHINE_COUNT; i++) {
        // check address 
        if(strlen(machine_info[i].address) == 0)
            continue;
        // create client
        cl = clnt_create(machine_info[i].address, PRINTER, PRINTER_V1, "tcp");
        if (cl == NULL) 
            continue;
        // get load
        temp_load = get_load_1(NULL, cl);
        if(temp_load == NULL)
            continue;
        // print machine's load
        printf("%s: %.2f ", machine_info[i].name, *temp_load);
        // update available client
        if(load > *temp_load) {
            load = *temp_load;
            available_client = cl;
            machine_idx = i;
        }
    }
    printf("\n");
    printf("(execute on %s)\n", machine_info[machine_idx].name);

    return available_client;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        print_usage();
        return 1;
    }

    // load machine file
    load_machinefile();
    // get available client
    CLIENT *cl = available_client();
    if (cl == NULL) {
        printf("error: could not connect to server.\n");
        return 1;
    }
    // parse command line parameters
    if(strcmp(argv[1], "-max") == 0) {
        if(argc != 5) {
            printf("Invalid command line parameters.\n");
            print_usage();
            return 1;            
        }
        // prepare parameters
        exp_param param;
        param.N = atoi(argv[2]);
        param.M = atoi(argv[3]);
        param.S = atoi(argv[4]);
        // call RPC
        double *max_value = get_max_gpu_1(&param, cl);
        if(max_value == NULL) {
            printf("error: RPC failed!\n");
            return 1;          
        }
        printf("%f\n", *max_value);
        return 0;
    }
    else if(strcmp(argv[1], "-upd") == 0) {
        if(argc < 3) {
            printf("Invalid command line parameters.\n");
            print_usage();
            return 1;            
        }
        // make list
        int param_count = argc - 2;
        int value = atoi(argv[2]);
        list *l, *new;
        l = new = make_list(value);
        for(int i=1; i<param_count; i++) {
            value = atoi(argv[2+i]);
            new->next = make_list(value);
            new = new->next;
        }   
        // call RPC
        list* result = upd_list_1(l, cl);
        if(result == NULL) {
            printf("error: RPC failed!\n");
            return 1;          
        }
        // print result
        while (result != NULL) {
            printf("%.1f ", result->value);
            result = result->next;
        }
        printf("\n");
        return 0;
    }
    else {
        printf("Invalid command line parameters.\n");
        print_usage();
        return 1;
    }
}
