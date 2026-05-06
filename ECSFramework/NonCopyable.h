#pragma once


class CNonCopyable
{
protected:
    CNonCopyable() = default;

    ~CNonCopyable() = default;

    CNonCopyable(const CNonCopyable&) = delete;

    CNonCopyable& operator = (const CNonCopyable&) = delete;
};