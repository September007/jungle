
add_library(jungle_core INTERFACE )
add_library(jungle::core ALIAS jungle_core)
target_include_directories(jungle_core INTERFACE include)
target_link_libraries(jungle_core INTERFACE fmt::fmt-header-only pugixml::pugixml)