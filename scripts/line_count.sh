#!/bin/bash

echo "$(find -L src/cpp shaders -name '*.cc' -or -name '*.h' -or -name '*frag' -or -name '*.vert' | grep -v "/freetype-gl/" | xargs wc -l)"
