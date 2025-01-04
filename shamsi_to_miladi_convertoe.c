#include <stdio.h>

// تابعی برای بررسی سال کبیسه
int isLeapYear(int year) {
    return (year % 400 == 0) || (year % 100 != 0 && year % 4 == 0);
}

// تابعی برای تبدیل تاریخ میلادی به شمسی
void gregorianToJalali(int gYear, int gMonth, int gDay, int *jYear, int *jMonth, int *jDay) {
    // تعداد روزهای هر ماه میلادی
    int gDaysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    // تعداد روزهای هر ماه شمسی
    int jDaysInMonth[] = {31, 31, 31, 31, 31, 31, 30, 30, 30, 30, 30, 29};

    // محاسبه تعداد سال‌ها، ماه‌ها و روزهای میلادی
    int gy = gYear - 1600;
    int gm = gMonth - 1;
    int gd = gDay - 1;

    // محاسبه تعداد روزهای گذشته از تاریخ میلادی
    int gDayNo = 365 * gy + gy / 4 - gy / 100 + gy / 400;
    for (int i = 0; i < gm; ++i) {
        gDayNo += gDaysInMonth[i];
    }
    if (gm > 1 && isLeapYear(gYear)) {
        gDayNo++;
    }
    gDayNo += gd;

    // محاسبه تعداد روزهای گذشته از تاریخ شمسی
    int jDayNo = gDayNo - 79;
    int jNp = jDayNo / 12053;
    jDayNo %= 12053;

    *jYear = 979 + 33 * jNp + 4 * (jDayNo / 1461);
    jDayNo %= 1461;

    if (jDayNo >= 366) {
        *jYear += (jDayNo - 1) / 365;
        jDayNo = (jDayNo - 1) % 365;
    }

    *jMonth = 0;
    while (jDayNo >= jDaysInMonth[*jMonth]) {
        jDayNo -= jDaysInMonth[*jMonth];
        (*jMonth)++;
    }
    *jMonth += 1;
    *jDay = jDayNo + 1;
}

// تابعی برای محاسبه روز هفته
const char *getDayOfWeek(int year, int month, int day) {
    int d = day;
    int m = (month < 3) ? (month + 10) : (month - 2);
    int y = (month < 3) ? (year - 1) : year;
    int c = y / 100;
    y %= 100;
    int w = (d + (2 * m) + (3 * (m + 1) / 5) + y + (y / 4) - (c / 2) + 77) % 7;
    const char *daysOfWeek[] = {"Saturday", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
    return daysOfWeek[w];
}

int main() {
    int gYear, gMonth, gDay;
    printf("Enter Gregorian date (YYYY MM DD): ");
    scanf("%d %d %d", &gYear, &gMonth, &gDay);

    int jYear, jMonth, jDay;
    gregorianToJalali(gYear, gMonth, gDay, &jYear, &jMonth, &jDay);

    const char *dayOfWeek = getDayOfWeek(gYear, gMonth, gDay);

    printf("Jalali date: %d/%d/%d\n", jYear, jMonth, jDay);
    printf("Day of the week: %s\n", dayOfWeek);

    return 0;
}