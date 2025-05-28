#!/usr/bin/bash
noble_dev() { make run_asan ARGS=../ebb/scenes/test.noble USE_LOCAL_SYMLINK=yes $1 }
noble_dev_noasan() { make run ARGS=../ebb/scenes/test.noble USE_LOCAL_SYMLINK=yes $1 }
