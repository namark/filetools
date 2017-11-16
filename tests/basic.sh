#!/bin/sh

OnExit()
{
	trap "trap - TERM && $*" INT TERM EXIT
}

tempdir=$(mktemp -d)
OnExit "rm  -rf $tempdir"

echo
echo Testing in $tempdir

testfile="$tempdir/test.file"
original="$tempdir/original.file"

echo 1234567890abcdefghijklmnopqrstuvwxyz > "$testfile"

out/fshred -s 3 "$testfile"
mv "$testfile" "$original"
out/fglue "$tempdir/test.info"

echo
echo glued file:
cut -c1-200 "$testfile"
echo
echo original file:
cut -c1-200 "$original"
echo
echo difference:
diff "$testfile" "$original"
echo

