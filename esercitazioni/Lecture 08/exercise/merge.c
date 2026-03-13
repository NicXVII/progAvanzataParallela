#include "merge.h"

void merge(int * v1, int n1, int * v2, int n2, int * results)
{
  // TODO
  int i = 0, j = 0, k = 0;
  while (i < n1 && j < n2) {
    if (v1[i] < v2[j]) {
      results[k] = v1[i];
      i++;
    } else {
      results[k] = v2[j];
      j++;
    }
    k++;
  }
  while (i < n1) {
  results[k++] = v1[i++];
  }
  while (j < n2) {
    results[k++] = v2[j++];
  }
  }

void merge_branchless(int * v1, int n1, int * v2, int n2, int * results)
{
  // TODO
  int i = 0, j = 0, k = 0;
  while (i < n1 && j < n2) {
    int take_v1 = (v1[i] < v2[j]);  
    results[k] = take_v1 ? v1[i] : v2[j];
    i += take_v1;
    j += 1 - take_v1;
    k++;
  }
  while (i < n1) {
  results[k++] = v1[i++];
  }
  while (j < n2) {
    results[k++] = v2[j++];
  }
}
