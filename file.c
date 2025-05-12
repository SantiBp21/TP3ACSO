#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"

int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    struct inode inp;
    if(inode_iget(fs, inumber, &inp) < 0) return -1;
    int sectorNum = inode_indexlookup(fs, &inp, blockNum);
    if(sectorNum < 0) return -1;
    int bytesRead = diskimg_readsector(fs->dfd, sectorNum, buf);
    if(bytesRead < 0) return -1;
    int fileSize = inode_getsize(&inp);

    int blockStart = blockNum*DISKIMG_SECTOR_SIZE;
    int blockEnd = blockStart + DISKIMG_SECTOR_SIZE;
    if(blockEnd > fileSize) return (fileSize - blockStart);
    return DISKIMG_SECTOR_SIZE;
}
