# Siggly

Team Project specification 2024, semester 1

Generated date: 2024-04-10 11:59:40

Course: **ENGG2800**

---

Notes:

* For the purposes of this document, we consider “code” or “software” to refer to programs which runs on a PC and/or on an embedded system.
* This document uses the words "should", "must" and so on interchangeably to indicate a requirement of particular functionality. The use of "should" does not imply that functionality is optional.
* While we try our best to allow students to have the freedom to make their own design choices, it is not possible for the teaching staff to support every possible development system.
* This specification is a living document! Changes will occur and will be released throughout the semester. Some minor clarifications may appear on the discussion board only. You are expected to keep up to date with these.
* You are permitted to bundle outside orders with other teams in order to reduce the overall cost of shipping, as long as you take into account the UQ Academic Integrity and Student Conduct policy.
* You must only use PCBs which were ordered from ETSG (via [pinecone](https://pinecone2.uqcloud.net/)). Please see the course profile for details.
* There are a number of grade hurdle requirements outlined in the course profile - please make sure you read these *carefully*.
* You must follow all other requirements outlined in the [TP-STD standards](https://source.eait.uq.edu.au/gitlist/tp_std/tree/master/), otherwise your final product mark will be limited to a maximum of 50%.
* All assessment must only use parts from the approved suppliers! See TP-STD-003.
* You must individually commit to git in four weeks of the semester. See TP-STD-004 and blackboard for further details.
* Did I mention that you should read the TP-STD documents? If you don't meet these requirements (as applicable), you will be limited to a maximum of 50% of the total marks available for the final demo.

---

## Introduction
A function generator (sometimes also called waveform generator, frequency generator or signal generator) is a commonly used item in an electronic laboratory. The purpose of this device is to generate a voltage (or sometimes current) waveform which matches the shape of a mathematical function over time. This signal can then be used as a stimulus to a circuit in order to test it under various conditions that might be difficult to reproduce otherwise. Below are some examples of general use cases for function generators:

- Testing the gain of an amplifier
- Measuring the frequency response (ie which frequencies are allowed to pass through) of a circuit
- Using a previously measured waveform (such as an ECG [QRS complex](https://en.wikipedia.org/wiki/QRS_complex)) to test whether a device will work before using it in a real-world application
- Testing for distortion in an amplifier

## Device description

Your device must be able to generate a single channel voltage waveform. This waveform must be exposed to the user through a [BNC connector](https://en.wikipedia.org/wiki/BNC_connector). The maximum output voltage range for this waveform will be -5V to 5V. If the combination set by the user would go beyond this +/- 5V range, the output should *clip* at the ends of this range. For example, if the current voltage to be output would mathematically be -6V, the device should instead output -5V. The device must generate a voltage within +/- 50mV of the target voltage when generating waveforms into a 1k ohm (or greater) resistor. (Connecting directly to an oscilloscope is effectively connecting the output to a 1M resistor)

The device must have a "trigger output" BNC connector, which outputs a 0V to 5V pulse every time the waveform restarts after one period. 


All BNC connectors must have their outer shell connected to GND, and the inner contact used to carry the signal of interest.

### Built-in waveforms

Your device must be able to generate the following waveforms:

- DC
- Sine
- Square
- Triangle
- [Sawtooth/ramp](https://en.wikipedia.org/wiki/Sawtooth_wave)
- [Reverse sawtooth/ramp](https://en.wikipedia.org/wiki/Sawtooth_wave)

The following parameters of the waves must be controllable via the device interface (all ranges are inclusive of the bounds):

- Wave type
- Wave frequency (1Hz - 20kHz in steps of 1Hz) or wave period (user must be able to enter both of these by choosing one)
- Wave amplitude (0V to 5V, 50mV steps) or wave peak-to-peak voltage (-5V to 5V, 100mV steps; user must be able to enter both of these by choosing one)
- Wave offset (-5V to 5V, 100mV steps)


If waves are generated using a DAC, all waves must be generated with a resolution of at least 10 bits (1024 levels) and the output sample rate must be minimum of 160kHz (8 samples per period at 20kHz). Your device must always generate the maximum number of samples possible given this sample rate. Changes in amplitude and offset must be implemented in the analog domain, as performing it in the digital domain will reduce the voltage resolution of the output samples. More information about ADC and DAC sampling, resolution and quantisation error can be found [here](https://tp-info.uqcloud.net/books/analog-to-digital-converters/page/adcs-and-dacs-video).


Your device must be able to have the output waveform switched on and off using the user interface. If the waveform output is off, the waveform output BNC connector and the trigger output BNC connector should both have 0V applied to their outputs.



### LCD module, controls and indicators

Your device must have a power switch which is externally accessible to turn the device on and off. You must have an LED that is externally visible to indicate whether the device is powered on or not. 

Your device must contain an [LCD module](https://www.buydisplay.com/lcd-display-serial-graphic-display-128x64-st7920-white-on-blue) (ETSG part 24-05-03). It must also contain a suitable number of momentary pushbuttons (maximum 10; example part ETSG 03-78-01) that can be used to control the functionality of the device. Other than the power switch, these momentary buttons must be the only physical control inputs on your device.

The backlight of the LCD module must be controllable through the user interface. It must be able to be set to five different levels, where the lowest level is completely off, and the highest level is completely on.

Your device must have an LED in close proximity to the waveform output BNC connector (on the output board) which is on if a waveform is being generated, and off it is not.

It is expected you will develop a reasonable interface to show all of the adjustable parameters, and to allow the user to configure them inside the valid ranges. The entire configuration must be saved in non-volatile memory - if power is disconnected and reconnected, the device must return to the same state.

As mentioned previously, the output should clip at the minimum and maximum range of -5V to 5V. If this clipping occurs, an error indicator should be visible on the display.

### PC communication

Your device must use the [polyglot-turtle-xiao](https://github.com/jeremyherbert/polyglot-turtle-xiao) firmware with a Seeeduino Xiao (ETSG part 24-01-01) as the only means of communication with the PC over a USB connection. This connection must carry both current status and parameter data, as well as configuration information over the UART interface. The LCD must show a status icon to indicate whether the PC software is connected or not. An icon should still be present to indicate the negative state (disconnected).


No other means of communication with the PC is permitted.


## Power supply
Your device must expose a 3 pin Molex KK connector which can be used to supply +9V, -9V and GND from the lab power supply. Your device may also optionally be powered by the 5V supplied by polyglot-turtle-xiao board.

It is acceptable to connect the 3.3V output from the polyglot-turtle to your circuit, but you should not draw any significant current from it (max 50mA) as it is not designed for this purpose.


## PC software

The PC software for the device must be able to configure and select/send any of the waveform types/parameters to the device, and all parameters must be maintained on the device through power cycles. All parameters must be synchronised and updated live (within 1 second, without the user pressing any "send" or "receive" buttons) between the on-device user interface and the PC interface after any changes. If a text entry widget is used to enter the frequency or period, it is acceptable if the user needs to click an "apply" button (or similar) just for this single entry widget. 

Anything that is adjustable on the device must also be adjustable in the PC software.

When the PC software first connects to the device, it must transfer all of the settings on the device to the configuration controls in the PC software.




## Construction and physical dimensions

Your final product must be constructed using two custom designed PCBs, and the PCBs must be designed to be mounted on the acrylic frame, the design of which is available on Blackboard under Assessment => Product. You must create an "output PCB" that will be mounted at a right angle to the main PCB which contains the BNC connectors, silkscreen labelling, some LED indicators and optionally the power switch. You must not have any non-trivial circuits on the output PCB. The output PCB must have the function of the BNC connectors clearly labelled. You must not connect the output board to your main PCB using wires soldered between the boards; a cable with a non-reversible connector or a board-to-board connector must be used.

Submitting your product on breadboard will result in your maximum mark for the final demo being limited to 50%.

The following breakout boards are approved for use without triggering the grade hurdles outlined in the course profile:

* Seeeduino Xiao (ETSG part 24-01-01)
* LCD module (ETSG part 24-05-03)
* Any breakout board which contains only a DAC, ADC, op-amp or digital potentiometer IC

## Budget/Bill Of Materials (BOM)
Your final product must have a BOM total of $125 AUD or less (excluding GST). Your team will have a $200 AUD development budget available at ETSG; reimbursement up to this amount for parts purchased externally may be possible at the end of semester; further details and conditions will be made available on Blackboard. However, please be aware that reimbursements will **only** be possible if you have a tax invoice for the items in AUD.

You do not need to include the cost of the USB cable in your BOM, but you must still provide it with your submission. 

## Other components included in your locker
* A resistor kit containing various values
* A capacitor kit containing various values
* Various jumper wires
* ATMEGA328P (ETSG part 10-03-03): [https://www.digikey.com.au/product-detail/en/microchip-technology/ATMEGA328P-PU/ATMEGA328P-PU-ND/1914589](https://www.digikey.com.au/product-detail/en/microchip-technology/ATMEGA328P-PU/ATMEGA328P-PU-ND/1914589)
* AVR ISP breakout (ETSG part 16-56-01): [https://www.adafruit.com/product/1465](https://www.adafruit.com/product/1465)
