#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Path to kernel
#define CAPACITY_PATH "/sys/class/power_supply/BAT0/capacity"
#define CONSERVATION_PATH "/sys/bus/platform/drivers/ideapad_acpi/VPC2004:00/conservation_mode"

int read_value(const char *path) {
    FILE *file = fopen(path, "r");
    if (!file) return -1;
    int value = 0;
    if (fscanf(file, "%d", &value) != 1) value = -1;
    fclose(file);
    return value;
}

void write_value(const char *path, int value) {
    FILE *file = fopen(path, "w");
    if (file) {
        fprintf(file, "%d\n", value);
        fclose(file);
    }
}

int main() {
    // Change to the background (Daemon-Mode)
    if (daemon(0, 0) < 0) {
        return 1;
    }

    while (1) {
        int charge = read_value(CAPACITY_PATH);
        int current_mode = read_value(CONSERVATION_PATH);

        if (charge >= 0 && current_mode >= 0) {
            // If charge >= 80% and mode is off -> Mode on (stop the charge)
            if (charge >= 80 && current_mode == 0) {
                write_value(CONSERVATION_PATH, 1);
            }
            // If charge < 79% and mode is on -> mode off (charge to 80%)
            else if (charge < 79 && current_mode == 1) {
                write_value(CONSERVATION_PATH, 0);
            }
        }

        // 30 seconds sleep schlafen (saves energy)
        sleep(30);
    }
    return 0;
}
