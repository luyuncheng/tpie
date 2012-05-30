// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; c-file-style: "stroustrup"; -*-
// vi:set ts=4 sts=4 sw=4 noet cino=(0 :
// Copyright 2011, The TPIE development team
// 
// This file is part of TPIE.
// 
// TPIE is free software: you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
// 
// TPIE is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with TPIE.  If not, see <http://www.gnu.org/licenses/>

// file_stream memory usage test

#include "common.h"
#include <boost/filesystem/operations.hpp>
#include <tpie/file_stream.h>

using namespace tpie;

static const std::string TEMPFILE = "tmp";
static const size_t ITEMS = 16*1024*1024;

class file_stream_memory_test : public memory_test {
public:
	file_stream_memory_test(float block_factor = 1.0f) :
		m_block_factor(block_factor) {
		// Empty ctor
	}

	virtual ~file_stream_memory_test() {
		// Empty dtor
	}

	virtual void alloc() {
		m_stream = tpie_new<file_stream<size_t> >(m_block_factor);
	}

	virtual void use() {
		boost::filesystem::remove(TEMPFILE);
		m_stream->open(TEMPFILE);
		for (size_t i = 0; i < ITEMS; ++i) {
			m_stream->write(i);
		}
	}

	virtual void free() {
		tpie_delete<file_stream<size_t> >(m_stream);
		m_stream = 0;
	}

	virtual size_type claimed_size() {
		return file_stream<size_t>::memory_usage(m_block_factor);
	}

private:
	file_stream<size_t> * m_stream;
	const float m_block_factor;
};

int main(int argc, char **argv) {
	if (argc < 2 || std::string(argv[1]) != "memory") {
		std::cout << "Usage: " << argv[0] << " memory" << std::endl;
		return EXIT_FAILURE;
	}
	float block_factor = 1.0;
	if (argc > 2) {
		std::stringstream(std::string(argv[2])) >> block_factor;
	}
	tpie_init();
	file_stream_memory_test tester(block_factor);
	tester();
	tpie_finish();
}