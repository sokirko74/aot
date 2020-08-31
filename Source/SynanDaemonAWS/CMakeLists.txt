declare_cmake_min_version()
set(CMAKE_CXX_STANDARD 11)
project(SynanDaemonAWS LANGUAGES CXX)

find_package(aws-lambda-runtime REQUIRED)
find_package(AWSSDK COMPONENTS s3)

add_executable(${PROJECT_NAME} "main.cpp")
target_link_libraries(${PROJECT_NAME} PUBLIC
        AWS::aws-lambda-runtime
        ${AWSSDK_LINK_LIBRARIES}
        SynanDaemonLib
        )
aws_lambda_package_target(${PROJECT_NAME})
