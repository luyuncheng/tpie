//
// File: ami_device.cpp
// Author: Darren Erik Vengroff <darrenv@eecs.umich.edu>
// Created: 8/22/93
//

#include "lib_config.h"

#include <stdlib.h>
#include <string.h>

#include <err.h>
#include <device.h>


AMI_device::AMI_device(void) : argc(0), argv(NULL)
{
    TP_LOG_DEBUG_ID("In AMI_device(void).");
}


AMI_device::AMI_device(unsigned int count, char **strings) : argc(count), argv(NULL)
{
    char *s, *t;

    if (argc) {
        argv = new char*[argc];

        while (count--) {
            argv[count] = new char[strlen(strings[count]) + 1];
            // for (s = strings[count], t = argv[count]; *t++ = *s++; )
	    // [tavi] modified to avoid warning.
	    for (s = strings[count], t = argv[count]; *t; *t++ = *s++)
                ;
        }
                     
    }
}

AMI_device::AMI_device(const AMI_device& other) : argc(0), argv(NULL) {
    *this = other;
}

AMI_device::~AMI_device(void)
{
    dispose_contents();
}


AMI_device& AMI_device::operator=(const AMI_device& other) {

    if (this != &other) {

	char *s, *t;
	
	unsigned int count = other.argc;
	argc = other.argc;;

	if (argc) {
	    argv = new char*[argc];
	    
	    while (count--) {
		argv[count] = new char[strlen(other.argv[count]) + 1];
		// for (s = strings[count], t = argv[count]; *t++ = *s++; )
		// [tavi] modified to avoid warning.
		for (s = other.argv[count], t = argv[count]; *t; *t++ = *s++)
		    ;
	    }
	    
	} else {
	    argv = NULL;
	}
    }
    return *this;
};

const char * AMI_device::operator[](unsigned int index)
{
    if (argv)
	return argv[index];

    return NULL;
}

unsigned int AMI_device::arity()
{
    return argc;
}

void AMI_device::dispose_contents(void)
{
    if (argc) {
        while (argc--) {
            delete argv[argc];
        }

        tp_assert((argv != NULL), "Nonzero argc and NULL argv.");

        delete argv;
    }
}

tpie::ami::err AMI_device::set_to_path(const char *path)
{
    const char *s, *t;
    unsigned int ii;

    dispose_contents();

    // Count the components
    for (argc = 1, s = path; *s; s++) {
        if (*s == '|') {
            argc++;
        }
    }
                
    argv = new char*[argc];

    // copy the components one by one.  t points to the start of the 
    // current component and s is used to scan to the end of it.

    for (ii = 0, s = t = path; ii < argc; ii++, t = ++s) {
        // Move past the current component.
        while (*s && (*s != '|')) 
            s++;

        tp_assert(((*s == '|') || (ii == argc - 1)),
                  "Path ended before all components found.");
        
        // Copy the current component.
        argv[ii] = new char[s - t + 1];
        strncpy(argv[ii], t, s - t);
        argv[ii][s - t] = '\0';

		// make sure there is no trailing /
		for(TPIE_OS_LONGLONG i=s-t-1; i && (argv[ii][i]) == '/'; i--) {
		  argv[ii][i] = '\0';
		}
		tp_assert(strlen(argv[ii]) > 0, "non-null path specified");
		tp_assert(strlen(argv[ii]) == 1 ||
				  argv[ii][strlen(argv[ii])] != '/', "no / suffix");
    }

    return tpie::ami::NO_ERROR;
}


tpie::ami::err AMI_device::read_environment(const char *name)
{
    char *env_value = getenv(name);
    
    if (env_value == NULL) {
        return tpie::ami::ENV_UNDEFINED;
    }

    return set_to_path(env_value);
}


// Output of a device description:
std::ostream &operator<<(std::ostream &os, const AMI_device &dev)
{
    unsigned int ii;

    for (ii = 0; ii < dev.argc; ii++) {
        os << dev.argv[ii];
        if (ii < dev.argc - 1) {
            os << '|';
        }
    }
    
    return os;
}