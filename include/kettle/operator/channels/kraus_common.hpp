#pragma once

namespace ket
{

/*
    A tag passed to the constructor of the `KrausChannel` object to skip checking
    whether or not the matrices form a valid channel.
*/
struct kraus_channel_nocheck {};

}  // namespace ket
