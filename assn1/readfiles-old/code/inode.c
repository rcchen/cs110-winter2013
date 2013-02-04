#include <stdio.h>
#include <assert.h>

#include "inode.h"
#include "diskimg.h"



/*
 * Fetch the specified inode from the filesystem. 
 * Return 0 on success, -1 on error.  
 */
int
inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp)
{

    // Figure out the appropriate sector for the inumber
    // int maxinodes = DISKIMG_SECTOR_SIZE / sizeof(struct inode);
    // int startsector = (inumber - 1) / maxinodes + INODE_START_SECTOR;

    // Read the location to the sector for inodes
    // struct inode secbuf[maxinodes];
    // if (!diskimg_readsector(fs->dfd, startsector, secbuf)) {
        return -1;
    }

    // Fetch the correct inode from the sector
    int inodeindex = (inumber - 1) - (startsector - 2) * maxinodes;
    *inp = secbuf[inodeindex];

    // Successful retrieval
    return 0;
  
}

/*
 * Get the location of the specified file block of the specified inode.
 * Return the disk block number on success, -1 on error.  
 */
int
inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum)
{

    printf("I'm here (inode_indexlookup)\n");

    return -1;

}

/* 
 * Compute the size of an inode from its size0 and size1 fields.
 */
int
inode_getsize(struct inode *inp) 
{
  return ((inp->i_size0 << 16) | inp->i_size1); 
}
