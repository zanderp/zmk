#!/bin/sh

if [ -z "$1" ]; then
	echo "Usage: ./run-test.sh <testcase>"
	exit 1
fi

export testcase="$1"
echo "Running $testcase:"

west build -d build/tests/$testcase -b native_posix -- -DZMK_CONFIG=tests/$testcase > /dev/null
./build/tests/$testcase/zephyr/zmk.exe | grep "hid_listener_keycode_" | sed -e s/.*hid_listener_keycode_// > build/tests/$testcase/keycode_events.log

diff -au tests/$testcase/keycode_events.snapshot build/tests/$testcase/keycode_events.log
