add_library(mpu6050 
${CMAKE_CURRENT_LIST_DIR}/mpu6050.cpp
${CMAKE_CURRENT_LIST_DIR}/mpu6050.h
)

#target_include_directories(mpu6050
#    ${CMAKE_CURRENT_LIST_DIR}/
#)

#target_sources(mpu6050 
    #${CMAKE_CURRENT_LIST_DIR}/mpu6050.cpp
    #${CMAKE_CURRENT_LIST_DIR}/mpu6050.h
#)

target_link_libraries(mpu6050 
        hardware_i2c
        pico_stdlib
        )