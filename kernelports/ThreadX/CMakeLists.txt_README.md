# TraceRecorder Compile Time Instrumentation (CTI) with ThreadX
The compile time instrumentation (CTI) used by the TraceRecorder was developed for *make*.

To make it work with *cmake*, add the following to your **CMakeLists.txt** to create the necessary compile time definitions for each .c file.
```cmake
# Loop through source files and define unique macros
foreach(src_file IN LISTS SOURCE_FILES)
    get_filename_component(name_we ${src_file} NAME_WE)
    set_source_files_properties(${src_file} PROPERTIES 
    COMPILE_DEFINITIONS "__inside_${name_we}")
endforeach()
```