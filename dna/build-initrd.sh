#!/bin/sh

cd initrd && mkbootfs . | gzip -f > ../initrd.img
