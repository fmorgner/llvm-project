//===--- Annotations.h - Annotated source code for tests ---------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Annotations lets you mark points and ranges inside source code, for tests:
//
//    Annotations Example(R"cpp(
//       int complete() { x.pri^ }          // ^ indicates a point
//       void err() { [["hello" == 42]]; }  // [[this is a range]]
//       $definition^class Foo{};           // points can be named: "definition"
//       $fail[[static_assert(false, "")]]  // ranges can be named too: "fail"
//    )cpp");
//
//    StringRef Code = Example.code();              // annotations stripped.
//    std::vector<size_t> PP = Example.points();    // all unnamed points
//    size_t P = Example.point();                   // there must be exactly one
//    llvm::Range R = Example.range("fail");        // find named ranges
//
// Points/ranges are coordinates into `code()` which is stripped of annotations.
//
// Ranges may be nested (and points can be inside ranges), but there's no way
// to define general overlapping ranges.
//
//===---------------------------------------------------------------------===//

#ifndef LLVM_TESTING_SUPPORT_ANNOTATIONS_H
#define LLVM_TESTING_SUPPORT_ANNOTATIONS_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include <vector>

namespace llvm {

/// Two offsets pointing to a continuous substring. End is not included, i.e.
/// represents a half-open range.
struct Range {
  size_t Begin = 0;
  size_t End = 0;
};

class Annotations {
public:
  // Parses the annotations from Text. Crashes if it's malformed.
  Annotations(llvm::StringRef Text);

  // The input text with all annotations stripped.
  // All points and ranges are relative to this stripped text.
  llvm::StringRef code() const { return Code; }

  // Returns the position of the point marked by ^ (or $name^) in the text.
  // Crashes if there isn't exactly one.
  size_t point(llvm::StringRef Name = "") const;
  // Returns the position of all points marked by ^ (or $name^) in the text.
  std::vector<size_t> points(llvm::StringRef Name = "") const;

  // Returns the location of the range marked by [[ ]] (or $name[[ ]]).
  // Crashes if there isn't exactly one.
  Range range(llvm::StringRef Name = "") const;
  // Returns the location of all ranges marked by [[ ]] (or $name[[ ]]).
  std::vector<Range> ranges(llvm::StringRef Name = "") const;

private:
  std::string Code;
  llvm::StringMap<llvm::SmallVector<size_t, 1>> Points;
  llvm::StringMap<llvm::SmallVector<Range, 1>> Ranges;
};

} // namespace llvm

#endif // LLVM_CLANG_TOOLS_EXTRA_UNITTESTS_CLANGD_ANNOTATIONS_H
