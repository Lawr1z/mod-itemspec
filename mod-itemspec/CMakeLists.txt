# ItemSpec Module CMakeLists.txt
# Place this in: modules/mod-itemspec/CMakeLists.txt

AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/ItemSpec.cpp")

# If using script loader approach:
AC_ADD_SCRIPT_LOADER("ItemSpec" "${CMAKE_CURRENT_LIST_DIR}/src/itemspec_loader.cpp")