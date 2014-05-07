#include "lfds_freelist_internal.h"





/****************************************************************************/
void lfds_freelist_delete( struct lfds_freelist_state *fs, void (*user_data_delete_function)(void *user_data, void *user_state), void *user_state )
{
  struct lfds_freelist_element
    *fe;

  void
    *user_data;

  assert( fs != NULL );
  // TRD : user_data_delete_function can be NULL
  // TRD : user_state can be NULL

  // TRD : leading load barrier not required as it will be performed by the pop

  while( lfds_freelist_pop(fs, &fe) )
  {
    if( user_data_delete_function != NULL )
    {
      lfds_freelist_get_user_data_from_element( fe, &user_data );
      user_data_delete_function( user_data, user_state );
    }

    lfds_liblfds_aligned_free( fe );
  }

  lfds_liblfds_aligned_free( fs );

  return;
}

