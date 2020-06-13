

foreach(root ${BOARD_ROOT})
	find_path(BOARD_DIR
	    NAMES ${BOARD}_defconfig
	    PATHS ${root}/boards/*/*
	    NO_DEFAULT_PATH
	    )
    	if(BOARD_DIR)
		if (EXISTS "${BOARD_DIR}/keymap")
			list(APPEND KEYMAP_DIRS ${BOARD_DIR}/keymap)
		endif()
	endif()

	if(DEFINED SHIELD)
		find_path(shields_refs_list
		    NAMES ${SHIELD}.overlay
		    PATHS ${root}/boards/shields/*
		    NO_DEFAULT_PATH
		    )
		foreach(shield_path ${shields_refs_list})
			if (EXISTS "${shield_path}/keymap")
				list(APPEND KEYMAP_DIRS ${shield_path}/keymap)
			endif()
		endforeach()
	endif()
endforeach()

find_path(BASE_KEYMAPS_DIR
	NAMES ${KEYMAP}/keymap.overlay
	PATHS ${KEYMAP_DIRS}
	NO_DEFAULT_PATH
)

if (BASE_KEYMAPS_DIR)
	set(KEYMAP_DIR "${BASE_KEYMAPS_DIR}/${KEYMAP}" CACHE STRING "Selected keymap directory")
	message(STATUS "Using keymap directory: ${KEYMAP_DIR}/")
	# Used to let local imports of custom keycodes work as expected
	list(APPEND DTS_ROOT ${KEYMAP_DIR})
	if (EXISTS "${KEYMAP_DIR}/include")
		include_directories("${KEYMAP_DIR}/include")
	endif()
	set(DTC_OVERLAY_FILE ${KEYMAP_DIR}/keymap.overlay)
endif()
