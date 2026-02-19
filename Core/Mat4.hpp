/*********************************************************************
 * \file	Mat4.hpp
 * \brief	Defines a custom Matrix 4x4 library with common graphics
			transformation matrices and Math operations.
 *
 * \author	Wong Woon Li, woonli.wong, 2301308
 * \email	woonli.wong@digipen.edu
 * \date	20 September 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

#ifndef MAT_4_HPP
#define MAT_4_HPP

struct Vec3;
struct Vec4;
struct Mat3;

constexpr const float PI = 3.14159265358979323846f;

// Matrix elements stored in memory using column-major order.
struct Mat4 {
	float a[16] = { 0 };

	Mat4() = default;
	
	/*!***********************************************************************
	\brief
		Constructs a Mat4 with the initializers.
	\param[in] const float& e00, const float& e01, const float& e02, const float& e03,
				 const float& e10, const float& e11, const float& e12, const float& e13,
				 const float& e20, const float& e21, const float& e22, const float& e23,
				 const float& e30, const float& e31, const float& e32, const float& e33
		Initializers for the Mat4.
	*************************************************************************/
	Mat4(const float& e00, const float& e01, const float& e02, const float& e03,
		 const float& e10, const float& e11, const float& e12, const float& e13,
		 const float& e20, const float& e21, const float& e22, const float& e23,
		 const float& e30, const float& e31, const float& e32, const float& e33);

	Mat4(const float arr[16]);

	/*!***********************************************************************
	\brief
		Copy constructor for Mat4.
	\param[in] const Mat4& m
		Mat4 to copy from.
	*************************************************************************/
	Mat4(const Mat4& m);

	/*!***********************************************************************
	\brief
		Assignment operator for Mat4.
	\param[in] const Mat4& m
		Mat4 to copy from.
	\return
		Reference to Mat4 object with copied values.
	*************************************************************************/
	Mat4& operator=(const Mat4& m);

	/*!***********************************************************************
	\brief
		Matrix-matrix multiplication.
	\param[in] const Mat4& rhs
		Rhs of the matrix multiplication.
	\return
		*this * rhs.
	*************************************************************************/
	Mat4 operator*(const Mat4& rhs);

	/*!***********************************************************************
	\brief
		Scalar Matrix multiplication.
	\param[in] float scalar
		Scalar of the matrix multiplication.
	\return
		*this * scalar
	*************************************************************************/
	Mat4 operator*(float scalar);

	/*!***********************************************************************
	\brief
		Matrix-matrix compound multiplication.
	\param[in] const Mat4& rhs
		Rhs of the matrix multiplication.
	\return
		*this = *this * rhs
	*************************************************************************/
	Mat4& operator*=(const Mat4& rhs);

	/*!***********************************************************************
	\brief
		Getter/setter function for element of Matrix at position i.
	\param[in] const unsigned& i
		Position of element to get/set.
	\return
		Reference to the element at position i.
	*************************************************************************/
	inline float& operator[](const unsigned& i) { return a[i]; }

	/*!***********************************************************************
	\brief
		Getter function for element of const Matrix at position i.
	\param[in] const unsigned& i
		Position of element to get.
	\return
		Const reference to the element at position i.
	*************************************************************************/
	inline const float& operator[](const unsigned& i) const { return a[i]; }

	/*!***********************************************************************
	\brief
		Set all Matrix values to 0.
	*************************************************************************/
	void SetToZero();

	/*!***********************************************************************
	\brief
		Set Matrix to an identity matrix.
	*************************************************************************/
	void SetToIdentity();
	
	/*!***********************************************************************
	\brief
		Set Matrix to the specified values in row-major order.
	\param[in] const float& e00, const float& e01, const float& e02, const float& e03,
			   const float& e10, const float& e11, const float& e12, const float& e13,
			   const float& e20, const float& e21, const float& e22, const float& e23,
			   const float& e30, const float& e31, const float& e32, const float& e33
		Values to set Matrix to.
	*************************************************************************/
	void SetTo(const float& e00, const float& e01, const float& e02, const float& e03,
			   const float& e10, const float& e11, const float& e12, const float& e13,
			   const float& e20, const float& e21, const float& e22, const float& e23,
			   const float& e30, const float& e31, const float& e32, const float& e33);

	/*!***********************************************************************
	\brief
		Getter/setter function for element of Matrix at specified row and col.
	\param[in] unsigned int row
		Row of matrix to access.
	\param[in] unsigned int col
		Column of matrix to access.
	\return
		Reference to the element at specified row and column.
	*************************************************************************/
	inline float& GetElement(unsigned int row, unsigned int col) { return a[col * 4 + row]; }

	/*!***********************************************************************
	\brief
		Getter/setter function for element of const Matrix at specified row
		and col.
	\param[in] unsigned int row
		Row of matrix to access.
	\param[in] unsigned int col
		Column of matrix to access.
	\return
		Const reference to the element at specified row and column.
	*************************************************************************/
	inline const float& GetElement(unsigned int row, unsigned int col) const { return a[col * 4 + row]; }

	/*!***********************************************************************
	\brief
		Return determinant of Matrix.
	\return
		Determinant of Matrix.
	*************************************************************************/
	float Determinant() const;

	/*!***********************************************************************
	\brief
		Transpose the Matrix in place.
	\return
		Transpose of the current Matrix.
	*************************************************************************/
	Mat4& TransposeInPlace();

	/*!***********************************************************************
	\brief
		Return transpose of Matrix.
	\return
		Transpose of Matrix.
	*************************************************************************/
	Mat4 Transpose() const;

	/*!***********************************************************************
	\brief
		Inverse the Matrix in place.
	\return
		True if inverse is found and false otherwise.
	*************************************************************************/
	bool InverseInPlace();

	/*!***********************************************************************
	\brief
		Return inverse of Matrix.
	\return
		Inverse of Matrix.
	*************************************************************************/
	Mat4 Inverse() const;

	/*!***********************************************************************
	\brief
		Return a translation matrix based on Vec3(x, y, z).
	\param[in] float x, float y, float z
		Vector to translate.
	\return
		Translation matrix based on Vec3(x, y, z).
	*************************************************************************/
	static Mat4	BuildTranslation(float x, float y, float z);

	/*!***********************************************************************
	\brief
		Return a translation matrix based on xyz.
	\param[in] const Vec3& xyz
		Vector to translate.
	\return
		Translation matrix based on xyz.
	*************************************************************************/
	static Mat4	BuildTranslation(const Vec3& xyz);

	/*!***********************************************************************
	\brief
		Return a rotation matrix based on rotation along Z-axis by specified
		degrees.
	\param[in] float degrees
		Degrees to rotate.
	\return
		Rotation matrix based on rotation along Z-axis by specified degrees.
	*************************************************************************/
	static Mat4 BuildZRotation(float degrees);

	/*!***********************************************************************
	\brief
		Return a rotation matrix based on rotation along X-axis by specified
		degrees.
	\param[in] float degrees
		Degrees to rotate.
	\return
		Rotation matrix based on rotation along X-axis by specified degrees.
	*************************************************************************/
	static Mat4 BuildXRotation(float degrees);

	/*!***********************************************************************
	\brief
		Return a rotation matrix based on rotation along Y-axis by specified
		degrees.
	\param[in] float degrees
		Degrees to rotate.
	\return
		Rotation matrix based on rotation along Y-axis by specified degrees.
	*************************************************************************/
	static Mat4 BuildYRotation(float degrees);

	/*!***********************************************************************
	\brief
		Return a rotation matrix based on rotation along the specified axis
		by specified degrees.
	\param[in] float degrees
		Degrees to rotate.
	\param[in] float x, float y, float z
		Axis to rotate.
	\return
		Rotation matrix based on rotation along the specified axis by specified
		degrees.
	*************************************************************************/
	static Mat4	BuildRotation(float degrees, float x, float y, float z);

	/*!***********************************************************************
	\brief
		Return a rotation matrix based on rotation along the specified axis
		by specified degrees.
	\param[in] float degrees
		Degrees to rotate.
	\param[in] const Vec3& axis
		Axis to rotate.
	\return
		Rotation matrix based on rotation along the specified axis by specified
		degrees.
	*************************************************************************/
	static Mat4	BuildRotation(float degrees, const Vec3& axis);

	/*!***********************************************************************
	\brief
		Return a scale matrix based on scaling about the specified pivot point
		and specified scalar vector.
	\param[in] float cx, float cy, float cz
		Pivot point of scale.
	\param[in] float x, float y, float z
		Scale vector.
	\return
		Scale matrix based on scaling about the specified pivot point and
		specified scalar vector.
	*************************************************************************/
	static Mat4	BuildScaling(float cx, float cy, float cz, float x, float y, float z);

	/*!***********************************************************************
	\brief
		Return a scale matrix based on scaling about the specified pivot point
		and specified scalar vector.
	\param[in] float cx, float cy, float cz
		Pivot point of scale.
	\param[in] float x, float y, float z
		Scale vector.
	\return
		Scale matrix based on scaling about the specified pivot point and
		specified scalar vector.
*************************************************************************/
	static Mat4	BuildScaling(float x, float y, float z);

	/*!***********************************************************************
	\brief
		Return a scale matrix based on scaling about the specified pivot point
		and specified scalar vector.
	\param[in] const Vec3& pivot
		Pivot point of scale.
	\param[in] const Vec3& scaleFactors
		Scale vector.
	\return
		Scale matrix based on scaling about the specified pivot point and
		specified scalar vector.
	*************************************************************************/
	static Mat4	BuildScaling(const Vec3& pivot, const Vec3& scaleFactors);

	/*!***********************************************************************
	\brief
		Return a view (camera) matrix based on the eye, target and up vector.
	\param[in] const Vec3& eye
		Eye (position) of camera.
	\param[in] const Vec3& tgt
		Target vector of camera.
	\param[in] const Vec3& up
		Up vector of camera.
	\return
		View (camera) matrix based on the eye, target and up vector.
	*************************************************************************/
	static Mat4	BuildViewMtx(const Vec3& eye, const Vec3& tgt, const Vec3& up);

	/*!***********************************************************************
	\brief
		Return a symmetric perspective projection matrix based on vertical FOV,
		aspect ratio, near and far plane.
	\param[in] float vfov
		Vertical FOV in degrees.
	\param[in] float aspect
		Aspect ratio of viewfinder.
	\param[in] float near, float far
		Near and far plane of the frustum.
	\return
		Symmetric perspective projection matrix based on vertical FOV, aspect
		ratio, near and far plane.
	*************************************************************************/
	static Mat4	BuildSymPerspective(float vfov, float aspect, float near, float far);

	/*!***********************************************************************
	\brief
		Return an asymmetric perspective projection matrix based on the left,
		right, bottom, top, near and far planes.
	\param[in] float l, float r, float b, float t, float n, float f
		Planes of the frustum.
	\return
		Asymmetric perspective projection matrix based on the left, right,
		bottom, top, near and far planes.
	*************************************************************************/
	static Mat4	BuildAsymPerspective(float l, float r, float b, float t, float n, float f);

	/*!***********************************************************************
	\brief
		Return an orthographic projection matrix based on the left, right,
		bottom, top, near and far planes.
	\param[in] float l, float r, float b, float t, float n, float f
		Planes of the frustum.
	\return
		Orthographic projection matrix based on the left, right, bottom, top,
		near and far planes.
	*************************************************************************/
	static Mat4	BuildOrtho(float l, float r, float b, float t, float n, float f);

	/*!***********************************************************************
	\brief
		Return a viewport transformation matrix to convert NDC coordinates to
		viewport coordinates.
	\param[in] float x, float y
		NDC coordinates.
	\param[in] float w, float h
		Width and height of viewport.
	\return
		Viewport transformation matrix to convert NDC coordinates to viewport
		coordinates.
	*************************************************************************/
	static Mat4	BuildViewport(float x, float y, float w, float h);

	static Mat4 BuildNDCToScreen(int w, int h);
	static Mat4 BuildScreenToNDC(int w, int h);
	
	// Defined in Math.hpp. Include Math.hpp to use these functions.
	/*!***********************************************************************
	\brief
		Matrix-Vec4 multiplication.
	\param[in] const Vec4& rhs
		Vec4 to multiply with.
	\return
		*this * rhs.
	*************************************************************************/
	Vec4 operator*(const Vec4& rhs);
	Vec3 operator*(const Vec3& rhs);

	/*!***********************************************************************
	\brief
		Get an entire row of the Matrix as a Vec3.
	\param[in] unsigned int row
		Row of matrix to get.
	\return
		Entire row of the Matrix as a Vec3.
	*************************************************************************/
	Vec3 GetRow3(unsigned int) const;

	/*!***********************************************************************
	\brief
		Get an entire row of the Matrix as a Vec4.
	\param[in] unsigned int row
		Row of matrix to get.
	\return
		Entire row of the Matrix as a Vec4.
	*************************************************************************/
	Vec4 GetRow4(unsigned int) const;

	/*!***********************************************************************
	\brief
		Get an entire column of the Matrix as a Vec3.
	\param[in] unsigned int col
		Column of matrix to get.
	\return
		Entire column of the Matrix as a Vec3.
	*************************************************************************/
	Vec3 GetCol3(unsigned int) const;

	/*!***********************************************************************
	\brief
		Get an entire column of the Matrix as a Vec4.
	\param[in] unsigned int col
		Column of matrix to get.
	\return
		Entire column of the Matrix as a Vec4.
	*************************************************************************/
	Vec4 GetCol4(unsigned int) const;

	/*!***********************************************************************
	\brief
		Return the cofactor of the Matrix based on the specified row and col.
	\param[in] unsigned int row, unsigned int col
		Row and column of matrix to get cofactor of.
	\return
		Cofactor of the Matrix based on the specified row and col.
	*************************************************************************/
	float Cofactor(unsigned int row, unsigned int col) const;

	/*!***********************************************************************
	\brief
		Return a Mat3 based on this Mat4 by ignoring the specified row and col.
	\param[in] unsigned int row, unsigned int col
		Row and column of matrix to ignore.
	\return
		Mat3 based on this Mat4 by ignoring the specified row and col.
	*************************************************************************/
	inline Mat3 CreateSubMat3(unsigned int row, unsigned int col) const;

	Vec3 GetTranslation() const {
		return Vec3(a[12], a[13], a[14]);
	}

	Vec3 GetScale() const {
		float scaleX = std::sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
		float scaleY = std::sqrt(a[4] * a[4] + a[5] * a[5] + a[6] * a[6]);
		float scaleZ = std::sqrt(a[8] * a[8] + a[9] * a[9] + a[10] * a[10]);
		return Vec3(scaleX, scaleY, scaleZ);
	}

	Vec3 GetRotation() const {
		// Extract scale first
		Vec3 scale = GetScale();

		// Normalize rotation matrix by removing scale
		Mat4 normalized = *this;
		normalized.a[0] /= scale.x;
		normalized.a[1] /= scale.x;
		normalized.a[2] /= scale.x;
		normalized.a[4] /= scale.y;
		normalized.a[5] /= scale.y;
		normalized.a[6] /= scale.y;
		normalized.a[8] /= scale.z;
		normalized.a[9] /= scale.z;
		normalized.a[10] /= scale.z;

		// Extract angles
		float pitch = std::atan2(-normalized.a[6], normalized.a[10]);
		float yaw = std::asin(normalized.a[2]);
		float roll = std::atan2(-normalized.a[1], normalized.a[0]);

		return Vec3(pitch, yaw, roll);
	}
};

inline Mat4::Mat4(const float& e00, const float& e01, const float& e02, const float& e03,
		   const float& e10, const float& e11, const float& e12, const float& e13,
		   const float& e20, const float& e21, const float& e22, const float& e23,
		   const float& e30, const float& e31, const float& e32, const float& e33) {
	 a[0] = e00,   a[1] = e10,   a[2] = e20,   a[3] = e30;
	 a[4] = e01,   a[5] = e11,   a[6] = e21,   a[7] = e31;
	 a[8] = e02,   a[9] = e12,  a[10] = e22,  a[11] = e32;
	a[12] = e03,  a[13] = e13,  a[14] = e23,  a[15] = e33;
}

inline Mat4::Mat4(const float arr[16]) {
	for (int i = 0; i < 16; ++i) {
		a[i] = arr[i];
	}
}

inline Mat4::Mat4(const Mat4& m) {
	 a[0] = m.a[0],   a[1] = m.a[1],   a[2] = m.a[2],   a[3] = m.a[3];
	 a[4] = m.a[4],   a[5] = m.a[5],   a[6] = m.a[6],   a[7] = m.a[7];
	 a[8] = m.a[8],   a[9] = m.a[9],  a[10] = m.a[10], a[11] = m.a[11];
	a[12] = m.a[12], a[13] = m.a[13], a[14] = m.a[14], a[15] = m.a[15];
}

inline Mat4& Mat4::operator=(const Mat4& m) {
	 a[0] = m.a[0],   a[1] = m.a[1],   a[2] = m.a[2],   a[3] = m.a[3];
	 a[4] = m.a[4],   a[5] = m.a[5],   a[6] = m.a[6],   a[7] = m.a[7];
	 a[8] = m.a[8],   a[9] = m.a[9],  a[10] = m.a[10], a[11] = m.a[11];
	a[12] = m.a[12], a[13] = m.a[13], a[14] = m.a[14], a[15] = m.a[15];

	return *this;
}

inline Mat4 Mat4::operator*(const Mat4& rhs) {
	Mat4 mat4{};
	for (unsigned int i = 0; i < 4; ++i) {
		for (unsigned int j = 0; j < 4; ++j) {
			for (unsigned int k = 0; k < 4; ++k) {
				mat4.GetElement(i, j) += GetElement(i, k) * rhs.GetElement(k, j);
			}
		}
	}

	return mat4;
}

inline Mat4 Mat4::operator*(float scalar) {
	Mat4 mat4;
	for (unsigned int i = 0; i < 16; ++i) {
		mat4[i] = (*this)[i] * scalar;
	}
	return mat4;
}

inline Mat4& Mat4::operator*=(const Mat4& rhs) {
	*this = *this * rhs;
	return *this;
}

inline void Mat4::SetToZero() {
	for (unsigned int i = 0; i < 16; ++i) {
		a[i] = 0.f;
	}
}

inline void Mat4::SetToIdentity() {
	SetToZero(); // First set all elements to zero
	GetElement(0, 0) = 1.0f;
	GetElement(1, 1) = 1.0f;
	GetElement(2, 2) = 1.0f;
	GetElement(3, 3) = 1.0f;
}

inline void Mat4::SetTo(const float& e00, const float& e01, const float& e02, const float& e03,
	const float& e10, const float& e11, const float& e12, const float& e13,
	const float& e20, const float& e21, const float& e22, const float& e23,
	const float& e30, const float& e31, const float& e32, const float& e33) {
	 a[0] = e00,  a[1] = e10,  a[2] = e20,  a[3] = e30;
	 a[4] = e01,  a[5] = e11,  a[6] = e21,  a[7] = e31;
	 a[8] = e02,  a[9] = e12, a[10] = e22, a[11] = e32;
	a[12] = e03, a[13] = e13, a[14] = e23, a[15] = e33;
}

inline float Mat4::Determinant() const {
	float det = 0.0f;

	// Cofactor expansion along the first row
	det += GetElement(0, 0) * Cofactor(0, 0);
	det -= GetElement(0, 1) * Cofactor(0, 1);
	det += GetElement(0, 2) * Cofactor(0, 2);
	det -= GetElement(0, 3) * Cofactor(0, 3);

	return det;
}

inline Mat4 Mat4::Transpose() const {
	Mat4 mat4;
	for (unsigned int i = 0; i < 4; ++i) {
		for (unsigned int j = 0; j < 4; ++j) {
			mat4.GetElement(i, j) = GetElement(j, i);
		}
	}
	return mat4;
}

inline Mat4& Mat4::TransposeInPlace() {
	for (unsigned int i = 0; i < 4; ++i) {
		for (unsigned int j = i + 1; j < 4; ++j) {
			std::swap(GetElement(i, j), GetElement(j, i));
		}
	}
	return *this;
}

inline bool Mat4::InverseInPlace() {
	float det = Determinant();
	if (det == 0.0f) {
		return false;  // Matrix is singular, cannot invert
	}

	Mat4 inverse = Inverse(); 
	*this = inverse;
	return true;
}

inline Mat4 Mat4::Inverse() const {
	Mat4 mInverse;
	float det = Determinant();
	if (det == 0.0f) {
		// Matrix is singular and cannot be inverted
		return *this;
	}

	float invDet = 1.0f / det;

	// Cofactors for row 0
	mInverse.GetElement(0, 0) = Cofactor(0, 0) * invDet;
	mInverse.GetElement(1, 0) = -Cofactor(0, 1) * invDet;
	mInverse.GetElement(2, 0) = Cofactor(0, 2) * invDet;
	mInverse.GetElement(3, 0) = -Cofactor(0, 3) * invDet;

	// Cofactors for row 1
	mInverse.GetElement(0, 1) = -Cofactor(1, 0) * invDet;
	mInverse.GetElement(1, 1) = Cofactor(1, 1) * invDet;
	mInverse.GetElement(2, 1) = -Cofactor(1, 2) * invDet;
	mInverse.GetElement(3, 1) = Cofactor(1, 3) * invDet;

	// Cofactors for row 2
	mInverse.GetElement(0, 2) = Cofactor(2, 0) * invDet;
	mInverse.GetElement(1, 2) = -Cofactor(2, 1) * invDet;
	mInverse.GetElement(2, 2) = Cofactor(2, 2) * invDet;
	mInverse.GetElement(3, 2) = -Cofactor(2, 3) * invDet;

	// Cofactors for row 3
	mInverse.GetElement(0, 3) = -Cofactor(3, 0) * invDet;
	mInverse.GetElement(1, 3) = Cofactor(3, 1) * invDet;
	mInverse.GetElement(2, 3) = -Cofactor(3, 2) * invDet;
	mInverse.GetElement(3, 3) = Cofactor(3, 3) * invDet;

	return mInverse;
}

inline Mat4 Mat4::BuildTranslation(float x, float y, float z) {
	Mat4 mTrans{};
	mTrans.SetToIdentity();
	mTrans.GetElement(0, 3) = x;
	mTrans.GetElement(1, 3) = y;
	mTrans.GetElement(2, 3) = z;
	return mTrans;
}

inline Mat4 Mat4::BuildZRotation(float degrees) {
	Mat4 mRot{};
	float rad = degrees * (PI / 180.f);
	mRot.SetToIdentity();
	mRot.GetElement(0, 0) = cosf(rad);
	mRot.GetElement(0, 1) = -sinf(rad);
	mRot.GetElement(1, 0) = sinf(rad);
	mRot.GetElement(1, 1) = cosf(rad);

	return mRot;
}

inline Mat4 Mat4::BuildXRotation(float degrees) {
	Mat4 mRot{};
	float rad = degrees * (PI / 180.f);
	mRot.SetToIdentity();
	mRot.GetElement(1, 1) = cosf(rad);
	mRot.GetElement(1, 2) = -sinf(rad);
	mRot.GetElement(2, 1) = sinf(rad);
	mRot.GetElement(2, 2) = cosf(rad);

	return mRot;
}

inline Mat4 Mat4::BuildYRotation(float degrees) {
	Mat4 mRot{};
	float rad = degrees * (PI / 180.f);
	mRot.SetToIdentity();
	mRot.GetElement(0, 0) = cosf(rad);
	mRot.GetElement(0, 2) = sinf(rad);
	mRot.GetElement(2, 0) = -sinf(rad);
	mRot.GetElement(2, 2) = cosf(rad);

	return mRot;
}

inline Mat4 Mat4::BuildScaling(float cx, float cy, float cz, float x, float y, float z) {
	Mat4 mScale{};
	mScale.SetToIdentity();
	mScale.GetElement(0, 0) = x;
	mScale.GetElement(1, 1) = y;
	mScale.GetElement(2, 2) = z;

	mScale.GetElement(0, 3) = cx * (1.0f - x);
	mScale.GetElement(1, 3) = cy * (1.0f - y);
	mScale.GetElement(2, 3) = cz * (1.0f - z);
	return mScale;
}

inline Mat4 Mat4::BuildScaling(float x, float y, float z)
{
	Mat4 mScale{};
	mScale.SetToIdentity();
	mScale.GetElement(0, 0) = x;
	mScale.GetElement(1, 1) = y;
	mScale.GetElement(2, 2) = z;

	return mScale;
}

inline Mat4 Mat4::BuildSymPerspective(float vfov, float a, float n, float f) {
	float cotFov = 1.f / tanf(vfov / 2.f);
	Mat4 mSym{ cotFov / a, 0.f,	   0.f,					 0.f,
			   0.f,		   cotFov, 0.f,					 0.f,
			   0.f,		   0.f,    -((f + n) / (f - n)), -((2 * n * f) / (f - n)),
			   0.f,		   0.f,    -1.f,				 0.f };
	
	return mSym;
}

inline Mat4 Mat4::BuildAsymPerspective(float l, float r, float b, float t, float n, float f) {
	Mat4 mAsym{ (2.f * n) / (r - l), 0.f,				  (r + l) / (r - l),    0.f,
				0.f,				 (2.f * n) / (t - b), (t + b) / (r - b),    0.f,
				0.f,				 0.f,				  -((f + n) / (f - n)), -((2 * n * f) / (f - n)),
				0.f,				 0.f,				  -1.f,				    0.f };

	return mAsym;
}

inline Mat4 Mat4::BuildOrtho(float l, float r, float b, float t, float n, float f) {
	Mat4 mOrtho{ 2.f - (r - l), 0.f,           0.f,				 -((r + l) / (r - l)),
				 0.f,		    2.f - (t - b), 0.f,				 -((t + b) / (t - b)),
				 0.f,           0.f,           -(2.f / (f - n)), -((f + n) / (f - n)),
				 0.f,		    0.f,			  0.f,			 1.f };

	return mOrtho;
}

inline Mat4 Mat4::BuildViewport(float x, float y, float w, float h) {
	Mat4 mVP{ w / 2.f, 0.f,     0.f,  (w / 2.f) + x,
			  0.f,     h / 2.f, 0.f,  (h / 2.f) + y,
			  0.f,	   0.f,	    0.5f,  0.5f,
			  0.f,	   0.f,	    0.f,   1.f };
	
	return mVP;
}

inline Mat4 Mat4::BuildNDCToScreen(int w, int h)
{
	Mat4 ndcToScreen{};
	ndcToScreen.SetToIdentity();

	// Scale NDC to screen space
	ndcToScreen.GetElement(0, 0) = w / 2.0f;  // Scale x
	ndcToScreen.GetElement(1, 1) = h / 2.0f; // Scale y

	// Translate by (windowWidth / 2, windowHeight / 2) to adjust for the screen coordinates
	ndcToScreen.GetElement(0, 3) = w / 2.0f;  // Translate x
	ndcToScreen.GetElement(1, 3) = h / 2.0f; // Translate y

	return ndcToScreen;
}

inline Mat4 Mat4::BuildScreenToNDC(int w, int h)
{
	Mat4 screenToNDC{};
	screenToNDC.SetToIdentity();

	// Scale the X and Y to range [-1, 1]
	screenToNDC.GetElement(0, 0) = 2.0f / w;  // Scale x
	screenToNDC.GetElement(1, 1) = 2.0f / h; // Scale y

	// Translate by (-1, -1) to adjust for the bottom-left origin
	screenToNDC.GetElement(0, 3) = -1.0f;  // Translate x by -1
	screenToNDC.GetElement(1, 3) = -1.0f;  // Translate y by -1

	return screenToNDC;
}

#endif // !MAT_4_HPP