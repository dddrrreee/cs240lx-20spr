This next lab we are going to write an i2c driver.

Before lab, make sure you:
  - Read about the I2C interface in the annotated Broadcom doc in our `docs/` directory.
    Note they confusingly call I2C "BSC".  You'll find the description on pages 28-37.
    Do a pull since i added some annotations.

  - Read the i2c wikipedia page for the bit banging version.


A photo is in the lab's docs directory; it's a small rectangle: 
  - `9-i2c-drive/docs/adc-i2c.pdf`.
  - The data sheet is also in there: `ads1114.pdf`.  It's long, but we only need a few
    pieces.  I'll annotate those in a bit.

Hardware:
  - Solder the pins in the small ADC we have since that will make it easier to test our
    code with a known result.
  - Solder the pins the the microphone that got sent.  
  - Plug the potentiometer (the knob thing with three legs) into your breadboad and see
    that when you connect an LED that you can control its brightness.
  
If you have time, it'd be good to be able to quickly connect up your light array
and use the microphone output to control it.
