#ifndef VEC_4_HPP
#define VEC_4_HPP

struct Vec4 {
	union {
		struct {
			float x, y, z, w;
		};
		struct {
			float r, g, b, a;
		};
	};
};

using Color = Vec4;

#endif // !VEC_4_HPP

