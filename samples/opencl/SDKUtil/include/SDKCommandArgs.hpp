#ifndef SDKCOMMANDARGS_HPP_
#define SDKCOMMANDARGS_HPP_

/**
 * Header Files
 */
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <ctime>
#include <string.h>
#include <SDKCommon.hpp>

/**
 * namespace streamsdk
 */
namespace streamsdk
{

	/**
	 * CmdArgsEnum
	 * Enum for datatype of CmdArgs
	 */
	enum CmdArgsEnum
	{
	  CA_ARG_INT,
	  CA_ARG_FLOAT, 
	  CA_ARG_DOUBLE, 
	  CA_ARG_STRING,
	  CA_NO_ARGUMENT
	};

	/**
	 * Option
	 * struct implements the feilds in option
	 */
	struct Option
	{
	  std::string  _sVersion;
	  std::string  _lVersion;
	  std::string  _description;
	  CmdArgsEnum  _type;
	  void *       _value;
	};

	/**
	 * SDKCommandArgs
	 * class implements Support for Command Line Options for the sample
	 */
	class SDKCommandArgs
	{
		private:
			int _numArgs;				/**< number of arguments */
			int _argc;					/**< number of arguments */
			int _seed;					/**< seed value */
			char ** _argv;				/**< array of char* holding CmdLine Options */
			Option * _options;			/**< struct option object */

			/**
			 * Constructor
			 * Defined Privately and cannot be called directly
			 */
			SDKCommandArgs(void)
			{
				_options = NULL; 
				_numArgs = 0; 
				_argc = 0; 
				_argv = NULL;
				_seed = 123;
			}

			/**
			 * match
			 * function 
			 * @param argv array of CMDLine Options
			 * @param argc Number of CMdLine Options
			 * @return 0 on success Positive if expected and Non-zero on failure
			 */
			int match(char ** argv, int argc);
	      
		public:

			/**
			 * Constructor
			 * @param numArgs number of arguments
			 * @param options array of option objects
			 */
			SDKCommandArgs(int numArgs,Option * options) 
			: _numArgs(numArgs), _options(options)
			{}

			/**
			 * Destructor
			 */
			~SDKCommandArgs();
			
			/**
			 * AddOption
			 * Function to a new CmdLine Option
			 * @param op Option object
			 * @return 0 on success Positive if expected and Non-zero on failure
			 */
			int AddOption(Option* op);

			/**
			 * DeleteOption
			 * Function to a delete CmdLine Option
			 * @param op Option object
			 * @return 0 on success Positive if expected and Non-zero on failure
			 */
			int DeleteOption(Option* op);

			/**
			 * parse
			 * Function to parse the Cmdline Options
			 * @param argv array of strings of CmdLine Options
			 * @param argc Number of CmdLine Options
			 * @return 0 on success Positive if expected and Non-zero on failure
			 */
			int parse(char ** argv, int argc);

			/**
			 * isArgSet
			 * Function to check if a argument is set
			 * @param arg option string
			 * @param shortVer if the option is short version. (Optional Param Default to false)
			 * @return true if success else false
			 */
			bool isArgSet(std::string arg, bool shortVer = false);

			/**
			 * help
			 * Function to write help text
			 */
			void help(void);
	};
}

#endif
