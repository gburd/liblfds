#include "lfds_slist_internal.h"





/****************************************************************************/
void lfds_slist_internal_link_element_to_head( struct lfds_slist_state *ss, struct lfds_slist_element *volatile se )
{
  LFDS_ALIGN(LFDS_ALIGN_SINGLE_POINTER) struct lfds_slist_element
    *se_next;

  assert( ss != NULL );
  assert( se != NULL );

  LFDS_BARRIER_LOAD;

  se_next = ss->head;

  do
  {
    se->next = se_next;
  }
  while( se->next != (se_next = (struct lfds_slist_element *) lfds_abstraction_cas((volatile lfds_atom_t *) &ss->head, (lfds_atom_t) se, (lfds_atom_t) se->next)) );

  return;
}





/****************************************************************************/
void lfds_slist_internal_link_element_after_element( struct lfds_slist_element *volatile lfds_slist_in_list_element, struct lfds_slist_element *volatile se )
{
  LFDS_ALIGN(LFDS_ALIGN_SINGLE_POINTER) struct lfds_slist_element
    *se_prev,
    *se_next;

  assert( lfds_slist_in_list_element != NULL );
  assert( se != NULL );

  LFDS_BARRIER_LOAD;

  se_prev = (struct lfds_slist_element *) lfds_slist_in_list_element;

  se_next = se_prev->next;

  do
  {
    se->next = se_next;
  }
  while( se->next != (se_next = (struct lfds_slist_element *) lfds_abstraction_cas((volatile lfds_atom_t *) &se_prev->next, (lfds_atom_t) se, (lfds_atom_t) se->next)) );

  return;
}

