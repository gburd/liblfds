/***** the library wide include file *****/
#include "liblfds_internal.h"

/***** pragmas *****/

/***** defines *****/
#define LFDS_STACK_POINTER 0
#define LFDS_STACK_COUNTER 1
#define LFDS_STACK_PAC_SIZE 2

/***** structures *****/
#pragma pack( push, LFDS_ALIGN_DOUBLE_POINTER )

struct lfds_stack_state
{
  // TRD : must come first for alignment
  struct lfds_stack_element
    *volatile top[LFDS_STACK_PAC_SIZE];

  lfds_atom_t
    aba_counter;

  struct lfds_freelist_state
    *fs;
};

struct lfds_stack_element
{
  struct lfds_stack_element
    *next[LFDS_STACK_PAC_SIZE];

  struct lfds_freelist_element
    *fe;

  void
    *user_data;
};

#pragma pack( pop )

/***** private prototypes *****/
int lfds_stack_internal_freelist_init_function( void **user_data, void *user_state );
void lfds_stack_internal_freelist_delete_function( void *user_data, void *user_state );

void lfds_stack_internal_new_element_from_freelist( struct lfds_stack_state *ss, struct lfds_stack_element *se[LFDS_STACK_PAC_SIZE], void *user_data );
void lfds_stack_internal_new_element( struct lfds_stack_state *ss, struct lfds_stack_element *se[LFDS_STACK_PAC_SIZE], void *user_data );
void lfds_stack_internal_init_element( struct lfds_stack_state *ss, struct lfds_stack_element *se[LFDS_STACK_PAC_SIZE], struct lfds_freelist_element *fe, void *user_data );

void lfds_stack_internal_push( struct lfds_stack_state *ss, struct lfds_stack_element *se[LFDS_STACK_PAC_SIZE] );

void lfds_stack_internal_validate( struct lfds_stack_state *ss, struct lfds_validation_info *vi, enum lfds_data_structure_validity *stack_validity, enum lfds_data_structure_validity *freelist_validity );

