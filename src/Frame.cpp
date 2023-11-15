#include "Frame.hh"

// STD headers
#include <cstring>
#include <algorithm>

// EXCEPTION headers
#include "exceptions/BadAlloc.hh"
#include "exceptions/InitializationFailed.hh"

// File const values
static const char CLASS_NAME[] = "Frame";

ZMQ::Frame::Frame() :
    m_message_field(nullptr),
    m_message_field_size(0)
{

}

ZMQ::Frame::Frame(const char* message_information,
                  size_t message_size) :
    m_message_field(nullptr),
    m_message_field_size(0)
{
    SetFrameInformation(message_information, message_size);
}

ZMQ::Frame::Frame(const std::string& message_text) :
    m_message_field(nullptr),
    m_message_field_size(0)
{
    SetMessageString(message_text);
}

ZMQ::Frame::Frame(const Frame& other) :
    m_message_field(other.m_message_field),
    m_message_field_size(other.m_message_field_size)
{

}

ZMQ::Frame::~Frame()
{
    ClearFrameMemoryContainer();
}

bool
ZMQ::Frame::IsEmpty() const
{
    if (m_message_field == nullptr)
    {
        return true;
    }
    return false;
}

size_t
ZMQ::Frame::GetFrameMessageSize() const
{
    return m_message_field_size;
}

bool
ZMQ::Frame::ContainValidString() const
{
    // Check the existence of the null terminator
    const char* str_end =
        static_cast<const char*>(std::memchr(m_message_field,
                                             '\0',
                                             m_message_field_size));
    // If it was found, and it's in the end of the array it's valid
    if (str_end != nullptr &&
        str_end == static_cast<const char*>(m_message_field) + m_message_field_size - 1)
    {
        return true;
    }
    return false;
}

void
ZMQ::Frame::SetFrameInformation(const char* message_information,
                                size_t message_size)
{
    // Allocate space and copy information
    AllocateFrameMemorySize(message_size);
    memcpy(m_message_field, message_information, message_size);
}

unsigned int
ZMQ::Frame::GetFrameInformation(const char* message_information,
                                size_t max_size)
{
    // Check number of bytes to copy
    unsigned int copy_size = std::min(max_size, m_message_field_size);

    // Performance data copy and return
    memcpy((void *)message_information, m_message_field, copy_size);
    return copy_size;
}

void
ZMQ::Frame::SetMessageString(const std::string& message)
{
    // Check if it needs to be cleared
    if (IsEmpty() == false)
    {
        ClearFrameMemoryContainer();
    }

    // Get the string size
    size_t message_text_size = message.size();
    AllocateFrameMemorySize(message_text_size);
    memcpy(m_message_field, message.c_str(), message_text_size);
}

std::string
ZMQ::Frame::GetMessageString() const
{
    // Check if contain valid string
    if (ContainValidString() != true)
    {
        throw InitializationFailed(CLASS_NAME,
                                   "GetMessageString",
                                   "Invalid string: string info mismatch");
    }

    // Construct message to return
    char* temp_cstr_buffer = new char[m_message_field_size + 1];
    memcpy(temp_cstr_buffer, m_message_field, m_message_field_size);
    temp_cstr_buffer[m_message_field_size] = '\0';
    std::string message_text (temp_cstr_buffer);

    // Clear allocated memory
    delete[] temp_cstr_buffer;

    // Return string structure
    return message_text;
}

std::vector<char>
ZMQ::Frame::GetRawData() const
{
    // Create a vector with the message inside
    std::vector<char> v(m_message_field_size);
    std::memcpy(v.data(), m_message_field, m_message_field_size);

    return v;
}

ZMQ::Frame&
ZMQ::Frame::operator=(const Frame& other)
{
    if (this == &other)
    {
        return *this;
    }
    // check if it needs to be released
    if (IsEmpty() == false)
    {
        ClearFrameMemoryContainer();
    }

    // Copy data to class
    AllocateFrameMemorySize(other.m_message_field_size);
    memcpy(m_message_field, other.m_message_field, m_message_field_size);
    
    // Return self
    return *this;
}

void
ZMQ::Frame::ClearFrameMemoryContainer()
{
    if (m_message_field != nullptr)
    {
        // Delete and initialize memory
        // When deleting the pointer should be cast to the data type new was called
        delete[] static_cast<char*>(m_message_field);
        m_message_field_size = 0;
        m_message_field = nullptr;
    }
}

void
ZMQ::Frame::AllocateFrameMemorySize(size_t alloc_size)
{
    try {
        // Alloc and check if valid pointer
        m_message_field = new char[alloc_size];
    }
    catch (std::bad_alloc&)
    {
        throw BadAlloc(CLASS_NAME,
                       "AllocateFrameMemorySize",
                       "Unable to allocate memory: requested size is too large");
    }
    // Set length
    m_message_field_size = alloc_size;
    std::memset(m_message_field, 0, m_message_field_size);
}