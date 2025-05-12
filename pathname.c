#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
    if(pathname[0] != '/') return -1;
    struct direntv6 dirEnt;
    int inumber = ROOT_INUMBER;
    char pathCopy[strlen(pathname)+1];  
    strcpy(pathCopy, pathname);
    char *token = strtok(pathCopy, "/");

    while(token != NULL){
        if(directory_findname(fs, token, inumber, &dirEnt) < 0) return -1;
        inumber = dirEnt.d_inumber;
        token = strtok(NULL, "/");
    }
    return inumber;
}
