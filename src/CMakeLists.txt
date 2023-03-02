# for each "src/x.cpp", generate target "x"
file(GLOB_RECURSE all_srcs CONFIGURE_DEPENDS *.cc)
add_executable(PL0 ${all_srcs})
