#ifndef SDKFILE_HPP_
#define SDKFILE_HPP_

/**
 * Header Files
 */
#include <vector>
#include <string>
#include <fstream>
#include <malloc.h>
#include <SDKCommon.hpp>

/**
 * namespace streamsdk
 */
namespace streamsdk
{
/**
 * getCurrentDir
 * Get current directory
 * @return string
 */
extern std::string getCurrentDir();

/**
 * class SDKFile
 * for the opencl program file processing
 */
class SDKFile
{
public:
    /**
	 *Default constructor
	 */
    SDKFile(): source_(""){}

    /**
	 * Destructor
	 */
    ~SDKFile(){};

    /**
	 * Opens the CL program file
	 * @return true if success else false
	 */
    bool open(const char* fileName);

	/**
	 * writeBinaryToFile
	 * @param fileName Name of the file
	 * @param binary char binary array
	 * @param numBytes number of bytes
	 * @return true if success else false
	 */
    int writeBinaryToFile(const char* fileName, const char* binary, size_t numBytes);

	/**
	 * readBinaryToFile
	 * @param fileName name of file
	 * @return true if success else false
	 */
    int readBinaryFromFile(const char* fileName);

    /**
	 * Replaces Newline with spaces
	 */
    void replaceNewlineWithSpaces()
    {
        size_t pos = source_.find_first_of('\n', 0);
        while(pos != -1)
        {
            source_.replace(pos, 1, " ");
            pos = source_.find_first_of('\n', pos + 1);
        }
        pos = source_.find_first_of('\r', 0);
        while(pos != -1)
        {
            source_.replace(pos, 1, " ");
            pos = source_.find_first_of('\r', pos + 1);
        }
    }

	/**
	 * source
	 * Returns a pointer to the string object with the source code
	 */
    const std::string&  source() const { return source_; }

private:
    /**
	 * Disable copy constructor
	 */
    SDKFile(const SDKFile&);

    /**
	 * Disable operator=
	 */
    SDKFile& operator=(const SDKFile&);

    std::string     source_;    //!< source code of the CL program
};

} // namespace streamsdk

#endif  // SDKFile_HPP_
