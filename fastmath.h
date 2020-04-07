/*
 * Unoptimized and incorrect implementations of math functtions.
 * They are accurate enough for their use.
 */
// Fast sine uses a lookup table that has 360 entries and wraps
// around after. Uses degree instead of radians
float fast_sin(int deg);

// Simulates a triangle wave with an amplitude of 1
float fast_triangle(int deg);

// Clamps a float to either floor or roof, depending on what
// is nearer.
int clamp(float val, int floor, int roof);
