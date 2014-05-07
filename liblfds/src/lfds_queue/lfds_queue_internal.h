/***** the library wide include file *****/
#include "liblfds_internal.h"

/***** pragmas *****/

/***** defines *****/
#define LFDS_QUEUE_STATE_UNKNOWN               -1
#define LFDS_QUEUE_STATE_EMPTY                  0
#define LFDS_QUEUE_STATE_ENQUEUE_OUT_OF_PLACE   1
#define LFDS_QUEUE_STATE_ATTEMPT_DELFDS_QUEUE        2

#define LFDS_QUEUE_POINTER 0
#define LFDS_QUEUE_COUNTER 1
#define LFDS_QUEUE_PAC_SIZE 2

/***** structures *****/
#pragma pack( push, LFDS_ALIGN_DOUBLE_POINTER )

struct lfds_queue_state
{
  struct lfds_queue_element
    *volatile enqueue[LFDS_QUEUE_PAC_SIZE],
    *volatile dequeue[LFDS_QUEUE_PAC_SIZE];

  lfds_atom_t
    aba_counter;

  struct lfds_freelist_state
    *fs;
};

struct lfds_queue_element
{
  // TRD : next in a lfds_queue requires volatile as it is target of CAS
  struct lfds_queue_element
    *volatile next[LFDS_QUEUE_PAC_SIZE];

  struct lfds_freelist_element
    *fe;

  void
    *user_data;
};

#pragma pack( pop )

/***** externs *****/

/***** private prototypes *****/
int lfds_queue_internal_freelist_init_function( void **user_data, void *user_state );
void lfds_queue_internal_freelist_delete_function( void *user_data, void *user_state );

void lfds_queue_internal_new_element_from_freelist( struct lfds_queue_state *qs, struct lfds_queue_element *qe[LFDS_QUEUE_PAC_SIZE], void *user_data );
void lfds_queue_internal_guaranteed_new_element_from_freelist( struct lfds_queue_state *qs, struct lfds_queue_element * qe[LFDS_QUEUE_PAC_SIZE], void *user_data );
void lfds_queue_internal_init_element( struct lfds_queue_state *qs, struct lfds_queue_element *qe[LFDS_QUEUE_PAC_SIZE], struct lfds_freelist_element *fe, void *user_data );

void lfds_queue_internal_queue( struct lfds_queue_state *qs, struct lfds_queue_element *qe[LFDS_QUEUE_PAC_SIZE] );

void lfds_queue_internal_validate( struct lfds_queue_state *qs, struct lfds_validation_info *vi, enum lfds_data_structure_validity *lfds_queue_validity, enum lfds_data_structure_validity *lfds_freelist_validity );

