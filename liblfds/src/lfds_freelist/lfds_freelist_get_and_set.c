#include "lfds_freelist_internal.h"





/****************************************************************************/
void *lfds_freelist_get_user_data_from_element( struct lfds_freelist_element *fe, void **user_data )
{
  assert( fe != NULL );
  // TRD : user_data can be NULL

  LFDS_BARRIER_LOAD;

  if( user_data != NULL )
    *user_data = fe->user_data;

  return( fe->user_data );
}





/****************************************************************************/
void lfds_freelist_set_user_data_in_element( struct lfds_freelist_element *fe, void *user_data )
{
  assert( fe != NULL );
  // TRD : user_data can be NULL

  fe->user_data = user_data;

  LFDS_BARRIER_STORE;

  return;
}

