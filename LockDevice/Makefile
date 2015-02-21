SOURCES=`find . -follow -type f | grep -v Makefile | grep -v firmware | grep -v '.bin'`

build:
	rm -f ./firmware_*
	spark compile $(SOURCES)
	spark flash --usb firmware_*
