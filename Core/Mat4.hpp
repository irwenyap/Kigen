#ifndef MAT_4_HPP
#define MAT_4_HPP

struct Mat4 {
	float a[16] = { 0 };

	Mat4() {};
		
	Mat4(const float& a0, const float& a1, const float& a2, const float& a3,
		const float& a4, const float& a5, const float& a6, const float& a7,
		const float& a8, const float& a9, const float& a10, const float& a11,
		const float& a12, const float& a13, const float& a14, const float& a15) {
		a[0] = a0, a[1] = a1, a[2] = a2, a[3] = a3;
		a[4] = a4, a[5] = a5, a[6] = a6, a[7] = a7;
		a[8] = a8, a[9] = a9, a[10] = a10, a[11] = a11;
		a[12] = a12, a[13] = a13, a[14] = a14, a[15] = a15;
	};

	inline float& operator[](const unsigned& i) {
		return a[i];
	}

	inline const float& operator[](const unsigned& i) const {
		return a[i];
	}
};

#endif // !MAT_4_HPP

