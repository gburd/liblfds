#include "lfds_queue_internal.h"





/****************************************************************************/
int lfds_queue_enqueue( struct lfds_queue_state *qs, void *user_data )
{
  LFDS_ALIGN(LFDS_ALIGN_DOUBLE_POINTER) struct lfds_queue_element
    *qe[LFDS_QUEUE_PAC_SIZE];

  assert( qs != NULL );
  // TRD : user_data can be NULL

  lfds_queue_internal_new_element_from_freelist( qs, qe, user_data );

  if( qe[LFDS_QUEUE_POINTER] == NULL )
    return( 0 );

  lfds_queue_internal_queue( qs, qe );

  return( 1 );
}





/****************************************************************************/
int lfds_queue_guaranteed_enqueue( struct lfds_queue_state *qs, void *user_data )
{
  LFDS_ALIGN(LFDS_ALIGN_DOUBLE_POINTER) struct lfds_queue_element
    *qe[LFDS_QUEUE_PAC_SIZE];

  assert( qs != NULL );
  // TRD : user_data can be NULL

  lfds_queue_internal_guaranteed_new_element_from_freelist( qs, qe, user_data );

  if( qe[LFDS_QUEUE_POINTER] == NULL )
    return( 0 );

  lfds_queue_internal_queue( qs, qe );

  return( 1 );
}





/****************************************************************************/
void lfds_queue_internal_queue( struct lfds_queue_state *qs, struct lfds_queue_element *qe[LFDS_QUEUE_PAC_SIZE] )
{
  LFDS_ALIGN(LFDS_ALIGN_DOUBLE_POINTER) struct lfds_queue_element
    *enqueue[LFDS_QUEUE_PAC_SIZE],
    *next[LFDS_QUEUE_PAC_SIZE];

  unsigned char
    cas_result = 0;

  assert( qs != NULL );
  assert( qe != NULL );

  // TRD : the DCAS operation issues a read and write barrier, so we don't need a read barrier in the do() loop

  LFDS_BARRIER_LOAD;

  do
  {
    enqueue[LFDS_QUEUE_POINTER] = qs->enqueue[LFDS_QUEUE_POINTER];
    enqueue[LFDS_QUEUE_COUNTER] = qs->enqueue[LFDS_QUEUE_COUNTER];

    next[LFDS_QUEUE_POINTER] = enqueue[LFDS_QUEUE_POINTER]->next[LFDS_QUEUE_POINTER];
    next[LFDS_QUEUE_COUNTER] = enqueue[LFDS_QUEUE_POINTER]->next[LFDS_QUEUE_COUNTER];

    /* TRD : this if() ensures that the next we read, just above,
             really is from qs->enqueue (which we copied into enqueue)
    */

    LFDS_BARRIER_LOAD;

    if( qs->enqueue[LFDS_QUEUE_POINTER] == enqueue[LFDS_QUEUE_POINTER] and qs->enqueue[LFDS_QUEUE_COUNTER] == enqueue[LFDS_QUEUE_COUNTER] )
    {
      if( next[LFDS_QUEUE_POINTER] == NULL )
      {
        qe[LFDS_QUEUE_COUNTER] = next[LFDS_QUEUE_COUNTER] + 1;
        cas_result = lfds_abstraction_dcas( (volatile lfds_atom_t *) enqueue[LFDS_QUEUE_POINTER]->next, (lfds_atom_t *) qe, (lfds_atom_t *) next );
      }
      else
      {
        next[LFDS_QUEUE_COUNTER] = enqueue[LFDS_QUEUE_COUNTER] + 1;
        lfds_abstraction_dcas( (volatile lfds_atom_t *) qs->enqueue, (lfds_atom_t *) next, (lfds_atom_t *) enqueue );
      }
    }
  }
  while( cas_result == 0 );

  qe[LFDS_QUEUE_COUNTER] = enqueue[LFDS_QUEUE_COUNTER] + 1;
  lfds_abstraction_dcas( (volatile lfds_atom_t *) qs->enqueue, (lfds_atom_t *) qe, (lfds_atom_t *) enqueue );

  return;
}





/****************************************************************************/
int lfds_queue_dequeue( struct lfds_queue_state *qs, void **user_data )
{
  LFDS_ALIGN(LFDS_ALIGN_DOUBLE_POINTER) struct lfds_queue_element
    *enqueue[LFDS_QUEUE_PAC_SIZE],
    *dequeue[LFDS_QUEUE_PAC_SIZE],
    *next[LFDS_QUEUE_PAC_SIZE];

  unsigned char
    cas_result = 0;

  int
    rv = 1,
    state = LFDS_QUEUE_STATE_UNKNOWN,
    finished_flag = LOWERED;

  assert( qs != NULL );
  assert( user_data != NULL );

  // TRD : the DCAS operation issues a read and write barrier, so we don't need a read barrier in the do() loop

  LFDS_BARRIER_LOAD;

  do
  {
    dequeue[LFDS_QUEUE_POINTER] = qs->dequeue[LFDS_QUEUE_POINTER];
    dequeue[LFDS_QUEUE_COUNTER] = qs->dequeue[LFDS_QUEUE_COUNTER];

    enqueue[LFDS_QUEUE_POINTER] = qs->enqueue[LFDS_QUEUE_POINTER];
    enqueue[LFDS_QUEUE_COUNTER] = qs->enqueue[LFDS_QUEUE_COUNTER];

    next[LFDS_QUEUE_POINTER] = dequeue[LFDS_QUEUE_POINTER]->next[LFDS_QUEUE_POINTER];
    next[LFDS_QUEUE_COUNTER] = dequeue[LFDS_QUEUE_POINTER]->next[LFDS_QUEUE_COUNTER];

    /* TRD : confirm that dequeue didn't move between reading it
             and reading its next pointer
    */

    LFDS_BARRIER_LOAD;

    if( dequeue[LFDS_QUEUE_POINTER] == qs->dequeue[LFDS_QUEUE_POINTER] and dequeue[LFDS_QUEUE_COUNTER] == qs->dequeue[LFDS_QUEUE_COUNTER] )
    {
      if( enqueue[LFDS_QUEUE_POINTER] == dequeue[LFDS_QUEUE_POINTER] and next[LFDS_QUEUE_POINTER] == NULL )
        state = LFDS_QUEUE_STATE_EMPTY;

      if( enqueue[LFDS_QUEUE_POINTER] == dequeue[LFDS_QUEUE_POINTER] and next[LFDS_QUEUE_POINTER] != NULL )
        state = LFDS_QUEUE_STATE_ENQUEUE_OUT_OF_PLACE;

      if( enqueue[LFDS_QUEUE_POINTER] != dequeue[LFDS_QUEUE_POINTER] )
        state = LFDS_QUEUE_STATE_ATTEMPT_DELFDS_QUEUE;

      switch( state )
      {
        case LFDS_QUEUE_STATE_EMPTY:
          *user_data = NULL;
          rv = 0;
          finished_flag = RAISED;
        break;

        case LFDS_QUEUE_STATE_ENQUEUE_OUT_OF_PLACE:
          next[LFDS_QUEUE_COUNTER] = enqueue[LFDS_QUEUE_COUNTER] + 1;
          lfds_abstraction_dcas( (volatile lfds_atom_t *) qs->enqueue, (lfds_atom_t *) next, (lfds_atom_t *) enqueue );
        break;

        case LFDS_QUEUE_STATE_ATTEMPT_DELFDS_QUEUE:
          *user_data = next[LFDS_QUEUE_POINTER]->user_data;

          next[LFDS_QUEUE_COUNTER] = dequeue[LFDS_QUEUE_COUNTER] + 1;
          cas_result = lfds_abstraction_dcas( (volatile lfds_atom_t *) qs->dequeue, (lfds_atom_t *) next, (lfds_atom_t *) dequeue );

          if( cas_result == 1 )
            finished_flag = RAISED;
        break;
      }
    }
  }
  while( finished_flag == LOWERED );

  if( cas_result == 1 )
    lfds_freelist_push( qs->fs, dequeue[LFDS_QUEUE_POINTER]->fe );

  return( rv );
}

