#include <stdio.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <omp.h>

void swap(int64_t *ptr1, int64_t *ptr2 ){
        int64_t temp;
        temp = *ptr1;
        *ptr1 = *ptr2;
        *ptr2 = temp;
    }


int64_t choosePivot(int64_t *a, int64_t lo, int64_t hi){
    return ((lo+hi)/2);
}

int64_t partitionArray(int64_t *a, int64_t lo, int64_t hi){
    int64_t pivotIndex, pivotValue, storeIndex;
    pivotIndex = choosePivot(a, lo, hi);
    pivotValue = a[pivotIndex];
    (void) swap(&a[hi], &a[pivotIndex]);
    storeIndex = lo;
    for (int64_t i=lo; i<= hi-1; i++){
        if (a[i] < pivotValue){
            (void) swap(&a[i], &a[storeIndex]);
            storeIndex++;
        }
    }
    (void) swap(&a[storeIndex],&a[hi]);
    return storeIndex;
}


void Quicksort(int64_t *a, int64_t lo, int64_t hi){
    if (lo < hi){
        int64_t p = partitionArray(a, lo, hi);

#pragma omp task shared(a) firstprivate(lo,p)
        {
            (void) Quicksort(a, lo, p-1);
        } //Left branch

(void) Quicksort(a, p+1, hi); //Right branch

#pragma omp taskwait
    }
}


/* -----------------------------------------------------------------------
   Tests the result
 * ----------------------------------------------------------------------- */
void testit(int64_t *v, int64_t * nelements) {
  register int k;
	int not_sorted = 0;
  for (k = 0; k < (*nelements) - 1; k++)
    if (v[k] > v[k + 1]) {
      not_sorted = 1;
      break;
    }
    if (not_sorted)
    {
    printf("Array NOT sorted.\n");
        exit(1);
    }
    
//   if (not_sorted)
//     printf("Array NOT sorted.\n");
// 	else
//     printf("Array sorted.\n");
}

/* -----------------------------------------------------------------------
   Sets randomly the values for the array
 * ----------------------------------------------------------------------- */
void initialize(int64_t *v, int64_t seed, int64_t nelements) {
  unsigned i;
   srandom(seed);
   for(i = 0; i < nelements; i++)
     v[i] = (int64_t)random();
}

int main(int argc, char** argv) {
    // #pragma omp parallel
    // {
    // printf("this\n");
    // printf("is\n");
    // printf("a test\n");

    // }
    int64_t nelements;
    int seed;
    if (argc==1){
        nelements = 1000000;
        seed = 1;
    } else if (argc==2){
        nelements = atoi(argv[1]);
        seed = 1;
    }else if (argc==3){
        nelements = atoi(argv[1]);
        seed = atoi(argv[2]);
    }
    int64_t a[nelements];
    initialize(a,1,nelements);
    double start_time = omp_get_wtime();
#pragma omp parallel default(none) shared(a, nelements)
    {
#pragma omp single nowait
        {
            (void) Quicksort(a,0,nelements-1);
    }
    }
    double time = omp_get_wtime() - start_time;
    printf("Time taken: %f\n",time);
        testit(a,&nelements);
    return 0;
}