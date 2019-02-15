#-----------------------------------------------------------------------------------#
#   This CMakeLists.txt was recycled from the original allegro examples CMakeLists  #
#   To be able to compile, you need to append the following commands to Allegro5    #
#   CMakeLists.txt:                                                                 #
#                                                                                   #
#       option(WANT_MINIMOOG "Build MiniMoog program" on)                           #
#                                                                                   #
#       if(WANT_MINIMOOG)                                                           #
#           add_subdirectory(RTS_MiniMoog)                                          #
#       endif(WANT_MINIMOOG)                                                        #
#-----------------------------------------------------------------------------------#


include_directories(
    ../addons/acodec
    ../addons/audio
    ../addons/color
    ../addons/font
    ../addons/image
    ../addons/main
    ../addons/memfile
    ../addons/native_dialog
    ../addons/physfs
    ../addons/primitives
    ../addons/ttf
    ../RTS_MiniMoog
    )


#-----------------------------------------------------------------------------#

# Conditionally build an example program.  If any of its arguments is the exact
# string "x", do nothing.  Otherwise strip off the "x" prefixes on arguments
# and call the example macro.
function(example name)
    set(is_console)
    set(args)
    foreach(arg ${ARGN})
        if(arg STREQUAL "x")
            message(STATUS "Not building ${name}")
            return()
        endif()
        if(arg STREQUAL "CONSOLE")
            set(is_console ON)
        else()
            string(REGEX REPLACE "^x" "" arg ${arg})
            list(APPEND args ${arg})
        endif()
    endforeach(arg)

    if(WIN32)
        if(is_console)
            # We need stdout and stderr available from cmd.exe,
            # so we must not use WIN32 here.
            set(EXECUTABLE_TYPE)
        else()
            set(EXECUTABLE_TYPE "WIN32")
        endif(is_console)
    endif(WIN32)

    if(IPHONE)
        set(EXECUTABLE_TYPE MACOSX_BUNDLE)
    endif(IPHONE)

    add_example(${name} ${args})
endfunction(example)

macro(add_example nm)
    if(WANT_POPUP_EXAMPLES AND SUPPORT_NATIVE_DIALOG)
        add_our_executable(${nm} ${ARGN} ${ALLEGRO_LINK_WITH} ${ALLEGRO_MAIN_LINK_WITH} ${NATIVE_DIALOG_LINK_WITH})
    else()
        add_our_executable(${nm} ${ARGN} ${ALLEGRO_LINK_WITH} ${ALLEGRO_MAIN_LINK_WITH} )
    endif()
endmacro(add_example nm)
#-----------------------------------------------------------------------------#
#-------------------Setting Allegro Dependent files---------------------------#
#-----------------------------------------------------------------------------#

set(AUDIO x${AUDIO_LINK_WITH})
set(ACODEC x${ACODEC_LINK_WITH})
set(COLOR x${COLOR_LINK_WITH})
set(DIALOG x${NATIVE_DIALOG_LINK_WITH})
set(FONT x${FONT_LINK_WITH})
set(IMAGE x${IMAGE_LINK_WITH})
set(MEMFILE x${MEMFILE_LINK_WITH})
set(PHYSFS x${PHYSFS_LINK_WITH})
set(PRIM x${PRIMITIVES_LINK_WITH})
set(TTF x${TTF_LINK_WITH})
set(NIHGUI nihgui.cpp ${FONT} ${PRIM})



#-----------------------------------------------------------------------------#
#-------------------Setting Application Dependent files-----------------------#
#-----------------------------------------------------------------------------#

set(KEYBOARD keyboard.c)
set(UI ui.c)
set(TIMEMNG timemng.c)
set(PTASK ptask.c)

#----------------------------------------------------------------------Linking#
set(PTASK ptask.c ${TIMEMNG} )
set(SIGNAL_SRC signal_src.c ${PTASK})
set(KEYBOARD keyboard.c ${SIGNAL_SRC} ${PTASK} )
set(UI ui.c ${PTASK} ${FONT} ${PRIM} ${TTF} ${SIGNAL_SRC} )

#-----------------------------------------------------------------------------#
#-------------------Build Additive_Synths (MiniMoog) Program------------------#
#-----------------------------------------------------------------------------#

example(additive_Synths minimoog.c ${PTASK} ${FONT} ${AUDIO} ${ACODEC} ${TTF} ${IMAGE} ${SIGNAL_SRC} ${UI} ${KEYBOARD})

#data directory contains some extra font used by the application

copy_data_dir_to_build(RTS_MiniMoog data .)


#-----------------------------------------------------------------------------#
#END OF FILE
