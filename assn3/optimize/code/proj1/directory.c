
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>


/*
 * Lookup the specified name (name) in the directory (dirinumber). If found, return the 
 * directory entry in dirEnt. Return 0 on success and something negative on failure. 
 */
int
directory_findname(struct unixfilesystem *fs, const char *name,
                   int dirinumber, struct direntv6 *dirEnt)
{

    // First fetch the inode corresponding to dirinumber
    struct inode in;
    int err = inode_iget(fs, dirinumber, &in);
    if (err < 0) return err;

    // Get the number of blocks
    int size = inode_getsize(&in);
    int numBlocks = (size + DISKIMG_SECTOR_SIZE - 1) / DISKIMG_SECTOR_SIZE;

    // Next access the block that has the directory entries
    int numdirent = DISKIMG_SECTOR_SIZE / sizeof(struct direntv6);
    struct direntv6 dir[numdirent];

    // Cached for the strncp function
    int targetlen = strlen(name);
    
    // Loop through the blocks
    for (int bno = 0; bno < numBlocks; bno++) {

        // Get the number of bytes we want to consider
        int bytes = file_getblock(fs, dirinumber, bno, dir);

        // Loop through the directory entries
        for (int i = 0; i < numdirent; i++) {

            // Use strncmp to see if we have a match on the character name
            if (strncmp(dir[i].d_name, name, targetlen) == 0) {
            
                // Found it! Set values of dirEnt and return success
                strncpy(dirEnt->d_name, dir[i].d_name, targetlen);
                dirEnt->d_inumber = dir[i].d_inumber;
                return 0;

            }

        }
    }

    return -1;

}
