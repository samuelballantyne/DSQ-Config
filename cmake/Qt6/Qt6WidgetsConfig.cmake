set(Qt6Widgets_FOUND TRUE)
if(NOT TARGET Qt6::Widgets)
    add_library(Qt6::Widgets INTERFACE IMPORTED)
endif()
