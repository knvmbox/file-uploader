add_library(qxlsxqt5 STATIC IMPORTED)
set_target_properties(qxlsxqt5 PROPERTIES
  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/ext-lib/QXlsx/lib/libQXlsxQt5.a"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/ext-lib/"
)

