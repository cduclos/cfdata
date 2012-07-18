#! /bin/sh

libtoolize
aclocal \
&& automake --add-missing \
&& autoheader \
&& autoconf
