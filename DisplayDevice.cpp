#include "DisplayDevice.hpp"

DisplayDevice::DisplayDevice(): m_width(128), m_height(64) {}
DisplayDevice::DisplayDevice(std::uint8_t width, std::uint8_t height)  : m_width(width), m_height(height) {}
