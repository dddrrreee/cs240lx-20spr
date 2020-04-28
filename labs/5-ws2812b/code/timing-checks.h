// engler
//
// The WS2818b requires nanosecond accurate writes; it's easy to mess these up with
// random changes.  The code below attempts to performance check that the code
// workes within its budget.
//
// note: the code can't be compiled in a seperate module since it needs inlined
// functions, so we just include the whole thing at the end of the neopixel.c file.
// 
// some problems:
//  1. obviously monitoring changes timing, so we never really know *exactly* how 
//  long something takes.  
//
//  2. while we detect if the routines the code calls has different timings, these
//  calls are to *copies* of the code (b/c of inlining).  the neopixel code calls 
//  its own copy of the routine (again: inlining).  as a result, our "checks" can 
//  pass routines that will fail when used by neopixel.  e.g., b/c their alignment
//  differs, changinging timing. 
// 
// partial solution to the above: you could use a second pi and hard poll on the pins, 
// recording when/how long the pin was hi/low.   this primitive oscilliscope is 
// actually easy to build, and useful for other things.
//
//
// DO NOT DELETE *inline*.
// you *HAVE* to inline, otherwise its a function pointer call.
// duh.
static inline void 
time_check(const char *fn, int pin, void (*fp)(unsigned), int exp) { 

	unsigned s = cycle_cnt_read();
	unsigned e = cycle_cnt_read();
	unsigned overhead = e - s;
	assert(overhead==8);

	dev_barrier();
	s = cycle_cnt_read();
		fp(pin);
	e = cycle_cnt_read();
	dev_barrier();
	unsigned t = e - s - 8;

	// +/- 10 cycles seems right?
	// const unsigned tol = 10;
	// wtf, why so high?
    int tol = 20;
	printk("%s: expected %u cycles, got %u cycles\n", fn, exp, t);
	assert(t <= (exp + tol) && t >= (exp - tol));
}

// gcc bug work around 
static void checkt0l(unsigned);
static void checkt0h(unsigned);
static void checkt1h(unsigned);
static void checkt1l(unsigned);
static void checktreset(unsigned); 

#define get_raw_time() *(volatile unsigned *)0x20003004
static void time_usec(int pin) {

#define run(str, f,exp,pin) do {			\
	unsigned b = get_raw_time();		\
	f(pin); f(pin); f(pin); f(pin); f(pin);		\
	f(pin); f(pin); f(pin); f(pin); f(pin);		\
						\
	f(pin); f(pin); f(pin); f(pin); f(pin);		\
	f(pin); f(pin); f(pin); f(pin); f(pin);		\
						\
	f(pin); f(pin); f(pin); f(pin); f(pin);		\
	f(pin); f(pin); f(pin); f(pin); f(pin);		\
						\
	f(pin); f(pin); f(pin); f(pin); f(pin);		\
	f(pin); f(pin); f(pin); f(pin); f(pin);		\
						\
	f(pin); f(pin); f(pin); f(pin); f(pin);		\
	f(pin); f(pin); f(pin); f(pin); f(pin);		\
						\
	unsigned e = get_raw_time();		\
	printk("%s: 100 check: expected %u ns, got %u ns\n", str, exp, (e-b) * 1000/50);	\
  } while(0)

#if 0
	T0H = to_cycles(350),	// Width of a 0 bit in ns
	T1H = to_cycles(900),	// Width of a 1 bit in ns
	T0L = to_cycles(900),	// Width of a 0 bit in ns
	T1L = to_cycles(350), 	// Width of a 1 bit in ns
	FLUSH = to_cycles(50 *1000); // Width of the low gap between bits to cause a frame to latch
#endif


	
	checkt0h(pin);
	checkt0l(pin);

	checkt1h(pin);
	checkt1l(pin);
	checktreset(pin); 
}

static void checkt0h(unsigned pin) {
	run("t0h", t0h,350,pin);
}

static void checkt0l(unsigned pin) {
	run("t0l", t0l,900,pin);
}

static void checkt1h(unsigned pin) {
	run("t1h", t1h,900,pin);
}

static void checkt1l(unsigned pin) {
	run("t1l", t1l,350,pin);
}
static void checktreset(unsigned pin) {
	run("treset", treset, 50000,pin);
}

void check_timings(int pin) {
	unsigned s = cycle_cnt_read();
	unsigned e = cycle_cnt_read();
	unsigned overhead = e - s;
	printk("overhead = %u\n", overhead);

	// asm volatile (".align 5");
	run("gpio_off_raw", gpio_set_off_raw, 22, pin);
	run("gpio_on_raw", gpio_set_on_raw, 22, pin);

	s = cycle_cnt_read();
		delay_ms(1000);
	e = cycle_cnt_read();
	unsigned t= (e - s) / (1000 * 1000);
	printk("1sec should take about 700M cycle: %u, %u\n", e - s,t);
	// should check a tighter tol (say 50k?)
	assert(t >= MHz-1 && t <= MHz +1);

#if 0
	s = cycle_cnt_read();
		gpio_set_off(pin);
	e = cycle_cnt_read();
	printk("gpio off takes %d\n", e - s);

	s = cycle_cnt_read();
		gpio_set_on(pin);
	e = cycle_cnt_read();
	printk("gpio on takes %d\n", e - s);
#endif

	asm volatile (".align 4");
	time_check("t1h", pin, t1h, T1H);
	time_check("t0h", pin, t0h, T0H);
	time_check("t1l", pin, t1l, T1L);
	time_check("t0l", pin, t0l, T0L);
	time_check("treset", pin, treset, FLUSH);

	time_usec(pin);
}
