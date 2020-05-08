#ifndef __LSM6DS33_H__
#define __LSM6DS33_H__

enum { lsm6ds33_default_addr = 0b1101011 }; // this is the gyro/accel;

// accel
typedef enum {
        // P46 of LSM6DS33.pdf: 00 = 2g, 01 = 16g, 10 = 4g, 11 = 8g
        // we encode the scale in the upper 16 bits, the device bit
        // pattern in the low.
        lsm6ds33_2g = (2 << 16) | 0b00,
        lsm6ds33_4g = (4 << 16) | 0b10,
        lsm6ds33_8g = (8 << 16) | 0b11,
        lsm6ds33_16g = (16 << 16) | 0b01
} lsm6ds33_g_t;

// see lsm6ds33.h
static inline int legal_G(lsm6ds33_g_t s) {
    return s == lsm6ds33_2g
    || s == lsm6ds33_4g
    || s == lsm6ds33_8g
    || s == lsm6ds33_16g;
}


static inline int legal_g_bits(unsigned b) {
    return b <= 0b11;
}

static inline unsigned g_scale(lsm6ds33_g_t g) {
    unsigned bits = g&0xff;
    assert(bits <= 0b11);
    switch(bits) {
    case 0b00: return 2;
    case 0b01: return 16;
    case 0b10: return 4;
    case 0b11: return 8;
    default: panic("impossible\n");
    }
}


// accel
typedef enum  {
	// p12 of application note.  xlm_hm_mode=0 -> all are high perf
	lsm6ds33_off	= 0,
	lsm6ds33_13hz   = 0b0001,  	// low power (xl_hm_mode=1)
	lsm6ds33_26hz   = 0b0010,  	// low power
	lsm6ds33_52hz   = 0b0011,  	// low power
	lsm6ds33_104hz  = 0b0100,	// normal power
	lsm6ds33_208hz  = 0b0101,	// normal power
	lsm6ds33_416hz  = 0b0110,	// high performance
	lsm6ds33_833hz  = 0b0111,	// high performance
	lsm6ds33_1660hz = 0b1000,	// high performance
	// next two only legal for accel, not gyro
	lsm6ds33_3330hz = 0b1001,	// high performance
	lsm6ds33_6660hz = 0b1010,	// high performance
} lsm6ds33_hz_t;

typedef enum {
	// wait: page 15 has 125,250,500,2000??
    // we encode the scale in the upper 16 bits, the device bit
    // pattern in the low.
	lsm6ds33_245dps  = (245 << 16) | 0b00,
	lsm6ds33_500dps  = (500 << 16) | 0b01,
	lsm6ds33_1000dps = (1000 << 16) | 0b10,
	lsm6ds33_2000dps = (2000 << 16) | 0b11
} lsm6ds33_dps_t;
		

/* register addresses: LSM6DS33-AN4682.pdf p8 */
enum regAddr {
      FUNC_CFG_ACCESS   = 0x01,

      FIFO_CTRL1        = 0x06,
      FIFO_CTRL2        = 0x07,
      FIFO_CTRL3        = 0x08,
      FIFO_CTRL4        = 0x09,
      FIFO_CTRL5        = 0x0A,
      ORIENT_CFG_G      = 0x0B,

      INT1_CTRL         = 0x0D,
      INT2_CTRL         = 0x0E,
      WHO_AM_I          = 0x0F,
      CTRL1_XL          = 0x10,
      CTRL2_G           = 0x11,
      CTRL3_C           = 0x12,
      CTRL4_C           = 0x13,
      CTRL5_C           = 0x14,
      CTRL6_G           = 0x15,
      CTRL7_G           = 0x16,
      CTRL8_XL          = 0x17,
      CTRL9_XL          = 0x18,
      CTRL10_C          = 0x19,

      WAKE_UP_SRC       = 0x1B,
      TAP_SRC           = 0x1C,
      D6D_SRC           = 0x1D,
      STATUS_REG        = 0x1E,

      OUT_TEMP_L        = 0x20,
      OUT_TEMP_H        = 0x21,
      OUTX_L_G          = 0x22,
      OUTX_H_G          = 0x23,
      OUTY_L_G          = 0x24,
      OUTY_H_G          = 0x25,
      OUTZ_L_G          = 0x26,
      OUTZ_H_G          = 0x27,
      OUTX_L_XL         = 0x28,
      OUTX_H_XL         = 0x29,
      OUTY_L_XL         = 0x2A,
      OUTY_H_XL         = 0x2B,
      OUTZ_L_XL         = 0x2C,
      OUTZ_H_XL         = 0x2D,


      FIFO_STATUS1      = 0x3A,
      FIFO_STATUS2      = 0x3B,
      FIFO_STATUS3      = 0x3C,
      FIFO_STATUS4      = 0x3D,
      FIFO_DATA_OUT_L   = 0x3E,
      FIFO_DATA_OUT_H   = 0x3F,
      TIMESTAMP0_REG    = 0x40,
      TIMESTAMP1_REG    = 0x41,
      TIMESTAMP2_REG    = 0x42,

      STEP_TIMESTAMP_L  = 0x49,
      STEP_TIMESTAMP_H  = 0x4A,
      STEP_COUNTER_L    = 0x4B,
      STEP_COUNTER_H    = 0x4C,

      FUNC_SRC          = 0x53,

      TAP_CFG           = 0x58,
      TAP_THS_6D        = 0x59,
      INT_DUR2          = 0x5A,
      WAKE_UP_THS       = 0x5B,
      WAKE_UP_DUR       = 0x5C,
      FREE_FALL         = 0x5D,
      MD1_CFG           = 0x5E,
      MD2_CFG           = 0x5F,
};


// it's easier to bundle these together.
typedef struct { int x,y,z; } imu_xyz_t;


// note: the names are short and overly generic: this saves typing, but 
// can cause problems later.
typedef struct {
    uint8_t addr;
    unsigned hz;
    unsigned g;
} accel_t;


#endif
