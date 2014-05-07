#include "lfds_liblfds_internal.h"





/****************************************************************************/
void lfds_liblfds_aligned_free( void *memory )
{
  assert( memory != NULL );

  // TRD : the "void *" stored above memory points to the root of the allocation
  lfds_abstraction_free( *( (void **) memory - 1 ) );

  return;
}

