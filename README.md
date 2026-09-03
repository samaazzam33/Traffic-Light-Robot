# Traffic Light Robot

## Student Information

- **Student:** Sama Rizk El-Saeed Azzam
- **Email:** samaazzam33@gmail.com

## Project Information

This project is **Project 2: Traffic Light Robot** from the Five C Practice Projects.
It is a plain C99 console program that simulates a traffic light at a crossing.

The light cycles through:

- Green
- Yellow
- Red
- Green again

The program also manages waiting cars, extends the green light when traffic is
busy, supports night mode with a blinking yellow light, and stores the latest
20 light events in a log.

## Main Features

- Draw the current traffic light and its remaining ticks.
- Advance the simulation by one tick.
- Add cars to the waiting queue.
- Toggle between day mode and night mode.
- Display the recent light history.
- Display a crossing report with cars passed, waiting cars, night/busy status,
  and the status byte in binary and hexadecimal.
- Validate numeric input so invalid menu entries do not crash the program.

## Important Rules

- Green normally lasts 5 ticks.
- Yellow lasts 2 ticks.
- Red lasts 4 ticks.
- When more than 6 cars are waiting, green lasts 7 ticks.
- During green, up to 2 waiting cars pass on each tick.
- At night, the normal colors are ignored and the yellow light blinks.
- The log keeps only the newest 20 events.

## Build and Run

Compile with any C99 compiler using:

```text
gcc -std=c99 -Wall -Wextra -o app main.c
```

Run the program with:

```text
./app
```

On Windows, run:

```text
app.exe
```

## Project Files

- `main.c` - Complete source code for the Traffic Light Robot.
- `README.md` - Project and student information.
- `screanshots/` - Screenshots of the fresh start, an interesting action, and
  the crossing report.
