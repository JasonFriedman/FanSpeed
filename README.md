# FanSpeed

![A device for measuring fan speed][images/example.jpg]

These are instructions on how to build an ESP32 based device with a screen (using the Lily TTGO T-display) to measure the speed of a computer fan.

A [short technical explanation](technicalNotes.md) explains how the velocity is measured and how the device was validated.

Part list:

1. [LilyGo TTGO T display](https://lilygo.cc/products/lilygo%C2%AE-ttgo-t-display-1-14-inch-lcd-esp32-control-board) or a [clone](https://www.aliexpress.com/item/1005005970553639.html)
2. Two wires with alligator clips at one end, and a wire at the other end (I used black for ground, and yellow for the tacho - I bought wire with alligotor clips at both ends and cut them in half)
3. [3D printed box for the device](https://makerworld.com/en/models/1388397-ttgo-t-display-enclosure)

Construction:

1. Make a hole in the side of the case next to pin 26
2. Pass the wires through the hole
3. Solder the yellow wire to pin 26
4. Solder the black wire to G (ground), there is one two pins away from pin 26

Program:

- The [program](fanCounter/fanCounter.ino) can be copied to the device using the [Arduino IDE](https://www.arduino.cc/en/software/)

Using the device:

![pin diagram for a 4 wire computer fan pin][images/4wire.jpg]

- Connect the black wire to the ground (GND) of the motor (leftmost pin)
- Connect the yellow wire to the tacho (TACH) of the motor (third pin)
- If you are using a 3 pin connector, it will be the same (there will not be the fourth wire - PWM)

The device can be powered using the USB-C cable (e.g. using a phone charger or external battery)

In the Arduino code, there is an option to also add a DS18B20 temperature sensor

[images/example.jpg]: images/example.jpg
[images/4wire.jpg]: images/4wire.jpg