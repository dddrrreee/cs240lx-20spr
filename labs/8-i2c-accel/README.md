## Using an accelerometer

Make sure you do the prelab!

Today we're going to communicate with an accelerometer (the  LSM6DS3)
using I2C.   These devices are useful for controlling stuff based
on motion.

    [adafruit product page](https://www.adafruit.com/product/2809)
    for the LIS3DH Triple-Axis Accelerometer (+-2g/4g/8g/16g).

While many cheap devices use SPI, it has a lot of wires.  I2C just
needs two.  At a high level I2C devices have a set of (perhaps pretend)
8-bit registers that you use I2C to read from and write to.
  - reads: give the device address you want to talk to (there can be 
   multiple on the same wires) and the register number you want to read,
  get back an
  8-bit quantity.  For example: `v = i2c_read(addr, reg)`.

  - writes: give device address, register number you want to write to
    and 8-bit value you want to write.
	For example: `i2c_write(addr, reg, v)`.

---------------------------------------------------------------------------
### Part1: fill in the accelerometer code in the code directory.

Use the datasheet and application note from the prelab.  Start with the 
simple cookbook example they give and make sure your stuff looks reasonable!

More stuff:
  - Change the scaling for gravity (should see the numbers halve if you 
	double the scale, etc)
  - Change from low power to high power.
  - Convert to float (not you'll have to recompile for floating point).

---------------------------------------------------------------------------
### Part2: fill in the gyroscope code in the code directory.

Use the datasheet and application note from the prelab.  Start with
the simple cookbook example they give and make sure your stuff looks
reasonable!

---------------------------------------------------------------------------
### Extension: display the readings using your light strip.

The nice thing about the light strip is that you can do high-dimensional displays easily.
One dumb way:
   - give the accel half and the gyro half (or do not!  just map their coordinate system
     to the entire thing).
   - map the 3-d point of each reading to a location.
   - map the velocity of the point (or the accel, no pun) to a color.
   - display!

This should probably be required, but I'll have it as an extension.
---------------------------------------------------------------------------
### Some Legit Extensions

If you finish, there's lots of tricks to play.  A major one is doing correcton to 
the device errors.  The accel / gyro device is not that accurate without correction.  This 
article discusses the issues and what can be done (simply):
    - [Correct for hard-ion](https://www.fierceelectronics.com/components/compensating-for-tilt-hard-iron-and-soft-iron-effects)

Different writeups in `./docs` for different directions:
   1. [Make a legit compass](./docs/AN203_Compass_Heading_Using_Magnetometers.pdf)
   2. [Calibrate](./docs/AN4246.pdf)
   3. [Even more compass + Calibrate](./docs/AN4248.pdf)
   4. [Location](./docs/madgewick-estimate.pdf)
