all:
	arm-linux-androideabi-gcc transmitter.c -o transmitter
send:
	adb push transmitter /sdcard/
