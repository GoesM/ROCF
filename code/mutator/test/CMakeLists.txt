add_executable(mutator_test mutator_test.cc)
add_executable(knobs_test knobs_test.cc)
add_executable(yaml_mutator_test yaml_mutator_test.cpp)
# add_executable(sequence_mutator_test sequence_mutator_test.cc)

target_link_libraries(mutator_test mutator)
target_link_libraries(knobs_test mutator)
target_link_libraries(yaml_mutator_test mutator)
# target_link_libraries(sequence_mutator_test mutator)



# .so
install(TARGETS mutator_test
    DESTINATION lib
)
install(TARGETS knobs_test
    DESTINATION lib
)
install(TARGETS yaml_mutator_test
    DESTINATION lib
)
# install(TARGETS sequence_mutator_test
#     DESTINATION lib
# )
