#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

#include <pthread.h>
#include <assert.h>
#include "diskimg.h"
#include "disksim.h"

#include "cachemem.h"
#include <stdlib.h>
#include <string.h>

#include "debug.h"


static uint64_t numreads, numwrites;


/* Open a disk image for I/O.  Returns an open file descriptor, or -1 if
 * unsuccessful  */
int
diskimg_open (char *pathname, int readOnly)
{
  return disksim_open(pathname, readOnly);
}

/*
 * Return the size of the disk image in bytes, or -1 if unsuccessful.
 */
int
diskimg_getsize(int fd)
{
  return disksim_getsize(fd);
}


/*
 * Read the specified sector from the disk.  Return number of bytes read, or -1
 * on error.
 */
int
diskimg_readsector(int fd, int sectorNum, void *buf)
{
  numreads++;
  
  for (int i = 0; i < sectorsFilled; i++)
  {
	if (sectors[i] == sectorNum)
	{
	  memcpy(buf, (char*)cacheMemPtr + 512 * i, 512);
	  return 512;
	}
  }

  int bytesRead = disksim_readsector(fd, sectorNum, buf);

  int sectorIndex = sectorsFilled;
  if (sectorsFilled == numSectors - 1)
    sectorIndex = rand() % numSectors;
  else
	sectorsFilled++;

  sectors[sectorIndex] = sectorNum;
  memcpy((char*)cacheMemPtr + 512 * sectorIndex, buf, 512);

  return bytesRead;
  //return disksim_readsector(fd, sectorNum, buf);
}
/*
 * Write the specified sector to the disk.  Return number of bytes written,
 * -1 on error.
 */
int diskimg_writesector(int fd, int sectorNum, void *buf)
{
  numwrites++;
  return disksim_writesector(fd, sectorNum, buf);
}

/*
 * Clean up from a previous diskimg_open() call.  Returns 0 on success, -1 on
 * error
 */
int
diskimg_close(int fd)
{
  return disksim_close(fd);
}

void
diskimg_dumpstats(FILE *file)
{
  fprintf(file, "Diskimg: %"PRIu64" reads, %"PRIu64" writes\n",
          numreads, numwrites);
}
