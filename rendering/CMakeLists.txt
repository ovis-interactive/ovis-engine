add_library(
  ovis-rendering

  # include/ovis/rendering/render_target_viewport.hpp src/render_target_viewport.cpp
  include/ovis/rendering/render_pass.hpp src/render_pass.cpp
  include/ovis/rendering/clear_pass.hpp src/clear_pass.cpp
  include/ovis/rendering/primitive_renderer.hpp src/primitive_renderer.cpp
)
add_library(ovis::rendering ALIAS ovis-rendering)

target_include_directories(
  ovis-rendering
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
  ovis-rendering
  PUBLIC
    ovis::core
    ovis::graphics
)

target_add_assets(
  ovis-rendering

  assets/primitive_rendering.shader.vert assets/primitive_rendering.shader.frag
)

if (OVIS_BUILD_TESTS)
  add_executable(
    ovis-rendering-test

    test/test_clear_pass.cpp
  )

  target_link_libraries(
    ovis-rendering-test
    PRIVATE
      ovis::rendering
      ovis::test
  )

  if (OVIS_EMSCRIPTEN)
    add_test(
      ovis-rendering-test
      ${NODE_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/ovis-rendering-test.js
    )
  else ()
    add_test(ovis-rendering-test ovis-rendering-test)
  endif ()
endif ()
