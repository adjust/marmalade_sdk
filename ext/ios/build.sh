#!/usr/bin/env bash

set -e

SRCDIR=./sdk
LIBOUTDIR=../../source/sdk/iOS/
SCRIPTDIR=../../scripts/

(cd $SRCDIR/Scripts; sh build.sh)

rm -v -rf $LIBOUTDIR/AdjustSdk.framework
cp -v -R $SRCDIR/Frameworks/Static/AdjustSdk.framework $LIBOUTDIR
sh $SCRIPTDIR/symlink_fix.sh
