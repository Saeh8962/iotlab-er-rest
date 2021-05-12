/*
 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *      Example resource
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include <string.h>
#include "rest-engine.h"
#include "er-coap.h"

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]", (lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3], (lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

PROCESS(temp_process, "temp");

static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_event_handler();

/*
 * Example for an event resource.
 * Additionally takes a period parameter that defines the interval to call [name]_periodic_handler().
 * A default post_handler takes care of subscriptions and manages a list of subscribers to notify.
 */
EVENT_RESOURCE(res_temp,
               "title=\"Temperature\";obs",
               res_get_handler, // get_handler
               NULL, // post_handler
               NULL, // put_handler
               NULL, // delete_handler
               res_event_handler); // event_handler

/*
 * Use local resource state that is accessed by res_get_handler() and altered by res_event_handler() or PUT or POST.
 */

static uint16_t temperature = 25;
static struct etimer et;

static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "TEMP %u", temperature));

  /* A post_handler that handles subscriptions/observing will be called for periodic resources by the framework. */
}


/*
 * Additionally, res_event_handler must be implemented for each EVENT_RESOURCE.
 * It is called through <res_name>.trigger(), usually from the server process.
 */
static void
res_event_handler()
{

  /* Usually a condition is defined under with subscribers are notified, e.g., event was above a threshold. */
  if(1) {
    PRINTF("temp %u for /%s\n", temperature, res_temp.url);

    /* Notify the registered observers which will trigger the res_get_handler to create the response. */
    REST.notify_subscribers(&res_temp);
  }
}

void 
init_res_temp() {
  process_start(&temp_process, NULL);
}

#define CONF_HIGH_DURATION   300
#define CONF_LOW_DURATION   150

static uint8_t is_high = 0;
static uint32_t time = 0;
static uint32_t time_start = 0;

PROCESS_THREAD(temp_process, ev, data)
{
  PROCESS_BEGIN();
    

  while (1) {
    etimer_set(&et, CLOCK_SECOND);
    PROCESS_WAIT_EVENT();
    time = clock_time() / CLOCK_SECOND;

    if (time % 10 == 0) {
      res_temp.trigger();
    }

    if (is_high) {
      if (time_start == 0) {
        time_start = time;
      }
      if (time - time_start > CONF_HIGH_DURATION) {
        time_start = 0;
        is_high =  0;
      }
      temperature = 50 + (random_rand() % 25);
    } else {
      if (time_start == 0) {
        time_start = time;
      }
      if (time - time_start > CONF_LOW_DURATION) {
        time_start = 0;
        is_high =  1;
      }
      temperature = 25 + (random_rand() % 25);
    }

  }

  PROCESS_END();

  return PT_ENDED;
}
