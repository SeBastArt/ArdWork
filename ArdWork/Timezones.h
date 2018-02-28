
#ifndef _TIMEZONES_h
#define _TIMEZONES_h

#include "Timezone.h"

// Germany (Berlin)
static TimeChangeRule CEST = { "CEST", Last, Sun, Mar, 2, 120 };     //Central European Summer Time
static TimeChangeRule CET = { "CET ", Last, Sun, Oct, 3, 60 };       //Central European Standard Time
static Timezone CE(CEST, CET);

// US Eastern Time Zone (New York, Detroit)
static TimeChangeRule usEDT = { "EDT", Second, Sun, Mar, 2, -240 };  //UTC - 4 hours
static TimeChangeRule usEST = { "EST", First, Sun, Nov, 2, -300 };   //UTC - 5 hours
static Timezone usEastern(usEDT, usEST);

// United Kingdom (London, Belfast)
static TimeChangeRule BST = { "BST", Last, Sun, Mar, 1, 60 };        // British Summer Time
static TimeChangeRule GMT = { "GMT", Last, Sun, Oct, 2, 0 };         // Standard Time
static Timezone UK(BST, GMT);

// US Central Time Zone (Chicago, Houston)
static TimeChangeRule usCDT = { "CDT", Second, dowSunday, Mar, 2, -300 };
static TimeChangeRule usCST = { "CST", First, dowSunday, Nov, 2, -360 };
static Timezone usCT(usCDT, usCST);

// US Mountain Time Zone (Denver, Salt Lake City)
static TimeChangeRule usMDT = { "MDT", Second, dowSunday, Mar, 2, -360 };
static TimeChangeRule usMST = { "MST", First, dowSunday, Nov, 2, -420 };
static Timezone usMT(usMDT, usMST);

// Arizona is US Mountain Time Zone but does not use DST
static Timezone usAZ(usMST, usMST);

// US Pacific Time Zone (Las Vegas, Los Angeles)
static TimeChangeRule usPDT = { "PDT", Second, dowSunday, Mar, 2, -420 };
static TimeChangeRule usPST = { "PST", First, dowSunday, Nov, 2, -480 };
static Timezone usPT(usPDT, usPST);

typedef struct {
	uint8_t lang;
	char* _name;
	Timezone _timezone;
} timezone_struct;

static const timezone_struct timezone_Arr[]{
	{ 0, "Berlin", CE },
	{ 1, "New York, Detroit", usEastern },
	{ 2, "London, Belfast", UK },
	{ 3, "Chicago, Houston", usCT },
	{ 4, "Denver, Salt Lake City", usMT },
	{ 5, "Arizona", usAZ },
	{ 6, "Las Vegas, Los Angeles", usPT }

};

static time_t GetLocalTime(int _timezone) {
	time_t utc_time = now();
	time_t local_time = ((Timezone)timezone_Arr[_timezone]._timezone).toLocal(utc_time);
	return local_time;
}

static time_t GetUTCTime() {
	time_t utc_time = now();
	return utc_time;
}

#endif