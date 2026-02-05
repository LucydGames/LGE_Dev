 bgfx\.build\win64_vs2022\bin\shadercRelease.exe -f frag.sc -o fragd3d.bin --type f --platform windows -p s_5_0 -i bgfx/src --varyingdef varying.def.sc
 bgfx\.build\win64_vs2022\bin\shadercRelease.exe -f vertex.sc -o vertexd3d.bin --type v --platform windows -p s_5_0 -i bgfx/src --varyingdef varying.def.sc

 bgfx\.build\win64_vs2022\bin\shadercRelease.exe -f frag.sc -o fragspirv.bin --type f --platform windows -p spirv16-13 -i bgfx/src --varyingdef varying.def.sc
 bgfx\.build\win64_vs2022\bin\shadercRelease.exe -f vertex.sc -o vertexspirv.bin --type v --platform windows -p spirv16-13 -i bgfx/src --varyingdef varying.def.sc

bgfx\.build\win64_vs2022\bin\shadercRelease.exe -f fs_bump.sc -o fs_bumpd3d.bin --type f --platform windows -p s_5_0 --verbose -i bgfx/src --varyingdef bump/varying.def.sc
bgfx\.build\win64_vs2022\bin\shadercRelease.exe -f vs_bump.sc -o vs_bumpd3d.bin --type v --platform windows -p s_5_0 --verbose -i bgfx/src --varyingdef bump/varying.def.sc

