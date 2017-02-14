<u>**MT7610U for Linux**</u>

Driver for 802.11ac USB Adapter with MT7610U chipset  
Only STA/Monitor Mode is supported, no AP.  

<u>At least v4.0 is needed to compile this module</u>  
sorry people with older kernels, the code is removed.

Currently tested on X86_64 platform(s) **only**,  
cross compile possible, but not fully tested, see TODO.  

For compiling type  
`make`  
in source dir  

To install the firmware files  
`sudo make installfw`

For install the driver use  
`sudo insmod mt7610u.ko`  

To Unload driver you need to disconnect the device  
unload is working, need further testing  

If the driver fails building consult your distro how to  
install the kernel sources and build an <u>external</u> module.
  
Questions about this will **silently** ignored !  
They are <u>plenty</u> information around the web.  

**NOTES**  

The original driver is a mess and **PITA**  
Both mt7610u and mt7612u <u>can</u> work with the same driver.  
**but currently dont't**  

Code which is missing in one driver, may found in the other driver.  
i.e STA, AP, RSSI, LED handling stuff  

**NOTES for an/ac only devices (5GHz)**  

Sitecom, Develo and TP-LLink released a 5GHz **only** device  
`0x7392:0xc711 Devolo Wifi ac Stick`  
`0x0df6:0x0079 Sitecom Europe B.V. ac  Stick`  
`0x2357:0x0105 TP-LINK Archer T1U`  
which are also supported, but currenty the driver will  
scan the missing 2.4GHz band too, see TODO  
  
**BUG on monitor**  
- Due lack of time monitor is *only* barely tested.  
- Channel switching may work.    
- Monior mode can *currently* exhaust memory, will be fixed  
  
  
**BUGS**  
- can't unload driver, must disconnect device(s) (fixed ?)  
- remove stupid endianess with device descriptors  
  

**TODO**, in order of no appearance ;-)  
- fix 5GHz only device  
- fix unloading driver (fixed ?)  
- do more function typesafe  
- cross compile check with real hw on $target  
- check for wrong typecasts  
- remove/strip hardcoded `RT2870STA.dat`  
- check for wrong variable sizes (driver was for 32bit)  
- update to more USB-IDs  
- check if monitor mode is working  
- more cleanup and other stuff  
- fix compile warnings  
- misc. other stuff  

**FUTURE**  
- mac80211, but currently I'm mopping up the mess  

Hans Ulli Kroll <ulli.kroll@googlemail.com>




