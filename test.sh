#!/bin/bash
try() {
	expected="$1"
	input="$2"

	./redmagic "$input" > tmp.s
	gcc -o tmp tmp.s
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else
		echo "$expected expected, but got $actual"
		exit 1
	fi
}

try 0 0
try 42 42
try 21 '5+20-4'
try 41 " 12 + 34 - 5 "
try 25 " 4 + 7 * 3 "
try 2 "20 / 5 - 2"
try 5 "+3 + 2"
try 6 "-4 + 10"


echo OK

