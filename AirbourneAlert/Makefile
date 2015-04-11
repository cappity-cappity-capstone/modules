SOURCES=`find . -follow -type f | grep -v Makefile | grep -v firmware | grep -v '.bin'`

all: clean build flash

clean:
	rm -f ./firmware_*

build:
	spark compile $(SOURCES)

flash:
	spark flash --usb firmware_*
