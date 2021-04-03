/*	Copyright (C) 2004 Garrett A. Kajmowicz

	This file is part of the uClibc++ Library.

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "basic_definitions"
#include "exception"
#include "cstddef"

#ifndef __STD_NEW_OPERATOR
#define __STD_NEW_OPERATOR 1

#pragma GCC visibility push(default)

namespace std{
	class _UCXXEXPORT bad_alloc : public exception {};

	struct _UCXXEXPORT nothrow_t {};
	extern const nothrow_t nothrow;

	typedef void (*new_handler)();
	_UCXXEXPORT new_handler set_new_handler(new_handler new_p) throw();
}


_UCXXEXPORT void* operator new(std::size_t numBytes) throw(std::bad_alloc);
_UCXXEXPORT void operator delete(void* ptr) throw();

_UCXXEXPORT void* operator new[](std::size_t numBytes) throw(std::bad_alloc);
_UCXXEXPORT void operator delete[](void * ptr) throw();

#ifndef NO_NOTHROW
_UCXXEXPORT void* operator new(std::size_t numBytes, const std::nothrow_t& ) throw();
_UCXXEXPORT void operator delete(void* ptr, const std::nothrow_t& ) throw();

_UCXXEXPORT void* operator new[](std::size_t numBytes, const std::nothrow_t& ) throw();
_UCXXEXPORT void operator delete[](void* ptr, const std::nothrow_t& ) throw();
#endif

	/* Placement operators */
inline void* operator new(std::size_t, void* ptr) throw() {return ptr; }
inline void operator delete(void* , void *) throw() { }
	
inline void* operator new[](std::size_t, void *p) throw() { return p; }
inline void operator delete[](void* , void *) throw() {}

#pragma GCC visibility pop

#endif

