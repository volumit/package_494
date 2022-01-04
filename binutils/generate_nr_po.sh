#! /bin/bash

PACKAGE=$1
SRCDIR=$2
TOP_SRCDIR=$3
BINUTILS_ROOT=$SRCDIR/../..
POTFILES=$4
if [ "x$5" != "x" ]; then
    EXTRA_POTFILES=$5
fi;

NUMGETTEXT=$BINUTILS_ROOT/numgettext.sh
SWAP=$BINUTILS_ROOT/swap_error_numbers.sh

trap "rm -f nr.pot nr.tmp nr_old.po" 0 1
rm -f nr.pot nr.tmp

$NUMGETTEXT $POTFILES $TOP_SRCDIR
if [ "x$EXTRA_POTFILES" != "x" ]; then
    $NUMGETTEXT $EXTRA_POTFILES ..
fi;

# Change the Warning: [99999] like messages back to [99999] Warning:
# It is needed to keep the numbers identical.
cp $SRCDIR/nr.po ./nr_old.po 
$SWAP nr_old.po -r

msginit --no-translator -l en -i nr.pot -o - | \
    java -jar $BINUTILS_ROOT/ernie.jar -pool $PACKAGE -ancient nr_old.po -since "experimental" > nr.po

if [ $? -ne 0 ]; then
    exit 1
fi;

java -jar $BINUTILS_ROOT/pot2xml.jar -pool $PACKAGE -tex -lang C++ nr.po $PACKAGE.xml

$SWAP nr.po

exit 0
