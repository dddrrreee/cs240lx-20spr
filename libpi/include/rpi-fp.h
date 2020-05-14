#ifndef __RPI_FP_H__
#define __RPI_FP_H__

#ifdef RPI_FP_ENABLED

    void enable_fpu(void);

    // use to catch linker error if not.
    extern unsigned fp_is_enabled;

#endif

#endif
