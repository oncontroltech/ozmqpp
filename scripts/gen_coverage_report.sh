#!/bin/bash

lcov --capture --directory ../cmake-build-debug --output-file coverage.info
genhtml coverage.info --output-directory coverage_html
