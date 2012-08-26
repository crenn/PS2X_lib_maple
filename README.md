This is a port of Bill Porter's PS2X library for the Arduino to the Leaflabs
Maple platform. The original library can be found here:

    http://www.billporter.info/playstation-2-controller-arduino-library-v1-0/

How to use the library
------------------------------------------------------------------------------
The library is based on the PS2X_lib v1.41 and has been modified to accept a
reference to a HardwareSPI object as well as a speed if desired. The
config_gamepad function has also changed as only the CS/NSS pin needs to be
defined for the library to work.

Some useful information can be found in the following links:

    http://www.gamesx.com/controldata/psxcont/psxcont.htm
    http://www.electro-tech-online.com/electronic-projects-design-ideas-reviews/36220-read-playstation-controller-code.html
    http://sophiateam.undrgnd.free.fr/psx/index.html
    http://www.lynxmotion.com/images/files/ps2cmd01.txt

And a breakout cable can be found here:

    http://www.lynxmotion.com/p-73-ps2-controller-cable.aspx

Port done by crenn (www.crennsmind.com)