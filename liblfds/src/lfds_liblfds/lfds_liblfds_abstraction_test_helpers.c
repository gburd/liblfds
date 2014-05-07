#include "lfds_liblfds_internal.h"





/****************************************************************************/
void lfds_liblfds_abstraction_test_helper_increment_non_atomic( lfds_atom_t *shared_counter )
{
  /* TRD : lfds_atom_t must be volatile or the compiler
           optimizes it away into a single store
  */

  volatile lfds_atom_t
    count = 0;

  assert( shared_counter != NULL );

  while( count++ < 10000000 )
    (*(lfds_atom_t *) shared_counter)++;

  return;
}





/****************************************************************************/
void lfds_liblfds_abstraction_test_helper_increment_atomic( volatile lfds_atom_t *shared_counter )
{
  lfds_atom_t
    count = 0;

  assert( shared_counter != NULL );

  while( count++ < 10000000 )
    lfds_abstraction_increment( shared_counter );

  return;
}





/****************************************************************************/
void lfds_liblfds_abstraction_test_helper_cas( volatile lfds_atom_t *shared_counter, lfds_atom_t *local_counter )
{
  lfds_atom_t
    loop = 0,
    original_destination;

  LFDS_ALIGN(LFDS_ALIGN_SINGLE_POINTER) lfds_atom_t
    exchange,
    compare;

  assert( shared_counter != NULL );
  assert( local_counter != NULL );

  while( loop++ < 1000000 )
  {
    do
    {
      compare = *shared_counter;
      exchange = compare + 1;

      original_destination = lfds_abstraction_cas( shared_counter, exchange, compare );
    }
    while( original_destination != compare );

    (*local_counter)++;
  }

  return;
}





/****************************************************************************/
void lfds_liblfds_abstraction_test_helper_dcas( volatile lfds_atom_t *shared_counter, lfds_atom_t *local_counter )
{
  lfds_atom_t
    loop = 0;

  LFDS_ALIGN(LFDS_ALIGN_DOUBLE_POINTER) lfds_atom_t
    exchange[2],
    compare[2];

  assert( shared_counter != NULL );
  assert( local_counter != NULL );

  while( loop++ < 1000000 )
  {
    compare[0] = *shared_counter;
    compare[1] = *(shared_counter+1);

    do
    {
      exchange[0] = compare[0] + 1;
      exchange[1] = compare[1];
    }
    while( 0 == lfds_abstraction_dcas(shared_counter, exchange, compare) );

    (*local_counter)++;
  }

  return;
}

