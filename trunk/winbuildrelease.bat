cd src
make clean
make EXT=STD BEEPER=BUZZER KEYRPT=DELAY
mv gruvin9x.hex ../gruvin9x-stock.hex
make clean  
make 
mv gruvin9x.hex ../gruvin9x.hex
make clean
make BEEPER=SPEAKER KEYRPT=DELAY
mv gruvin9x.hex ../gruvin9x-std-speaker.hex
make clean
make EXT=FRSKY BEEPER=BUZZER KEYRPT=DELAY
mv gruvin9x.hex ../gruvin9x-frsky-nospeaker.hex
make clean
