 
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "csapp.h"

/* The lsd_radix_sort function takes a pointer to the text, an array to place sorted 
suffix indices, a void pointer to a space of 256 integer entries for keeping counts,
the length of the text and a suff_index variable to keep a record of the current 
round of sorting.
 */

struct MSD_args{
char *text; 
int *sorted;
int lower; 
int upper;
int char_pos;
int length;
};


void lsd_radix_sort(char *text, int *sorted, void *counts, int length, int suff_index)
{

int *count_array = counts;
int curr_suff;

/* tmp_sort holds initial values of the previous sort order. It serves a double
 as a temporary sorting  array to  avoid overwriting already sorted 
values, as well as a placeholder for key-values of current suffixes */
int tmp_sort[suff_index + 1]; 
for (int i =0; i <= suff_index; i++){
    tmp_sort[i] = sorted[i];
}

/* curr_suff holds the correct position of the column character for the suffix 
under consideration. Suffixes are not directly represented, but are calculated 
from the length of the text and their key-value.*/
for(int k = 0;k < suff_index; k++){
    curr_suff = length - (suff_index - tmp_sort[k]);   
    count_array[text[curr_suff]] = count_array[text[curr_suff]] + 1;
}

count_array[text[length]] = count_array[text[length]] + 1;
tmp_sort[suff_index] = suff_index; 


for (int i = 1; i < 256; i++){
    count_array[i] = count_array[i] + count_array[i -1]; 
}

for(int k = suff_index;k >=0; k--){
    curr_suff = length - (suff_index - tmp_sort[k]);
    sorted[count_array[text[curr_suff]] - 1] = tmp_sort[k];
    count_array[text[curr_suff]] = count_array[text[curr_suff]] -1;  
}
}

/* msd_radix_sort performs radix sort based on the most significant digit. 
Its input (in order) is a pointer to the string, a pointer to an initial 
array of suffix indexes, a lower and upper bound, the current character 
position and the length of the string itself.*/

void msd_radix_sort(char *text, int *sorted, int lower, int upper, int char_pos, int length)
{

int range = upper - lower;

// recursion termination condition
if (range <=0)
    return;

int items = range + 1;
int *count_array = (void *) calloc(256, sizeof(int));
int *upper_array = (void *) calloc(256, sizeof(int));
int *tmp_array = (void *) calloc(items, sizeof(int));
int new_upper = 0;
int dead_end = 0;
int bad_index;


/* if condition:suffix at last position set dead_end to 1, record suffix index j
 at bad_index and set first sort position to j */
for (int j=lower; j <= upper; j++){ 
    if (sorted[j] + char_pos == length){
        tmp_array[0] = sorted[j];
        dead_end = 1;
        bad_index = j;
        continue;
    }    
   count_array[text[sorted[j] + char_pos]] = count_array[text[sorted[j] + char_pos]] + 1;
}

for (int i = 0; i < 256 ; i++){
    upper_array[i] = count_array[i];
}

for (int i = 0; i < 256 ; i++){
    count_array[i] = count_array[i] + count_array[i -1];
}

// dead_end in tmp_array indexing to determine real sort position to prevent overwrite on first position
for(int j = upper; j >=lower; j--){
    if(bad_index == j)
        continue;
    tmp_array[count_array[text[sorted[j] + char_pos]] - 1 + dead_end] = sorted[j];
    count_array[text[sorted[j] + char_pos]] = count_array[text[sorted[j] + char_pos]] - 1;
} 

int i = lower;
for (int j = 0;  j < items; j++){
    sorted[i] = tmp_array[j];
    i++;
}

int j = lower + dead_end;

free(count_array);
free((void*)(tmp_array));

while (j < upper)
{
    new_upper = upper_array[text[sorted[j] + char_pos]] + j -1;
    msd_radix_sort(text, sorted, j , new_upper , char_pos + 1, length);
    j =  new_upper + 1;
}

free(upper_array);
}


void *thread_peer_msd_radix_sort(void *vargs)
{

pthread_t my_id = pthread_self();
struct MSD_args *sort_args  =  (struct MSD_args*) vargs;
int upper = (*sort_args).upper;
int lower = (*sort_args).lower;
int range = upper - lower;
int out;

// recursion termination condition
if (range <=0)
    pthread_exit(&out);

int items = range + 1;
int *count_array = (void *) calloc(256, sizeof(int));
int *upper_array = (void *) calloc(256, sizeof(int));
int *tmp_array = (void *) calloc(items, sizeof(int));
int new_upper = 0;
int dead_end = 0;
int bad_index;
int *sorted = (*sort_args).sorted;
int char_pos = (*sort_args).char_pos;
int length = (*sort_args).length;
char *text = (*sort_args).text;

free((void*)sort_args);

/* if condition:suffix at last position set dead_end to 1, record suffix index j
 at bad_index and set first sort position to j */
for (int j=lower; j <= upper; j++){ 
    if (sorted[j] + char_pos == length){
        tmp_array[0] = sorted[j];
        dead_end = 1;
        bad_index = j;
        continue;
    }    
   count_array[text[sorted[j] + char_pos]] = count_array[text[sorted[j] + char_pos]] + 1;
}

for (int i = 0; i < 256 ; i++){
    upper_array[i] = count_array[i];
}

for (int i = 0; i < 256 ; i++){
    count_array[i] = count_array[i] + count_array[i -1];
}

// dead_end in tmp_array indexing to determine real sort position to prevent overwrite on first position
for(int j = upper; j >=lower; j--){
    if(bad_index == j)
        continue;
    tmp_array[count_array[text[sorted[j] + char_pos]] - 1 + dead_end] = sorted[j];
    count_array[text[sorted[j] + char_pos]] = count_array[text[sorted[j] + char_pos]] - 1;
} 

int i = lower;
for (int j = 0;  j < items; j++){
    sorted[i] = tmp_array[j];
    i++;
}

int j = lower + dead_end;

free(count_array);
free((void*)(tmp_array));

while (j < upper)
{
    new_upper = upper_array[text[sorted[j] + char_pos]] + j -1;
    msd_radix_sort(text, sorted, j , new_upper , char_pos + 1, length);
    j =  new_upper + 1;
}
free(upper_array);
pthread_exit(NULL);

}


/* msd_radix_sort performs radix sort based on the most significant digit. 
Its input (in order) is a pointer to the string, a pointer to an initial 
array of suffix indexes, a lower and upper bound, the current character 
position and the length of the string itself.*/

void *thread_main_msd_radix_sort(void *vargs)
{

pthread_t my_id = pthread_self();
struct MSD_args *sort_args  =  (struct MSD_args*) vargs;
int upper = (*sort_args).upper;
int lower = (*sort_args).lower;
int range = upper - lower;
int out;

// recursion termination condition
if (range <=0)
    pthread_exit(&out);

int items = range + 1;
int *count_array = (void *) calloc(256, sizeof(int));
int *upper_array = (void *) calloc(256, sizeof(int));
int *tmp_array = (void *) calloc(items, sizeof(int));
int new_upper = 0;
int dead_end = 0;
int bad_index;
int *sorted = (*sort_args).sorted;
int char_pos = (*sort_args).char_pos;
int length = (*sort_args).length;
char *text = (*sort_args).text;

free((void*)sort_args);

/* if condition:suffix at last position set dead_end to 1, record suffix index j
 at bad_index and set first sort position to j */
for (int j=lower; j <= upper; j++){ 
    if (sorted[j] + char_pos == length){
        tmp_array[0] = sorted[j];
        dead_end = 1;
        bad_index = j;
        continue;
    }    
   count_array[text[sorted[j] + char_pos]] = count_array[text[sorted[j] + char_pos]] + 1;
}

for (int i = 0; i < 256 ; i++){
    upper_array[i] = count_array[i];
}

for (int i = 0; i < 256 ; i++){
    count_array[i] = count_array[i] + count_array[i -1];
}

// dead_end in tmp_array indexing to determine real sort position to prevent overwrite on first position
for(int j = upper; j >=lower; j--){
    if(bad_index == j)
        continue;
    tmp_array[count_array[text[sorted[j] + char_pos]] - 1 + dead_end] = sorted[j];
    count_array[text[sorted[j] + char_pos]] = count_array[text[sorted[j] + char_pos]] - 1;
} 

int i = lower;
for (int j = 0;  j < items; j++){
    sorted[i] = tmp_array[j];
    i++;
}

int j = lower + dead_end;
int count = 0;
int divident, remainder;

free(count_array);
free((void*)(tmp_array));

while (j < upper){
        new_upper = upper_array[text[sorted[j] + char_pos]] + j -1;
        j =  new_upper + 1;
        count++;
}
divident = count / 6;
remainder = count % 6;

j = lower + dead_end;


for(int i=0; i < divident; i++){
    pthread_t thread_ids[6];
    for(int k = 0; k < 6; k++){
        struct MSD_args (*new_sort_args) = malloc (sizeof(struct MSD_args));
        new_upper = upper_array[text[sorted[j] + char_pos]] + j -1;
        (*new_sort_args).text= text;
        (*new_sort_args).sorted= sorted;
        (*new_sort_args).lower= j;
        (*new_sort_args).upper= new_upper;
        (*new_sort_args).char_pos= (char_pos + 1);
        (*new_sort_args).length= length;
        Pthread_create(&thread_ids[k], NULL, thread_peer_msd_radix_sort, new_sort_args);
        j =  new_upper + 1;
        
    }

    for(int k = 0; k < 6; k++){
        //printf("Thread %ld done and k is %d \n", thread_ids[k], k );
        Pthread_join(thread_ids[k], NULL); 
    }
}

pthread_t thread_ids[remainder];
for(int k =0; k < remainder; k++){
    struct MSD_args (*new_sort_args) = malloc (sizeof(struct MSD_args));
        new_upper = upper_array[text[sorted[j] + char_pos]] + j -1;
        (*new_sort_args).text= text;
        (*new_sort_args).sorted= sorted;
        (*new_sort_args).lower= j;
        (*new_sort_args).upper= new_upper;
        (*new_sort_args).char_pos= (char_pos+1);
        (*new_sort_args).length= length;
        Pthread_create(&thread_ids[k], NULL, thread_peer_msd_radix_sort, new_sort_args);
        j =  new_upper + 1;
}

for(int k = 0; k < remainder; k++){
    //printf("Thread %ld done and k is %d \n", thread_ids[k], k );
    Pthread_join(thread_ids[k], NULL);
}

free(upper_array);
pthread_exit(NULL);
}


int main(int argc, char *argv[]) {

if (argc !=2){
    fprintf(stderr, "Input filename address only");
    exit(0);
}
    
rio_t rio;
int fd;
struct stat txt_stats;

fd = Open(argv[1], O_RDONLY, S_IRUSR);
Stat(argv[1], &txt_stats);
int full_size = txt_stats.st_size;
int text_end = full_size - 2;
int real_size = text_end +1;
int x=0;

char *mybuff = (void *) malloc(full_size);
void *counts = (void *) malloc(sizeof(int)*256);
int *sorted_buff = (void *) malloc(sizeof(int)* (text_end+1));
memset(sorted_buff, 0, (text_end + 1) * sizeof(int));

Rio_readinitb(&rio, fd);
rio_readnb(&rio, mybuff, 8092);


/* since counts are continuously used and reused with each loop,
memset is used to 'wipe out' the array faster than iterating over
every element */

for (int i = 0; i <  text_end + 1; i++){
    counts =  memset(counts, 0, sizeof(int) *256);
    lsd_radix_sort(mybuff, sorted_buff, counts, text_end , i );
}

printf("\nSuffix sort by index (radix sort): ");
for (int i = 0; i <  text_end + 1; i++){
   printf("%d ", sorted_buff[i]);
}



printf("\nTransformed text (radix sort): ");
for (int i = 0; i <  real_size; i++){
    x= (sorted_buff[i] - 1);

    printf("%c", mybuff[(x < 0) ? (x % real_size + real_size) : (x % real_size)]);
}

counts =  memset(counts, 0, sizeof(int) *256);
for(int i = 0; i <= text_end; i++){
    sorted_buff[i] = i;
}

msd_radix_sort(mybuff, sorted_buff, 0, text_end, 0,text_end + 1);

printf("\nSuffix sort by index (MSD sort): ");
for (int i = 0; i <  text_end + 1; i++){
   printf("%d ", sorted_buff[i]);
}

printf("\nTransformed text (MSD sort): ");
for (int i = 0; i <  real_size; i++){
    x= (sorted_buff[i] - 1);
    printf("%c", mybuff[(x < 0) ? (x % real_size + real_size) : (x % real_size)]);
}


counts =  memset(counts, 0, sizeof(int) *256);
for(int i = 0; i <= text_end; i++){
     sorted_buff[i] = i;
 }

struct MSD_args (*new_sort_args) = malloc (sizeof(struct MSD_args));
    (*new_sort_args).text= mybuff;
    (*new_sort_args).sorted= sorted_buff;
    (*new_sort_args).lower= 0;
    (*new_sort_args).upper= text_end;
    (*new_sort_args).char_pos= 0;
    (*new_sort_args).length= (text_end + 1);

pthread_t tid;

Pthread_create(&tid, NULL, thread_main_msd_radix_sort,new_sort_args);

Pthread_join(tid, NULL);

printf("\nSuffix sort by index (Threaded MSD sort): ");
for (int i = 0; i <  text_end + 1; i++){
   printf("%d ", sorted_buff[i]);
}

printf("\nTransformed text (Threaded MSD sort): ");
for (int i = 0; i <  real_size; i++){
    x= (sorted_buff[i] - 1);
    printf("%c", mybuff[(x < 0) ? (x % real_size + real_size) : (x % real_size)]);
}


free((void*)mybuff);
free((void*)counts);
free((void*)sorted_buff);



}
