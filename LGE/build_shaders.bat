bgfx\.build\win64_vs2022\bin\shadercRelease.exe -f frag.sc -o fragd3d.bin --type f --platform windows -p s_5_0 -i bgfx/src
bgfx\.build\win64_vs2022\bin\shadercRelease.exe -f vertex.sc -o vertexd3d.bin --type v --platform windows -p s_5_0 -i bgfx/src

bgfx\.build\win64_vs2022\bin\shadercRelease.exe -f frag.sc -o fragspirv.bin --type f --platform windows -p spirv16-13 -i bgfx/src
bgfx\.build\win64_vs2022\bin\shadercRelease.exe -f vertex.sc -o vertexspirv.bin --type v --platform windows -p spirv16-13 -i bgfx/src

