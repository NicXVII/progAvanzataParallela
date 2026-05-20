#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[])
{
  // 1. apri il file
FILE *fd = fopen("file.txt", "r");

// 2. ottieni il file descriptor
int fdnum = fileno(fd);

// 3. ottieni la dimensione
struct stat sbuf;
stat("file.txt", &sbuf);

// 4. mappa in memoria
char *data = mmap((void*)0, sbuf.st_size,
                  PROT_READ, MAP_SHARED, fdnum, 0);

// 5. usa come array!
printf("%c", data[0]);

// 6. chiudi
munmap(data, sbuf.st_size);
fclose(fd);
  }
  for (int i = 0; i < n; i++) {
    arr[i] = i;
  }
  for (int i = 0; i < n; i++) {
    printf("%d ", arr[i]);
  }
  printf("\n");
  munmap(arr, size);
  return 0;
}