# MagicCoat
Magic Coat Using WS2812B LED strips

The goal was to create a Magic Coat, that simmered blue with occasional bursts of color.

Unfortunately, the LED strips chosen proved to be not durable enough and the circuits kept breaking.
Still there are many good lessons learned.


This code shows examples of 
   Multiple (2) Channels for LED Strips control using the FastLED library
   Using Program Memory to overcome RAM shortage
   An Array Map to make Non 2D array of LED strips appear to be a 2D array.



Electronics:
After some experiments, WS2812B (tri color, individually addressable) LED strips were selected.
Both the LED strips and the 8MHz Arduino could be powered off of a single 3.3 volt battery.

Coat:
The coat is a full length, 18th century military style.  It is fitted in the arms and chest, but flares at 
the waist. It was sewn with a (double) layer of chiffon on top of the base fabric creating a space into which 
the LED strips could be inserted.  Button holes were sewn into the base fabric so the strips could be inserted
from the inside. 

LED Strips:
There are 23 strips, 19 LEDs long, in the lower half (waist down) of the coat and 17 strips of various LED 
lengths in the upper (body) portion. Power to the strips is supplied at the top of each strand, but the single 
control line is daisy chained between each strip.  There are two “channels” or control lines.  One for the 23 
lower strips, (23*19=) 437 LEDs, and a second for the upper 17 strips, 208 LEDs. 

Coat Features:
The coat would have a “random” pattern LEDs on to a dim blue.  Various LEDs would sparkle, by shining a 
brighter blue.  Then, on occasion, there would be a burst of green along an individual strip.

Upon startup (and at the beginning of each cycle), the coat would initialize by have a bright blue ring, 
“rain” down the coat, leaving the random blue LEDs on in its wake.  Then at the end of a cycle (x number 
of minutes), there would be a Red fire that would burn up from the bottom, leaving everything off.  The cycle
would repeat.
 
Code Details:
There are too many total LEDs (645) to fit both channels the limited RAM space available on the Arduino Pro
Mini 328 (3.3V/8MHz).  Thus the “random” background pattern is stored in Program memory and retrieved whenever 
an update to either the lower or upper half was needed.  

The lower half of the coat’s LEDs could be treated as a simple two dimensional array (23x19).  In order to have 
the upper half “appear” as a 2D array (with missing elements), a 2D map was created with actual index values.  
A “-1” was used to fill the missing values and indicate invalid index.

Since the mapping function needs to be used on the upper half, but not the lower, there are sister routines 
depending up which portion of the coat is being updated.