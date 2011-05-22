This PCB is a rework of Bryan's V3.0 PCB to use the 100pin ATMega 2560. It was primarily done to enable the 
use of JTAG debugging and also to free up some pins for future fun.

If anyone spots any mistakes I want to know, it's not optional, you have to tell, it's the law !
Just respond in the group discussion.

========
= Todo =
========
* You will notice there are some loose tracks on the left hand side of the board. These are to be used to drive a
  vibrating motor for alarms etc. It will probably just consist of another 3v regulator and a transistor, maybe a 
  series inductor to separate the motor rail from the battery, and a couple of caps for the regulator. You will
  notice the supply for the motor goes right back to source, didn't want a dirty motor causing supply problems.
  But first I need to get my hands on a suitable motor and see what sort of voltage and current it needs. I am 
  thinking Playstation2-3 controllers or similar, off to eBay.......

* Move component labels about for the silkscreen, but first I am going to get a look at a V2.14 board.
   
* Copper pour.


===========================
= Differences V3.0 - V4.0 =
===========================

Major
-----
* Replaced 64 pin AMega 2561 with 100 pin 2560
* Added JTAG header
* Changed 10pin ISP header to 6pin
* Added breakout headers for spare pins, the whole of port L, half of port F (4x ADCs), and port H (inc. a second 
  serial port)
* Remaining 4 pins broken out to test point pads here and there.
* Switched the I2C interface to use hardware I2C pins 
* Switched the ADC usage from port F to port K to free up JTAG pins
* Added interfacing, connectors etc. for two rotary encoders as per Multiplex Evo sets (Hardware debouncing 
  with a little RC network). Added these on pins with interrupt triggers just in case needed.
* Switched pin for IDL_SW1 with Spare1 just to make routing easier
* Signals KEY_Y2 and KEY_Y3 have replaced the digital GND signals on the 9 pin headers that go to the pot gimbals 
  and trim switches. Because the trim buttons are now in a switch matrix, digi GND is no longer needed but the 
  matrix signals KEY_Y2 and KEY_Y3 are. On the V3 board these signals came out on a separate 2 pin header but this 
  can be avoided by doing this.
  ** NOTE ** The switch matrix requires a little mod/fix to operate correctly. On both of the horizontal 
  trim PCBs the trim switches are connected to analogue GND and not digi GND as they should be. In order for the 
  matrix to operate correctly we must remove the connection to analogue GND (by cutting PCB traces) and connect it 
  across to the digi GND point (now the KEY_Yn point) on the vertical trim board. 
  Pictures for this mod are in the .\Data Sheets\Switch Matrix Changes\ directory

Minor
-----
* Added mounting holes into schematic so they don't disappear if you tell the netlist update to delete components 
  it can't find 
* Amalgamated the projects KiCAD libraries, and restructured their directories to better reflect KiCAD's own 
  library structure. (They confuse me less this way)
* Changed the sdcard board only to accept the library changes, no physical changes

=====================
= Component changes =
=====================
I renamed a few library parts that also exist in the standard KiCAD libraries to avoid confusion. 

These items are usually standard KiCAD parts that have been copied to the Gruvin library and tweaked a 
little for the needs of the project or to fix errors. These and only these parts have been given the 
_GRUVIN suffix on top of the normal KiCAD name. Original Gruvin library parts are just named normally.

I also edited a few parts.

The diffs are:

Footprints/Modules
------------------
SM0603        -> SM0603_GRUVIN         Silkscreen outline different
PIN_ARRAY_3X1 _> PIN_ARRAY_3X1_GRUVIN  Larger spacing and different 3D representation
PIN_ARRAY_3X2 -> PIN_ARRAY_3x2_GRUVIN  Bigger pads & didn't like 3D representation in std KiCAD part
PIN_ARRAY_4X2 -> PIN_ARRAY_4X2_GRUVIN  Bigger pads & no 3d rep
PIN_ARRAY_4X1 -> PIN_ARRAY_4X1_GRUVIN  Bigger pads & no 3d rep
PIN_ARRAY_5X2 -> PIN_ARRAY_5X2_GRUVIN  Bigger pads on 8 pins
SW_PUSH_6x4MM                          Bigger pads

Schematic components
--------------------
ATMEGA2560-A  -> ATMEGA2560-A_GRUVIN   Would you believe the standard KiCAD part has two power pins missing !
ATMEGA64      -> ATMEGA64_GRUVIN       Not sure, need to ask Bryan
CONN_30       -> CONN_30_LCD_GRUVIN    Added the tabs of the SMT LCD connector in, also put LCD in the name.
 

=======================================
= For reference V2.14 to V3.0 changes =
=======================================

* Change of speaker to PWM pin (provides MUCH cleaner tonnes, too.)
* Existing speaker output retained on original pin also -- this was in fact intended for vibrating motor (optionally.)
* Added breakout of I2C (This was more of a change to the SDcard interface)
* Added transistor in PPM_IN linen (Principal reason was for buffering/amplification.)
* Added breakout of Back Light signal, Vcc, and GND to 3 pin header
* Rearranged stick/trim switch interface wiring to remove awkward/stuffed-up cross-over bug. (This is a biggie!)
* Fix of reversed Collector and Emitter connections on Q3
* Small corrections to hole and switch positions. V2.14 was well within acceptable tolerances though.