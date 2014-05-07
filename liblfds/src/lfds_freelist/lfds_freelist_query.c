#include "lfds_freelist_internal.h"





/****************************************************************************/
void lfds_freelist_query( struct lfds_freelist_state *fs, enum lfds_freelist_query_type query_type, void *query_input, void *query_output )
{
  assert( fs != NULL );
  // TRD : query type can be any value in its range
  // TRD : query_input can be NULL in some cases
  assert( query_output != NULL );

  LFDS_BARRIER_LOAD;

  switch( query_type )
  {
    case LFDS_FREELIST_QUERY_ELEMENT_COUNT:
      assert( query_input == NULL );

      *(lfds_atom_t *) query_output = fs->element_count;
    break;

    case LFDS_FREELIST_QUERY_VALIDATE:
      // TRD : query_input can be NULL

      lfds_freelist_internal_validate( fs, (struct lfds_validation_info *) query_input, (enum lfds_data_structure_validity *) query_output );
    break;
  }

  return;
}





/****************************************************************************/
void lfds_freelist_internal_validate( struct lfds_freelist_state *fs, struct lfds_validation_info *vi, enum lfds_data_structure_validity *lfds_freelist_validity )
{
  struct lfds_freelist_element
    *fe,
    *fe_slow,
    *fe_fast;

  lfds_atom_t
    element_count = 0;

  assert( fs != NULL );
  // TRD : vi can be NULL
  assert( lfds_freelist_validity != NULL );

  *lfds_freelist_validity = LFDS_VALIDITY_VALID;

  fe_slow = fe_fast = (struct lfds_freelist_element *) fs->top[LFDS_FREELIST_POINTER];

  /* TRD : first, check for a loop
           we have two pointers
           both of which start at the top of the lfds_freelist
           we enter a loop
           and on each iteration
           we advance one pointer by one element
           and the other by two

           we exit the loop when both pointers are NULL
           (have reached the end of the lfds_freelist)

           or

           if we fast pointer 'sees' the slow pointer
           which means we have a loop
  */

  if( fe_slow != NULL )
    do
    {
      fe_slow = fe_slow->next[LFDS_FREELIST_POINTER];

      if( fe_fast != NULL )
        fe_fast = fe_fast->next[LFDS_FREELIST_POINTER];

      if( fe_fast != NULL )
        fe_fast = fe_fast->next[LFDS_FREELIST_POINTER];
    }
    while( fe_slow != NULL and fe_fast != fe_slow );

  if( fe_fast != NULL and fe_slow != NULL and fe_fast == fe_slow )
    *lfds_freelist_validity = LFDS_VALIDITY_INVALID_LOOP;

  /* TRD : now check for expected number of elements
           vi can be NULL, in which case we do not check
           we know we don't have a loop from our earlier check
  */

  if( *lfds_freelist_validity == LFDS_VALIDITY_VALID and vi != NULL )
  {
    fe = (struct lfds_freelist_element *) fs->top[LFDS_FREELIST_POINTER];

    while( fe != NULL )
    {
      element_count++;
      fe = (struct lfds_freelist_element *) fe->next[LFDS_FREELIST_POINTER];
    }

    if( element_count < vi->min_elements )
      *lfds_freelist_validity = LFDS_VALIDITY_INVALID_MISSING_ELEMENTS;

    if( element_count > vi->max_elements )
      *lfds_freelist_validity = LFDS_VALIDITY_INVALID_ADDITIONAL_ELEMENTS;
  }

  return;
}

