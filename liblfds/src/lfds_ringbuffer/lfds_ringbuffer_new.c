#include "lfds_ringbuffer_internal.h"





/****************************************************************************/
int lfds_ringbuffer_new( struct lfds_ringbuffer_state **rs, lfds_atom_t number_elements, int (*user_data_init_function)(void **user_data, void *user_state), void *user_state )
{
  int
    rv = 0;

  assert( rs != NULL );
  // TRD : number_elements can be any value in its range
  // TRD : user_data_init_function can be NULL
  // TRD : user_state can be NULL

  *rs = (struct lfds_ringbuffer_state *) lfds_liblfds_aligned_malloc( sizeof(struct lfds_ringbuffer_state), LFDS_ALIGN_DOUBLE_POINTER );

  if( *rs != NULL )
  {
    lfds_freelist_new( &(*rs)->fs, number_elements, user_data_init_function, user_state );

    if( (*rs)->fs != NULL )
    {
      lfds_queue_new( &(*rs)->qs, number_elements );

      if( (*rs)->qs != NULL )
        rv = 1;

      if( (*rs)->qs == NULL )
      {
        lfds_liblfds_aligned_free( *rs );
        *rs = NULL;
      }
    }

    if( (*rs)->fs == NULL )
    {
      lfds_liblfds_aligned_free( *rs );
      *rs = NULL;
    }
  }

  LFDS_BARRIER_STORE;

  return( rv );
}





/****************************************************************************/
#pragma warning( disable : 4100 )

void lfds_ringbuffer_use( struct lfds_ringbuffer_state *rs )
{
  assert( rs != NULL );

  LFDS_BARRIER_LOAD;

  return;
}

#pragma warning( default : 4100 )

