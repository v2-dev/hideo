#!/bin/bash

#execute a single client
httperf --server localhost --port 9992

#A more realistic test case :1000 HTTP requests at a rate of 10 requests per second.
httperf --server localhost --port 9992 --num-conns 100 --rate 10 --timeout 1
