#if !defined(SEEN_ENGINE_TABLECELLS_H_20201022_)
#define SEEN_ENGINE_TABLECELLS_H_20201022_ 1
#pragma once

#include <string>
#include <utility>

#include <boost/variant.hpp>

/// An error type for our cell data.
///
/// Currently this is a singleton type (so all errors are effectively
/// the same).
struct Error
{
   constexpr friend bool operator==(Error, Error) { return true;}
   constexpr friend bool operator< (Error, Error) { return false;}
};

/// Data that we want to put in a \c cListBox cell.
using CellData = boost::variant<std::string, int, bool, Error>;

/// Cell data with attached formatting.
struct FormattedCellData {
   CellData val_;
   std::string fmt_;
};

/// Create a formatted cell with text.
inline
FormattedCellData mk_text(std::string val)
{
   auto str = val;
   return FormattedCellData{std::move(val), std::move(str)};
}

/// Create a formatted cell with an error.
inline
FormattedCellData mk_error(std::string errtext)
{
   return FormattedCellData{Error{}, std::move(errtext)};
}

/// Create a formatted cell with a numeric value.
inline
FormattedCellData mk_num(int val)
{
   return FormattedCellData{val, std::to_string(val)};
};

/// Create a formatted "Yes/No" cell.
inline
FormattedCellData mk_yesno(bool val)
{
   return FormattedCellData{val, val ? "Yes" : "No"};
};

/// Create a formatted cell with a numeric percentage.
inline
FormattedCellData mk_percent(int val)
{
   return FormattedCellData{val, std::to_string(val) + '%'};
};

/// Create a formatted cell with a health value.
inline
FormattedCellData mk_health(int val)
{
   if(val <= 0)
      return FormattedCellData{val, "DEAD"};
   else
      return FormattedCellData{val, std::to_string(val) + '%'};
};

#endif
