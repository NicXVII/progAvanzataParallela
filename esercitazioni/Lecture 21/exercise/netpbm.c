#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "netpbm.h"

int open_image(char * path, netpbm_ptr img)
{
  /*Apre una immagine popolando la struttura img passata come argomento. Si
  suggerisce di ottenere le dimensioni dell’immagine (altezza e larghezza) nonché
  quelle del file e l’offset usando le funzioni fscanf, ftell e stat. Si può poi
  procedere a mappare in memoria il file impostando correttamente il puntatore
  data. Deve ritornare un valore negativo in caso di errore e zero in caso di
  successo.*/

  if (path == NULL || img == NULL) {
    return -1;
  }

  FILE * fd = fopen(path, "r+");
  if (fd == NULL) {
    return -2;
  }

  char magic[3] = {0};
  int max_value = 0;
  if (fscanf(fd, "%2s", magic) != 1 || strcmp(magic, "P5") != 0) {
    fclose(fd);
    return -3;
  }

  if (fscanf(fd, "%d %d", &img->width, &img->height) != 2) {
    fclose(fd);
    return -4;
  }

  if (fscanf(fd, "%d", &max_value) != 1 || max_value != 255) {
    fclose(fd);
    return -5;
  }

  img->offset = (int) ftell(fd);
  if (img->offset < 0) {
    fclose(fd);
    return -6;
  }

  struct stat st;
  if (stat(path, &st) != 0) {
    fclose(fd);
    return -7;
  }

  if (st.st_size < img->offset + (long) img->width * (long) img->height) {
    fclose(fd);
    return -8;
  }

  char * map = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED,
                    fileno(fd), 0);
  if (map == MAP_FAILED) {
    fclose(fd);
    return -9;
  }

  img->size = (int) st.st_size;
  img->fd = fd;
  img->data = map + img->offset;
  printf("Image opened: %s (width: %d, height: %d)\n", path, img->width, img->height);


  return 0;
}

int empty_image(char * path, netpbm_ptr img, int width, int height)
{
  FILE * fd = fopen(path, "w+");
  if (fd == NULL) {
    return -1;
  }
  int written = fprintf(fd, "P5\n%d %d\n255\n", width, height);
  ftruncate(fileno(fd), written + width * height);
  fclose(fd);
  return open_image(path, img);
}

char * pixel_at(netpbm_ptr img, int x, int y)
{
  if (img == NULL || img->data == NULL) {
    return NULL;
  }
  if (x < 0 || x >= img->width || y < 0 || y >= img->height) {
    return NULL;
  }
  return &img->data[y * img->width + x];
}

int close_image(netpbm_ptr img)
{
  if (img == NULL) {
    return -1;
  }

  if (img->data != NULL && img->size > 0) {
    if (munmap(img->data - img->offset, img->size) != 0) {
      return -2;
    }
  }

  if (img->fd != NULL) {
    if (fclose(img->fd) != 0) {
      return -3;
    }
  }

  img->data = NULL;
  img->fd = NULL;
  return 0;
}
