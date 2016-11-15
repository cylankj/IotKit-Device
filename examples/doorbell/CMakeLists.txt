set ( name "doorbell_demo" )
file ( GLOB sources *.c *.h )
add_executable ( ${name} ${sources} )
target_link_libraries ( ${name} iotk-lib iotk_pal-lib msgpack )