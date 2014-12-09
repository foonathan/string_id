// Copyright (C) 2014 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef FOONATHAN_STRING_ID_DATABASE_HPP_INCLUDED
#define FOONATHAN_STRING_ID_DATABASE_HPP_INCLUDED

#include "detail/database.hpp"
#include "config.hpp"

namespace foonathan { namespace string_id
{
    /// \brief The database where the strings are stored.
    /// \detail Its exact type is implementation dependent and you don't need to worry about it,
    /// just pass it to \ref string_id.<br>
    /// You can control its behavior via the macros listed in config.hpp.in.
#if FOONATHAN_STRING_ID_DATABASE && FOONATHAN_STRING_ID_MULTITHREADED
    using database = detail::thread_safe_database;
#elif FOONATHAN_STRING_ID_DATABASE
    using database = detail::single_threaded_database;
#else
    using database = detail::dummy_database;
#endif
}}

#endif // FOONATHAN_STRING_ID_DATABASE_HPP_INCLUDED
