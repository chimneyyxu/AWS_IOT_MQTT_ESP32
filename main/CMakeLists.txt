set(COMPONENT_SRCS
	"app_main.c"
	"mqtt_demo_mutual_auth.c"
	"iot_creat.c"
	)

set(COMPONENT_ADD_INCLUDEDIRS
	"."
	"${CMAKE_CURRENT_LIST_DIR}"
	)

idf_component_register(SRCS "iot_creat.c" "${COMPONENT_SRCS}"
					   INCLUDE_DIRS ${COMPONENT_ADD_INCLUDEDIRS}
					  )