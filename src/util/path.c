#include "path.h"

#define PATH_SEP '/'

int
path_join(const char * path1, const char * path2, char ** pathjoined)
{
    if ((!path1) || (!path2)) {
        return -1;
    }

    int lenpath1 = strlen(path1);
    int lenpath2 = strlen(path2);
    int lenpathjoined = 0;
    int add_seperator = 0;

    if ((lenpath1 != 0)) {
        lenpathjoined = lenpath1;
        if (path1[lenpath1-1] != PATH_SEP) {
            ++lenpathjoined;
            add_seperator = 1;
        }
    }
    if ((lenpath2 != 0)) {
        lenpathjoined += lenpath2;
        if (path2[0] == PATH_SEP) {
            --lenpathjoined;
            add_seperator = 0;
        }
    }

    *pathjoined = calloc(sizeof(char *), lenpathjoined+1);
    if (pathjoined == NULL) {
        return -1;
    }
    strcpy(*pathjoined, path1);
    if (add_seperator==1) {
        (*pathjoined)[lenpath1] = PATH_SEP;
    }
    strcpy((*pathjoined)+(sizeof(char)*(lenpath1+add_seperator)), path2);

    return 0;
}



int
path_join_real(const char * path1, const char * path2, char ** pathjoined)
{
    char * realp = NULL;
    int rc = 0;

    if ((rc = path_join(path1, path2, &realp)) != 0) {
        return rc;
    } 

    if (((*pathjoined) = realpath(realp, NULL)) == NULL) {
        rc = -1;
    }
    free(realp);
    return rc;
}
