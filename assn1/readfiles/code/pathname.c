
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>



/*
 * Return the inumber associated with the specified pathname. This need only
 * handle absolute paths. Return a negative number if an error is encountered.
 */
int
pathname_lookup(struct unixfilesystem *fs, const char *pathname)
{

    char* path = strdup(pathname);
    char* tok = NULL;
    tok = strtok(path,"/");
    int direntry = ROOT_INUMBER;
    while (tok != NULL)
    {
        struct direntv6 dirEnt;
        //printf("dirent: %s\n", dirEnt.d_name);
        if (directory_findname(fs, tok, direntry, &dirEnt) != 0)
            return direntry;
        direntry = dirEnt.d_inumber;
        tok = strtok(NULL,"/");
    }

    return direntry;

}
