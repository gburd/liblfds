#include "lfds_ringbuffer_internal.h"





/****************************************************************************/
#pragma warning( disable : 4100 )

void lfds_ringbuffer_query( struct lfds_ringbuffer_state *rs, enum lfds_ringbuffer_query_type query_type, void *query_input, void *query_output )
{
  assert( rs != NULL );
  // TRD : query_type can be any value in its range
  // TRD : query_input can be NULL
  assert( query_output != NULL );

  switch( query_type )
  {
    case LFDS_RINGBUFFER_QUERY_VALIDATE:
      // TRD : query_input can be NULL

      lfds_ringbuffer_internal_validate( rs, (struct lfds_validation_info *) query_input, (enum lfds_data_structure_validity *) query_output, ((enum lfds_data_structure_validity *) query_output)+2 );
    break;
  }

  return;
}

#pragma warning( default : 4100 )





/****************************************************************************/
void lfds_ringbuffer_internal_validate( struct lfds_ringbuffer_state *rs, struct lfds_validation_info *vi, enum lfds_data_structure_validity *lfds_queue_validity, enum lfds_data_structure_validity *lfds_freelist_validity )
{
  assert( rs != NULL );
  // TRD : vi can be NULL
  assert( lfds_queue_validity != NULL );
  assert( lfds_freelist_validity != NULL );

  lfds_queue_query( rs->qs, LFDS_QUEUE_QUERY_VALIDATE, vi, lfds_queue_validity );

  if( vi != NULL )
  {
    struct lfds_validation_info
      lfds_freelist_vi;

    lfds_atom_t
      total_elements;

    lfds_freelist_query( rs->fs, LFDS_FREELIST_QUERY_ELEMENT_COUNT, NULL, (void *) &total_elements );
    lfds_freelist_vi.min_elements = total_elements - vi->max_elements;
    lfds_freelist_vi.max_elements = total_elements - vi->min_elements;
    lfds_freelist_query( rs->fs, LFDS_FREELIST_QUERY_VALIDATE, (void *) &lfds_freelist_vi, (void *) lfds_freelist_validity );
  }

  if( vi == NULL )
    lfds_freelist_query( rs->fs, LFDS_FREELIST_QUERY_VALIDATE, NULL, (void *) lfds_freelist_validity );

  return;
}

