<u>**MT7610U for Linux**</u>

Driver for 802.11ac USB Adapter with MT7610U chipset  
Only STA Mode is supported, no AP.  

Monitor mode is not tested.

<u>At least v4.0 is needed to compile this module</u>  
sorry people with older kernels, the code is removed.

Currently tested on X86_64 platform(s) **only**,  
cross compile possible, but not fully tested.  

For compiling type  
`make`  
in source dir  

For install the driver use  
`sudo insmod mt7610u.ko`  

To Unload driver you need to disconnect the device

If the driver fails building consult your distro how to  
install the kernel sources and build an external module.
  
Questions about this will **silently** ignored !  
They are <u>plenty</u> information around the web.  

**NOTES**  
The original driver is a mess and **PITA**  
Both mt7610u and mt7612u <u>can</u> work with the  
same driver.  
Code which is missing in one driver, may found in the other driver.  
i.e STA, AP, RSSI, LED handling stuff  

**BUGS**  
- can't unload driver, must disconnect device(s) first  
  

**TODO**, in order of no appearance ;-)  
- fix unloading driver  
- do more function typesafe  
- cross compile check with real hw on $target  
- strip fw files and use kernel firmware load  
- check for wrong typecasts  
- remove/strip hardcoded `RT2870STA.dat`  
- check for wrong variable sizes (driver was for 32bit)  
- update to more USB-IDs  
- check if monitor mode is working  
- more cleanup and other stuff  
- fix compile warnings  
- misc. other stuff  


Hans Ulli Kroll <ulli.kroll@googlemail.com>




