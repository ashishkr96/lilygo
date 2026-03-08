#include <Arduino.h>
#include "date_calc.h"

static const char *MONTH_NAMES[] = {
    "", "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

// h=0→Sat, 1→Sun, …, 6→Fri  (Zeller's congruence output)
static const char *DOW_EN[] = {
    "Saturday", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday"
};
static const char *DOW_HI[] = {
    "शनिवार",   // Sat
    "रविवार",   // Sun
    "सोमवार",   // Mon
    "मंगलवार",  // Tue
    "बुधवार",   // Wed
    "गुरुवार",  // Thu
    "शुक्रवार", // Fri
};

static int monthNum(const char *m) {
    const char *n[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    for (int i = 0; i < 12; i++)
        if (m[0]==n[i][0] && m[1]==n[i][1] && m[2]==n[i][2]) return i + 1;
    return 1;
}

void buildDateInfo(DateInfo *di) {
    char mname[4] = {__DATE__[0], __DATE__[1], __DATE__[2], '\0'};
    di->month = monthNum(mname);
    di->day   = atoi(__DATE__ + 4);
    di->year  = atoi(__DATE__ + 7);

    // Zeller's congruence
    int m = di->month, y = di->year, d = di->day;
    if (m < 3) { m += 12; y--; }
    int k = y % 100, j = y / 100;
    int h = (d + 13*(m+1)/5 + k + k/4 + j/4 - 2*j) % 7;
    if (h < 0) h += 7;

    di->dayName  = DOW_EN[h];
    di->hindiDay = DOW_HI[h];
    snprintf(di->dateStr, sizeof(di->dateStr), "%s %d, %d",
             MONTH_NAMES[di->month], di->day, di->year);
}
