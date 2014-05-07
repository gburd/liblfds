#include "lfds_slist_internal.h"





/****************************************************************************/
int lfds_slist_get_user_data_from_element( struct lfds_slist_element *se, void **user_data )
{
  int
    rv = 1;

  assert( se != NULL );
  assert( user_data != NULL );

  LFDS_BARRIER_LOAD;

  *user_data = (void *) se->user_data_and_flags[LFDS_SLIST_USER_DATA];

  if( (lfds_atom_t) se->user_data_and_flags[LFDS_SLIST_FLAGS] & LFDS_SLIST_FLAG_DELETED )
    rv = 0;

  return( rv );
}





/****************************************************************************/
int lfds_slist_set_user_data_in_element( struct lfds_slist_element *se, void *user_data )
{
  LFDS_ALIGN(LFDS_ALIGN_DOUBLE_POINTER) void
    *user_data_and_flags[2],
    *new_user_data_and_flags[2];

  int
    rv = 1;

  assert( se != NULL );
  // TRD : user_data can be NULL

  LFDS_BARRIER_LOAD;

  user_data_and_flags[LFDS_SLIST_USER_DATA] = se->user_data_and_flags[LFDS_SLIST_USER_DATA];
  user_data_and_flags[LFDS_SLIST_FLAGS] = se->user_data_and_flags[LFDS_SLIST_FLAGS];

  new_user_data_and_flags[LFDS_SLIST_USER_DATA] = user_data;

  do
  {
    new_user_data_and_flags[LFDS_SLIST_FLAGS] = user_data_and_flags[LFDS_SLIST_FLAGS];
  }
  while( !((lfds_atom_t) user_data_and_flags[LFDS_SLIST_FLAGS] & LFDS_SLIST_FLAG_DELETED) and
         0 == lfds_abstraction_dcas((volatile lfds_atom_t *) se->user_data_and_flags, (lfds_atom_t *) new_user_data_and_flags, (lfds_atom_t *) user_data_and_flags) );

  if( (lfds_atom_t) user_data_and_flags[LFDS_SLIST_FLAGS] & LFDS_SLIST_FLAG_DELETED )
    rv = 0;

  LFDS_BARRIER_STORE;

  return( rv );
}





/****************************************************************************/
struct lfds_slist_element *lfds_slist_get_head( struct lfds_slist_state *ss, struct lfds_slist_element **se )
{
  assert( ss != NULL );
  assert( se != NULL );

  LFDS_BARRIER_LOAD;

  *se = (struct lfds_slist_element *) ss->head;

  lfds_slist_internal_move_to_first_undeleted_element( se );

  return( *se );
}





/****************************************************************************/
struct lfds_slist_element *lfds_slist_get_next( struct lfds_slist_element *se, struct lfds_slist_element **next_se )
{
  assert( se != NULL );
  assert( next_se != NULL );

  LFDS_BARRIER_LOAD;

  *next_se = (struct lfds_slist_element *) se->next;

  lfds_slist_internal_move_to_first_undeleted_element( next_se );

  return( *next_se );
}





/****************************************************************************/
struct lfds_slist_element *lfds_slist_get_head_and_then_next( struct lfds_slist_state *ss, struct lfds_slist_element **se )
{
  assert( ss != NULL );
  assert( se != NULL );

  if( *se == NULL )
    lfds_slist_get_head( ss, se );
  else
    lfds_slist_get_next( *se, se );

  return( *se );
}





/****************************************************************************/
void lfds_slist_internal_move_to_first_undeleted_element( struct lfds_slist_element **se )
{
  assert( se != NULL );

  while( *se != NULL and (lfds_atom_t) (*se)->user_data_and_flags[LFDS_SLIST_FLAGS] & LFDS_SLIST_FLAG_DELETED )
    (*se) = (struct lfds_slist_element *) (*se)->next;

  return;
}

