#!/bin/sh

echo Processing $1

# just update copyright date
sed -E \
	-f scripts/copyright.sed \
	$1 > $1.sedded
exit

# change target/HC08/ to target/S08/
sed -E \
	-f scripts/target-hc08.sed \
	$1 > $1.sedded
exit

# change homepage link to https
sed -E \
	-f scripts/homepage.sed \
	$1 > $1.sedded
exit

# update copyright/license mentions
sed -E \
	-f scripts/license.sed \
	$1 > $1.sedded
exit

# change homepage link and update copyright date
sed -E \
	-f scripts/homepage.sed \
	-f scripts/copyright.sed \
	$1 > $1.sedded
exit

# change [if] and friends to .if and friends
sed -E \
	-f scripts/conditional-comp.sed \
	$1 > $1.sedded
exit

# Had to cascade because keywords.sed was interacting badly with homepage.sed.
# Not at all clear why. Doing it this way was easier than digging around. :-(
sed -E \
	-f scripts/homepage.sed \
	-f scripts/copyright.sed \
	$1 \
| sed -E \
	-f scripts/keywords.sed \
	> $1.sedded
