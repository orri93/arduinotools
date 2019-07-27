@ECHO OFF

REM SET BOARD=SparkFun:avr:promicro:cpu=8MHzatmega32U4
SET BOARD=arduino:avr:uno

PUSHD %~dp0
arduino-cli upload -v -t -p COM24 -b %BOARD%
POPD