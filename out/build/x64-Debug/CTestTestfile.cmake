# CMake generated Testfile for 
# Source directory: I:/tools/PSP360
# Build directory: I:/tools/PSP360/out/build/x64-Debug
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(all_unit_tests "I:/tools/PSP360/out/build/x64-Debug/PSP360_tests.exe")
set_tests_properties(all_unit_tests PROPERTIES  _BACKTRACE_TRIPLES "I:/tools/PSP360/CMakeLists.txt;74;add_test;I:/tools/PSP360/CMakeLists.txt;0;")
subdirs("external/stb_dxt")
subdirs("external/gtest")
