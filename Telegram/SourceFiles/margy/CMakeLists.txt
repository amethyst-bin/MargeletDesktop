# MargyDesktop module
target_sources(Telegram PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/margy_config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/margy_config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/badges/margy_badge_types.h
    ${CMAKE_CURRENT_SOURCE_DIR}/badges/margy_badge_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/badges/margy_badge_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/badges/margy_badge_icon.h
    ${CMAKE_CURRENT_SOURCE_DIR}/badges/margy_badge_icon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/badges/margy_plane_3d.h
    ${CMAKE_CURRENT_SOURCE_DIR}/badges/margy_plane_3d.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/badges/margy_badge_box.h
    ${CMAKE_CURRENT_SOURCE_DIR}/badges/margy_badge_box.cpp
)

target_include_directories(Telegram PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
