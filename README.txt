README
-----------------------
SETUP
Create a new folder in C:.../pico-v1.5.1/pico-examples/___, e.g. "assignment"

Pull the repo into C:.../pico-v1.5.1/pico-examples/(your folder name here)

Add the following line to the CMakeLists.txt within C:.../pico-v1.5.1/pico-examples at the bottom
add_subdirectory(your folder name here)

Create a new CMakeLists.txt file in C:.../pico-v1.5.1/pico-examples/(your folder name here) with the following text (you should see the INF2004-T12-Assignment folder from Github here)
if (NOT PICO_NO_HARDWARE)
    add_subdirectory(INF2004-T12-Assignment)
endif ()

-----------------------
