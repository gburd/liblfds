#include "lfds_slist_internal.h"





/****************************************************************************/
void lfds_slist_delete( struct lfds_slist_state *ss )
{
  lfds_slist_single_threaded_physically_delete_all_elements( ss );

  lfds_liblfds_aligned_free( ss );

  return;
}





/****************************************************************************/
int lfds_slist_logically_delete_element( struct lfds_slist_state *ss, struct lfds_slist_element *se )
{
  LFDS_ALIGN(LFDS_ALIGN_DOUBLE_POINTER) void
    *volatile user_data_and_flags[2],
    *volatile new_user_data_and_flags[2];

  unsigned char
    cas_rv = 0;

  assert( ss != NULL );
  assert( se != NULL );

  LFDS_BARRIER_LOAD;

  user_data_and_flags[LFDS_SLIST_USER_DATA] = se->user_data_and_flags[LFDS_SLIST_USER_DATA];
  user_data_and_flags[LFDS_SLIST_FLAGS] = se->user_data_and_flags[LFDS_SLIST_FLAGS];

  do
  {
    new_user_data_and_flags[LFDS_SLIST_USER_DATA] = user_data_and_flags[LFDS_SLIST_USER_DATA];
    new_user_data_and_flags[LFDS_SLIST_FLAGS] = (void *) ((lfds_atom_t) user_data_and_flags[LFDS_SLIST_FLAGS] | LFDS_SLIST_FLAG_DELETED);
  }
  while( !((lfds_atom_t) user_data_and_flags[LFDS_SLIST_FLAGS] & LFDS_SLIST_FLAG_DELETED) and 0 == (cas_rv = lfds_abstraction_dcas((volatile lfds_atom_t *) se->user_data_and_flags, (lfds_atom_t *) new_user_data_and_flags, (lfds_atom_t *) user_data_and_flags)) );

  if( cas_rv == 1 )
    if( ss->user_data_delete_function != NULL )
      ss->user_data_delete_function( (void *) user_data_and_flags[LFDS_SLIST_USER_DATA], ss->user_state );

  return( cas_rv );
}





/****************************************************************************/
void lfds_slist_single_threaded_physically_delete_all_elements( struct lfds_slist_state *ss )
{
  struct lfds_slist_element
    *volatile se,
    *volatile se_temp;

  LFDS_BARRIER_LOAD;

  se = ss->head;

  while( se != NULL )
  {
    // TRD : if a non-deleted element and there is a delete function, call the delete function
    if( ss->user_data_delete_function != NULL )
      ss->user_data_delete_function( (void *) se->user_data_and_flags[LFDS_SLIST_USER_DATA], ss->user_state );

    se_temp = se;
    se = se->next;
    lfds_liblfds_aligned_free( (void *) se_temp );
  }

  lfds_slist_internal_init_slist( ss, ss->user_data_delete_function, ss->user_state );

  LFDS_BARRIER_STORE;

  return;
}

