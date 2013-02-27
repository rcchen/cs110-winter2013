#include <stdio.h>
#include <assert.h>

#include "inode.h"
#include "diskimg.h"

#define NUM_BLOCKS 256

/*
 * Fetch the specified inode from the filesystem. 
 * Return 0 on success, -1 on error.  
 */
int
inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp)
{

    // Figure out the sector that the inode resides in
    int numinodes = DISKIMG_SECTOR_SIZE / sizeof(struct inode);
    int sectorNum = INODE_START_SECTOR + (inumber - 1) / numinodes;

    // Now read the contents of the sector into a buffer
    struct inode buf[numinodes];
    if (!diskimg_readsector(fs->dfd, sectorNum, buf))
        return -1; // return on error condition 

    // Write inp to the pointer
    *inp = buf[(inumber - 1) % numinodes];

    // Return int on success
    return 0;
  
}


/*
 * Get the location of the specified file block of the specified inode.
 * Return the disk block number on success, -1 on error.  
 */
int
inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum)
{

    // Check to see if this is a large file or not
    if (inp->i_mode & ILARG) {

        // Handles doubly indirected blocks
        if (blockNum > NUM_BLOCKS * 7 - 1) {

            // Figure out the first layer of indirection
            int first = inp->i_addr[7];
            uint16_t buf[NUM_BLOCKS];
            int firstbytes = diskimg_readsector(fs->dfd, first, buf);
            if (firstbytes == -1)
                return -1;
           
            // Now deal with the second layer
            int firstindex = (blockNum - NUM_BLOCKS * 7) / NUM_BLOCKS;
            int second = buf[firstindex];
            uint16_t buf2[NUM_BLOCKS];
            int secondbytes = diskimg_readsector(fs->dfd, second, buf2);
            if (secondbytes == -1)
                return -1;

            // Finally retrieve the right one
            int secondindex = (blockNum - NUM_BLOCKS * 7) - NUM_BLOCKS * (firstindex);
            int index = buf2[secondindex];

            return index;

        } 
        
        // Handles files without doubly indirected blocks
        else {

            // Figure out the layer of indirection
            int indirectblock = inp->i_addr[blockNum / NUM_BLOCKS];
            uint16_t buf[NUM_BLOCKS];
            int bytesread = diskimg_readsector(fs->dfd, indirectblock, buf);
            if (bytesread == -1)
                return -1;

            // Access the correct block number
            int indirectindex = blockNum - (blockNum / NUM_BLOCKS) * NUM_BLOCKS;

            // printf("%d\n", indirectindex);

            return buf[indirectindex];

        }
    
    } return inp->i_addr[blockNum];

}

/* 
 * Compute the size of an inode from its size0 and size1 fields.
 */
int
inode_getsize(struct inode *inp) 
{

  return ((inp->i_size0 << 16) | inp->i_size1); 

}
