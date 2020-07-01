#ifndef __SINGLE_STEP_H__
#define __SINGLE_STEP_H__

// initialize single step mode.
int single_step_init(void);

// run a single routine <user_fn> in single step mode using stack <sp>
int single_step_run(int user_fn(void), uint32_t sp);

#endif
