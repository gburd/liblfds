/***** the library wide include file *****/
#include "liblfds_internal.h"

/***** defines *****/
#define LFDS_FREELIST_POINTER 0
#define LFDS_FREELIST_COUNTER 1
#define LFDS_FREELIST_PAC_SIZE 2

/***** structures *****/
#pragma pack( push, LFDS_ALIGN_DOUBLE_POINTER )

struct lfds_freelist_state
{
  struct lfds_freelist_element
    *volatile top[LFDS_FREELIST_PAC_SIZE];

  int
    (*user_data_init_function)( void **user_data, void *user_state );

  void
    *user_state;

  lfds_atom_t
    aba_counter,
    element_count;
};

struct lfds_freelist_element
{
  struct lfds_freelist_element
    *next[LFDS_FREELIST_PAC_SIZE];

  void
    *user_data;
};

#pragma pack( pop )

/***** private prototypes *****/
lfds_atom_t lfds_freelist_internal_new_element( struct lfds_freelist_state *fs, struct lfds_freelist_element **fe );
void lfds_freelist_internal_validate( struct lfds_freelist_state *fs, struct lfds_validation_info *vi, enum lfds_data_structure_validity *lfds_freelist_validity );

