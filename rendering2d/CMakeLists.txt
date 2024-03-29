add_library(
  ovis-rendering2d
  
  include/ovis/rendering2d/renderer2d.hpp src/renderer2d.cpp
  include/ovis/rendering2d/shape2d.hpp src/shape2d.cpp
  include/ovis/rendering2d/text.hpp src/text.cpp
  include/ovis/rendering2d/font_atlas.hpp src/font_atlas.cpp
)
add_library(ovis::rendering2d ALIAS ovis-rendering2d)

target_include_directories(
  ovis-rendering2d
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
  ovis-rendering2d
  PUBLIC
    ovis::rendering
    stb::truetype
)

target_add_assets(
  ovis-rendering2d

  assets/rendering2d.schema.json
  assets/shape2d.shader.vert
  assets/shape2d.shader.frag
  assets/NotoSans-Regular.font.ttf
)

if (OVIS_BUILD_TESTS)
  add_executable(
    ovis-rendering2d-test

    test/test_shape2d.cpp
    test/test_renderer2d.cpp
  )
  target_link_libraries(
    ovis-rendering2d-test
    PRIVATE
      ovis::rendering2d
      ovis::test
  )
  # target_add_assets(
  #   ovis-rendering2d-test

  # )
  target_embed_assets(ovis-rendering2d-test)

  if (OVIS_EMSCRIPTEN)
    add_test(
      ovis-rendering2d-test
      ${NODE_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/ovis-rendering2d-test.js
    )
  else ()
    add_test(
      NAME ovis-rendering2d-test
      COMMAND ovis-rendering2d-test --assets-directory ${CMAKE_CURRENT_BINARY_DIR}/assets/ovis-rendering2d-test/
    )
  endif ()
endif ()
