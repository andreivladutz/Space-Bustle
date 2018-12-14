avr-objcopy -O ihex -R .eeprom -R .eesafe bin\Release\SpaceBustle.elf bin\Release\SpaceBustle.elf.hex
avr-objcopy --no-change-warnings -j .eeprom --change-section-lma .eeprom=0 -O ihex bin\Release\SpaceBustle.elf bin\Release\SpaceBustle.elf.eep.hex
avr-size --mcu=atmega328p --format=avr bin\Release\SpaceBustle.elf
avrdude -V -C "C:\WinAVR\bin/avrdude.conf" -patmega328p -carduino -P\\.\COM7 -b115200 -D -Uflash:w:bin\Release\SpaceBustle.elf.hex:i