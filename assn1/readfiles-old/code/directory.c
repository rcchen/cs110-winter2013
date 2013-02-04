
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

    

    // fprintf(stderr, "directory_lookupname(name=%s dirinumber=%d)\n", name, dirinumber); 
    // return -1;

}
