SRCDIR = ./sdk
LIBOUTDIR = ../../source/sdk/iOS/
SCRIPTDIR = ../../scripts/

default: copy

build:	
	cd $(SRCDIR)/Scripts; \
	sh adjust_build.sh

copy: build
	rm -rf $(LIBOUTDIR)/AdjustSdk.framework; \
	cp -R $(SRCDIR)/Frameworks/Static/AdjustSdk.framework $(LIBOUTDIR); \
	sh $(SCRIPTDIR)/symlink_fix.sh
