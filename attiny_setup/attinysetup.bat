echo off
ECHO "Verbinde jetzt den Arduino Uno (ohne ATTiny!) mit deinem Computer."
pause
cls
ECHO Der Geraetemanager wird geoffnet sobald du eine Taste drueckst.
ECHO Schaue unter "Anschluesse (COM & LPT)" nach, welcher Port in Klammern hinter "Arduino Uno" angegeben wird.
ECHO z.B.: "v Anschluesse (COM & LPT)"
ECHO            "Arduino Uno (COM3)"
pause
start devmgmt.msc
:INPUT
ECHO Gib den Portnamen ein - z.B. "COM3":
set /p port=
IF x%port:COM=%==x%port% (GOTO ONWRONGINPUT)

avrdude -c arduino -p m328p -P %port% -U flash:w:ArduinoISP.hex
IF %ERRORLEVEL% NEQ 0 (GOTO ONERRORUNO)

cls
ECHO Verbinde jetzt den ATTiny85 mit dem Arduino Uno.
pause
cls

avrdude -c arduino -b 19200 -p attiny85 -P %port% -U lfuse:w:0xE2:m
IF %ERRORLEVEL% NEQ 0 (GOTO ONERRORATTINY)
avrdude -c arduino -b 19200 -p attiny85 -P %port% -U hfuse:w:0xDF:m
IF %ERRORLEVEL% NEQ 0 (GOTO ONERRORATTINY)
avrdude -c arduino -b 19200 -p attiny85 -P %port% -U efuse:w:0xFF:m
IF %ERRORLEVEL% NEQ 0 (GOTO ONERRORATTINY)
avrdude -c arduino -b 19200 -p attiny85 -P %port% -U flash:w:firmware.hex
IF %ERRORLEVEL% NEQ 0 (GOTO ONERRORATTINY)
cls
ECHO ATTiny wurde erfolgreich bespielt. Folge den weiteren Anweisungen in der Anleitung.
pause
exit

:ONWRONGINPUT
ECHO Der Portname muss mit "COM" anfangen.
GOTO INPUT

:ONERRORUNO
ECHO Es ist ein Fehler beim Bespielen des Arduino Uno aufgetreten.
ECHO Schaue in der Command Line nach, welcher Fehler angegeben wird und versuche es erneut.
ECHO Moegliche Probleme:
ECHO "can't open device" - der Port wurde nicht korrekt angegeben.
ECHO "not in sync" - der Attiny ist schon mit dem Arduino Uno verbunden.
pause
cls
exit 0

:ONERRORATTINY
ECHO Es ist ein Fehler beim Bespielen des ATTiny aufgetreten.
ECHO Schaue in der Command Line nach, welcher Fehler angegeben wird und versuche es erneut.
ECHO Moegliche Probleme:
ECHO "Yikes!  Invalid device signature." - der ATTiny85 ist nicht korrekt mit dem Arduino Uno verbunden.
pause
