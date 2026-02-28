include(ExternalProject)

set(MOLTENVK_VERSION "v1.2.3")

# Build only for the native architecture to avoid cross-compilation failures.
# The universal "macos" target tries to build both arm64 + x86_64, which fails
# when cross-compiling on arm64 runners (e.g. macos-14 GitHub Actions).
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
  set(MOLTENVK_BUILD_TARGET "macos-arm64")
else()
  set(MOLTENVK_BUILD_TARGET "macos-x86_64")
endif()

ExternalProject_Add(MoltenVK
  GIT_REPOSITORY https://github.com/KhronosGroup/MoltenVK.git
  GIT_TAG ${MOLTENVK_VERSION}

  CONFIGURE_COMMAND ${CMAKE_CURRENT_LIST_DIR}/configure.sh <LOG_DIR> <SOURCE_DIR> ${MOLTENVK_VERSION}

  PATCH_COMMAND ${CMAKE_CURRENT_LIST_DIR}/patch.sh ${CMAKE_SOURCE_DIR}/Externals/MoltenVK/patches/ ${MOLTENVK_VERSION}

  BUILD_COMMAND make -C <SOURCE_DIR> ${MOLTENVK_BUILD_TARGET}
  BUILD_IN_SOURCE ON
  BUILD_BYPRODUCTS <SOURCE_DIR>/Package/Release/MoltenVK/dylib/macOS/libMoltenVK.dylib

  INSTALL_COMMAND ""

  LOG_CONFIGURE ON
  LOG_BUILD ON
  LOG_OUTPUT_ON_FAILURE ON
)
