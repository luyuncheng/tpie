//
// File: ami_gen_perm_object.h
// Author: Darren Vengroff <darrenv@eecs.umich.edu>
// Created: 12/15/94
//
// $Id: ami_gen_perm_object.h,v 1.4 2003-04-17 12:22:15 jan Exp $
//
#ifndef _TPIE_AMI_GEN_PERM_OBJECT_H
#define _TPIE_AMI_GEN_PERM_OBJECT_H

// Get definitions for working with Unix and Windows
#include <portability.h>

// For AMI_err.
#include <err.h>

namespace tpie {

    namespace ami {

// A class of object that computes permutation destinations.
	class gen_perm_object {
	public:
	    virtual err initialize(TPIE_OS_OFFSET len) = 0;
	    virtual TPIE_OS_OFFSET destination(TPIE_OS_OFFSET src) = 0;
	    virtual ~gen_perm_object() {};
	};

    }  //  ami namespace

}  //  tpie namespace
 
#endif // _TPIE_AMI_GEN_PERM_OBJECT_H 