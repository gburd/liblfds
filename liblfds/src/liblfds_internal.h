/***** public prototypes *****/
#include "liblfds.h"

/***** defines *****/
#define and &&
#define or  ||

#define RAISED   1
#define LOWERED  0

#define NO_FLAGS 0x0

/***** private prototypes *****/
void *lfds_liblfds_aligned_malloc( size_t size, size_t align_in_bytes );
void lfds_liblfds_aligned_free( void *memory );

static LFDS_INLINE lfds_atom_t lfds_abstraction_cas( volatile lfds_atom_t *destination, lfds_atom_t exchange, lfds_atom_t compare );
static LFDS_INLINE unsigned char lfds_abstraction_dcas( volatile lfds_atom_t *destination, lfds_atom_t *exchange, lfds_atom_t *compare );
static LFDS_INLINE lfds_atom_t lfds_abstraction_increment( volatile lfds_atom_t *value );

/***** inlined code *****/
#include "lfds_abstraction/lfds_abstraction_cas.c"
#include "lfds_abstraction/lfds_abstraction_dcas.c"
#include "lfds_abstraction/lfds_abstraction_increment.c"

