#EBFNflora

All'avvio vengono inizializzati i registri per la comunicazione seriale (UART), l'uso dei timer (TIMER1), i pin digitali, l'LCD (usata [libreria esterna](http://winavr.scienceprog.com/example-avr-projects/avr-gcc-4-bit-and-8-bit-lcd-library.html)), l'interfaccia ADC (scritta appositamente in [adc.c](src/adc.c)) e gli external interrupt (INT0, INT1). Dopo aver attivato gli interrupt globali

```
EIMSK = (1<<INT1)|(1<<INT0); //Turn ON INT0 and INT1
EICRA = (1<<ISC11)|(1<<ISC10)|(1<<ISC01)|(1<<ISC00);
sei(); //enable interrupts globally
```

si entra in un loop in cui si richiede ripetutamente all'utente se intende configurare i parametri di misurazione in EEPROM:

```
while(waitingForOutput){
	LCDclr();
	printOnLCD("Setup sensors?");
	LCDGotoXY(0,1);
	printOnLCD("[Y/n]?");
	delayMs(300);
}
```

Se viene premuto il bottone collegato al pin 2 (INT0) la variabile globale reset è settata a 1, altrimenti se viene premuto quello relativo al pin 3 (INT1), reset è settato a 0. In entrambi i casi waitingForOutput è settato a 0 causando così l'uscita dal ciclo.

```
ISR(INT0_vect){ //external interrupt service routine
	if(waitingForOutput && DigIO_getValue(buttonLeft) == HIGH){
		reset = 1;
		waitingForOutput = 0;
	}
}

ISR(INT1_vect){ //external interrupt service routine
	if(waitingForOutput && DigIO_getValue(buttonRight) == HIGH){
		reset = 0;
		waitingForOutput = 0;
	}
}
```

Nel primo caso (reset = 1) l'MCU interagisce con l'utente per ottenere i nuovi parametri di configurazione e salvarli in EEPROM, ognuno in blocchi da 16 byte come di seguito riportato. Sono state scritte funzioni apposite per lettura/scrittura da/su EEPROM definite in [eepromParams.c](src/eepromParams.c)

![GitHub Logo](EBFNflora_buffer.PNG)

Nel secondo caso (reset = 0) i valori già presenti in EEPROM - quelli dell'ultima inizializzazione - sono salvati subito in variabili globali.

```
timer = get_EEPROM_timer();
minLight = get_EEPROM_minLight();
maxLight = get_EEPROM_maxLight();
minTemp = get_EEPROM_minTemp();
maxTemp = get_EEPROM_maxTemp();
maxPoll = get_EEPROM_maxPoll();
R1 = get_EEPROM_r1();
C1 = get_EEPROM_c1();
C2 = get_EEPROM_c2();
C3 = get_EEPROM_c3();

struct Timer* timerSensors = Timer_create("timer_0", timer, timerFn, NULL); 
Timer_start(timerSensors);
```

Il valore in timer è usato per chiamare ogni timer millisecondi la funzione timerFn che si occupa di effettuare il sensing dell'ambiente.

```
void timerFn(void* args){
	LCDclr();
	if(i==0){
		controlLight();
		i++;
		return;
	}
	else if(i==1){
		controlTemp();
		i++;
		return;
	}
	else if(i==2){
		controlPoll();
		i = 0;
		return;
	}
}
```

**N.B:** il TIMER1 con un prescaler di 128 è in grado di supportare una durata massima di 4195 ms: un valore maggiore causa un overflow portando così il timeout 0 ms.
