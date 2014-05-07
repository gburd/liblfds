/***** the library wide include file *****/
#include "liblfds_internal.h"

/***** defines *****/

/***** structures *****/
#pragma pack( push, LFDS_ALIGN_DOUBLE_POINTER )

struct lfds_ringbuffer_state
{
  struct lfds_queue_state
    *qs;

  struct lfds_freelist_state
    *fs;
};

#pragma pack( pop )

/***** externs *****/

/***** private prototypes *****/
void lfds_ringbuffer_internal_validate( struct lfds_ringbuffer_state *rs, struct lfds_validation_info *vi, enum lfds_data_structure_validity *lfds_queue_validity, enum lfds_data_structure_validity *lfds_freelist_validity );

