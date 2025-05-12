#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int directory_findname(struct unixfilesystem *fs, const char *name,
    int dirinumber, struct direntv6 *dirEnt) {
    struct inode dirInode;
    if(inode_iget(fs, dirinumber, &dirInode) < 0) return -1;
    int fileSize = inode_getsize(&dirInode);
    int entryNum = fileSize/sizeof(struct direntv6);
    char buf[DISKIMG_SECTOR_SIZE];

    for(int i = 0; i < entryNum; i++){
        int blockNum = i/(DISKIMG_SECTOR_SIZE/sizeof(struct direntv6));
        int bytesRead = file_getblock(fs, dirinumber, blockNum, buf);
        
        if(bytesRead < 0) return -1;

        int offsetInBlock = i % (DISKIMG_SECTOR_SIZE / sizeof(struct direntv6));
        struct direntv6* entry = (struct direntv6*)(buf + offsetInBlock * sizeof(struct direntv6));

        if(strncmp(name, entry->d_name, sizeof(entry->d_name)) == 0){
            *dirEnt = *entry;
            return 0;
        }
    }
    return -1;
}

