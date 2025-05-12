#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"

#define INODE_SIZE 32
#define INODE_START_SECTOR 2

int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    int inodes_per_sector = DISKIMG_SECTOR_SIZE/INODE_SIZE;
    int sector = INODE_START_SECTOR + (inumber-1)/inodes_per_sector;
    int offset = (inumber - 1) % inodes_per_sector;
    uint8_t buf[DISKIMG_SECTOR_SIZE];
    if(diskimg_readsector(fs->dfd, sector, buf) < 0) return -1;
    struct inode *inodes = (struct inode *)buf;
    *inp = inodes[offset];
    return 0;
}

int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp,
    int blockNum) {
    if(blockNum < 0) return -1;

    uint16_t block_addr;
    uint8_t buf[DISKIMG_SECTOR_SIZE];
    if((inp->i_mode & ILARG) == 0){  
        if(!(blockNum < 8)) return -1;
        block_addr = inp->i_addr[blockNum];
        }else{  
            int addr_per_block = (DISKIMG_SECTOR_SIZE/sizeof(uint16_t));
            if(blockNum < 7*addr_per_block) {
                int index_block = blockNum/addr_per_block;
                int index_offset = blockNum % addr_per_block;

                if(diskimg_readsector(fs->dfd, inp->i_addr[index_block], buf) < 0) return -1;
                uint16_t *indices = (uint16_t*)buf;
                block_addr = indices[index_offset];
            }else{  
                blockNum -= 7*addr_per_block;
                int double_indirect_index = blockNum/addr_per_block;
                int single_indirect_index = blockNum % addr_per_block;
                if(diskimg_readsector(fs->dfd, inp->i_addr[7], buf) < 0) return -1;
                uint16_t *double_indirect_block = (uint16_t*)buf;
                if(diskimg_readsector(fs->dfd, double_indirect_block[double_indirect_index], buf) < 0) return -1;
                uint16_t *single_indirect_block = (uint16_t*)buf;
                block_addr = single_indirect_block[single_indirect_index];
            }
    }
    return block_addr;
}

int inode_getsize(struct inode *inp) {
    return ((inp->i_size0 << 16) | inp->i_size1);
}
