include_directories(
	${CMAKE_SOURCE_DIR}/QueueLib
	)

add_executable(QueueExample
	QueueExample.cpp
	)

target_link_libraries(QueueExample
	QueueLib
	)
