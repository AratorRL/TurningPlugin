#pragma once

string vecToString(Vector v)
{
    return "(" + to_string(v.X) + ", " + to_string(v.Y) + ", " + to_string(v.Z) + ")";
}

string rotToString(Rotator r)
{
    return "(" + to_string(r.Pitch) + ", " + to_string(r.Yaw) + ", " + to_string(r.Roll) + ")";
}