#!/bin/sh

echo "DOWNLOADING: https://bootstrap.pypa.io/pip/2.7/get-pip.py ..."
curl -sSL https://bootstrap.pypa.io/pip/2.7/get-pip.py -o get-pip.py
python get-pip.py
