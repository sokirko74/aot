rm rml_sources.tar
touch rml_sources.tar
gfind . -name "CMakeLists.txt" -print -exec  tar rf rml_sources.tar \{\} \;
gfind . -name "*.h" -print -exec tar rf rml_sources.tar \{\} \;
gfind . -name "*.cpp" -print -exec tar rf rml_sources.tar \{\} \;
gfind . -name "*.vcproj" -print -exec tar rf rml_sources.tar \{\} \;
gfind . -name "*.sln" -print -exec tar rf rml_sources.tar \{\} \;
gfind . -name "DoxyFile" -print -exec  tar rf rml_sources.tar \{\} \;
gfind . -name "*.l" -print -exec  tar rf rml_sources.tar \{\} \;
gfind . -name "*.y" -print -exec  tar rf rml_sources.tar \{\} \;
gfind . -name "*.cc" -print -exec tar rf rml_sources.tar \{\} \;
gzip rml_sources1.tar