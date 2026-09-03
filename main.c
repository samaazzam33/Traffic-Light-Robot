#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define TICKS_GREEN    5U
#define TICKS_YELLOW   2U
#define TICKS_RED      4U
#define QUEUE_BUSY     6U
#define LOG_LEN       20U

typedef enum {
    LIGHT_GREEN = 0,
    LIGHT_YELLOW,
    LIGHT_RED
} LightState_t;

#define BIT_NIGHT      0U
#define BIT_BUSY       1U
#define BIT_BLINK_ON   2U

#define SET_BIT(reg, n)    ((reg) |= (uint8_t)(1U << (n)))
#define CLR_BIT(reg, n)    ((reg) &= (uint8_t)~(1U << (n)))
#define TOGGLE_BIT(reg, n) ((reg) ^= (uint8_t)(1U << (n)))
#define READ_BIT(reg, n)   ((uint8_t)(((reg) >> (n)) & 1U))

static LightState_t light;
static uint8_t status;
static uint8_t ticksLeft;
static uint8_t carsWaiting;
static uint32_t carsPassed;
static char logLine[LOG_LEN];

static void resetCrossing(void);
static uint8_t ticksFor(LightState_t s);
static LightState_t nextState(LightState_t s);
static void drawLight(void);
static void tick(void);
static void addCars(void);
static void toggleNight(void);
static void pushLog(char c);
static void showLog(void);
static void crossingReport(void);

static void resetCrossing(void)
{
    light = LIGHT_RED;
    status = 0U;
    ticksLeft = TICKS_RED;
    carsWaiting = 0U;
    carsPassed = 0U;
    logLine[0] = '\0';
}

static uint8_t ticksFor(LightState_t s)
{
    if (s == LIGHT_GREEN) {
        if (READ_BIT(status, BIT_BUSY)) {
            return TICKS_GREEN + 2U;
        }
        return TICKS_GREEN;
    }

    if (s == LIGHT_YELLOW) {
        return TICKS_YELLOW;
    }

    return TICKS_RED;
}

static LightState_t nextState(LightState_t s)
{
    if (s == LIGHT_GREEN) {
        light = LIGHT_YELLOW;
    } else if (s == LIGHT_YELLOW) {
        light = LIGHT_RED;
    } else {
        light = LIGHT_GREEN;
    }

    return light;
}

static void drawLight(void)
{
    const char *name;
    char green = 'O';
    char yellow = 'O';
    char red = 'O';

    if (READ_BIT(status, BIT_NIGHT)) {
        name = "YELLOW";
        if (READ_BIT(status, BIT_BLINK_ON)) {
            yellow = '*';
        }
    } else if (light == LIGHT_GREEN) {
        name = "GREEN";
        green = '*';
    } else if (light == LIGHT_YELLOW) {
        name = "YELLOW";
        yellow = '*';
    } else {
        name = "RED";
        red = '*';
    }

    printf("\n   [%c]\n", green);
    printf("   [%c]\n", yellow);
    printf("   [%c]\n", red);
    printf("Colour: %s\n", name);
    printf("Ticks left: %u\n", ticksLeft);
    printf("Cars waiting: %u\n", carsWaiting);
}

static void tick(void)
{
    uint8_t passed = 0U;

    if (READ_BIT(status, BIT_NIGHT)) {
        TOGGLE_BIT(status, BIT_BLINK_ON);
        pushLog('y');
        return;
    }

    if (ticksLeft > 0U) {
        ticksLeft--;
    }

    if (light == LIGHT_GREEN) {
        while (passed < 2U && carsWaiting > 0U) {
            carsWaiting--;
            carsPassed++;
            passed++;
        }
    }

    if (ticksLeft == 0U) {
        ticksLeft = ticksFor(nextState(light));
    }

    if (light == LIGHT_GREEN) {
        pushLog('G');
    } else if (light == LIGHT_YELLOW) {
        pushLog('Y');
    } else {
        pushLog('R');
    }

    if (carsWaiting > QUEUE_BUSY) {
        SET_BIT(status, BIT_BUSY);
    } else {
        CLR_BIT(status, BIT_BUSY);
    }
}

static void addCars(void)
{
    unsigned int amount;

    printf("How many cars arrived? ");

    if (scanf("%u", &amount) != 1) {
        printf("Please enter a valid number.\n");
        scanf("%*s");
        return;
    }

    if (amount > 255U || amount > (255U - carsWaiting)) {
        printf("Number is too large.\n");
        return;
    }

    carsWaiting = (uint8_t)(carsWaiting + amount);

    if (carsWaiting > QUEUE_BUSY) {
        SET_BIT(status, BIT_BUSY);
    } else {
        CLR_BIT(status, BIT_BUSY);
    }

    printf("%u cars added.\n", amount);
}

static void toggleNight(void)
{
    if (READ_BIT(status, BIT_NIGHT)) {
        CLR_BIT(status, BIT_NIGHT);
        CLR_BIT(status, BIT_BLINK_ON);
        printf("Day mode is ON.\n");
    } else {
        SET_BIT(status, BIT_NIGHT);
        SET_BIT(status, BIT_BLINK_ON);
        printf("Night mode is ON. Yellow is blinking.\n");
    }
}

static void pushLog(char c)
{
    uint8_t len = 0U;

    while (len < LOG_LEN && logLine[len] != '\0') {
        len++;
    }

    if (len < LOG_LEN - 1U) {
        logLine[len] = c;
        logLine[len + 1U] = '\0';
    } else {
        memmove(logLine, logLine + 1U, LOG_LEN - 2U);
        logLine[LOG_LEN - 2U] = c;
        logLine[LOG_LEN - 1U] = '\0';
    }
}

static void showLog(void)
{
    printf("\n===== LIGHT LOG =====\n");
    printf("%s\n", logLine);
}

static void crossingReport(void)
{
    uint8_t i;

    printf("\n===== CROSSING REPORT =====\n");
    printf("Cars passed: %lu\n", (unsigned long)carsPassed);
    printf("Cars waiting: %u\n", carsWaiting);

    if (READ_BIT(status, BIT_NIGHT)) {
        printf("Night: YES\n");
    } else {
        printf("Night: NO\n");
    }

    if (READ_BIT(status, BIT_BUSY)) {
        printf("Busy: YES\n");
    } else {
        printf("Busy: NO\n");
    }

    printf("Status binary: ");
    for (i = 8U; i > 0U; i--) {
        printf("%u", READ_BIT(status, (uint8_t)(i - 1U)));
    }

    printf("\nStatus hex: 0x%02X\n", status);
}

int main(void)
{
    unsigned int choice;

    resetCrossing();

    do {
        printf("\n===== TRAFFIC LIGHT ROBOT =====\n");
        printf("1. Draw Light\n");
        printf("2. Tick\n");
        printf("3. Add Cars\n");
        printf("4. Toggle Night\n");
        printf("5. Show Log\n");
        printf("6. Crossing Report\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%u", &choice) != 1) {
            printf("Please enter a valid menu number.\n");
            scanf("%*s");
            continue;
        }

        switch (choice) {
        case 1:
            drawLight();
            break;
        case 2:
            tick();
            break;
        case 3:
            addCars();
            break;
        case 4:
            toggleNight();
            break;
        case 5:
            showLog();
            break;
        case 6:
            crossingReport();
            break;
        case 0:
            printf("Goodbye.\n");
            break;
        default:
            printf("Invalid choice.\n");
            break;
        }
    } while (choice != 0U);

    return 0;
}