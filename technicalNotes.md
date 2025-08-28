**Measuring motor (fan) velocity**

Computer fans generally use [DC brushless motors](https://en.wikipedia.org/wiki/Brushless_DC_electric_motor). A DC motor requires constantly changing the polarity of the electromagnets to keep the motor turning. While some motors use a commutator to do this, this requires a brush, which will wear out. Brushless motors are controlled by an integrated circuit (IC) and so do not require brushes (hence the name) \- the magnets change their polarity at the right times, controlled by the IC.

![Computer fan without the blades][images/image1.jpg]  
*A computer fan (without the blades) \- the electromagnets can be seen.*

The blades of the fan are attached to a permanent circular magnet

![Blades of a computer fan][images/image2.jpg]  
*The blades of a computer fan \- the circular magnet can also be seen (the fan lost some blades in a collision, do not try at home).*

Many electric motors, such as those used in computer fans, also include a [Hall effect sensor](https://en.wikipedia.org/wiki/Hall_effect_sensor). This sensor measures the magnetic field. As the blades are rotating, the permanent magnet is also rotating, causing a constantly changing magnetic field. In this case, the sensor is a CC6407 Hall effect sensor. It changes the voltage when a particular threshold is crossed.  
![Hall effect sensor][images/image3.jpg]  
*The hall effect sensor (CC6407) is shown by the red ellipse.*

This can be observed using an oscilloscope (see picture below), a video demonstrating how the signal changes as the motor gets faster can be found in this video (with an explanation in Hebrew).

https://github.com/user-attachments/assets/13270831-8d5a-45f5-8470-a30962973197

![Oscilloscope showing the signal from the third pin of the fan][images/image4.jpg]  
*An oscilloscope showing the signal received from the third pin of the fan*

Every rotation causes the signal to rise and fall twice \- so we just need to count how many times this occurs per second, and then divide the result by two. We will be using a development board based on the [ESP32 microcontroller](https://en.wikipedia.org/wiki/ESP32), with the program written using the Arduino environment. The ESP32 has a nice feature to help us with this called [PCNT](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/pcnt.html) (pulse counter). The problem with counting pulses is that the microcontroller can only do one thing at a time, so we may miss the changes if we include it in our program. Another approach is to use [interrupts](https://roboticsbackend.com/arduino-interrupts/), but the pulse counter is a simpler solution.

Our program updates approximately once a second. This line of the program calculates the speed:

`int speedHz = 1000.0 * (float)deltaCount / (float)deltaTime/PULSES_PER_REVOLUTION;`

In this case, PULSES\_PER\_REVOLUTION is set to 2, deltaTime is the change in time since we last checked, and deltaCount is the number of pulses since we last checked. The 1000 is needed because deltaTime is in units of milliseconds (ms) and we want the result in Hz \= 1/s.

The full Arduino program can be found [here](fanCounter/fanCounter.ino)

In order to validate the speed \- we filmed the device at 240 Hz (iPhone slow motion), examples can be found for [15 Hz](videos/motor_speed_test_15Hz.mov), [28 Hz](videos/motor_speed_test_28Hz.mov), and [46 Hz](videos/motor_speed_test_46Hz.mov).

[images/image1.jpg]: images/image1.jpg
[images/image2.jpg]: images/image2.jpg
[images/image3.jpg]: images/image3.jpg
[images/image4.jpg]: images/image4.jpg
