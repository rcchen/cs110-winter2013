#include <stdio.h>
#include <assert.h>

#include "file.h"
#include "inode.h"
#include "diskimg.h"

/*
 * Fetch the specified file block from the specified inode.
 * Return the number of valid bytes in the block, -1 on error.
 */
int
file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf)
{

    // First retrieve the inode in question
    struct inode node;
    int inode_iget_status = inode_iget(fs, inumber, &node);
    if (inode_iget_status == -1)
        return -1;

    // Then we want to get the block
    int block = inode_indexlookup(fs, &node, blockNum);
    if (!block)
        return -1;

    // Now retrive the contents of the block and put it into the buffer
    int sector_status = diskimg_readsector(fs->dfd, block, buf);

    // Figure out what the size was supposed to be
    int inodesize = inode_getsize(&node);

    // Now figure out what the valid number of bytes to return is
    int bytesRemaining = inodesize - blockNum * 512;
    if (bytesRemaining < 512) {
        return bytesRemaining;
    }

    return sector_status;

}
