add_library(exampleutil STATIC IMPORTED)
set_target_properties(exampleutil PROPERTIES
  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/ext-lib/webp/libs/libexampleutil.a"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/ext-lib/webp/"
)

add_library(imagedec STATIC IMPORTED)
set_target_properties(imagedec PROPERTIES
  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/ext-lib/webp/libs/libimagedec.a"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/ext-lib/webp/"
)

add_library(imageenc STATIC IMPORTED)
set_target_properties(imageenc PROPERTIES
  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/ext-lib/webp/libs/libimageenc.a"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/ext-lib/webp/"
)

add_library(imageioutil STATIC IMPORTED)
set_target_properties(imageioutil PROPERTIES
  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/ext-lib/webp/libs/libimageioutil.a"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/ext-lib/webp/"
)

add_library(sharpyuv STATIC IMPORTED)
set_target_properties(sharpyuv PROPERTIES
  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/ext-lib/webp/libs/libsharpyuv.a"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/ext-lib/webp/"
)

add_library(webp STATIC IMPORTED)
set_target_properties(webp PROPERTIES
  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/ext-lib/webp/libs/libwebp.a"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/ext-lib/webp/"
)

add_library(webpdemux STATIC IMPORTED)
set_target_properties(webpdemux PROPERTIES
  IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/ext-lib/webp/libs/libwebpdemux.a"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/ext-lib/webp/"
)
