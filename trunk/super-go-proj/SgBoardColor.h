//----------------------------------------------------------------------------
/** @file SgBoardColor.h
    State of a point on the board for games with Black, White, Empty states. */
//----------------------------------------------------------------------------

#ifndef SG_BOARDCOLOR_H
#define SG_BOARDCOLOR_H

#include <climits>
#include "SgBlackWhite.h"
#include <Poco/Debugger.h>

//----------------------------------------------------------------------------

/** Empty point. */
const int SG_EMPTY = 2;

/** Border point (outside of playing area) */
const int SG_BORDER = 3;

//----------------------------------------------------------------------------

// SgEBWIterator and maybe other code relies on that
poco_static_assert(SG_BLACK == 0);
poco_static_assert(SG_WHITE == 1);
poco_static_assert(SG_EMPTY == 2);

//----------------------------------------------------------------------------

/** SG_BLACK, SG_WHITE, or SG_EMPTY */
typedef int SgEmptyBlackWhite;

/** SG_BLACK, SG_WHITE, SG_EMPTY, or SG_BORDER */
typedef int SgBoardColor;

#define poco_assert_EBW(c) \
    poco_assert(c == SG_BLACK || c == SG_WHITE || c == SG_EMPTY)

#define poco_assert_COLOR(c) \
poco_assert(c == SG_BLACK || c == SG_WHITE || c == SG_EMPTY || c == SG_BORDER)

inline bool SgIsEmptyBlackWhite(SgBoardColor c)
{
    return c == SG_BLACK || c == SG_WHITE || c == SG_EMPTY;
}

inline SgBoardColor SgOpp(SgBoardColor c)
{
    poco_assert_COLOR(c);
    return c <= SG_WHITE ? SgOppBW(c) : c;
}

inline char SgEBW(SgEmptyBlackWhite color)
{
    poco_assert_EBW(color);
    return color == SG_EMPTY ? 'E' : color == SG_BLACK ? 'B' : 'W';
}

//----------------------------------------------------------------------------

/** Iterator over three colors, Empty, Black and White.
    Works analogously to SgBWIterator. */
class SgEBWIterator
{
public:
    SgEBWIterator()
        : m_color(SG_BLACK)
    { }

    /** Advance the state of the iteration to the next element.
        Relies on current coding: incrementing SG_WHITE will yield value
        outside of {SG_EMPTY, SG_BLACK, SG_WHITE} */
    void operator++()
    {
        ++m_color;
    }

    /** Return the value of the current element. */
    SgEmptyBlackWhite operator*() const
    {
        return m_color;
    }

    /** Return true if iteration is valid, otherwise false. */
    operator bool() const
    {
        return m_color <= SG_EMPTY;
    }

private:
    SgEmptyBlackWhite m_color;

    /** Not implemented */
    SgEBWIterator(const SgEBWIterator&);

    /** Not implemented */
    SgEBWIterator& operator=(const SgEBWIterator&);
};

//----------------------------------------------------------------------------

#endif // SG_SGBOARDCOLOR_H
