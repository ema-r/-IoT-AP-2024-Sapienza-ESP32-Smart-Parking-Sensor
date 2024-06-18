# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/fracchio/esp/esp-idf/components/bootloader/subproject"
  "/home/fracchio/Scrivania/-IoT-AP-2024-Sapienza-ESP32-Smart-Parking-Sensor/iLot/build/bootloader"
  "/home/fracchio/Scrivania/-IoT-AP-2024-Sapienza-ESP32-Smart-Parking-Sensor/iLot/build/bootloader-prefix"
  "/home/fracchio/Scrivania/-IoT-AP-2024-Sapienza-ESP32-Smart-Parking-Sensor/iLot/build/bootloader-prefix/tmp"
  "/home/fracchio/Scrivania/-IoT-AP-2024-Sapienza-ESP32-Smart-Parking-Sensor/iLot/build/bootloader-prefix/src/bootloader-stamp"
  "/home/fracchio/Scrivania/-IoT-AP-2024-Sapienza-ESP32-Smart-Parking-Sensor/iLot/build/bootloader-prefix/src"
  "/home/fracchio/Scrivania/-IoT-AP-2024-Sapienza-ESP32-Smart-Parking-Sensor/iLot/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/fracchio/Scrivania/-IoT-AP-2024-Sapienza-ESP32-Smart-Parking-Sensor/iLot/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/fracchio/Scrivania/-IoT-AP-2024-Sapienza-ESP32-Smart-Parking-Sensor/iLot/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
