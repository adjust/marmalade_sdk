SRCDIR = ./sdk
LIBOUTDIR = ../../source/sdk/iOS/

default: copy

build:	
	cd $(SRCDIR); \
	xcodebuild -target framework -configuration Release

copy: build
	rm -rf $(LIBOUTDIR)/Adjust.framework; \
	cp -R $(SRCDIR)/Adjust.framework $(LIBOUTDIR); \
	rm -rf $(SRCDIR)/Adjust.framework; \
	rm -rf $(LIBOUTDIR)/Adjust.framework/Adjust; \
	rm -rf $(LIBOUTDIR)/Adjust.framework/Headers; \
	ln -s $(LIBOUTDIR)/Adjust.framework/Versions/A/Adjust $(LIBOUTDIR)/Adjust.framework/Adjust; \
	ln -s $(LIBOUTDIR)/Adjust.framework/Versions/A/Headers $(LIBOUTDIR)/Adjust.framework/Headers
