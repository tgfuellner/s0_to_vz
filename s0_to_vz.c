/*
S0/Impulse to Volkszaehler

Compile as follows:

    gcc -o s0 s0.c -lwiringPi

Run as follows:

    sudo ./s0

 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <curl/curl.h>          /* multiprotocol file transfer library */


// Use GPIO Pin 17, which is Pin 0 for wiringPi library

#define PIN 0

#define NAME "s0_to_vz"
#define VERSION "1.0"

/* Hostname, FQDN or IP of your VZ - normaly this should be 'localhost' */
const char* VZSERVER = "demo.volkszaehler.org";
/* Path to the VZ middleware.php script, WITHOUT preposed and trailing slash */
const char* VZPATH = "middleware.php";
/* HTTP Port of your VZ */
const int VZPORT = 80;
const char* VZUUID = "a03572f0-9d6b-11e3-ae04-d7f3e0294bbb";


CURL *easyhandle;
CURLM *multihandle;

// the event counter 
volatile int eventCounter = 0;


unsigned long long unixtime() {
    struct timeval tv;

    gettimeofday(&tv,NULL);
    unsigned long long ms_timestamp = (unsigned long long)(tv.tv_sec) * 1000
                                    + (unsigned long long)(tv.tv_usec) / 1000;

    return ms_timestamp;
}

void update_curl_handle() {
		static unsigned long long ut_last=0;
		unsigned long long ut = unixtime();
        static char url[128];

		if (ut_last && ut - ut_last > 120) {
			curl_multi_remove_handle(multihandle, easyhandle);

			sprintf(url, "http://%s:%d/%s/data/%s.json?ts=%llu",
                                VZSERVER, VZPORT, VZPATH, VZUUID, ut);

			//printf("period=%llu", ut-ut_last);
		
			curl_easy_setopt(easyhandle, CURLOPT_URL, url);
			curl_multi_add_handle(multihandle, easyhandle);
		}

        ut_last = ut;
}

// -------------------------------------------------------------------------
// myInterrupt:  called every time an event occurs
void myInterrupt(void) {
    static CURLMcode multihandle_res;
    static int running_handles;

    eventCounter++;
    update_curl_handle();

    if ((multihandle_res = curl_multi_perform(multihandle, &running_handles))
          != CURLM_OK) {
        fprintf(stderr, "HTTP_POST(): %s\n",curl_multi_strerror(multihandle_res));
    }
}

// -------------------------------------------------------------------------
// main
int main(void) {
    FILE* devnull = NULL;
    static char errorBuffer[CURL_ERROR_SIZE+1];

    curl_global_init(CURL_GLOBAL_ALL);
    multihandle = curl_multi_init();
    easyhandle = curl_easy_init();

    //curl_easy_setopt(easyhandle, CURLOPT_URL, url);
    curl_easy_setopt(easyhandle, CURLOPT_POSTFIELDS, "");
    curl_easy_setopt(easyhandle, CURLOPT_USERAGENT, NAME " " VERSION );
    curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, devnull);
    curl_easy_setopt(easyhandle, CURLOPT_ERRORBUFFER, errorBuffer);

    curl_multi_add_handle(multihandle, easyhandle);


    // sets up the wiringPi library
    if (wiringPiSetup () < 0) {
        fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno));
        return 1;
    }

    if ( wiringPiISR (PIN, INT_EDGE_RISING, &myInterrupt) < 0 ) {
        fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
        return 1;
    }

    /*
    // display counter value every second.
    while ( 1 ) {
      printf( "%d\n", eventCounter );
      eventCounter = 0;
      delay( 1000 ); // wait 1 second
    }
    */

    curl_global_cleanup();
    return 0;
}
