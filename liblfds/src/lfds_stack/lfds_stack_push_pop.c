#include "lfds_stack_internal.h"





/****************************************************************************/
int lfds_stack_push( struct lfds_stack_state *ss, void *user_data )
{
  LFDS_ALIGN(LFDS_ALIGN_DOUBLE_POINTER) struct lfds_stack_element
    *se[LFDS_STACK_PAC_SIZE];

  assert( ss != NULL );
  // TRD : user_data can be NULL

  lfds_stack_internal_new_element_from_freelist( ss, se, user_data );

  if( se[LFDS_STACK_POINTER] == NULL )
    return( 0 );

  lfds_stack_internal_push( ss, se );

  return( 1 );
}





/****************************************************************************/
int lfds_stack_guaranteed_push( struct lfds_stack_state *ss, void *user_data )
{
  LFDS_ALIGN(LFDS_ALIGN_DOUBLE_POINTER) struct lfds_stack_element
    *se[LFDS_STACK_PAC_SIZE];

  assert( ss != NULL );
  // TRD : user_data can be NULL

  /* TRD : this function allocated a new lfds_freelist element and uses that
           to push onto the lfds_stack, guaranteeing success (unless malloc()
           fails of course)
  */

  lfds_stack_internal_new_element( ss, se, user_data );

  // TRD : malloc failed
  if( se[LFDS_STACK_POINTER] == NULL )
    return( 0 );

  lfds_stack_internal_push( ss, se );

  return( 1 );
}





/****************************************************************************/
void lfds_stack_internal_push( struct lfds_stack_state *ss, struct lfds_stack_element *se[LFDS_STACK_PAC_SIZE] )
{
  LFDS_ALIGN(LFDS_ALIGN_DOUBLE_POINTER) struct lfds_stack_element
    *original_se_next[LFDS_STACK_PAC_SIZE];

  assert( ss != NULL );
  assert( se != NULL );

  LFDS_BARRIER_LOAD;

  original_se_next[LFDS_STACK_POINTER] = ss->top[LFDS_STACK_POINTER];
  original_se_next[LFDS_STACK_COUNTER] = ss->top[LFDS_STACK_COUNTER];

  do
  {
    se[LFDS_STACK_POINTER]->next[LFDS_STACK_POINTER] = original_se_next[LFDS_STACK_POINTER];
    se[LFDS_STACK_POINTER]->next[LFDS_STACK_COUNTER] = original_se_next[LFDS_STACK_COUNTER];
  }
  while( 0 == lfds_abstraction_dcas((volatile lfds_atom_t *) ss->top, (lfds_atom_t *) se, (lfds_atom_t *) original_se_next) );

  return;
}





/****************************************************************************/
int lfds_stack_pop( struct lfds_stack_state *ss, void **user_data )
{
  LFDS_ALIGN(LFDS_ALIGN_DOUBLE_POINTER) struct lfds_stack_element
    *se[LFDS_STACK_PAC_SIZE];

  assert( ss != NULL );
  assert( user_data != NULL );

  LFDS_BARRIER_LOAD;

  se[LFDS_STACK_COUNTER] = ss->top[LFDS_STACK_COUNTER];
  se[LFDS_STACK_POINTER] = ss->top[LFDS_STACK_POINTER];

  do
  {
    if( se[LFDS_STACK_POINTER] == NULL )
      return( 0 );
  }
  while( 0 == lfds_abstraction_dcas((volatile lfds_atom_t *) ss->top, (lfds_atom_t *) se[LFDS_STACK_POINTER]->next, (lfds_atom_t *) se) );

  *user_data = se[LFDS_STACK_POINTER]->user_data;

  lfds_freelist_push( ss->fs, se[LFDS_STACK_POINTER]->fe );

  return( 1 );
}

