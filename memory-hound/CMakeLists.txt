set(LLVM_LINK_COMPONENTS support)

add_clang_executable(memory-hound
  MemoryHound.cpp
  )
target_link_libraries(memory-hound
  clangTooling
#clangBasic
#clangASTMatchers
  )
