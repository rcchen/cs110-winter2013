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
    
    // Retrieve the correct inode
    struct inode node;
    if (!inode_iget(fs, inumber, &node))
        return -1;

    // Now get the block index
    int blockNumber = inode_indexlookup(fs, &node, blockNum);
    if (!blockNumber)
        return -1;

    // Now copy the stuff from the block to the buffer
    printf("Got here: inum %d, blocknum %d\n", inumber, blockNum);

    return 0;

    // fprintf(stderr, "file_getblock(inumber = %d, blockNum = %d) unimplemented. returning -1\n", inumber, blockNum);
    // return -1;  
}
