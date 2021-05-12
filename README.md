A Quick Introduction to the Erbium (Er) REST Engine
===================================================

FILES
-------------

- er-example-server.c: A RESTful server example showing how to use the REST
  layer to develop server-side applications (at the moment only CoAP is
  implemented for the REST Engine).

PRELIMINARIES
-------------

- Make sure rpl-border-router has the same stack and fits into mote memory:
  You can disable RDC in border-router project-conf.h (not really required as BR keeps radio turned on).
    #undef NETSTACK_CONF_RDC
    #define NETSTACK_CONF_RDC     nullrdc_driver
- Alternatively, you can use the native-border-router together with the slip-radio.
- For convenience, define the Cooja addresses in /etc/hosts
      aaaa::0212:7401:0001:0101 cooja1
      aaaa::0212:7402:0002:0202 cooja2
      ...
- Get the Copper (Cu) CoAP user-agent from
  [https://addons.mozilla.org/en-US/firefox/addon/copper-270430](https://addons.mozilla.org/en-US/firefox/addon/copper-270430)
- Optional: Save your target as default target
      make TARGET=sky savetarget

COOJA HOWTO
-----------

###Starting servers:

    make TARGET=cooja iot-labs.csc

Open new terminal

    make connect-router-cooja

- Start Copper and discover resources at coap://cooja2:5683/ or coap://[aaaa::212:7402:2:202]:5683/
- Choose "Click button on Sky 2" from the context menu of mote 2 (server) after
  requesting /sensors/button
- Do the same when observing /sensors/button
