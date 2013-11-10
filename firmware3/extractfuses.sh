#!/bin/sh
# Copyright 2010 Stefan Reinhold <development@ithron.de>. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
#
#    1. Redistributions of source code must retain the above copyright notice, this list of
#       conditions and the following disclaimer.
#
#    2. Redistributions in binary form must reproduce the above copyright notice, this list
#       of conditions and the following disclaimer in the documentation and/or other materials
#       provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY STEFAN REINHOLD ``AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL STEFAN REINHOLD OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The views and conclusions contained in the software and documentation are those of the
# authors and should not be interpreted as representing official policies, either expressed
# or implied, of Stefan Reinhold.

FUSECMD="avr-objdump -s -j .fuse $2"

function help_message
{
        echo "Usage: $0 [a|l|h|e|avrdude] elf-file"
}

function objdump_error
{
        if ($FUSECMD 2>&1 | grep -q "section.*not found")
        then
                cat<<EOF 1>&2
Error: The .fuse section was not found.

In avr-libc 1.8 the FUSES helper macro does not add the attribute "used". As a
result the section can be dropped due to linker optimizations.  This appears to
be fixed in avr-libc as of at least svn r2297.

As a workaround, use
__fuse_t __fuse __attribute__((section (".fuse"), used)) = 
{
    .low = LFUSE_DEFAULT,
    .high = HFUSE_DEFAULT,
    .extended = EFUSE_DEFAULT
};

rather than

FUSES =
{
    .low = LFUSE_DEFAULT,
    .high = HFUSE_DEFAULT,
    .extended = EFUSE_DEFAULT
};
EOF
        fi
        exit 1
}

if [ $# -ne 2 ]
then
        help_message
        exit 0
fi

fuses=`$FUSECMD | tail -n 1 | sed 's/^ *[0-9a-f]\{6,6\}[ \t][ \t]*\([0-9a-f]\{6,6\}\).*/\1/'`

if [ "$fuses"x == x ]
then
        objdump_error
fi

low=`echo $fuses | sed 's/\([0-9a-f]\{2,2\}\)[0-9a-f]\{4,4\}/\1/'`
high=`echo $fuses | sed 's/[0-9a-f]\{2,2\}\([0-9a-f]\{2,2\}\)[0-9a-f]\{2,2\}/\1/'`
extended=`echo $fuses | sed 's/[0-9a-f]\{4,4\}\([0-9a-f]\{2,2\}\)/\1/'`

case $1 in
        "a") echo $fuses ;;
        "l") echo $low ;;
        "h") echo $high ;;
        "e") echo $extended ;;
        "avrdude") echo "-U lfuse:w:0x$low:m -U hfuse:w:0x$high:m -U efuse:w:0x$extended:m" ;;
        *) help_message
esac