#include "matrix_multiply.h"
#include "matrix.h"

void simple_multiply(float * A, float * B, float * C, int n)
{
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      for (int k = 0; k < n; k++) {
	C[i * n + j] += A[i * n + k] * B[k * n + j];
      }
    }
  }
}

void transposed_multiply(float * A, float * B, float * C, int n)
{

  /*
    * P Nel secondo caso si deve considerare il fatto che nella seconda matrice
  accediamo colonna per colonna, quindi in una rappresentazione row major
  abbiamo i valori delle righe che sono consecutivi in memoria, non quelli delle
  colonne. Si deve quindi cambiare la rappresentazione della seconda matrice
  in modo che sia in forma column major (si veda la figura), ovvero dove i
  dati sono memorizzati in modo che i dati delle colonne siano consecutivi.
  Si implementi poi la moltiplicazione righe per colonne di due matrici in
  cui la prima è in forma row major e la seconda in forma column major.
   */
  // TODO
  float * BT = to_column_major(B, n); 

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      for (int k = 0; k < n; k++) {
	C[i * n + j] += A[i * n + k] * BT[ j* n + k];
      }
    }
  }


}

void kernel(float * A, float * B, float * C, int x, int dx, int y, int dy, int z, int dz, int n)
{
  /*
   * Moltiplicazione di blocchi. Si prende il blocco A[x:x+dx, z:z+dz] e si moltiplica con B[z:z+dz, y+dy]
   * sommando il risultato nella posizioni C[x:x+dx, y:y+dy].
   * Prestare attenzione che x+dx, y+dy e z+dz potenzialmente potrebbero essere maggiori di n,
   * quindi serve limitarsi a n come dimensione.
   */
  // TODO
    for (int i = x; i < x + dx && i < n; i++) {
    for (int j = y; j < y + dy && j < n; j++) {
      for (int k = z; k < z + dz && k < n; k++) {
  C[i * n + j] += A[i * n + k] * B[k * n + j];
      }
    }
  }


  
}

void blocked_multiply(float * A, float * B, float * C, int n)
{
  const int s1 = 16; // Provare inizialmente con 2 o 4
  const int s2 = 16;
  const int s3 = 16;

  // TODO
  for (int i = 0; i < n; i += s1) {
    for (int j = 0; j < n; j += s2) {
      for (int k = 0; k < n; k += s3) {
  kernel(A, B, C, i, s1, j, s2, k, s3, n);
      } 
    }
  }
}
