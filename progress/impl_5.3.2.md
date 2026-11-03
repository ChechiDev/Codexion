# Impl 5.3.2 Destroy coder

Files: coders/src/cleanup/destroy_coder.c (new), coders/main.h (prototype, Updated), coders/Makefile (SRC, Updated), tests/unit/test_destroy_coder.c (new).

Results (run sequentially): make check green (acceptance 5.3.2 + unit + integrity OK); make tsan "tsan clean"; make memcheck "valgrind clean"; norminette coders/ no errors; valgrind on test_5.3.2.bin exit 0.
