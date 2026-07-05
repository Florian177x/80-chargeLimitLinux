# 80-chargeLimitLinux

An ultra-lightweight background service (daemon) written in C works on Lenovo ideapad laptop running Linux (tested on Ubuntu).

This tool bypasses a limitation found in modern Lenovo firmwares, where the built-in conservation mode is hardcoded to cap the battery at 60%. This program monitors the battery level in the background and automatically freezes charging once it reaches **80%**.

## Features
* **ultra low CPU Overhead:** The program utilizes the native C `sleep()` function. It consumes close to no processor resources while idling, saving your system's performance.
* **Native & Efficient:** Compiled directly into machine code—significantly more resource-efficient than background Python or Bash scripts.
* **Systemd Integration:** Runs quietly as a system-wide background service and automatically launches on system boot.

---

## Installation & Compilation

Follow these steps to build the program from source and set it up as a system service.

### 1. Install Prerequisites
Ensure the GCC compiler and basic build essentials are installed on your system:
```bash
sudo apt update
sudo apt install gcc build-essential tlp
```

### 2. Compile the Program
Compile the source code using maximum optimization (`-O3`):
```bash
gcc -O3 80_hack.c -o 80_hack
```

### 3. Move to System Path
Move the compiled binary to a secure system-wide location:
```bash
sudo mv 80_hack /usr/local/bin/
```

---

## Setting Up the System Service (systemd)

Because the program needs to write directly to the Linux kernel ACPI interfaces under `/sys/`, it is managed via `systemd` to run with the necessary privileges.

### 1. Create the Service File
Create a configuration file for the service:
```bash
sudo nano /etc/systemd/system/80-hack.service
```

Paste the following content into the file:
```ini
[Unit]
Description=Lenovo IdeaPad 80 Percent Battery Hack
After=multi-user.target

[Service]
Type=forking
ExecStart=/usr/local/bin/80_hack
Restart=always

[Install]
WantedBy=multi-user.target
```
*(Save by pressing `Ctrl+O`, `Enter`, and exit using `Ctrl+X`)*

### 2. Enable and Start the Service
Reload the systemd manager, enable the service to launch on boot, and start it immediately:
```bash
sudo systemctl daemon-reload
sudo systemctl enable 80-hack.service
sudo systemctl start 80-hack.service
```

---

## Usage & Control

### Check Service Status
To verify that the program is running correctly and actively monitoring your battery in the background:
```bash
sudo systemctl status 80-hack.service
```

### For Travel: Charge to 100% (Temporarily Stop Service)
If you are leaving your desk and need full battery capacity for a trip, you can temporarily disable the hack:
```bash
# 1. Stop the background service
sudo systemctl stop 80-hack.service

# 2. Turn off the hardware conservation mode
sudo tlp setcharge 0 0
```
*Your laptop will now charge normally all the way up to 100%.*

### Back at the Desk: Limit to 80% Again
Once you are back on stationary AC power, simply restart the service. The program will automatically take over control as soon as the battery drops below 79%:
```bash
sudo systemctl start 80-hack.service
```

---

## How It Works
The program reads the kernel hardware interfaces every 30 seconds:
1. When the battery level reaches `Charge >= 80%` on AC power, it sets `conservation_mode` to `1`. The hardware freezes the current charge level.
2. If the battery drops below `79%` due to natural self-discharge or brief unplugged use, the program sets the mode to `0` (Off), allowing the device to top itself back up to exactly 80%.
