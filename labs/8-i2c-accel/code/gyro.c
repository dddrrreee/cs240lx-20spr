// engler, cs240lx initial driver code.
//
// everything is put in here so it's easy to find.  when it works,
// seperate it out.
//
// KEY: document why you are doing what you are doing.
//  **** put page numbers for any device-specific things you do ***
//  **** put page numbers for any device-specific things you do ***
//  **** put page numbers for any device-specific things you do ***
//  **** put page numbers for any device-specific things you do ***
//  **** put page numbers for any device-specific things you do ***
//  **** put page numbers for any device-specific things you do ***
//  **** put page numbers for any device-specific things you do ***
// 
// also: a sentence or two will go a long way in a year when you want 
// to re-use the code.
#include "rpi.h"
#include "i2c.h"
#include "lsm6ds33.h"
#include <limits.h>

/**********************************************************************
 * some helpers
 */

// const unsigned lsm6ds33_addr = 0b1101011; // this is the gyro/accel;

enum { VAL_WHO_AM_I      = 0x69, };

static imu_xyz_t xyz_mk(int x, int y, int z) {
    return (imu_xyz_t){.x = x, .y = y, .z = z};
}


// read register <reg> from i2c device <addr>
uint8_t imu_rd(uint8_t addr, uint8_t reg) {
    assert(addr);
    assert(reg);
    i2c_write(addr, &reg, 1);
        
    uint8_t v;
    i2c_read(addr,  &v, 1);
    return v;
}

// write register <reg> with value <v> 
void imu_wr(uint8_t addr, uint8_t reg, uint8_t v) {
    assert(addr);
    assert(reg);
    uint8_t data[2];
    data[0] = reg;
    data[1] = v;
    i2c_write(addr, data, 2);
    // printk("writeReg: %x=%x\n", reg, v);
}

// <base_reg> = lowest reg in sequence. --- hw will auto-increment 
// if you set IF_INC during initialization.
int imu_rd_n(uint8_t addr, uint8_t base_reg, uint8_t *v, uint32_t n) {
    assert(addr);
    assert(base_reg);
    i2c_write(addr, &base_reg, 1);
    return i2c_read(addr, v, n);
}

/**********************************************************************
 * simple gyro setup and use.
 */

typedef struct {
    uint8_t addr;
    unsigned hz;
    unsigned dps;
    unsigned scale;
} gyro_t;

static int mdps_raw(uint8_t hi, uint8_t lo) {
    unimplemented();
}

// returns m degree per sec
static int mdps_scaled(int v, int dps_scale) {
    unimplemented();
}


// see p 15 of the datasheet.  confusing we have to do this.
unsigned dps_to_scale(unsigned fs) {
    switch(fs) {
    case 245: unimplemented();
    case 500: unimplemented();
    case 1000: unimplemented();
    case 2000: unimplemented();
    default: assert(0);
    }
}

// takes in raw data and scales it.
static imu_xyz_t gyro_scale(gyro_t *h, imu_xyz_t xyz) {
    int s = h->scale;
    int x = mdps_scaled(xyz.x, s);
    int y = mdps_scaled(xyz.y, s);
    int z = mdps_scaled(xyz.z, s);
    return xyz_mk(x,y,z);
}

gyro_t init_gyro(uint8_t addr, lsm6ds33_dps_t dps, lsm6ds33_hz_t hz) {
    dev_barrier();

    if(!legal_dps(dps))
        panic("invalid dps: %x\n", dps);
    if(!legal_gyro_hz(hz)) 
        panic("invalid hz: %x\n", hz);

    // gyroscope turn off / turn on time = 80ms!  app note p22
    unimplemented();

    dev_barrier();
}

// if GDA of status reg (bit offset = 1) is 1 then there is data.
int gyro_has_data(gyro_t *h) {
    unimplemented();
}

imu_xyz_t gyro_rd(gyro_t *h) {
    unimplemented();
}

static void test_dps(int expected, uint8_t h, uint8_t l, int dps_scale) {
    int v = mdps_raw(h,l);
    // the tests are in terms of dps not mdps
    int s = mdps_scaled(v, dps_scale) / 1000;
    printk("raw=%d, scaled=%d, expected=%d\n", v, s, expected);

    // can have issues b/c of roundoff error 
    if(expected != s
    && expected != (s+1)
    && expected != (s-1))
        panic("expected %d, got = %d, scale=%d\n", expected, s, dps_scale);
}

void do_gyro_test(void) {
    // electronics.stackexchange.com/questions/56234/understanding-the-sensitivity-of-an-l3g4200d-gyroscope
    // explanation of why the number is weird.   the guess is so you get
    // extra range and can compensate the 0 rotation drift and still fit
    // w/in an ADC's range (i think)
    //
    // app note, p 26
    //  32767
    //
    // p 15 of the data sheet says: says 125, 245, 500, 1000, 2000
    //  125 4.375, 
    //  245 8.75,  ---- i think this is 250?  doesn't make sense.
    //  500 17.50
    //  1000 35
    //  2000 70
    // 
    // self test is on 245dps
    // so for 245 we get a 8750 scaling factor
    unsigned scale = dps_to_scale(245);
    test_dps(0, 0x00, 0x00, scale);
    test_dps(100, 0x2c, 0xa4, scale);
    test_dps(200, 0x59, 0x49, scale);
    test_dps(-100, 0xd3, 0x5c, scale);
    test_dps(-200, 0xa6, 0xb7, scale);
    printk("dps scaling passed\n");

    gyro_t h = init_gyro(lsm6ds33_default_addr, lsm6ds33_245dps, lsm6ds33_208hz);

    for(int i = 0; i < 10; i++) {
        imu_xyz_t v = gyro_rd(&h);
        printk("gyro raw values: x=%d,y=%d,z=%d\n", v.x,v.y,v.z);

        v = gyro_scale(&h, v);
        printk("gyro scaled values in mdps: x=%d,y=%d,z=%d\n", v.x,v.y,v.z);

        delay_ms(500);
    }
}

/**********************************************************************
 * trivial driver.
 */
void notmain(void) {
    uart_init();

    delay_ms(100);   // allow time for device to boot up.
    i2c_init();
    delay_ms(100);   // allow time to settle after init.


    uint8_t dev_addr = lsm6ds33_default_addr;
    uint8_t v = imu_rd(dev_addr, WHO_AM_I);
    if(v != VAL_WHO_AM_I)
        panic("Initial probe failed: expected %x, got %x\n", VAL_WHO_AM_I, v);
    else
        printk("SUCCESS: lsm acknowledged our ping!!\n");

    // part 2
    do_gyro_test();

    clean_reboot();
}
