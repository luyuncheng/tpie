// Copyright (C) 2001 Octavian Procopiuc
//
// File:    bte_coll_base.h
// Authors: Octavian Procopiuc <tavi@cs.duke.edu>
//          (using some code by Rakesh Barve)
//
// $Id: bte_coll_base.h,v 1.1 2001-05-17 19:48:20 tavi Exp $
//
// BTE_collection_base class and various basic definitions.
//

#ifndef _BTE_COLL_BASE_H
#define _BTE_COLL_BASE_H

#include <stdlib.h>

// Include the registration based memory manager.
#define MM_IMP_REGISTER
#include <mm.h>

// For persist.
#include <persist.h>
// For stdio_stack.
#include <stdio_stack.h>
// For BTE_err
#include <bte_base_stream.h>

// BTE_COLLECTION types passed to constructors.
enum BTE_collection_type {
  BTE_READ_COLLECTION = 1,    // Open existing stream read only.
  BTE_WRITE_COLLECTION,	      // Open for read/write. Create if non-existent.
  BTE_WRITE_NEW_COLLECTION    // Open for read/write a new collection,
                              // even if a nonempty file with that name exists.
};

// BTE collection status.
enum BTE_collection_status {
  BTE_COLLECTION_STATUS_VALID = 0,
  BTE_COLLECTION_STATUS_INVALID = 1,
};

// Maximum length of the file names.
#define BTE_COLLECTION_PATH_NAME_LENGTH 128

// Number of bytes in the header's user_data_ field.
#define BTE_COLLECTION_USER_DATA_LENGTH 512

// The magic number of the files storing blocks. Stored in the header.
#define BTE_COLLECTION_HEADER_MAGIC_NUMBER 0x123ABC

// Default file name suffixes
#define BTE_COLLECTION_BLK_SUFFIX ".blk"
#define BTE_COLLECTION_STK_SUFFIX ".stk"


// The in-memory representation of the BTE_COLLECTION header.
// This data structure is read from/written to the first 
// (physical) page of the blocks file.

class BTE_collection_header {
public:

  // Unique header identifier. Set to BTE_COLLECTION_HEADER_MAGIC_NUMBER
  unsigned int magic_number;
  // Should be 1 for current version.
  unsigned int version;
  // # of bytes in this structure.
  size_t header_length;
  // The number of blocks consumed by this bte_coll.
  size_t total_blocks;
  // The highest bid any block of this block collection has, PLUS 1 (always <= total_blocks).
  size_t last_block; 
  // The number of valid blocks in this block collection.
  size_t used_blocks;
  // The size of a physical block on the device this stream resides.
  size_t os_block_size;
  // Size in bytes of each logical block.
  size_t block_size;
  // Some data to be filled by the user of the collection.
  char user_data[BTE_COLLECTION_USER_DATA_LENGTH];
  
  // Default constructor.
  BTE_collection_header();
};

//
// A base class for all implementations of block collection classes.
//
class BTE_collection_base {
protected:
  
  // An stdio_stack of off_t's.
  stdio_stack<off_t> *freeblock_stack_; 

  // File descriptor for the file backing the block collection.
  int bcc_fd_;

  char base_file_name_[BTE_COLLECTION_PATH_NAME_LENGTH];

  // Various parameters (will be stored into the file header block).
  BTE_collection_header header_;

  size_t os_block_size_;

  // Persistency flag. Set during construction and using the persist()
  // method.
  persistence per_;

  // Status of the collection. Set during construction.
  BTE_collection_status status_;

  // Read-only flag. Set during construction.
  bool read_only_;

  // Number of blocks from this collection that are currently in memory
  size_t in_memory_blocks_;

private:
  // Helper functions. We don't want them inherited.

  // Initialization common to all constructors.
  void shared_init(BTE_collection_type type, size_t logical_block_factor);

  // Read header from disk.
  BTE_err read_header(char *bcc_name);

  // Write header to disk.
  BTE_err write_header(char* bcc_name);

  void remove_stack_file();

protected:

  // Needs to be inlined!
  BTE_err register_memory_allocation(size_t sz) {
    if (MM_manager.register_allocation(sz) != MM_ERROR_NO_ERROR) {
      status_ = BTE_COLLECTION_STATUS_INVALID;
      LOG_FATAL_ID("Memory manager error in allocation.");
      return BTE_ERROR_MEMORY_ERROR;
    }
    return BTE_ERROR_NO_ERROR;
  }

  // Needs to be inlined!
  BTE_err register_memory_deallocation(size_t sz) {
    if (MM_manager.register_deallocation(sz) != MM_ERROR_NO_ERROR) {
      status_ = BTE_COLLECTION_STATUS_INVALID;
      LOG_FATAL_ID("Memory manager error in deallocation.");
      return BTE_ERROR_MEMORY_ERROR;
    }
    return BTE_ERROR_NO_ERROR;
  }

  off_t bid_to_file_offset(off_t bid) const 
    { return header_.os_block_size + header_.block_size * bid; }

  void create_stack();

  // Common code for all new_block implementations.
  BTE_err new_block_shared(off_t& bid);

  // Common code for all delete_block implementations.
  BTE_err delete_block_shared(off_t bid);

public:

  BTE_collection_base(const char *base_name, BTE_collection_type ct, 
		      size_t logical_block_factor);

  // Return the total number of used blocks.
  size_t size() const { return header_.used_blocks; }
                          	
  // Return the total number of blocks consumed by the block collection.
  size_t file_size() const { return header_.total_blocks; }

  // Return the logical block size in bytes.
  size_t block_size() const { return header_.block_size; }

  // Return the logical block factor.
  size_t block_factor() const 
    { return header_.block_size / header_.os_block_size; }

  // Return the status of the collection.
  BTE_collection_status status() const { return status_; }

  // Set the persistence flag. 
  void persist(persistence p) { per_ = p; }

  const char *base_file_name() const { return base_file_name_; }

  void *user_data() { return (void *) header_.user_data; }

  // Destructor.
  ~BTE_collection_base(); 

#if defined(__sun__) 
  static bool direct_io;
#endif
};

#endif //_BTE_COLL_BASE_H
