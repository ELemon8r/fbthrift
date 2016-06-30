/*
 * Copyright 2016 Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef THRIFT_FATAL_DEBUG_H_
#define THRIFT_FATAL_DEBUG_H_ 1

#include <folly/Range.h>

#include <string>

/**
 * READ ME FIRST: this header enhances Thrift with debugging utilities.
 *
 * Please refer to the top of `thrift/lib/cpp2/fatal/reflection.h` on how to
 * enable compile-time reflection for Thrift types. The present header relies on
 * it for its functionality.
 *
 * TROUBLESHOOTING:
 *  - make sure you've followed the instructions on `reflection.h` to enable
 *    generation of compile-time reflection;
 *  - make sure you've included the metadata for your Thrift types, as specified
 *    in `reflection.h`.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */

#include <thrift/lib/cpp2/fatal/internal/debug-inl-pre.h>

#include <thrift/lib/cpp2/fatal/pretty_print.h>

namespace apache { namespace thrift {

/**
 * Compares two objects for equality, field-by-fields, in a DFS traversal.
 * Returns a boolean that tells whether they're equal or not.
 *
 * Once a mismatch has been found, the callback is called as if with the
 * following signature:
 *
 *  void operator ()(
 *    T const &lhs,
 *    T const &rhs,
 *    folly::StringPiece path,
 *    folly::StringPiece message
 *  ) const;
 *
 *  lhs: the left-hand side mismatched field
 *  rhs: the right-hand side mismatched field
 *  path: the path in the DFS where the mismatch happened
 *  message: the message explaining the mismatch
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename T, typename Callback>
bool debug_equals(T const &lhs, T const &rhs, Callback &&callback) {
  std::string path("<root>");
  return detail::debug_equals(path, lhs, rhs, callback);
}

/**
 * A handy callback for `debug_equals()` that outputs to a given stream.
 *
 * See `make_debug_output_callback` for a convenient way to create an instance
 * of this callback.
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */
template <typename Output>
struct debug_output_callback {
  explicit debug_output_callback(Output &out): out_(out) {}

  template <typename  T>
  void operator ()(
    T const &lhs,
    T const &rhs,
    folly::StringPiece path,
    folly::StringPiece message
  ) const {
    out_ << path << ": " << message;
    out_ << std::endl << "  lhs:" << std::endl;
    pretty_print(out_, lhs, "  ", "    ");
    out_ << std::endl << "  rhs:" << std::endl;
    pretty_print(out_, rhs, "  ", "    ");
    out_ << std::endl;
  }

private:
  Output &out_;
};

/**
 * A convenient way to create an instance of `debug_output_callback`.
 *
 * Example:
 *
 *  bool const equals = debug_equals(
 *    lhs,
 *    rhs,
 *    make_debug_output_callback(std::cout)
 *  );
 *
 *  EXPECT_TRUE(
 *    debug_equals(
 *      lhs,
 *      rhs,
 *      make_debug_output_callback(LOG(ERROR))
 *    )
 *  );
 *
 * @author: Marcelo Juchem <marcelo@fb.com>
 */

template <typename Output>
debug_output_callback<Output> make_debug_output_callback(Output &output) {
  return debug_output_callback<Output>(output);
}

}} // apache::thrift

#include <thrift/lib/cpp2/fatal/internal/debug-inl-post.h>

#endif // THRIFT_FATAL_DEBUG_H_
