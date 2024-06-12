@echo off
rem para o nano novo
rem arduino-cli upload -b arduino:avr:uno -p COM25 -v -i trampuinoBlink.hex -t
set TRAMPOLINE_BASE_PATH=C:\ESA\trampoline
goil --target=avr/arduino/nano --templates=%TRAMPOLINE_BASE_PATH%/goil/templates/ blink.oil
arduino-cli upload -b arduino:avr:nano:cpu=atmega328old -p COM10 -v -i trampuinoBlink.hex -t
