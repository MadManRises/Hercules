//#include <stdlib.h>
//#include <stdio.h>
//#include <sys/time.h>
//#include <time.h>
//#include <string.h>
#include "hashmap.h"
#include "stack.h"
#include "hashmap.c"
#include "stack.c"

typedef long int __time_t;
typedef long int __suseconds_t;
struct  timeval2 {
  __time_t tv_sec ;
  __suseconds_t tv_usec ;
}  ;

#define ID2IPERF_KEY_MAX_LENGTH (256)
#define ID2IPERF_KEY_PREFIX ("somekey")
#define ID2IPERF_KEY_COUNT (1024*1024)
#define ID2IPERF_CONTEXT_SIZE 5000

typedef struct id2iperf_data_structs {
    char key_string[ID2IPERF_KEY_MAX_LENGTH];
    double myTime;
    double outerDiff;
    int measurements;
    int numberOfStmts;
} id2iperf_data_struct;

typedef struct id2iperf_times {
  double start;
  double end;
  double diff;
  double outerStart;
} id2iperf_time;

double id2iperf_getTime() {
  struct timeval2 t1;
  gettimeofday(&t1, NULL);
  return (t1.tv_sec) * 1000.0 + t1.tv_usec / 1000.0;
}

double id2iperf_getElapsedTime(struct timeval2 t1, struct timeval2 t2) {
    return (t2.tv_sec - t1.tv_sec) * 1000.0 + (t2.tv_usec - t1.tv_usec) / 1000.0;       
}

pstack id2iperf_context = 0;
pstack id2iperf_times = 0;
double id2iperf_context_time = 0;
double id2iperf_measurement_time = 0.081396;
int id2iperf_measurement_counter = 0;
id2iperf_data_struct* id2iperf_tmpvalue;
map_t id2iperf_mymap;

int id2iperf_printHashMap(any_t *t1, any_t t2, char* key) {
  *t1 = t2;
  printf("%s -> %f ms, %f ms (measurements: %d; statements: %d)\n", key, id2iperf_tmpvalue->myTime, id2iperf_tmpvalue->outerDiff, id2iperf_tmpvalue->measurements, id2iperf_tmpvalue->numberOfStmts);
  return 0;
}

int id2iperf_addHashMap(any_t *t1, any_t t2, char* key) {
  *t1 = t2;
  id2iperf_context_time += id2iperf_tmpvalue->myTime;
  return 0;
}

void id2iperf_time_start() {
  id2iperf_mymap = hashmap_new();
  id2iperf_tmpvalue = malloc(sizeof(id2iperf_data_struct));
  id2iperf_time* t = malloc(sizeof(id2iperf_time));
  t->start = id2iperf_getTime();
  push(&id2iperf_times, t, 0);
}

void id2iperf_time_end() {
  double tmpTime = id2iperf_getTime();
  id2iperf_time* t = pop(&id2iperf_times, 0);
  t->end = tmpTime;
  id2iperf_measurement_counter++;
  hashmap_iterate(id2iperf_mymap, (PFany) id2iperf_addHashMap, (void**)(&id2iperf_tmpvalue));
  id2iperf_data_struct* true_entry = malloc(sizeof(id2iperf_data_struct));
  true_entry->myTime = t->end - t->start;
  true_entry->measurements = 1;
  true_entry->numberOfStmts = 0;
  true_entry->outerDiff = 0;
  hashmap_put(id2iperf_mymap, "BASE", true_entry);
  printf("-- Hercules Performance --\n");
  printf("Hashmap size: %d\n", hashmap_length(id2iperf_mymap));
  printf("Measurement counter: %d\n", id2iperf_measurement_counter);
  hashmap_get(id2iperf_mymap, "BASE", (void**)(&id2iperf_tmpvalue));
  double total_time = true_entry->myTime;
  printf("Total time: %f ms\n", total_time);
  //double measurement_time = total_time - (id2iperf_measurement_counter * id2iperf_measurement_time);
  //printf("Approximated execution time without measurement time: %f ms\n", measurement_time);
  hashmap_iterate(id2iperf_mymap, (PFany) id2iperf_printHashMap, (void**)(&id2iperf_tmpvalue));
  free(id2iperf_tmpvalue);
  free(t);
}

void id2iperf_time_before(char *id2iperf_contextName) {
  double tmpTime = id2iperf_getTime();
  id2iperf_time_helper(id2iperf_contextName);
}

void id2iperf_time_before_counter(char *id2iperf_contextName, int currentIdentifier) {
  double tmpTime = id2iperf_getTime();
  printf("Performance counter: %d\n", currentIdentifier);
  id2iperf_time_helper(id2iperf_contextName);
}

void id2iperf_time_helper(char *id2iperf_contextName) {
  //printf("Before: %s, %d\n", id2iperf_contextName, currentIdentifier);
  push(&id2iperf_context, id2iperf_contextName, 1);
  id2iperf_measurement_counter++;
  id2iperf_time* t = malloc(sizeof(id2iperf_time));
  t->outerStart = tmpTime;
  t->diff = 0;
  t->end = 0;
  push(&id2iperf_times, t, 0);
  t->start = id2iperf_getTime();
}

void id2iperf_time_after(int statementNo) {
  double tmpTime = id2iperf_getTime();
  id2iperf_time* t = pop(&id2iperf_times, 0);
  t->end = id2iperf_getTime();
  t->diff = t->end - t->start;
  char *tmp_context = malloc(ID2IPERF_CONTEXT_SIZE * sizeof(char));
  stack_content(&id2iperf_context, tmp_context);
  //printf("After: %s\n", tmp_context);
  pop(&id2iperf_context, 1);
  if (hashmap_get(id2iperf_mymap, tmp_context, (void**)(&id2iperf_tmpvalue)) == MAP_MISSING) {
    id2iperf_data_struct* hashmap_entry = malloc(sizeof(id2iperf_data_struct));
    hashmap_put(id2iperf_mymap, tmp_context, hashmap_entry);
    hashmap_entry->myTime = t->diff;
    hashmap_entry->measurements = 1;
    hashmap_entry->numberOfStmts = statementNo;
    hashmap_entry->outerDiff = id2iperf_getTime() - t->outerStart;
  } else {
    hashmap_get(id2iperf_mymap, tmp_context, (void**)(&id2iperf_tmpvalue));
    id2iperf_tmpvalue->myTime += t->diff;
    id2iperf_tmpvalue->measurements++;
    id2iperf_tmpvalue->numberOfStmts += statementNo;
    id2iperf_tmpvalue->outerDiff += id2iperf_getTime() - t->outerStart;
    //free(tmp_context);
  }
  free(t);
}
