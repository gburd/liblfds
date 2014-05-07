#include "lfds_abstraction_internal_wrapper.h"





/****************************************************************************/
#if (!defined WIN_KERNEL_BUILD)

  /* TRD : any OS except Windows kernel on any CPU with any compiler

           !WIN_KERNEL_BUILD  indicates not Windows kernel
  */

  void *lfds_abstraction_malloc( size_t size )
  {
    return( malloc(size) );
  }

#endif





/****************************************************************************/
#if (defined WIN_KERNEL_BUILD)

  /* TRD : any Windows (kernel) on any CPU with the Microsoft C compiler

           WIN_KERNEL_BUILD  indicates Windows kernel
  */

  void *lfds_abstraction_malloc( size_t size )
  {
    return( ExAllocatePoolWithTag(NonPagedPool, size, 'sdfl') );
  }

#endif

